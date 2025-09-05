// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

module raddbg.async;

////////////////////////////////
//~ rjf: Work Function Type

public alias ASYNC_WorkFunctionType = void* function(ulong thread_idx, void* input);

////////////////////////////////
//~ rjf: Work Types

enum ASYNC_Priority {
  Low,
  High,
  COUNT
}

struct ASYNC_WorkParams {
  void* input;
  void** output;
  Semaphore semaphore;
  ulong* completion_counter;
  ulong* working_counter;
  ulong endt_us;
  ASYNC_Priority priority;
}

struct ASYNC_Work {
  ASYNC_WorkFunctionType* work_function;
  void* input;
  void** output;
  Semaphore semaphore;
  ulong* completion_counter;
  ulong* working_counter;
}

////////////////////////////////
//~ rjf: Task-Based Work Types

struct ASYNC_Task {
  Semaphore semaphore;
  void* output;
}

struct ASYNC_TaskNode {
  ASYNC_TaskNode* next;
  ASYNC_Task* v;
}

struct ASYNC_TaskList {
  ASYNC_TaskNode* first;
  ASYNC_TaskNode* last;
  ulong count;
}

////////////////////////////////
//~ rjf: Root (Per-Worker-Thread Arena Bundle)

struct ASYNC_Root {
  Arena** arenas;
}

////////////////////////////////
//~ rjf: Shared State Bundle

struct ASYNC_Ring {
  ulong ring_size;
  ubyte* ring_base;
  ulong ring_write_pos;
  ulong ring_read_pos;
  Mutex ring_mutex;
  CondVar ring_cv;
}

struct ASYNC_Shared {
  Arena* arena;
  
  // rjf: user -> work thread ring buffers
  ASYNC_Ring[ASYNC_Priority.COUNT] rings;
  Mutex ring_mutex;
  CondVar ring_cv;
  
  // rjf: work threads
  OS_Handle* work_threads;
  ulong work_threads_count;
  ulong work_threads_live_count;
}

////////////////////////////////
//~ rjf: Globals

static B32 async_work_thread_depth = 0;
static ulong async_work_thread_idx = 0;
__gshared ASYNC_Shared* async_shared = null;

////////////////////////////////
//~ rjf: Top-Level Layer Initialization

void async_init(CmdLine *cmdline) {
  Arena* arena = arena_alloc();
  async_shared = push_array(arena, ASYNC_Shared, 1);
  async_shared.arena = arena;
  for EachEnumVal(ASYNC_Priority, p)
  {
    ASYNC_Ring* ring = &async_shared.rings[p];
    ring.ring_size  = MB(8);
    ring.ring_base  = push_array_no_zero(arena, ubyte, ring.ring_size);
    ring.ring_mutex = mutex_alloc();
    ring.ring_cv    = cond_var_alloc();
  }
  async_shared.ring_mutex = mutex_alloc();
  async_shared.ring_cv = cond_var_alloc();
  String8 work_thread_count_string = cmd_line_string(cmdline, str8_lit("work_threads_count"));
  if (work_thread_count_string.size == 0 || !try_u64_from_str8_c_rules(work_thread_count_string, &async_shared.work_threads_count))
  {
    async_shared.work_threads_count = Max(4, os_get_system_info().logical_processor_count-1);
  }
  async_shared.work_threads_count = Max(4, async_shared.work_threads_count);
  async_shared.work_threads = push_array(arena, OS_Handle, async_shared.work_threads_count);
  for EachIndex(idx, async_shared.work_threads_count)
  {
    async_shared.work_threads[idx] = os_thread_launch(async_work_thread__entry_point, (void *)idx, 0);
  }
}

////////////////////////////////
//~ rjf: Top-Level Accessors

ulong async_thread_count() {
  return async_shared.work_threads_count;
}

////////////////////////////////
//~ rjf: Work Kickoffs

bool async_push_work_(ASYNC_WorkFunctionType *work_function, ASYNC_WorkParams *params) {
  // rjf: choose ring
  ASYNC_Ring* ring = &async_shared.rings[params.priority];
  
  // rjf: build work package 
  ASYNC_Work work;
  work.work_function = work_function;
  work.input              = params.input;
  work.output             = params.output;
  work.semaphore          = params.semaphore;
  work.completion_counter = params.completion_counter;
  work.working_counter    = params.working_counter;
  
  // rjf: loop; try to write into user -> writer ring buffer. if we're on a
  // worker thread, determine if we need to execute this task locally on this
  // thread, and skip ring buffer if so.
  bool queued_in_ring_buffer = false;
  bool need_to_execute_on_this_thread = false;
  OS_MutexScope(ring.ring_mutex) for (;;)
  {
    ulong num_available_work_threads = (async_shared.work_threads_count - ins_atomic_u64_eval(&async_shared.work_threads_live_count));
    if (num_available_work_threads == 0 && async_work_thread_depth > 0)
    {
      need_to_execute_on_this_thread = true;
      break;
    }
    ulong unconsumed_size = ring.ring_write_pos - ring.ring_read_pos;
    ulong available_size = ring.ring_size - unconsumed_size;
    if (available_size >= sizeof(work))
    {
      queued_in_ring_buffer = true;
      if (!MemoryIsZeroStruct(&params.semaphore))
      {
        os_semaphore_take(params.semaphore, max_ulong);
      }
      ring.ring_write_pos += ring_write_struct(ring.ring_base, ring.ring_size, ring.ring_write_pos, &work);
      break;
    }
    if (os_now_microseconds() >= params.endt_us)
    {
      break;
    }
    cond_var_wait(ring.ring_cv, ring.ring_mutex, params.endt_us);
  }
  
  // rjf: broadcast ring buffer cv if we wrote successfully
  if (queued_in_ring_buffer)
  {
    cond_var_broadcast(ring.ring_cv);
    cond_var_broadcast(async_shared.ring_cv);
  }
  
  // rjf: if we did not queue successfully, and we have determined that
  // we need to execute this work on the current thread, then execute the
  // work before returning
  if (need_to_execute_on_this_thread)
  {
    async_execute_work(work);
  }
  
  // rjf: return success signal
  bool result = (queued_in_ring_buffer || need_to_execute_on_this_thread);
  return result;
}
#define async_push_work(function, ...) async_push_work_((function), &(ASYNC_WorkParams){.endt_us = max_ulong, .priority = ASYNC_Priority.High, __VA_ARGS__})

////////////////////////////////
//~ rjf: Task-Based Work Helper

void async_task_list_push(Arena* arena, ASYNC_TaskList* list, ASYNC_Task* t) {
  ASYNC_TaskNode* n = push_array(arena, ASYNC_TaskNode, 1);
  SLLQueuePush(list.first, list.last, n);
  n.v = t;
  list.count += 1;
}

ASYNC_Task* async_task_launch_(Arena* arena, ASYNC_WorkFunctionType* work_function, ASYNC_WorkParams* params) {
  ASYNC_Task* task = push_array(arena, ASYNC_Task, 1);
  task.semaphore = os_semaphore_alloc(1, 1, str8_zero());
  ASYNC_WorkParams params_refined;
  MemoryCopyStruct(&params_refined, params);
  params_refined.endt_us = max_ulong;
  params_refined.semaphore = task.semaphore;
  if (params_refined.output == null)
  {
    params_refined.output = &task.output;
  }
  async_push_work_(work_function, &params_refined);
  return task;
}
#define async_task_launch(arena, work_function, ...) async_task_launch_((arena), (work_function), &(ASYNC_WorkParams){.endt_us = max_ulong, __VA_ARGS__})
void* async_task_join(ASYNC_Task* task) {
  void* result = null;
  if (task != null && !MemoryIsZeroStruct(&task.semaphore))
  {
    os_semaphore_take(task.semaphore, max_ulong);
    os_semaphore_release(task.semaphore);
    MemoryZeroStruct(&task.semaphore);
    result = (void*)ins_atomic_u64_eval(&task.output);
  }
  return result;
}
#define async_task_join_struct(task, T) (T *)async_task_join(task)

////////////////////////////////
//~ rjf: Work Execution

ASYNC_Work async_pop_work() {
  ASYNC_Work work;
  bool done = false;
  ASYNC_Priority taken_priority = ASYNC_Priority.Low;
  OS_MutexScope(async_shared.ring_mutex) for (;!done;)
  {
    for (ASYNC_Priority priority = ASYNC_Priority.High;; priority = (ASYNC_Priority)(priority - 1))
    {
      ASYNC_Ring* ring = &async_shared.rings[priority];
      OS_MutexScope(ring.ring_mutex)
      {
        ulong unconsumed_size = ring.ring_write_pos - ring.ring_read_pos;
        if (unconsumed_size >= sizeof(work))
        {
          ring.ring_read_pos += ring_read_struct(ring.ring_base, ring.ring_size, ring.ring_read_pos, &work);
          done = true;
          taken_priority = priority;
        }
      }
      if (done)
      {
        break;
      }
      if (priority == ASYNC_Priority.Low)
      {
        break;
      }
    }
    if (!done)
    {
      cond_var_wait(async_shared.ring_cv, async_shared.ring_mutex, max_ulong);
    }
  }
  cond_var_broadcast(async_shared.ring_cv);
  cond_var_broadcast(async_shared.rings[taken_priority].ring_cv);
  return work;
}

void async_execute_work(ASYNC_Work work) {
  //- rjf: run work
  async_work_thread_depth += 1;
  void* work_out = work.work_function(async_work_thread_idx, work.input);
  async_work_thread_depth -= 1;
  
  //- rjf: store output
  if (work.output != null)
  {
    ins_atomic_u64_eval_assign((ulong*)work.output, (ulong)work_out);
  }
  
  //- rjf: release semaphore
  if (!MemoryIsZeroStruct(&work.semaphore))
  {
    os_semaphore_drop(work.semaphore);
  }
  
  //- rjf: increment completion counter
  if (work.completion_counter != 0)
  {
    ins_atomic_u64_inc_eval(work.completion_counter);
  }
  
  //- rjf: decrement working counter
  if (work.working_counter != 0)
  {
    ins_atomic_u64_dec_eval(work.working_counter);
  }
}

////////////////////////////////
//~ rjf: Root Allocation/Deallocation

ASYNC_Root* async_root_alloc() {
  Arena* arena = arena_alloc();
  ASYNC_Root* root = push_array(arena, ASYNC_Root, 1);
  root.arenas = push_array(arena, Arena*, async_thread_count());
  root.arenas[0] = arena;
  for (ulong idx = 1; idx < async_thread_count(); idx += 1)
  {
    root.arenas[idx] = arena_alloc();
  }
  return root;
}

void async_root_release(ASYNC_Root* root) {
  for (ulong idx = 1; idx < async_thread_count(); idx += 1)
  {
    arena_release(root.arenas[idx]);
  }
  arena_release(root.arenas[0]);
}

Arena* async_root_thread_arena(ASYNC_Root* root) {
  return root.arenas[async_work_thread_idx];
}

////////////////////////////////
//~ rjf: Work Thread Entry Point

void async_work_thread__entry_point(void* p) {
  ulong thread_idx = (ulong)p;
  ThreadNameF("[async] work thread #%I64u", thread_idx);
  async_work_thread_idx = thread_idx;
  for (;;)
  {
    ASYNC_Work work = async_pop_work();
    ins_atomic_u64_inc_eval(&async_shared.work_threads_live_count);
    async_execute_work(work);
    ins_atomic_u64_dec_eval(&async_shared.work_threads_live_count);
  }
}

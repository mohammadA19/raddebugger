
module async;

////////////////////////////////
//~ rjf: Work Function Type

#define ASYNC_WORK_SIG(name) void* name(ulong thread_idx, void* input)
#define ASYNC_WORK_DEF(name) public ASYNC_WORK_SIG(name)
typedef ASYNC_WORK_SIG(ASYNC_WorkFunctionType);

////////////////////////////////
//~ rjf: Work Types

public enum ASYNC_Priority
{
  Low,
  High,
}

public struct ASYNC_WorkParams
{
  void* input;
  void** output;
  OS_Handle semaphore;
  ulong* completion_counter;
  ulong endt_us;
  ASYNC_Priority priority;
}

public struct ASYNC_Work
{
  ASYNC_WorkFunctionType* work_function;
  void* input;
  void** output;
  OS_Handle semaphore;
  ulong* completion_counter;
}

////////////////////////////////
//~ rjf: Task-Based Work Types

public struct ASYNC_Task
{
  OS_Handle semaphore;
  void* output;
}

public struct ASYNC_TaskNode
{
  ASYNC_TaskNode* next;
  ASYNC_Task* v;
}

public struct ASYNC_TaskList
{
  ASYNC_TaskNode* first;
  ASYNC_TaskNode* last;
  ulong count;
}

////////////////////////////////
//~ rjf: Shared State Bundle

public struct ASYNC_Ring
{
  ulong ring_size;
  ubyte* ring_base;
  ulong ring_write_pos;
  ulong ring_read_pos;
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
}

public struct ASYNC_Shared
{
  Arena* arena;
  
  // rjf: user . work thread ring buffers
  ASYNC_Ring[ASYNC_Priority.COUNT] rings;
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
  
  // rjf: work threads
  OS_Handle* work_threads;
  ulong work_threads_count;
  ulong work_threads_live_count;
}

////////////////////////////////
//~ rjf: Globals

static B32 async_work_thread_depth = 0;
static ulong async_work_thread_idx = 0;
__gshared ASYNC_Shared* async_shared = 0;

////////////////////////////////
//~ rjf: Top-Level Layer Initialization

public void async_init(CmdLine* cmdline)
{
  Arena* arena = arena_alloc();
  async_shared = push_array(arena, ASYNC_Shared, 1);
  async_shared.arena = arena;
  foreach(p; membersOf!ASYNC_Priority)
  {
    ASYNC_Ring* ring = &async_shared.rings[p];
    ring.ring_size  = MB(8);
    ring.ring_base  = push_array_no_zero(arena, U8, ring.ring_size);
    ring.ring_mutex = os_mutex_alloc();
    ring.ring_cv    = os_condition_variable_alloc();
  }
  async_shared.ring_mutex = os_mutex_alloc();
  async_shared.ring_cv = os_condition_variable_alloc();
  string work_thread_count_string = cmd_line_string(cmdline, "work_threads_count");
  if(work_thread_count_string.size == 0 || !try_u64_from_str8_c_rules(work_thread_count_string, &async_shared.work_threads_count))
  {
    async_shared.work_threads_count = max(1, os_get_system_info().logical_processor_count-1);
  }
  async_shared.work_threads = push_array(arena, OS_Handle, async_shared.work_threads_count);
  foreach (idx; 0 .. async_shared.work_threads_count)
  {
    async_shared.work_threads[idx] = os_thread_launch(async_work_thread__entry_point, cast(void*)idx, 0);
  }
}

////////////////////////////////
//~ rjf: Top-Level Accessors

public ulong async_thread_count()
{
  return async_shared->work_threads_count;
}

////////////////////////////////
//~ rjf: Work Kickoffs

public B32 async_push_work_(ASYNC_WorkFunctionType* work_function, ASYNC_WorkParams* params)
{
  // rjf: choose ring
  ASYNC_Ring* ring = &async_shared.rings[params->priority];
  
  // rjf: build work package 
  ASYNC_Work work = {0};
  work.work_function = work_function;
  work.input              = params->input;
  work.output             = params->output;
  work.semaphore          = params->semaphore;
  work.completion_counter = params->completion_counter;
  
  // rjf: loop; try to write into user -> writer ring buffer. if we're on a
  // worker thread, determine if we need to execute this task locally on this
  // thread, and skip ring buffer if so.
  B32 queued_in_ring_buffer = 0;
  B32 need_to_execute_on_this_thread = 0;
  OS_MutexScope(ring->ring_mutex) for(;;)
  {
    U64 num_available_work_threads = (async_shared->work_threads_count - ins_atomic_u64_eval(&async_shared->work_threads_live_count));
    if(num_available_work_threads == 0 && async_work_thread_depth > 0)
    {
      need_to_execute_on_this_thread = 1;
      break;
    }
    U64 unconsumed_size = ring->ring_write_pos - ring->ring_read_pos;
    U64 available_size = ring->ring_size - unconsumed_size;
    if(available_size >= sizeof(work))
    {
      queued_in_ring_buffer = 1;
      if(!os_handle_match(params->semaphore, os_handle_zero()))
      {
        os_semaphore_take(params->semaphore, max_U64);
      }
      ring->ring_write_pos += ring_write_struct(ring->ring_base, ring->ring_size, ring->ring_write_pos, &work);
      break;
    }
    if(os_now_microseconds() >= params->endt_us)
    {
      break;
    }
    os_condition_variable_wait(ring->ring_cv, ring->ring_mutex, params->endt_us);
  }
  
  // rjf: broadcast ring buffer cv if we wrote successfully
  if(queued_in_ring_buffer)
  {
    os_condition_variable_broadcast(ring->ring_cv);
    os_condition_variable_broadcast(async_shared->ring_cv);
  }
  
  // rjf: if we did not queue successfully, and we have determined that
  // we need to execute this work on the current thread, then execute the
  // work before returning
  if(need_to_execute_on_this_thread)
  {
    async_execute_work(work);
  }
  
  // rjf: return success signal
  B32 result = (queued_in_ring_buffer || need_to_execute_on_this_thread);
  return result;
}
#define async_push_work(function, ...) async_push_work_((function), &(ASYNC_WorkParams){.endt_us = max_ulong, .priority = ASYNC_Priority_High, __VA_ARGS__})

////////////////////////////////
//~ rjf: Task-Based Work Helper

public void async_task_list_push(Arena* arena, ASYNC_TaskList* list, ASYNC_Task* t);
public ASYNC_Task* async_task_launch_(Arena* arena, ASYNC_WorkFunctionType* work_function, ASYNC_WorkParams* params);
#define async_task_launch(arena, work_function, ...) async_task_launch_((arena), (work_function), &(ASYNC_WorkParams){.endt_us = max_ulong, __VA_ARGS__})
public void* async_task_join(ASYNC_Task* task);
#define async_task_join_struct(task, T) (T* )async_task_join(task)

////////////////////////////////
//~ rjf: Work Execution

public ASYNC_Work async_pop_work();
public void async_execute_work(ASYNC_Work work);

////////////////////////////////
//~ rjf: Work Thread Entry Point

public void async_work_thread__entry_point(void* p);

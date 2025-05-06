// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Work Function Type

#define ASYNC_WORK_SIG(name) void* name(uint64 thread_idx, void* input)
#define ASYNC_WORK_DEF(name) internal ASYNC_WORK_SIG(name)
typedef ASYNC_WORK_SIG(ASYNC_WorkFunctionType);

////////////////////////////////
//~ rjf: Work Types

public enum ASYNC_Priority
{
  Low,
  High,
  COUNT
}

struct ASYNC_WorkParams
{
  void* input;
  void** output;
  OS_Handle semaphore;
  uint64* completion_counter;
  uint64 endt_us;
  ASYNC_Priority priority;
}

struct ASYNC_Work
{
  ASYNC_WorkFunctionType* work_function;
  void* input;
  void** output;
  OS_Handle semaphore;
  uint64* completion_counter;
}

////////////////////////////////
//~ rjf: Task-Based Work Types

struct ASYNC_Task
{
  OS_Handle semaphore;
  void* output;
}

struct ASYNC_TaskNode
{
  ASYNC_TaskNode* next;
  ASYNC_Task* v;
}

struct ASYNC_TaskList
{
  ASYNC_TaskNode* first;
  ASYNC_TaskNode* last;
  uint64 count;
}

////////////////////////////////
//~ rjf: Shared State Bundle

struct ASYNC_Ring
{
  uint64 ring_size;
  uint8* ring_base;
  uint64 ring_write_pos;
  uint64 ring_read_pos;
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
}

struct ASYNC_Shared
{
  Arena* arena;
  
  // rjf: user -> work thread ring buffers
  ASYNC_Ring[ASYNC_Priority.COUNT] rings;
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
  
  // rjf: work threads
  OS_Handle* work_threads;
  uint64 work_threads_count;
  uint64 work_threads_live_count;
}

////////////////////////////////
//~ rjf: Globals

[ThreadStatic]
public static B32 async_work_thread_depth = 0;

[ThreadStatic]
public static uint64 async_work_thread_idx = 0;

public static ASYNC_Shared* async_shared = null;

////////////////////////////////
//~ rjf: Work Kickoffs

#define async_push_work(function, ...) async_push_work_((function), &(ASYNC_WorkParams){.endt_us = max_U64, .priority = ASYNC_Priority.High, __VA_ARGS__})

////////////////////////////////
//~ rjf: Task-Based Work Helper

#define async_task_launch(arena, work_function, ...) async_task_launch_((arena), (work_function), &(ASYNC_WorkParams){.endt_us = max_U64, __VA_ARGS__})
#define async_task_join_struct(task, T) (T *)async_task_join(task)

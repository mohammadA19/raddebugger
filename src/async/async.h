// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef ASYNC_H
#define ASYNC_H

////////////////////////////////
//~ rjf: Work Function Type

#define ASYNC_WORK_SIG(name) void *name(ulong thread_idx, void *input)
#define ASYNC_WORK_DEF(name) internal ASYNC_WORK_SIG(name)
typedef ASYNC_WORK_SIG(ASYNC_WorkFunctionType);

////////////////////////////////
//~ rjf: Work Types

typedef enum ASYNC_Priority
{
  ASYNC_Priority_Low,
  ASYNC_Priority_High,
  ASYNC_Priority_COUNT
}
ASYNC_Priority;

typedef struct ASYNC_WorkParams ASYNC_WorkParams;
struct ASYNC_WorkParams
{
  void *input;
  void **output;
  OS_Handle semaphore;
  ulong *completion_counter;
  ulong endt_us;
  ASYNC_Priority priority;
};

typedef struct ASYNC_Work ASYNC_Work;
struct ASYNC_Work
{
  ASYNC_WorkFunctionType *work_function;
  void *input;
  void **output;
  OS_Handle semaphore;
  ulong *completion_counter;
};

////////////////////////////////
//~ rjf: Task-Based Work Types

typedef struct ASYNC_Task ASYNC_Task;
struct ASYNC_Task
{
  OS_Handle semaphore;
  void *output;
};

typedef struct ASYNC_TaskNode ASYNC_TaskNode;
struct ASYNC_TaskNode
{
  ASYNC_TaskNode *next;
  ASYNC_Task *v;
};

typedef struct ASYNC_TaskList ASYNC_TaskList;
struct ASYNC_TaskList
{
  ASYNC_TaskNode *first;
  ASYNC_TaskNode *last;
  ulong count;
};

////////////////////////////////
//~ rjf: Shared State Bundle

typedef struct ASYNC_Ring ASYNC_Ring;
struct ASYNC_Ring
{
  ulong ring_size;
  byte *ring_base;
  ulong ring_write_pos;
  ulong ring_read_pos;
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
};

typedef struct ASYNC_Shared ASYNC_Shared;
struct ASYNC_Shared
{
  Arena *arena;
  
  // rjf: user -> work thread ring buffers
  ASYNC_Ring rings[ASYNC_Priority_COUNT];
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
  
  // rjf: work threads
  OS_Handle *work_threads;
  ulong work_threads_count;
  ulong work_threads_live_count;
};

////////////////////////////////
//~ rjf: Globals

thread_static B32 async_work_thread_depth = 0;
thread_static ulong async_work_thread_idx = 0;
global ASYNC_Shared *async_shared = 0;

////////////////////////////////
//~ rjf: Top-Level Layer Initialization

internal void async_init(CmdLine *cmdline);

////////////////////////////////
//~ rjf: Top-Level Accessors

internal ulong async_thread_count(void);

////////////////////////////////
//~ rjf: Work Kickoffs

internal B32 async_push_work_(ASYNC_WorkFunctionType *work_function, ASYNC_WorkParams *params);
#define async_push_work(function, ...) async_push_work_((function), &(ASYNC_WorkParams){.endt_us = max_U64, .priority = ASYNC_Priority_High, __VA_ARGS__})

////////////////////////////////
//~ rjf: Task-Based Work Helper

internal void async_task_list_push(Arena *arena, ASYNC_TaskList *list, ASYNC_Task *t);
internal ASYNC_Task *async_task_launch_(Arena *arena, ASYNC_WorkFunctionType *work_function, ASYNC_WorkParams *params);
#define async_task_launch(arena, work_function, ...) async_task_launch_((arena), (work_function), &(ASYNC_WorkParams){.endt_us = max_U64, __VA_ARGS__})
internal void *async_task_join(ASYNC_Task *task);
#define async_task_join_struct(task, T) (T *)async_task_join(task)

////////////////////////////////
//~ rjf: Work Execution

internal ASYNC_Work async_pop_work(void);
internal void async_execute_work(ASYNC_Work work);

////////////////////////////////
//~ rjf: Work Thread Entry Point

internal void async_work_thread__entry_point(void *p);

#endif // ASYNC_H

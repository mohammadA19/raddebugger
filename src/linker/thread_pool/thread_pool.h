// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define THREAD_POOL_TASK_FUNC(name) void name(Arena *arena, U64 worker_id, U64 task_id, void *raw_task)
typedef THREAD_POOL_TASK_FUNC(TP_TaskFunc);

struct TP_Arena
{
  U64     count;
  Arena **v;
}

struct TP_Temp
{
  U64   count;
  Temp *v;
}

struct TP_Worker
{
  U64                id;
  struct TP_Context *pool;
  OS_Handle          handle;
}

struct TP_Context
{
  B32          is_live;
  OS_Handle    exec_semaphore;
  OS_Handle    task_semaphore;
  OS_Handle    main_semaphore;

  U32          worker_count;
  TP_Worker   *worker_arr;

  TP_Arena    *task_arena;
  TP_TaskFunc *task_func;
  void        *task_data;
  U64          task_count;
  U64          task_done;
  S64          task_left;
}

TP_Context * tp_alloc(Arena *arena, U32 worker_count, U32 max_worker_count, String8 name);
void         tp_release(TP_Context *pool);
TP_Arena *   tp_arena_alloc(TP_Context *pool);
void         tp_arena_release(TP_Arena **arena_ptr);
TP_Temp      tp_temp_begin(TP_Arena *arena);
void         tp_temp_end(TP_Temp temp);
void         tp_for_parallel(TP_Context *pool, TP_Arena *arena, U64 task_count, TP_TaskFunc *task_func, void *task_data);
Rng1U64 *    tp_divide_work(Arena *arena, U64 item_count, U32 worker_count);


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define THREAD_POOL_TASK_FUNC(name) void name(Arena *arena, ulong worker_id, ulong task_id, void *raw_task)
typedef THREAD_POOL_TASK_FUNC(TP_TaskFunc);

typedef struct TP_Arena
{
  ulong     count;
  Arena **v;
} TP_Arena;

typedef struct TP_Temp
{
  ulong   count;
  Temp *v;
} TP_Temp;

typedef struct TP_Worker
{
  ulong                id;
  struct TP_Context *pool;
  OS_Handle          handle;
} TP_Worker;

typedef struct TP_Context
{
  B32          is_live;
  OS_Handle    exec_semaphore;
  OS_Handle    task_semaphore;
  OS_Handle    main_semaphore;

  uint          worker_count;
  TP_Worker   *worker_arr;

  TP_Arena    *task_arena;
  TP_TaskFunc *task_func;
  void        *task_data;
  ulong          task_count;
  ulong          task_done;
  long          task_left;
} TP_Context;

internal TP_Context * tp_alloc(Arena *arena, uint worker_count, uint max_worker_count, String8 name);
internal void         tp_release(TP_Context *pool);
internal TP_Arena *   tp_arena_alloc(TP_Context *pool);
internal void         tp_arena_release(TP_Arena **arena_ptr);
internal TP_Temp      tp_temp_begin(TP_Arena *arena);
internal void         tp_temp_end(TP_Temp temp);
internal void         tp_for_parallel(TP_Context *pool, TP_Arena *arena, ulong task_count, TP_TaskFunc *task_func, void *task_data);
internal Rng1U64 *    tp_divide_work(Arena *arena, ulong item_count, uint worker_count);


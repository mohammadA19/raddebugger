// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define THREAD_POOL_TASK_FUNC(name) void name(Arena *arena, u64 worker_id, u64 task_id, void *raw_task)
typedef THREAD_POOL_TASK_FUNC(TP_TaskFunc);

typedef struct TP_Arena
{
  u64     count;
  Arena **v;
} TP_Arena;

typedef struct TP_Temp
{
  u64   count;
  Temp *v;
} TP_Temp;

typedef struct TP_Worker
{
  u64                id;
  struct TP_Context *pool;
  OS_Handle          handle;
} TP_Worker;

typedef struct TP_Context
{
  b32          is_live;
  OS_Handle    exec_semaphore;
  OS_Handle    task_semaphore;
  OS_Handle    main_semaphore;

  u32          worker_count;
  TP_Worker   *worker_arr;

  TP_Arena    *task_arena;
  TP_TaskFunc *task_func;
  void        *task_data;
  u64          task_count;
  u64          task_done;
  i64          task_left;
} TP_Context;


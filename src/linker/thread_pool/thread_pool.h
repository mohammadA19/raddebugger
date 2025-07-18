// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define THREAD_POOL_TASK_FUNC(name) void name(Arena *arena, uint64 worker_id, uint64 task_id, void *raw_task)
typedef THREAD_POOL_TASK_FUNC(TP_TaskFunc);

typedef struct TP_Arena
{
  uint64     count;
  Arena **v;
} TP_Arena;

typedef struct TP_Temp
{
  uint64   count;
  Temp *v;
} TP_Temp;

typedef struct TP_Worker
{
  uint64                id;
  struct TP_Context *pool;
  OS_Handle          handle;
} TP_Worker;

typedef struct TP_Context
{
  B32          is_live;
  OS_Handle    exec_semaphore;
  OS_Handle    task_semaphore;
  OS_Handle    main_semaphore;

  uint32          worker_count;
  TP_Worker   *worker_arr;

  TP_Arena    *task_arena;
  TP_TaskFunc *task_func;
  void        *task_data;
  uint64          task_count;
  uint64          task_done;
  uint64          task_left;
} TP_Context;



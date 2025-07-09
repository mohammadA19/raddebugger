// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal u32 *
push_u32(Arena *arena, u32 value)
{
  u32 *result = push_array_no_zero(arena, u32, 1);
  *result = value;
  return result;
}

internal u64 *
push_u64(Arena *arena, u64 value)
{
  u64 *result = push_array_no_zero(arena, u64, 1);
  *result = value;
  return result;
}

internal u32 *
push_array_copy_u32(Arena *arena, u32 *v, u64 count)
{
  u32 *result = push_array_no_zero(arena, u32, count);
  MemoryCopyTyped(result, v, count);
  return result;
}

internal u64 *
push_array_copy_u64(Arena *arena, u64 *v, u64 count)
{
  u64 *result = push_array_no_zero(arena, u64, count);
  MemoryCopyTyped(result, v, count);
  return result;
}

internal u64 **
push_matrix_u64(Arena *arena, u64 rows, u64 columns)
{
  u64 **result = push_array_no_zero(arena, u64 *, rows);
  for (u64 row_idx = 0; row_idx < rows; row_idx += 1) {
    result[row_idx] = push_array(arena, u64, columns);
  }
  return result;
}

internal Arena **
alloc_fixed_size_arena_array(Arena *arena, u64 count, u64 res, u64 cmt)
{
  u64 data_size = sizeof(count) + sizeof(Arena *) * count;
  u8 *data = push_array_no_zero(arena, u8, data_size);
  u64 *count_ptr = (u64 *)data;
  Arena **arr = (Arena **)(count_ptr + 1);
  *count_ptr = count;

  ArenaParams params  = {0};
  params.reserve_size = res;
  params.commit_size  = cmt;

  for (u64 i = 0; i < count; i += 1) {
    Arena *fixed_arena = arena_alloc_(&params);
    arr[i] = fixed_arena;
  }

  return arr;
}

internal void
release_arena_array(Arena **arr)
{
  u64 *count_ptr = (u64 *)arr - 1;
  u64 count = *count_ptr;
  for (u64 i = 0; i < count; i += 1) {
    arena_release(arr[i]);
    arr[i] = 0;
  }
}


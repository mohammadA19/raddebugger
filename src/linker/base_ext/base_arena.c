// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal uint32 *
push_u32(Arena *arena, uint32 value)
{
  uint32 *result = push_array_no_zero(arena, uint32, 1);
  *result = value;
  return result;
}

internal uint64 *
push_u64(Arena *arena, uint64 value)
{
  uint64 *result = push_array_no_zero(arena, uint64, 1);
  *result = value;
  return result;
}

internal uint32 *
push_array_copy_u32(Arena *arena, uint32 *v, uint64 count)
{
  uint32 *result = push_array_no_zero(arena, uint32, count);
  MemoryCopyTyped(result, v, count);
  return result;
}

internal uint64 *
push_array_copy_u64(Arena *arena, uint64 *v, uint64 count)
{
  uint64 *result = push_array_no_zero(arena, uint64, count);
  MemoryCopyTyped(result, v, count);
  return result;
}

internal uint64 **
push_matrix_u64(Arena *arena, uint64 rows, uint64 columns)
{
  uint64 **result = push_array_no_zero(arena, uint64 *, rows);
  for (uint64 row_idx = 0; row_idx < rows; row_idx += 1) {
    result[row_idx] = push_array(arena, uint64, columns);
  }
  return result;
}

internal Arena **
alloc_fixed_size_arena_array(Arena *arena, uint64 count, uint64 res, uint64 cmt)
{
  uint64 data_size = sizeof(count) + sizeof(Arena *) * count;
  uint8 *data = push_array_no_zero(arena, uint8, data_size);
  uint64 *count_ptr = (uint64 *)data;
  Arena **arr = (Arena **)(count_ptr + 1);
  *count_ptr = count;

  ArenaParams params  = {0};
  params.reserve_size = res;
  params.commit_size  = cmt;

  for (uint64 i = 0; i < count; i += 1) {
    Arena *fixed_arena = arena_alloc_(&params);
    arr[i] = fixed_arena;
  }

  return arr;
}

internal void
release_arena_array(Arena **arr)
{
  uint64 *count_ptr = (uint64 *)arr - 1;
  uint64 count = *count_ptr;
  for (uint64 i = 0; i < count; i += 1) {
    arena_release(arr[i]);
    arr[i] = 0;
  }
}


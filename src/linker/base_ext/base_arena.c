// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal String8
push_cstr(Arena *arena, String8 str)
{
  ulong buffer_size = str.size + 1;
  byte *buffer = push_array_no_zero(arena, byte, buffer_size);
  MemoryCopy(buffer, str.str, str.size);
  buffer[str.size] = 0;
  String8 result = str8(buffer, buffer_size);
  return result;
}

internal uint *
push_u32(Arena *arena, uint value)
{
  uint *result = push_array_no_zero(arena, uint, 1);
  *result = value;
  return result;
}

internal ulong *
push_u64(Arena *arena, ulong value)
{
  ulong *result = push_array_no_zero(arena, ulong, 1);
  *result = value;
  return result;
}

internal uint *
push_array_copy_u32(Arena *arena, uint *v, ulong count)
{
  uint *result = push_array_no_zero(arena, uint, count);
  MemoryCopyTyped(result, v, count);
  return result;
}

internal ulong *
push_array_copy_u64(Arena *arena, ulong *v, ulong count)
{
  ulong *result = push_array_no_zero(arena, ulong, count);
  MemoryCopyTyped(result, v, count);
  return result;
}

internal ulong **
push_matrix_u64(Arena *arena, ulong rows, ulong columns)
{
  ulong **result = push_array_no_zero(arena, ulong *, rows);
  for (ulong row_idx = 0; row_idx < rows; row_idx += 1) {
    result[row_idx] = push_array(arena, ulong, columns);
  }
  return result;
}

internal Arena **
alloc_fixed_size_arena_array(Arena *arena, ulong count, ulong res, ulong cmt)
{
  ulong data_size = sizeof(count) + sizeof(Arena *) * count;
  byte *data = push_array_no_zero(arena, byte, data_size);
  ulong *count_ptr = (ulong *)data;
  Arena **arr = (Arena **)(count_ptr + 1);
  *count_ptr = count;

  ArenaParams params  = {0};
  params.reserve_size = res;
  params.commit_size  = cmt;

  for (ulong i = 0; i < count; i += 1) {
    Arena *fixed_arena = arena_alloc_(&params);
    arr[i] = fixed_arena;
  }

  return arr;
}

internal void
release_arena_array(Arena **arr)
{
  ulong *count_ptr = (ulong *)arr - 1;
  ulong count = *count_ptr;
  for (ulong i = 0; i < count; i += 1) {
    arena_release(arr[i]);
    arr[i] = 0;
  }
}


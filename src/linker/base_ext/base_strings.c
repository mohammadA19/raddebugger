// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal String8Node *
str8_list_push_raw(Arena *arena, String8List *list, void *data_ptr, u64 data_size)
{
  String8 data = str8((u8 *)data_ptr, data_size);
  String8Node *node = str8_list_push(arena, list, data);
  return node;
}

internal u64
str8_list_push_pad(Arena *arena, String8List *list, u64 offset, u64 align)
{
  u64 pad_size = AlignPow2(offset, align) - offset;
  u8 *pad = push_array(arena, u8, pad_size);
  MemorySet(pad, 0, pad_size);
  str8_list_push(arena, list, str8(pad, pad_size));
  return pad_size;
}

internal u64
str8_list_push_pad_front(Arena *arena, String8List *list, u64 offset, u64 align)
{
  u64 pad_size = AlignPow2(offset, align) - offset;
  u8 *pad = push_array(arena, u8, pad_size);
  MemorySet(pad, 0, pad_size);
  str8_list_push_front(arena, list, str8(pad, pad_size));
  return pad_size;
}

internal String8List
str8_list_arr_concat(String8List *v, u64 count)
{
  String8List result = {0};
  for (u64 i = 0; i < count; i += 1) {
    str8_list_concat_in_place(&result, &v[i]);
  }
  return result;
}

internal String8Node *
str8_list_push_many(Arena *arena, String8List *list, u64 count)
{
  String8Node *arr = push_array(arena, String8Node, count);
  for (u64 i = 0; i < count; ++i) {
    str8_list_push_node(list, arr + i);
  }
  return arr;
}

internal String8Node *
str8_list_pop_front(String8List *list)
{
  String8Node *node = 0;
  if (list->node_count) {
	node = list->first;
    assert(list->total_size >= list->first->string.size);
    list->node_count -= 1;
    list->total_size -= list->first->string.size;
    SLLQueuePop(list->first, list->last);
  }
  return node;
}

internal u64
hash_from_str8(String8 string)
{
  XXH64_hash_t hash64 = XXH3_64bits(string.str, string.size);
  return hash64;
}


// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal u64
void_list_count_nodes(VoidNode *head)
{
  u64 node_count = 0;
  for (VoidNode *curr = head; curr != 0; curr = curr->next) {
    ++node_count;
  }
  return node_count;
}

internal void
void_node_concat(VoidNode **head, VoidNode *node)
{
  assert(*head != node);
  node->next = *head;
  *head = node;
}

internal void
void_node_concat_atomic(VoidNode **head, VoidNode *node)
{
  assert(*head != node);
  node->next = atomic_exchange(head, node);
}

internal u64Node *
u64_list_push(Arena *arena, u64List *list, u64 data)
{
  u64Node *n = push_array(arena, u64Node, 1);
  n->next = 0;
  n->data = data;
  
  SLLQueuePush(list->first, list->last, n);
  ++list->count;
  
  return n;
}

internal void
u64_list_concat_in_place(u64List *list, u64List *to_concat)
{
  SLLConcatInPlace(list, to_concat);
}

internal []u64
u64_array_from_list(Arena *arena, u64List *list)
{
  []u64 result;
  result.count = 0;
  result.v = push_array(arena, u64, list->count);
  for (u64Node *n = list->first; n != NULL; n = n->next) {
    result.v[result.count++] = n->data;
  }
  return result;
}

internal void
u32_array_sort(u64 count, u32 *v)
{
  radsort(v, count, u32_is_before);
}

internal void
u64_array_sort(u64 count, u64 *v)
{
  radsort(v, count, u64_is_before);
}

internal void
u32_pair_radix_sort(u64 count, PairU32 *arr)
{
  Temp scratch = scratch_begin(0,0);

  PairU32 *temp = push_array(scratch.arena, PairU32, count);

  const u64 bit_count0 = 11;
  const u64 bit_count1 = 11;
  const u64 bit_count2 = 10;
  
  u32 *count0 = push_array(scratch.arena, u32, (1 << bit_count0));
  u32 *count1 = push_array(scratch.arena, u32, (1 << bit_count1));
  u32 *count2 = push_array(scratch.arena, u32, (1 << bit_count2));

  for (u64 i = 0; i < count; ++i) {
    u32 digit0 = (arr[i].v0 >> 0         ) % (1 << bit_count0);
    u32 digit1 = (arr[i].v0 >> bit_count0) % (1 << bit_count1);
    u32 digit2 = (arr[i].v0 >> bit_count1) % (1 << bit_count2);

    ++count0[digit0];
    ++count1[digit1];
    ++count2[digit2];
  }

  counts_to_offsets_array_u32((1 << bit_count0), count0);
  counts_to_offsets_array_u32((1 << bit_count1), count1);
  counts_to_offsets_array_u32((1 << bit_count2), count2);

  for (u64 i = 0; i < count; ++i) {
    u32 digit0 = (arr[i].v0 >> 0) % (1 << bit_count0);
    temp[count0[digit0]++] = arr[i];
  }

  for (u64 i = 0; i < count; ++i) {
    u32 digit1 = (temp[i].v0 >> bit_count0) % (1 << bit_count1);
    arr[count1[digit1]++] = temp[i];
  }

  for (u64 i = 0; i < count; ++i) {
    u32 digit2 = (arr[i].v0 >> bit_count1) % (1 << bit_count2);
    temp[count2[digit2]++] = arr[i];
  }

  MemoryCopyTyped(arr, temp, count);

  scratch_end(scratch);
}

internal b32
u32_array_compare([]u32 a, []u32 b)
{
  b32 are_equal = 0;
  if (a.count == b.count) {
    int cmp = MemoryCompare(a.v, b.v, sizeof(a.v[0]) * a.count);
    are_equal = (cmp == 0);
  }
  return are_equal;
}

internal []u64
u64_array_remove_duplicates(Arena *arena, []u64 in)
{
  []u64 result;
  result.count = 0;
  result.v = push_array(arena, u64, in.count);
  
  for (u64 i = 1; i < in.count; ++i) {
    b32 is_unique = in.v[i - 1] != in.v[i];
    if (is_unique) {
      result.v[result.count++] = in.v[i - 1];
    }
  }
  
  if (in.count > 0 && result.count > 0) {
    b32 is_unique = result.v[result.count - 1] != in.v[in.count - 1];
    if (is_unique) {
      result.v[result.count++] = in.v[in.count - 1];
    }
  }
  
  u64 slack_size = (in.count - result.count) * sizeof(result.v[0]);
  arena_pop(arena, slack_size);
  
  return result;
}

internal u64
sum_array_u64(u64 count, u64 *v)
{
  u64 result = 0;
  for (u64 i = 0; i < count; i += 1) {
    result += v[i];
  }
  return result;
}

internal u64
sum_matrix_u64(u64 rows, u64 cols, u64 **v)
{
  u64 result = 0;
  for (u64 i = 0; i < rows; ++i) {
    result += sum_array_u64(cols, v[i]);
  }
  return result;
}

internal u64
max_array_u64(u64 count, u64 *v)
{
  u64 result = 0;
  for (u64 i = 0; i < count; i += 1) {
    result = max(v[i], result);
  }
  return result;
}

internal u64
min_array_u64(u64 count, u64 *v)
{
  u64 result = max_U64;
  for (u64 i = 0; i < count; i += 1) {
    result = min(v[i], result);
  }
  return result;
}

internal void
counts_to_offsets_array_u32(u64 count, u32 *arr)
{
  u32 next_offset = 0;
  for (u64 i = 0; i < count; i += 1) {
    u32 current_offset = next_offset;
    next_offset += arr[i];
    arr[i] = current_offset;
  }
}

internal void
counts_to_offsets_array_u64(u64 count, u64 *arr)
{
  u64 next_offset = 0;
  for (u64 i = 0; i < count; i += 1) {
    u64 current_offset = next_offset;
    next_offset += arr[i];
    arr[i] = current_offset;
  }
}

internal u32 *
offsets_from_counts_array_u32(Arena *arena, u32 *v, u64 count)
{
  u32 *result = push_array_copy_u32(arena, v, count);
  counts_to_offsets_array_u32(count, result);
  return result;
}

internal u64 *
offsets_from_counts_array_u64(Arena *arena, u64 *v, u64 count)
{
  u64 *result = push_array_copy_u64(arena, v, count);
  counts_to_offsets_array_u64(count, result);
  return result;
}


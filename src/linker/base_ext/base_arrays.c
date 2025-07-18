// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal uint64
void_list_count_nodes(VoidNode *head)
{
  uint64 node_count = 0;
  for (VoidNode *curr = head; curr != 0; curr = curr->next) {
    ++node_count;
  }
  return node_count;
}

internal void
void_node_concat(VoidNode **head, VoidNode *node)
{
  Assert(*head != node);
  node->next = *head;
  *head = node;
}

internal void
void_node_concat_atomic(VoidNode **head, VoidNode *node)
{
  Assert(*head != node);
  node->next = ins_atomic_ptr_eval_assign(head, node);
}

internal uint64Node *
u64_list_push(Arena *arena, uint64List *list, uint64 data)
{
  uint64Node *n = push_array(arena, uint64Node, 1);
  n->next = 0;
  n->data = data;
  
  SLLQueuePush(list->first, list->last, n);
  ++list->count;
  
  return n;
}

internal void
u64_list_concat_in_place(uint64List *list, uint64List *to_concat)
{
  SLLConcatInPlace(list, to_concat);
}

internal uint64Array
u64_array_from_list(Arena *arena, uint64List *list)
{
  uint64Array result;
  result.count = 0;
  result.v = push_array(arena, uint64, list->count);
  for (uint64Node *n = list->first; n != NULL; n = n->next) {
    result.v[result.count++] = n->data;
  }
  return result;
}

internal void
u32_array_sort(uint64 count, uint32 *v)
{
  radsort(v, count, u32_is_before);
}

internal void
u64_array_sort(uint64 count, uint64 *v)
{
  radsort(v, count, u64_is_before);
}

internal void
u32_pair_radix_sort(uint64 count, Pairuint32 *arr)
{
  Temp scratch = scratch_begin(0,0);

  Pairuint32 *temp = push_array(scratch.arena, Pairuint32, count);

  const uint64 bit_count0 = 11;
  const uint64 bit_count1 = 11;
  const uint64 bit_count2 = 10;
  
  uint32 *count0 = push_array(scratch.arena, uint32, (1 << bit_count0));
  uint32 *count1 = push_array(scratch.arena, uint32, (1 << bit_count1));
  uint32 *count2 = push_array(scratch.arena, uint32, (1 << bit_count2));

  for (uint64 i = 0; i < count; ++i) {
    uint32 digit0 = (arr[i].v0 >> 0         ) % (1 << bit_count0);
    uint32 digit1 = (arr[i].v0 >> bit_count0) % (1 << bit_count1);
    uint32 digit2 = (arr[i].v0 >> bit_count1) % (1 << bit_count2);

    ++count0[digit0];
    ++count1[digit1];
    ++count2[digit2];
  }

  counts_to_offsets_array_u32((1 << bit_count0), count0);
  counts_to_offsets_array_u32((1 << bit_count1), count1);
  counts_to_offsets_array_u32((1 << bit_count2), count2);

  for (uint64 i = 0; i < count; ++i) {
    uint32 digit0 = (arr[i].v0 >> 0) % (1 << bit_count0);
    temp[count0[digit0]++] = arr[i];
  }

  for (uint64 i = 0; i < count; ++i) {
    uint32 digit1 = (temp[i].v0 >> bit_count0) % (1 << bit_count1);
    arr[count1[digit1]++] = temp[i];
  }

  for (uint64 i = 0; i < count; ++i) {
    uint32 digit2 = (arr[i].v0 >> bit_count1) % (1 << bit_count2);
    temp[count2[digit2]++] = arr[i];
  }

  MemoryCopyTyped(arr, temp, count);

  scratch_end(scratch);
}

internal B32
u32_array_compare(uint32Array a, uint32Array b)
{
  B32 are_equal = 0;
  if (a.count == b.count) {
    int cmp = MemoryCompare(a.v, b.v, sizeof(a.v[0]) * a.count);
    are_equal = (cmp == 0);
  }
  return are_equal;
}

internal uint64Array
u64_array_remove_duplicates(Arena *arena, uint64Array in)
{
  uint64Array result;
  result.count = 0;
  result.v = push_array(arena, uint64, in.count);
  
  for (uint64 i = 1; i < in.count; ++i) {
    B32 is_unique = in.v[i - 1] != in.v[i];
    if (is_unique) {
      result.v[result.count++] = in.v[i - 1];
    }
  }
  
  if (in.count > 0 && result.count > 0) {
    B32 is_unique = result.v[result.count - 1] != in.v[in.count - 1];
    if (is_unique) {
      result.v[result.count++] = in.v[in.count - 1];
    }
  }
  
  uint64 slack_size = (in.count - result.count) * sizeof(result.v[0]);
  arena_pop(arena, slack_size);
  
  return result;
}

internal uint64
sum_array_u64(uint64 count, uint64 *v)
{
  uint64 result = 0;
  for (uint64 i = 0; i < count; i += 1) {
    result += v[i];
  }
  return result;
}

internal uint64
sum_matrix_u64(uint64 rows, uint64 cols, uint64 **v)
{
  uint64 result = 0;
  for (uint64 i = 0; i < rows; ++i) {
    result += sum_array_u64(cols, v[i]);
  }
  return result;
}

internal uint64
max_array_u64(uint64 count, uint64 *v)
{
  uint64 result = 0;
  for (uint64 i = 0; i < count; i += 1) {
    result = Max(v[i], result);
  }
  return result;
}

internal uint64
min_array_u64(uint64 count, uint64 *v)
{
  uint64 result = max_uint64;
  for (uint64 i = 0; i < count; i += 1) {
    result = Min(v[i], result);
  }
  return result;
}

internal void
counts_to_offsets_array_u32(uint64 count, uint32 *arr)
{
  uint32 next_offset = 0;
  for (uint64 i = 0; i < count; i += 1) {
    uint32 current_offset = next_offset;
    next_offset += arr[i];
    arr[i] = current_offset;
  }
}

internal void
counts_to_offsets_array_u64(uint64 count, uint64 *arr)
{
  uint64 next_offset = 0;
  for (uint64 i = 0; i < count; i += 1) {
    uint64 current_offset = next_offset;
    next_offset += arr[i];
    arr[i] = current_offset;
  }
}

internal uint32 *
offsets_from_counts_array_u32(Arena *arena, uint32 *v, uint64 count)
{
  uint32 *result = push_array_copy_u32(arena, v, count);
  counts_to_offsets_array_u32(count, result);
  return result;
}

internal uint64 *
offsets_from_counts_array_u64(Arena *arena, uint64 *v, uint64 count)
{
  uint64 *result = push_array_copy_u64(arena, v, count);
  counts_to_offsets_array_u64(count, result);
  return result;
}


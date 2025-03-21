// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

ulong
void_list_count_nodes(VoidNode *head)
{
  ulong node_count = 0;
  for (VoidNode *curr = head; curr != 0; curr = curr->next) {
    ++node_count;
  }
  return node_count;
}

void
void_node_concat(VoidNode **head, VoidNode *node)
{
  Assert(*head != node);
  node->next = *head;
  *head = node;
}

void
void_node_concat_atomic(VoidNode **head, VoidNode *node)
{
  Assert(*head != node);
  node->next = ins_atomic_ptr_eval_assign(head, node);
}

U64Node *
u64_list_push(Arena *arena, U64List *list, ulong data)
{
  U64Node *n = push_array(arena, U64Node, 1);
  n->next = 0;
  n->data = data;
  
  SLLQueuePush(list->first, list->last, n);
  ++list->count;
  
  return n;
}

void
u64_list_concat_in_place(U64List *list, U64List *to_concat)
{
  SLLConcatInPlace(list, to_concat);
}

U64Array
u64_array_from_list(Arena *arena, U64List *list)
{
  U64Array result;
  result.count = 0;
  result.v = push_array(arena, ulong, list->count);
  for (U64Node *n = list->first; n != NULL; n = n->next) {
    result.v[result.count++] = n->data;
  }
  return result;
}

void
u32_array_sort(ulong count, uint *v)
{
  radsort(v, count, u32_is_before);
}

void
u64_array_sort(ulong count, ulong *v)
{
  radsort(v, count, u64_is_before);
}

void
u32_pair_radix_sort(ulong count, PairU32 *arr)
{
  Temp scratch = scratch_begin(0,0);

  PairU32 *temp = push_array(scratch.arena, PairU32, count);

  const ulong bit_count0 = 11;
  const ulong bit_count1 = 11;
  const ulong bit_count2 = 10;
  
  uint *count0 = push_array(scratch.arena, uint, (1 << bit_count0));
  uint *count1 = push_array(scratch.arena, uint, (1 << bit_count1));
  uint *count2 = push_array(scratch.arena, uint, (1 << bit_count2));

  for (ulong i = 0; i < count; ++i) {
    uint digit0 = (arr[i].v0 >> 0         ) % (1 << bit_count0);
    uint digit1 = (arr[i].v0 >> bit_count0) % (1 << bit_count1);
    uint digit2 = (arr[i].v0 >> bit_count1) % (1 << bit_count2);

    ++count0[digit0];
    ++count1[digit1];
    ++count2[digit2];
  }

  counts_to_offsets_array_u32((1 << bit_count0), count0);
  counts_to_offsets_array_u32((1 << bit_count1), count1);
  counts_to_offsets_array_u32((1 << bit_count2), count2);

  for (ulong i = 0; i < count; ++i) {
    uint digit0 = (arr[i].v0 >> 0) % (1 << bit_count0);
    temp[count0[digit0]++] = arr[i];
  }

  for (ulong i = 0; i < count; ++i) {
    uint digit1 = (temp[i].v0 >> bit_count0) % (1 << bit_count1);
    arr[count1[digit1]++] = temp[i];
  }

  for (ulong i = 0; i < count; ++i) {
    uint digit2 = (arr[i].v0 >> bit_count1) % (1 << bit_count2);
    temp[count2[digit2]++] = arr[i];
  }

  MemoryCopyTyped(arr, temp, count);

  scratch_end(scratch);
}

B32
u32_array_compare(U32Array a, U32Array b)
{
  B32 are_equal = 0;
  if (a.count == b.count) {
    int cmp = MemoryCompare(a.v, b.v, sizeof(a.v[0]) * a.count);
    are_equal = (cmp == 0);
  }
  return are_equal;
}

U64Array
u64_array_remove_duplicates(Arena *arena, U64Array in)
{
  U64Array result;
  result.count = 0;
  result.v = push_array(arena, ulong, in.count);
  
  for (ulong i = 1; i < in.count; ++i) {
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
  
  ulong slack_size = (in.count - result.count) * sizeof(result.v[0]);
  arena_pop(arena, slack_size);
  
  return result;
}

ulong
sum_array_u64(ulong count, ulong *v)
{
  ulong result = 0;
  for (ulong i = 0; i < count; i += 1) {
    result += v[i];
  }
  return result;
}

ulong
sum_matrix_u64(ulong rows, ulong cols, ulong **v)
{
  ulong result = 0;
  for (ulong i = 0; i < rows; ++i) {
    result += sum_array_u64(cols, v[i]);
  }
  return result;
}

ulong
max_array_u64(ulong count, ulong *v)
{
  ulong result = 0;
  for (ulong i = 0; i < count; i += 1) {
    result = Max(v[i], result);
  }
  return result;
}

ulong
min_array_u64(ulong count, ulong *v)
{
  ulong result = max_U64;
  for (ulong i = 0; i < count; i += 1) {
    result = Min(v[i], result);
  }
  return result;
}

void
counts_to_offsets_array_u32(ulong count, uint *arr)
{
  uint next_offset = 0;
  for (ulong i = 0; i < count; i += 1) {
    uint current_offset = next_offset;
    next_offset += arr[i];
    arr[i] = current_offset;
  }
}

void
counts_to_offsets_array_u64(ulong count, ulong *arr)
{
  ulong next_offset = 0;
  for (ulong i = 0; i < count; i += 1) {
    ulong current_offset = next_offset;
    next_offset += arr[i];
    arr[i] = current_offset;
  }
}

uint *
offsets_from_counts_array_u32(Arena *arena, uint *v, ulong count)
{
  uint *result = push_array_copy_u32(arena, v, count);
  counts_to_offsets_array_u32(count, result);
  return result;
}

ulong *
offsets_from_counts_array_u64(Arena *arena, ulong *v, ulong count)
{
  ulong *result = push_array_copy_u64(arena, v, count);
  counts_to_offsets_array_u64(count, result);
  return result;
}


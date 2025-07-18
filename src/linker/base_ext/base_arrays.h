// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct uint64Node
{
  struct uint64Node *next;
  uint64             data;
} uint64Node;

typedef struct uint64List
{
  uint64      count;
  uint64Node *first;
  uint64Node *last;
} uint64List;

typedef struct VoidNode
{
  struct VoidNode *next;
  void            *v;
} VoidNode;

////////////////////////////////

internal uint64Node * u64_list_push(Arena *arena, uint64List *list, uint64 data);
internal void      u64_list_concat_in_place(uint64List *list, uint64List *to_concat);
internal uint64Array  u64_array_from_list(Arena *arena, uint64List *list);

internal uint64Array u64_array_remove_duplicates(Arena *arena, uint64Array in);

internal void u32_array_sort(uint64 count, uint32 *v);
internal void u64_array_sort(uint64 count, uint64 *v);
internal B32  u32_array_compare(uint32Array a, uint32Array b);

internal uint64 sum_array_u64(uint64 count, uint64 *v);
internal uint64 max_array_u64(uint64 count, uint64 *v);
internal uint64 min_array_u64(uint64 count, uint64 *v);

internal void  counts_to_offsets_array_u32(uint64 count, uint32 *arr);
internal void  counts_to_offsets_array_u64(uint64 count, uint64 *arr);

internal uint32 * offsets_from_counts_array_u32(Arena *arena, uint32 *v, uint64 count);
internal uint64 * offsets_from_counts_array_u64(Arena *arena, uint64 *v, uint64 count);


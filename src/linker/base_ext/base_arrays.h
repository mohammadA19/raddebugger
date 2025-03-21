// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct U64Node
{
  struct U64Node *next;
  ulong             data;
} U64Node;

typedef struct U64List
{
  ulong      count;
  U64Node *first;
  U64Node *last;
} U64List;

typedef struct VoidNode
{
  struct VoidNode *next;
  void            *v;
} VoidNode;

////////////////////////////////

internal U64Node * u64_list_push(Arena *arena, U64List *list, ulong data);
internal void      u64_list_concat_in_place(U64List *list, U64List *to_concat);
internal U64Array  u64_array_from_list(Arena *arena, U64List *list);

internal U64Array u64_array_remove_duplicates(Arena *arena, U64Array in);

internal void u32_array_sort(ulong count, uint *v);
internal void u64_array_sort(ulong count, ulong *v);
internal B32  u32_array_compare(U32Array a, U32Array b);

internal ulong sum_array_u64(ulong count, ulong *v);
internal ulong max_array_u64(ulong count, ulong *v);
internal ulong min_array_u64(ulong count, ulong *v);

internal void  counts_to_offsets_array_u32(ulong count, uint *arr);
internal void  counts_to_offsets_array_u64(ulong count, ulong *arr);

internal uint * offsets_from_counts_array_u32(Arena *arena, uint *v, ulong count);
internal ulong * offsets_from_counts_array_u64(Arena *arena, ulong *v, ulong count);


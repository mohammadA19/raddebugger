// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct U64Node
{
  struct U64Node *next;
  U64             data;
}

struct U64List
{
  U64      count;
  U64Node *first;
  U64Node *last;
}

struct VoidNode
{
  struct VoidNode *next;
  void            *v;
}

////////////////////////////////

U64Node * u64_list_push(Arena *arena, U64List *list, U64 data);
void      u64_list_concat_in_place(U64List *list, U64List *to_concat);
U64Array  u64_array_from_list(Arena *arena, U64List *list);

U64Array u64_array_remove_duplicates(Arena *arena, U64Array in);

void u32_array_sort(U64 count, U32 *v);
void u64_array_sort(U64 count, U64 *v);
B32  u32_array_compare(U32Array a, U32Array b);

U64 sum_array_u64(U64 count, U64 *v);
U64 max_array_u64(U64 count, U64 *v);
U64 min_array_u64(U64 count, U64 *v);

void  counts_to_offsets_array_u32(U64 count, U32 *arr);
void  counts_to_offsets_array_u64(U64 count, U64 *arr);

U32 * offsets_from_counts_array_u32(Arena *arena, U32 *v, U64 count);
U64 * offsets_from_counts_array_u64(Arena *arena, U64 *v, U64 count);


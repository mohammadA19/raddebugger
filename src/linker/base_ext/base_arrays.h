// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

srtuct U64Node
{
  struct U64Node *next;
  ulong             data;
};

srtuct U64List
{
  ulong      count;
  U64Node *first;
  U64Node *last;
};

srtuct VoidNode
{
  struct VoidNode *next;
  void            *v;
};

////////////////////////////////

U64Node * u64_list_push(Arena *arena, U64List *list, ulong data);
void      u64_list_concat_in_place(U64List *list, U64List *to_concat);
U64Array  u64_array_from_list(Arena *arena, U64List *list);

U64Array u64_array_remove_duplicates(Arena *arena, U64Array in);

void u32_array_sort(ulong count, uint *v);
void u64_array_sort(ulong count, ulong *v);
B32  u32_array_compare(U32Array a, U32Array b);

ulong sum_array_u64(ulong count, ulong *v);
ulong max_array_u64(ulong count, ulong *v);
ulong min_array_u64(ulong count, ulong *v);

void  counts_to_offsets_array_u32(ulong count, uint *arr);
void  counts_to_offsets_array_u64(ulong count, ulong *arr);

uint * offsets_from_counts_array_u32(Arena *arena, uint *v, ulong count);
ulong * offsets_from_counts_array_u64(Arena *arena, ulong *v, ulong count);


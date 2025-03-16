// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct U64Node
{
  struct U64Node* next;
  uint64             data;
}

struct U64List
{
  uint64      count;
  U64Node* first;
  U64Node* last;
}

struct VoidNode
{
  struct VoidNode* next;
  void*            v;
}

////////////////////////////////

U64Node * u64_list_push(Arena* arena, U64List* list, uint64 data);
void      u64_list_concat_in_place(U64List* list, U64List* to_concat);
U64Array  u64_array_from_list(Arena* arena, U64List* list);

U64Array u64_array_remove_duplicates(Arena* arena, U64Array in);

void u32_array_sort(uint64 count, uint32* v);
void u64_array_sort(uint64 count, uint64* v);
B32  u32_array_compare(U32Array a, U32Array b);

uint64 sum_array_u64(uint64 count, uint64* v);
uint64 max_array_u64(uint64 count, uint64* v);
uint64 min_array_u64(uint64 count, uint64* v);

void  counts_to_offsets_array_u32(uint64 count, uint32* arr);
void  counts_to_offsets_array_u64(uint64 count, uint64* arr);

uint32 * offsets_from_counts_array_u32(Arena* arena, uint32* v, uint64 count);
uint64 * offsets_from_counts_array_u64(Arena* arena, uint64* v, uint64 count);


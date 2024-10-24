// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define MemoryCopyStr8(dst, s) MemoryCopy(dst, (s).str, (s).size)

internal String8 str8_cstring_capped_reverse(void *start, void *cap);

internal U64 str8_find_needle_reverse(String8 string, U64 start_pos, String8 needle, StringMatchFlags flags);

internal int str8_compar(String8 a, String8 b, B32 ignore_case);
internal int str8_compar_ignore_case(const void *a, const void *b);
internal int str8_compar_case_sensetive(const void *a, const void *b);

#define str8_list_push_struct(a,l,d) str8_list_push_raw(a, l, d, sizeof(*d))
internal String8Node * str8_list_push_raw(Arena *arena, String8List *list, void *data_ptr, U64 data_size);
internal U64           str8_list_push_pad(Arena *arena, String8List *list, U64 offset, U64 align);
internal U64           str8_list_push_pad_front(Arena *arena, String8List *list, U64 offset, U64 align);
internal String8List   str8_list_arr_concat(String8List *v, U64 count);
internal String8Node * str8_list_push_many(Arena *arena, String8List *list, U64 count);

internal String8 str8_from_bits_u32(Arena *arena, U32 x);
internal String8 str8_from_bits_u64(Arena *arena, U64 x);

// TODO: remove
internal String8Node * str8_list_pop_front(String8List *list);

internal String8 str8_from_memory_size2(Arena *arena, U64 size);
internal String8 str8_from_count(Arena *arena, U64 count);

internal U64 hash_from_str8(String8 string);


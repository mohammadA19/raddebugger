// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define MemoryCopyStr8(dst, s) MemoryCopy(dst, (s).str, (s).size)

internal int str8_compar(String8 a, String8 b, B32 ignore_case);
internal int str8_compar_ignore_case(const void *a, const void *b);
internal int str8_compar_case_sensitive(const void *a, const void *b);

#define str8_list_push_struct(a,l,d) str8_list_push_raw(a, l, d, sizeof(*d))
internal String8Node * str8_list_push_raw(Arena *arena, String8List *list, void *data_ptr, ulong data_size);
internal ulong           str8_list_push_pad(Arena *arena, String8List *list, ulong offset, ulong align);
internal ulong           str8_list_push_pad_front(Arena *arena, String8List *list, ulong offset, ulong align);
internal String8List   str8_list_arr_concat(String8List *v, ulong count);
internal String8Node * str8_list_push_many(Arena *arena, String8List *list, ulong count);

// TODO: remove
internal String8Node * str8_list_pop_front(String8List *list);

internal ulong hash_from_str8(String8 string);


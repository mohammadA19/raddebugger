// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define str8_list_push_struct(a,l,d) str8_list_push_raw(a, l, d, sizeof(*d))
internal String8Node * str8_list_push_raw(Arena *arena, String8List *list, void *data_ptr, uint64 data_size);
internal uint64           str8_list_push_pad(Arena *arena, String8List *list, uint64 offset, uint64 align);
internal uint64           str8_list_push_pad_front(Arena *arena, String8List *list, uint64 offset, uint64 align);
internal String8List   str8_list_arr_concat(String8List *v, uint64 count);
internal String8Node * str8_list_push_many(Arena *arena, String8List *list, uint64 count);

// TODO: remove
internal String8Node * str8_list_pop_front(String8List *list);

internal uint64 hash_from_str8(String8 string);


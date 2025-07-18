// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

internal uint32 *   push_u32(Arena *arena, uint32 value);
internal uint64 *   push_u64(Arena *arena, uint64 value);
internal uint32 *   push_array_copy_u32(Arena *arena, uint32 *v, uint64 count);
internal uint64 *   push_array_copy_u64(Arena *arena, uint64 *v, uint64 count);
internal uint64 **  push_matrix_u64(Arena *arena, uint64 rows, uint64 columns);
internal String8 push_cstr(Arena *arena, String8 str);

internal Arena ** alloc_fixed_size_arena_array(Arena *arena, uint64 count, uint64 res, uint64 cmt);
internal void     release_arena_array(Arena **arr);


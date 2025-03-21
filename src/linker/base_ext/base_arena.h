// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

internal uint *   push_u32(Arena *arena, uint value);
internal ulong *   push_u64(Arena *arena, ulong value);
internal uint *   push_array_copy_u32(Arena *arena, uint *v, ulong count);
internal ulong *   push_array_copy_u64(Arena *arena, ulong *v, ulong count);
internal ulong **  push_matrix_u64(Arena *arena, ulong rows, ulong columns);
internal String8 push_cstr(Arena *arena, String8 str);

internal Arena ** alloc_fixed_size_arena_array(Arena *arena, ulong count, ulong res, ulong cmt);
internal void     release_arena_array(Arena **arr);


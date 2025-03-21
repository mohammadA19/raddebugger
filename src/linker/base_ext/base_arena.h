// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

uint *   push_u32(Arena *arena, uint value);
ulong *   push_u64(Arena *arena, ulong value);
uint *   push_array_copy_u32(Arena *arena, uint *v, ulong count);
ulong *   push_array_copy_u64(Arena *arena, ulong *v, ulong count);
ulong **  push_matrix_u64(Arena *arena, ulong rows, ulong columns);
String8 push_cstr(Arena *arena, String8 str);

Arena ** alloc_fixed_size_arena_array(Arena *arena, ulong count, ulong res, ulong cmt);
void     release_arena_array(Arena **arr);


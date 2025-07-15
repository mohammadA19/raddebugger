// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

static U32 *   push_u32(Arena *arena, U32 value);
static U64 *   push_u64(Arena *arena, U64 value);
static U32 *   push_array_copy_u32(Arena *arena, U32 *v, U64 count);
static U64 *   push_array_copy_u64(Arena *arena, U64 *v, U64 count);
static U64 **  push_matrix_u64(Arena *arena, U64 rows, U64 columns);
static String8 push_cstr(Arena *arena, String8 str);

static Arena ** alloc_fixed_size_arena_array(Arena *arena, U64 count, U64 res, U64 cmt);
static void     release_arena_array(Arena **arr);


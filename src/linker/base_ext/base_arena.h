// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

U32 *   push_u32(Arena* arena, U32 value);
U64 *   push_u64(Arena* arena, U64 value);
U32 *   push_array_copy_u32(Arena* arena, U32* v, U64 count);
U64 *   push_array_copy_u64(Arena* arena, U64* v, U64 count);
U64 **  push_matrix_u64(Arena* arena, U64 rows, U64 columns);
String8 push_cstr(Arena* arena, String8 str);

Arena ** alloc_fixed_size_arena_array(Arena* arena, U64 count, U64 res, U64 cmt);
void     release_arena_array(Arena** arr);


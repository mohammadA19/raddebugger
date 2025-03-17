// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

uint32 *   push_u32(Arena* arena, uint32 value);
uint64 *   push_u64(Arena* arena, uint64 value);
uint32 *   push_array_copy_u32(Arena* arena, uint32* v, uint64 count);
uint64 *   push_array_copy_u64(Arena* arena, uint64* v, uint64 count);
uint64 **  push_matrix_u64(Arena* arena, uint64 rows, uint64 columns);
StringView push_cstr(Arena* arena, StringView str);

Arena ** alloc_fixed_size_arena_array(Arena* arena, uint64 count, uint64 res, uint64 cmt);
void     release_arena_array(Arena** arr);


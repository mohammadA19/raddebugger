// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

U32Array  bit_array_init32(Arena* arena, uint64 word_count);
uint64       bit_array_scan_left_to_right32(U32Array bit_array, uint64 lo, uint64 hi, B32 state);
uint64       bit_array_scan_right_to_left32(U32Array bit_array, uint64 lo, uint64 hi, B32 state);
Rng1U64   bit_array_scan_left_to_right32_contiguous(U32Array bit_array, uint64 lo, uint64 hi, B32 state, uint64 in_row_count);
Rng1U64   bit_array_scan_right_to_left32_contiguous(U32Array bit_array, uint64 lo, uint64 hi, B32 state, uint64 in_row_count);
B32       byte_scan_right_to_left(uint8* start, uint8* opl, uint8 byte, uint64* offset_out);
uint64       bit_array_find_next_unset_bit32(U32Array bit_array);
uint64       bit_array_find_next_set_bit32(U32Array bit_array);
void      bit_array_set_bit32(U32Array bit_array, uint64 idx, B32 state);
void      bit_array_set_bit_range32(U32Array bit_array, Rng1U64 range, B32 state);
uint32       bit_array_get_bit32(U32Array bit_array, uint64 idx);



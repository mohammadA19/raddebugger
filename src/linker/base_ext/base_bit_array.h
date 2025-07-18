// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

internal Sapn<uint32>  bit_array_init32(Arena *arena, uint64 word_count);
internal uint64       bit_array_scan_left_to_right32(Sapn<uint32> bit_array, uint64 lo, uint64 hi, B32 state);
internal uint64       bit_array_scan_right_to_left32(Sapn<uint32> bit_array, uint64 lo, uint64 hi, B32 state);
internal Rng1uint64   bit_array_scan_left_to_right32_contiguous(Sapn<uint32> bit_array, uint64 lo, uint64 hi, B32 state, uint64 in_row_count);
internal Rng1uint64   bit_array_scan_right_to_left32_contiguous(Sapn<uint32> bit_array, uint64 lo, uint64 hi, B32 state, uint64 in_row_count);
internal B32       byte_scan_right_to_left(uint8 *start, uint8 *opl, uint8 byte, uint64 *offset_out);
internal uint64       bit_array_find_next_unset_bit32(Sapn<uint32> bit_array);
internal uint64       bit_array_find_next_set_bit32(Sapn<uint32> bit_array);
internal void      bit_array_set_bit32(Sapn<uint32> bit_array, uint64 idx, B32 state);
internal void      bit_array_set_bit_range32(Sapn<uint32> bit_array, Rng1uint64 range, B32 state);
internal uint32       bit_array_get_bit32(Sapn<uint32> bit_array, uint64 idx);



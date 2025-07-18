// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal uint32Array
bit_array_init32(Arena *arena, uint64 word_count)
{
  uint32Array result;
  result.count = CeilIntegerDiv(word_count, 32);
  result.v = push_array(arena, uint32, word_count);
  return result;
}

internal uint64
bit_array_scan_left_to_right32(uint32Array bit_array, uint64 lo, uint64 hi, B32 state)
{
  Assert(lo < bit_array.count*32);
  Assert(hi <= bit_array.count*32);
  Assert(lo <= hi);
  Assert(state == 0 || state == 1);
  
  uint64 word_lo = lo / 32;
  uint64 word_hi = CeilIntegerDiv(hi, 32) - 1;
  
  uint64 word_idx = word_lo;
  uint64 bit_idx = 0;
  
  uint64 scan_count = hi - lo;
  if (scan_count < 32) {
    uint64 bit_lo = lo % 32;
    uint64 bit_hi = hi % 32;
    uint64 word = bit_array.v[word_idx];
    word ^= state - 1;
    word &= (1U << bit_hi) - (1U << bit_lo);
    if (word) {
      bit_idx = ctz32(word);
      goto exit;
    }
  } else {
    uint32 first_word = bit_array.v[word_idx];
    first_word ^= state - 1;
    first_word &= ~0u << (lo % 32);
    if (first_word) {
      bit_idx = ctz32(first_word);
      goto exit;
    }
    
    for (word_idx += 1; word_idx < word_hi; word_idx += 1) {
      uint32 word = bit_array.v[word_idx];
      word ^= state - 1;
      if (word != 0) {
        bit_idx = ctz32(word);
        goto exit;
      }
    }
    
    uint64 bit_hi = hi - (word_idx * 32);
    uint32 last_word = bit_array.v[word_idx];
    last_word ^= state - 1;
    last_word &= (1 << bit_hi) - 1;
    if (last_word) {
      bit_idx = ctz32(last_word);
      goto exit;
    }
  }
  
  word_idx = 0;
  bit_idx = max_uint32;
  
  exit:;
  
  uint64 result = word_idx * 32 + bit_idx;
  return result;
}

internal uint64
bit_array_scan_right_to_left32(uint32Array bit_array, uint64 lo, uint64 hi, B32 state)
{
  Assert(lo <= hi);
  Assert(state == 0 || state == 1);
  
  uint64 word_lo = lo / 32;
  uint64 word_hi = CeilIntegerDiv(hi, 32) - 1;
  
  uint64 word_idx = word_hi;
  uint64 bit_idx = -1;
  
  uint64 scan_count = hi - lo;
  if (scan_count < 32) {
    uint64 bit_lo = lo % 32;
    uint64 bit_hi = bit_lo + scan_count;
    uint32 word = bit_array.v[word_idx];
    for (bit_idx = bit_hi; bit_idx >= bit_lo; bit_idx -= 1) {
      uint32 bit = ExtractBit(word, bit_idx);
      if (bit == state) {
        goto exit;
      }
    }
  } else {
    uint32 last_word = bit_array.v[word_idx];
    uint64 bit_hi = hi % 32;
    for (bit_idx = bit_hi; bit_idx >= 0; bit_idx -= 1) {
      uint32 bit = ExtractBit(last_word, bit_idx);
      if (bit == state) {
        goto exit;
      }
    }
    
    for (word_idx -= 1; word_idx > word_lo; word_idx -= 1) {
      uint32 word = bit_array.v[word_idx];
      for (bit_idx = 32 - 1; bit_idx >= 0; bit_idx -= 1) {
        uint32 bit = ExtractBit(word, bit_idx);
        if (bit == state) {
          goto exit;
        }
      }
    }
    
    uint32 first_word = bit_array.v[word_idx];
    uint64 bit_lo = lo % 32;
    for (bit_idx = 32 - 1; bit_idx >= bit_lo; bit_idx -= 1) {
      uint32 bit = ExtractBit(first_word, bit_idx);
      if (bit == state) {
        goto exit;
      }
    }
  }
  
  word_idx = 0;
  bit_idx = max_uint32;
  
  exit:;
  
  uint64 result_s64 = word_idx * 32 + bit_idx;
  uint64 result_u64 = (uint64)result_s64;
  return result_u64;
}

internal Rng1uint64
bit_array_scan_left_to_right32_contiguous(uint32Array bit_array, uint64 lo, uint64 hi, B32 state, uint64 in_row_count)
{
  Rng1uint64 result = rng_1u64(max_uint64, max_uint64);
  
  uint64 curr_count = 0, rover = lo;
  while (curr_count < in_row_count) {
    rover = bit_array_scan_left_to_right32(bit_array, rover, hi, state);
    
    // no more bits in range
    if (rover >= hi) {
      break;
    }
    
    // set first match
    if (result.v[0] == max_uint64) {
      result = rng_1u64(rover, rover);
      continue;
    }
    
    // reset on non-contiguous range
    B32 is_bit_index_not_adjoined = (result.v[0] + 1 < rover);
    if (is_bit_index_not_adjoined) {
      curr_count = 0;
      result = rng_1u64(max_uint64, max_uint64);
      continue;
    }
    
    // advance
    result.v[1] = rover;
    curr_count -= 1;
  }
  
  // did we allocate enough bits?
  if (curr_count != in_row_count) {
    result = rng_1u64(max_uint64, max_uint64);
  }
  
  return result;
}

internal Rng1uint64
bit_array_scan_right_to_left32_contiguous(uint32Array bit_array, uint64 lo, uint64 hi, B32 state, uint64 in_row_count)
{
  Rng1uint64 result = rng_1u64(max_uint64, max_uint64);
  
  uint64 curr_count = 0, rover = lo;
  while (curr_count < in_row_count) {
    rover = bit_array_scan_right_to_left32(bit_array, lo, rover, state);
    
    // no more bits in range
    if (rover >= hi) {
      break;
    }
    
    // set first match
    if (result.v[0] == max_uint64) {
      result = rng_1u64(rover, rover);
      continue;
    }
    
    // reset on non-contiguous range
    B32 is_bit_index_not_adjoined = (result.v[0] + 1 < rover);
    if (is_bit_index_not_adjoined) {
      curr_count = 0;
      result = rng_1u64(max_uint64, max_uint64);
      continue;
    }
    
    // advance
    result.v[0] = rover;
    curr_count -= 1;
  }
  
  // did we allocate enough bits?
  if (curr_count != in_row_count) {
    result = rng_1u64(max_uint64, max_uint64);
  }
  
  return result;
}

internal uint64
bit_array_find_next_unset_bit32(uint32Array bit_array)
{
  uint64 result = bit_array_scan_left_to_right32(bit_array, 0, bit_array.count*32, 0);
  return result;
}

internal uint64
bit_array_find_next_set_bit32(uint32Array bit_array)
{
  uint64 result = bit_array_scan_left_to_right32(bit_array, 0, bit_array.count*32, 1);
  return result;
}

internal void
bit_array_set_bit32(uint32Array bit_array, uint64 idx, B32 state)
{
  Assert(idx < bit_array.count*32);
  uint64 word_idx = idx / 32;
  uint64 bit_idx = idx % 32;
  if (state) {
    bit_array.v[word_idx] |= (1 << bit_idx);
  } else {
    bit_array.v[word_idx] &= ~(1 << bit_idx);
  }
}

internal void
bit_array_set_bit_range32(uint32Array bit_array, Rng1uint64 range, B32 state)
{
  for (uint64 idx = range.min ; idx < range.max; idx += 1) {
    bit_array_set_bit32(bit_array, idx, state);
  }
}

internal uint32
bit_array_get_bit32(uint32Array bit_array, uint64 idx)
{
  Assert(idx < bit_array.count*32);
  uint64 word_idx = idx / 32;
  uint64 bit_idx = idx % 32;
  uint32 bit = (bit_array.v[word_idx] & (1 << bit_idx)) >> bit_idx;
  return bit;
}

internal B32
bit_array_is_bit_set(uint32Array bit_arr, uint64 bit_pos)
{
  uint64 word_idx = bit_pos / 32;
  Assert(word_idx < bit_arr.count);
  uint32 word = bit_arr.v[word_idx];
  uint64 bit_idx = bit_pos % 32;
  B32 is_set = !!(word & (1 << bit_idx));
  return is_set;
}



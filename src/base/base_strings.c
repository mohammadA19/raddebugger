// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Third Party Includes

#if !BUILD_SUPPLEMENTARY_UNIT
# define STB_SPRINTF_IMPLEMENTATION
# define STB_SPRINTF_STATIC
# include "third_party/stb/stb_sprintf.h"
#endif

////////////////////////////////
//~ NOTE(allen): String <. Integer Tables

read_only static uint8 integer_symbols[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

// NOTE(allen): Includes reverses for uppercase and lowercase hex.
read_only static uint8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

read_only static uint8 base64[64] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '_', '$',
};

read_only static uint8 base64_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
  0xFF,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,
  0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0xFF,0x3E,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
  0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0xFF,0xFF,0xFF,0xFF,0xFF,
};

////////////////////////////////
//~ rjf: Character Classification & Conversion Functions

B32
char_is_space(uint8 c){
  return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

B32
char_is_slash(uint8 c){
  return(c == '/' || c == '\\');
}

B32
char_is_digit(uint8 c, uint32 base){
  B32 result = 0;
  if (0 < base && base <= 16){
    uint8 val = integer_symbol_reverse[c];
    if (val < base){
      result = 1;
    }
  }
  return(result);
}

uint8
char_to_upper(uint8 c){
  if (char_is_lower(c)){
    c += ('A' - 'a');
  }
  return(c);
}

uint8
char_to_correct_slash(uint8 c){
  if(char_is_slash(c)){
    c = '/';
  }
  return(c);
}

////////////////////////////////
//~ rjf: C-String Measurement

uint64
cstring8_length(uint8* c){
  uint8* p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

uint64
cstring16_length(uint16* c){
  uint16* p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

uint64
cstring32_length(uint32* c){
  uint32* p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

////////////////////////////////
//~ rjf: String Constructors

StringView
str8_range(uint8* first, uint8* one_past_last){
  StringView result = {first, (uint64)(one_past_last - first)};
  return(result);
}

Span<char16>
str16_range(uint16* first, uint16* one_past_last){
  Span<char16> result = {first, (uint64)(one_past_last - first)};
  return(result);
}

Span<char32>
str32_range(uint32* first, uint32* one_past_last){
  Span<char32> result = {first, (uint64)(one_past_last - first)};
  return(result);
}

StringView
str8_cstring(char* c){
  StringView result = {(uint8*)c, cstring8_length((uint8*)c)};
  return(result);
}

Span<char16>
str16_cstring(uint16* c){
  Span<char16> result = {(uint16*)c, cstring16_length((uint16*)c)};
  return(result);
}

Span<char32>
str32_cstring(uint32* c){
  Span<char32> result = {(uint32*)c, cstring32_length((uint32*)c)};
  return(result);
}

StringView
str8_cstring_capped(void* cstr, void* cap)
{
  char* ptr = (char *)cstr;
  char* opl = (char *)cap;
  for (;ptr < opl && *ptr != 0; ptr += 1);
  uint64 size = (uint64)(ptr - (char *)cstr);
  StringView result = StringView((uint8*)cstr, size);
  return result;
}

Span<char16>
str16_cstring_capped(void* cstr, void* cap)
{
  uint16* ptr = (uint16 *)cstr;
  uint16* opl = (uint16 *)cap;
  for (;ptr < opl && *ptr != 0; ptr += 1);
  uint64 size = (uint64)(ptr - (uint16 *)cstr);
  Span<char16> result = .(cstr, size);
  return result;
}

StringView
str8_cstring_capped_reverse(void* raw_start, void* raw_cap)
{
  uint8* start = raw_start;
  uint8* ptr   = raw_cap;
  for(; ptr > start; )
  {
    ptr -= 1;

    if (*ptr == '\0')
    {
      break;
    }
  }
  uint64 size = (uint64)(ptr - start);
  StringView result  = StringView(start, size);
  return result;
}

////////////////////////////////
//~ rjf: String Stylization

StringView
upper_from_str8(Arena* arena, StringView str)
{
  str = push_str8_copy(arena, str);
  for(uint64 idx = 0; idx < str.Length; idx += 1)
  {
    str[idx] = char_to_upper(str[idx]);
  }
  return str;
}

StringView
lower_from_str8(Arena* arena, StringView str)
{
  str = push_str8_copy(arena, str);
  for(uint64 idx = 0; idx < str.Length; idx += 1)
  {
    str[idx] = str[idx].ToLower;
  }
  return str;
}

StringView
backslashed_from_str8(Arena* arena, StringView str)
{
  str = push_str8_copy(arena, str);
  for(uint64 idx = 0; idx < str.Length; idx += 1)
  {
    str[idx] = char_is_slash(str[idx]) ? '\\' : str[idx];
  }
  return str;
}

////////////////////////////////
//~ rjf: String Matching

B32
str8_match(StringView a, StringView b, StringMatchFlags flags)
{
  B32 result = 0;
  if(a.size == b.size && flags == 0)
  {
    result = MemoryMatch(a.Ptr, b.Ptr, b.size);
  }
  else if(a.size == b.size || (flags & StringMatchFlag_RightSideSloppy))
  {
    B32 case_insensitive  = (flags & StringMatchFlag_CaseInsensitive);
    B32 slash_insensitive = (flags & StringMatchFlag_SlashInsensitive);
    uint64 size              = Min(a.size, b.size);
    result = 1;
    for(uint64 i = 0; i < size; i += 1)
    {
      uint8 at = a[i];
      uint8 bt = b[i];
      if(case_insensitive)
      {
        at = char_to_upper(at);
        bt = char_to_upper(bt);
      }
      if(slash_insensitive)
      {
        at = char_to_correct_slash(at);
        bt = char_to_correct_slash(bt);
      }
      if(at != bt)
      {
        result = 0;
        break;
      }
    }
  }
  return result;
}

uint64
str8_find_needle(StringView str, uint64 start_pos, StringView needle, StringMatchFlags flags){
  uint8* p = str.Ptr + start_pos;
  uint64 stop_offset = Max(str.Length + 1, needle.size) - needle.size;
  uint8* stop_p = str.Ptr + stop_offset;
  if (needle.size > 0){
    uint8* string_opl = str.Ptr + str.Length;
    StringView needle_tail = str8_skip(needle, 1);
    StringMatchFlags adjusted_flags = flags | StringMatchFlag_RightSideSloppy;
    uint8 needle_first_char_adjusted = needle[0];
    if(adjusted_flags & StringMatchFlag_CaseInsensitive){
      needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
    }
    for (;p < stop_p; p += 1){
      uint8 haystack_char_adjusted = *p;
      if(adjusted_flags & StringMatchFlag_CaseInsensitive){
        haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
      }
      if (haystack_char_adjusted == needle_first_char_adjusted){
        if (str8_match(str8_range(p + 1, string_opl), needle_tail, adjusted_flags)){
          break;
        }
      }
    }
  }
  uint64 result = str.Length;
  if (p < stop_p){
    result = (uint64)(p - str.Ptr);
  }
  return(result);
}

uint64
str8_find_needle_reverse(StringView str, uint64 start_pos, StringView needle, StringMatchFlags flags)
{
  uint64 result = 0;
  for(int64 i = str.Length - start_pos - needle.size; i >= 0; --i)
  {
    StringView haystack = str8_substr(str, rng_1u64(i, i + needle.size));
    if(str8_match(haystack, needle, flags))
    {
      result = (uint64)i + needle.size;
      break;
    }
  }
  return result;
}

B32
str8_ends_with(StringView str, StringView end, StringMatchFlags flags){
  StringView postfix = str8_postfix(str, end.size);
  B32 is_match = str8_match(end, postfix, flags);
  return is_match;
}

////////////////////////////////
//~ rjf: String Slicing

StringView
str8_substr(StringView str, Rng1U64 range){
  range.min = ClampTop(range.min, str.Length);
  range.max = ClampTop(range.max, str.Length);
  str.Ptr += range.min;
  str.Length = dim_1u64(range);
  return(str);
}

StringView
str8_prefix(StringView str, uint64 size){
  str.Length = ClampTop(size, str.Length);
  return(str);
}

StringView
str8_skip(StringView str, uint64 amt){
  amt = ClampTop(amt, str.Length);
  str.Ptr += amt;
  str.Length -= amt;
  return(str);
}

StringView
str8_postfix(StringView str, uint64 size){
  size = ClampTop(size, str.Length);
  str.Ptr = (str.Ptr + str.Length) - size;
  str.Length = size;
  return(str);
}

StringView
str8_chop(StringView str, uint64 amt){
  amt = ClampTop(amt, str.Length);
  str.Length -= amt;
  return(str);
}

StringView
str8_skip_chop_whitespace(StringView str){
  uint8* first = str.Ptr;
  uint8* opl = first + str.Length;
  for (;first < opl; first += 1){
    if (!char_is_space(*first)){
      break;
    }
  }
  for (;opl > first;){
    opl -= 1;
    if (!char_is_space(*opl)){
      opl += 1;
      break;
    }
  }
  StringView result = str8_range(first, opl);
  return(result);
}

////////////////////////////////
//~ rjf: String Formatting & Copying

StringView
push_str8_cat(Arena* arena, StringView s1, StringView s2){
  StringView str;
  str.Length = s1.size + s2.size;
  str.Ptr = push_array_no_zero(arena, uint8, str.Length + 1);
  MemoryCopy(str.Ptr, s1.Ptr, s1.size);
  MemoryCopy(str.Ptr + s1.size, s2.Ptr, s2.size);
  str[str.Length] = 0;
  return(str);
}

StringView
push_str8_copy(Arena* arena, StringView s){
  StringView str;
  str.Length = s.size;
  str.Ptr = push_array_no_zero(arena, uint8, str.Length + 1);
  MemoryCopy(str.Ptr, s.Ptr, s.size);
  str[str.Length] = 0;
  return(str);
}

StringView
push_str8fv(Arena* arena, char* fmt, va_list args){
  va_list args2;
  va_copy(args2, args);
  uint32 needed_bytes = raddbg_vsnprintf(0, 0, fmt, args) + 1;
  StringView result = {0};
  result.Ptr = push_array_no_zero(arena, uint8, needed_bytes);
  result.size = raddbg_vsnprintf((char*)result.Ptr, needed_bytes, fmt, args2);
  result[result.size] = 0;
  va_end(args2);
  return(result);
}

StringView
push_str8f(Arena* arena, char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  StringView result = push_str8fv(arena, fmt, args);
  va_end(args);
  return(result);
}

////////////////////////////////
//~ rjf: String <=> Integer Conversions

//- rjf: str . integer

int64
sign_from_str8(StringView str, StringView* string_tail){
  // count negative signs
  uint64 neg_count = 0;
  uint64 i = 0;
  for (; i < str.Length; i += 1){
    if (str[i] == '-'){
      neg_count += 1;
    }
    else if (str[i] != '+'){
      break;
    }
  }
  
  // output part of str after signs
  *string_tail = str8_skip(str, i);
  
  // output integer sign
  int64 sign = (neg_count & 1)?-1:+1;
  return(sign);
}

B32
str8_is_integer(StringView str, uint32 radix){
  B32 result = 0;
  if (str.Length > 0){
    if (1 < radix && radix <= 16){
      result = 1;
      for (uint64 i = 0; i < str.Length; i += 1){
        uint8 c = str[i];
        if (!(c < 0x80) || integer_symbol_reverse[c] >= radix){
          result = 0;
          break;
        }
      }
    }
  }
  return(result);
}

uint64
u64_from_str8(StringView str, uint32 radix){
  uint64 x = 0;
  if (1 < radix && radix <= 16){
    for (uint64 i = 0; i < str.Length; i += 1){
      x *= radix;
      x += integer_symbol_reverse[str[i]&0x7F];
    }
  }
  return(x);
}

int64
s64_from_str8(StringView str, uint32 radix){
  int64 sign = sign_from_str8(str, &str);
  int64 x = (int64)u64_from_str8(str, radix) * sign;
  return(x);
}

uint32
u32_from_str8(StringView str, uint32 radix)
{
  uint64 x64 = u64_from_str8(str, radix);
  uint32 x32 = safe_cast_u32(x64);
  return x32;
}

int32
s32_from_str8(StringView str, uint32 radix)
{
  int64 x64 = s64_from_str8(str, radix);
  int32 x32 = safe_cast_s32(x64);
  return x32;
}

B32
try_u64_from_str8_c_rules(StringView str, uint64* x){
  B32 is_integer = 0;
  if (str8_is_integer(str, 10)){
    is_integer = 1;
    *x = u64_from_str8(str, 10);
  }
  else{
    StringView hex_string = str8_skip(str, 2);
    if (str8_match(str8_prefix(str, 2), ("0x"), 0) &&
        str8_is_integer(hex_string, 0x10)){
      is_integer = 1;
      *x = u64_from_str8(hex_string, 0x10);
    }
    else if (str8_match(str8_prefix(str, 2), ("0b"), 0) &&
             str8_is_integer(hex_string, 2)){
      is_integer = 1;
      *x = u64_from_str8(hex_string, 2);
    }
    else{
      StringView oct_string = str8_skip(str, 1);
      if (str8_match(str8_prefix(str, 1), ("0"), 0) &&
          str8_is_integer(hex_string, 010)){
        is_integer = 1;
        *x = u64_from_str8(oct_string, 010);
      }
    }
  }
  return(is_integer);
}

B32
try_s64_from_str8_c_rules(StringView str, int64* x){
  StringView string_tail = {0};
  int64 sign = sign_from_str8(str, &string_tail);
  uint64 x_u64 = 0;
  B32 is_integer = try_u64_from_str8_c_rules(string_tail, &x_u64);
  *x = x_u64*sign;
  return(is_integer);
}

//- rjf: integer . str

StringView
str8_from_memory_size(Arena* arena, uint64 size)
{
  StringView result;

  if(size < KB(1))
  {
    result = push_str8f(arena, "%llu Bytes", size);
  }
  else if(size < MB(1))
  {
    result = push_str8f(arena, "%llu.%02llu KiB", size / KB(1), ((size * 100) / KB(1)) % 100);
  }
  else if(size < GB(1))
  {
    result = push_str8f(arena, "%llu.%02llu MiB", size / MB(1), ((size * 100) / MB(1)) % 100);
  }
  else if(size < TB(1))
  {
    result = push_str8f(arena, "%llu.%02llu GiB", size / GB(1), ((size * 100) / GB(1)) % 100);
  }
  else
  {
    result = push_str8f(arena, "%llu.%02llu TiB", size / TB(1), ((size * 100) / TB(1)) % 100);
  }

  return result;
}

StringView
str8_from_count(Arena* arena, uint64 count)
{
  StringView result;

  if(count < 1 * 1000)
  {
    result = push_str8f(arena, "%llu", count);
  }
  else if(count < 1000000)
  {
    uint64 frac = ((count * 100) / 1000) % 100;
    if(frac > 0)
    {
      result = push_str8f(arena, "%llu.%02lluK", count / 1000, frac);
    }
    else
    {
      result = push_str8f(arena, "%lluK", count / 1000);
    }
  }
  else if(count < 1000000000)
  {
    uint64 frac = ((count * 100) / 1000000) % 100;
    if(frac > 0)
    {
      result = push_str8f(arena, "%llu.%02lluM", count / 1000000, frac);
    }
    else
    {
      result = push_str8f(arena, "%lluM", count / 1000000);
    }
  }
  else
  {
    uint64 frac = ((count * 100) * 1000000000) % 100;
    if(frac > 0)
    {
      result = push_str8f(arena, "%llu.%02lluB", count / 1000000000, frac);
    }
    else
    {
      result = push_str8f(arena, "%lluB", count / 1000000000, frac);
    }
  }

  return result;
}

StringView
str8_from_bits_u32(Arena* arena, uint32 x)
{
  uint8 c0 = 'a' + ((x >> 28) & 0xf);
  uint8 c1 = 'a' + ((x >> 24) & 0xf);
  uint8 c2 = 'a' + ((x >> 20) & 0xf);
  uint8 c3 = 'a' + ((x >> 16) & 0xf);
  uint8 c4 = 'a' + ((x >> 12) & 0xf);
  uint8 c5 = 'a' + ((x >>  8) & 0xf);
  uint8 c6 = 'a' + ((x >>  4) & 0xf);
  uint8 c7 = 'a' + ((x >>  0) & 0xf);
  StringView result = push_str8f(arena, "%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7);
  return result;
}

StringView
str8_from_bits_u64(Arena* arena, uint64 x)
{
  uint8 c0 = 'a' + ((x >> 60) & 0xf);
  uint8 c1 = 'a' + ((x >> 56) & 0xf);
  uint8 c2 = 'a' + ((x >> 52) & 0xf);
  uint8 c3 = 'a' + ((x >> 48) & 0xf);
  uint8 c4 = 'a' + ((x >> 44) & 0xf);
  uint8 c5 = 'a' + ((x >> 40) & 0xf);
  uint8 c6 = 'a' + ((x >> 36) & 0xf);
  uint8 c7 = 'a' + ((x >> 32) & 0xf);
  uint8 c8 = 'a' + ((x >> 28) & 0xf);
  uint8 c9 = 'a' + ((x >> 24) & 0xf);
  uint8 ca = 'a' + ((x >> 20) & 0xf);
  uint8 cb = 'a' + ((x >> 16) & 0xf);
  uint8 cc = 'a' + ((x >> 12) & 0xf);
  uint8 cd = 'a' + ((x >>  8) & 0xf);
  uint8 ce = 'a' + ((x >>  4) & 0xf);
  uint8 cf = 'a' + ((x >>  0) & 0xf);
  StringView result = push_str8f(arena,
                              "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                              c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf);
  return result;
}

StringView
str8_from_u64(Arena* arena, uint64 u64, uint32 radix, uint8 min_digits, uint8 digit_group_separator)
{
  StringView result = default;
  {
    // rjf: prefix
    StringView prefix = default;
    switch(radix)
    {
      case 16:{prefix = ("0x");}break;
      case 8: {prefix = ("0o");}break;
      case 2: {prefix = ("0b");}break;
    }
    
    // rjf: determine # of chars between separators
    uint8 digit_group_size = 3;
    switch(radix)
    {
      default:break;
      case 2:
      case 8:
      case 16:
      {digit_group_size = 4;}break;
    }
    
    // rjf: prep
    uint64 needed_leading_0s = 0;
    {
      uint64 needed_digits = 1;
      {
        uint64 u64_reduce = u64;
        for(;;)
        {
          u64_reduce /= radix;
          if(u64_reduce == 0)
          {
            break;
          }
          needed_digits += 1;
        }
      }
      needed_leading_0s = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
      uint64 needed_separators = 0;
      if(digit_group_separator != 0)
      {
        needed_separators = (needed_digits+needed_leading_0s)/digit_group_size;
        if(needed_separators > 0 && (needed_digits+needed_leading_0s)%digit_group_size == 0)
        {
          needed_separators -= 1;
        }
      }
      result.size = prefix.size + needed_leading_0s + needed_separators + needed_digits;
      result.Ptr = push_array_no_zero(arena, uint8, result.size + 1);
      result[result.size] = 0;
    }
    
    // rjf: fill contents
    {
      uint64 u64_reduce = u64;
      uint64 digits_until_separator = digit_group_size;
      for(uint64 idx = 0; idx < result.size; idx += 1)
      {
        if(digits_until_separator == 0 && digit_group_separator != 0)
        {
          result[result.size - idx - 1] = digit_group_separator;
          digits_until_separator = digit_group_size+1;
        }
        else
        {
          result[result.size - idx - 1] = integer_symbols[u64_reduce%radix].ToLower;
          u64_reduce /= radix;
        }
        digits_until_separator -= 1;
        if(u64_reduce == 0)
        {
          break;
        }
      }
      for(uint64 leading_0_idx = 0; leading_0_idx < needed_leading_0s; leading_0_idx += 1)
      {
        result[prefix.size + leading_0_idx] = '0';
      }
    }
    
    // rjf: fill prefix
    if(prefix.size != 0)
    {
      MemoryCopy(result.Ptr, prefix.Ptr, prefix.size);
    }
  }
  return result;
}

StringView
str8_from_s64(Arena* arena, int64 s64, uint32 radix, uint8 min_digits, uint8 digit_group_separator)
{
  StringView result = {0};
  // TODO(rjf): preeeeetty sloppy...
  if(s64 < 0)
  {
    Temp scratch = scratch_begin(&arena, 1);
    StringView numeric_part = str8_from_u64(scratch.arena, (uint64)(-s64), radix, min_digits, digit_group_separator);
    result = push_str8f(arena, "-%S", numeric_part);
    scratch_end(scratch);
  }
  else
  {
    result = str8_from_u64(arena, (uint64)s64, radix, min_digits, digit_group_separator);
  }
  return result;
}

////////////////////////////////
//~ rjf: String <=> Float Conversions

double
f64_from_str8(StringView str)
{
  // TODO(rjf): crappy implementation for now that just uses atof.
  double result = 0;
  if(str.Length > 0)
  {
    // rjf: find starting pos of numeric str, as well as sign
    double sign = +1.0;
    if(str[0] == '-')
    {
      sign = -1.0;
    }
    else if(str[0] == '+')
    {
      sign = 1.0;
    }
    
    // rjf: gather numerics
    uint64 num_valid_chars = 0;
    char buffer[64];
    B32 exp = 0;
    for(uint64 idx = 0; idx < str.Length && num_valid_chars < sizeof(buffer)-1; idx += 1)
    {
      if(char_is_digit(str[idx], 10) || str[idx] == '.' || str[idx] == 'e' ||
         (exp && (str[idx] == '+' || str[idx] == '-')))
      {
        buffer[num_valid_chars] = str[idx];
        num_valid_chars += 1;
        exp = 0;
        exp = (str[idx] == 'e');
      }
    }
    
    // rjf: null-terminate (the reason for all of this!!!!!!)
    buffer[num_valid_chars] = 0;
    
    // rjf: do final conversion
    result = sign * atof(buffer);
  }
  return result;
}

////////////////////////////////
//~ rjf: String List Construction Functions

String8Node*
str8_list_push_node(String8List* list, String8Node* node){
  SLLQueuePush(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += node.str.Length;
  return(node);
}

String8Node*
str8_list_push_node_set_string(String8List* list, String8Node* node, StringView str){
  SLLQueuePush(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += str.Length;
  node.str = str;
  return(node);
}

String8Node*
str8_list_push_node_front(String8List* list, String8Node* node){
  SLLQueuePushFront(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += node.str.Length;
  return(node);
}

String8Node*
str8_list_push_node_front_set_string(String8List* list, String8Node* node, StringView str){
  SLLQueuePushFront(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += str.Length;
  node.str = str;
  return(node);
}

String8Node*
str8_list_push(Arena* arena, String8List* list, StringView str){
  String8Node* node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_set_string(list, node, str);
  return(node);
}

String8Node*
str8_list_push_front(Arena* arena, String8List* list, StringView str){
  String8Node* node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_front_set_string(list, node, str);
  return(node);
}

void
str8_list_concat_in_place(String8List* list, String8List* to_push){
  if(to_push.node_count != 0){
    if (list.last){
      list.node_count += to_push.node_count;
      list.total_size += to_push.total_size;
      list.last.next = to_push.first;
      list.last = to_push.last;
    }
    else{
      *list = *to_push;
    }
    MemoryZeroStruct(to_push);
  }
}

String8Node*
str8_list_push_aligner(Arena* arena, String8List* list, uint64 min, uint64 align){
  String8Node* node = push_array_no_zero(arena, String8Node, 1);
  uint64 new_size = list.total_size + min;
  uint64 increase_size = 0;
  if (align > 1){
    // NOTE(allen): assert is power of 2
    Assert(((align - 1) & align) == 0);
    uint64 mask = align - 1;
    new_size += mask;
    new_size &= (~mask);
    increase_size = new_size - list.total_size;
  }
  static const uint8 zeroes_buffer[64] = {0};
  Assert(increase_size <= ArrayCount(zeroes_buffer));
  SLLQueuePush(list.first, list.last, node);
  list.node_count += 1;
  list.total_size = new_size;
  node.str.Ptr = (uint8*)zeroes_buffer;
  node.str.Length = increase_size;
  return(node);
}

String8Node*
str8_list_pushf(Arena* arena, String8List* list, char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  StringView str = push_str8fv(arena, fmt, args);
  String8Node* result = str8_list_push(arena, list, str);
  va_end(args);
  return(result);
}

String8Node*
str8_list_push_frontf(Arena* arena, String8List* list, char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  StringView str = push_str8fv(arena, fmt, args);
  String8Node* result = str8_list_push_front(arena, list, str);
  va_end(args);
  return(result);
}

String8List
str8_list_copy(Arena* arena, String8List* list){
  String8List result = {0};
  for (String8Node* node = list.first;
       node != 0;
       node = node.next){
    String8Node* new_node = push_array_no_zero(arena, String8Node, 1);
    StringView new_string = push_str8_copy(arena, node.str);
    str8_list_push_node_set_string(&result, new_node, new_string);
  }
  return(result);
}

String8List
str8_split(Arena* arena, StringView str, uint8* split_chars, uint64 split_char_count, StringSplitFlags flags){
  String8List list = {0};
  
  B32 keep_empties = (flags & StringSplitFlag_KeepEmpties);
  
  uint8* ptr = str.Ptr;
  uint8* opl = str.Ptr + str.Length;
  for (;ptr < opl;){
    uint8* first = ptr;
    for (;ptr < opl; ptr += 1){
      uint8 c = *ptr;
      B32 is_split = 0;
      for (uint64 i = 0; i < split_char_count; i += 1){
        if (split_chars[i] == c){
          is_split = 1;
          break;
        }
      }
      if (is_split){
        break;
      }
    }
    
    StringView str = str8_range(first, ptr);
    if (keep_empties || str.Length > 0){
      str8_list_push(arena, &list, str);
    }
    ptr += 1;
  }
  
  return(list);
}

String8List
str8_split_by_string_chars(Arena* arena, StringView str, StringView split_chars, StringSplitFlags flags){
  String8List list = str8_split(arena, str, split_chars.Ptr, split_chars.size, flags);
  return list;
}

String8List
str8_list_split_by_string_chars(Arena* arena, String8List list, StringView split_chars, StringSplitFlags flags){
  String8List result = {0};
  for (String8Node* node = list.first; node != 0; node = node.next){
    String8List split = str8_split_by_string_chars(arena, node.str, split_chars, flags);
    str8_list_concat_in_place(&result, &split);
  }
  return result;
}

StringView
str8_list_join(Arena* arena, String8List* list, StringJoin* optional_params){
  StringJoin join = {0};
  if (optional_params != 0){
    MemoryCopyStruct(&join, optional_params);
  }
  
  uint64 sep_count = 0;
  if (list.node_count > 0){
    sep_count = list.node_count - 1;
  }
  
  StringView result;
  result.size = join.pre.size + join.post.size + sep_count*join.sep.size + list.total_size;
  uint8* ptr = result.Ptr = push_array_no_zero(arena, uint8, result.size + 1);
  
  MemoryCopy(ptr, join.pre.Ptr, join.pre.size);
  ptr += join.pre.size;
  for (String8Node* node = list.first;
       node != 0;
       node = node.next){
    MemoryCopy(ptr, node.str.Ptr, node.str.Length);
    ptr += node.str.Length;
    if (node.next != 0){
      MemoryCopy(ptr, join.sep.Ptr, join.sep.size);
      ptr += join.sep.size;
    }
  }
  MemoryCopy(ptr, join.post.Ptr, join.post.size);
  ptr += join.post.size;
  
  *ptr = 0;
  
  return(result);
}

void
str8_list_from_flags(Arena* arena, String8List* list,
                     uint32 flags, StringView* flag_string_table, uint32 flag_string_count){
  for (uint32 i = 0; i < flag_string_count; i += 1){
    uint32 flag = (1 << i);
    if (flags & flag){
      str8_list_push(arena, list, flag_string_table[i]);
    }
  }
}

////////////////////////////////
//~ rjf; String Arrays

Span<StringView>
str8_array_from_list(Arena* arena, String8List* list)
{
  Span<StringView> array;
  array.count   = list.node_count;
  array.v = push_array_no_zero(arena, StringView, array.count);
  uint64 idx = 0;
  for(String8Node* n = list.first; n != 0; n = n.next, idx += 1)
  {
    array.v[idx] = n.str;
  }
  return array;
}

Span<StringView>
str8_array_reserve(Arena* arena, uint64 count)
{
  Span<StringView> arr;
  arr.count = 0;
  arr.v = push_array(arena, StringView, count);
  return arr;
}

////////////////////////////////
//~ rjf: String Path Helpers

StringView
str8_chop_last_slash(StringView str){
  if (str.Length > 0){
    uint8* ptr = str.Ptr + str.Length - 1;
    for (;ptr >= str.Ptr; ptr -= 1){
      if (*ptr == '/' || *ptr == '\\'){
        break;
      }
    }
    if (ptr >= str.Ptr){
      str.Length = (uint64)(ptr - str.Ptr);
    }
    else{
      str.Length = 0;
    }
  }
  return(str);
}

StringView
str8_skip_last_slash(StringView str){
  if (str.Length > 0){
    uint8* ptr = str.Ptr + str.Length - 1;
    for (;ptr >= str.Ptr; ptr -= 1){
      if (*ptr == '/' || *ptr == '\\'){
        break;
      }
    }
    if (ptr >= str.Ptr){
      ptr += 1;
      str.Length = (uint64)(str.Ptr + str.Length - ptr);
      str.Ptr = ptr;
    }
  }
  return(str);
}

StringView
str8_chop_last_dot(StringView str)
{
  StringView result = str;
  uint64 p = str.Length;
  for (;p > 0;){
    p -= 1;
    if (str[p] == '.'){
      result = str8_prefix(str, p);
      break;
    }
  }
  return(result);
}

StringView
str8_skip_last_dot(StringView str){
  StringView result = str;
  uint64 p = str.Length;
  for (;p > 0;){
    p -= 1;
    if (str[p] == '.'){
      result = str8_skip(str, p + 1);
      break;
    }
  }
  return(result);
}

PathStyle
path_style_from_str8(StringView str){
  PathStyle result = PathStyle_Relative;
  if (str.Length >= 1 && str[0] == '/'){
    result = PathStyle_UnixAbsolute;
  }
  else if (str.Length >= 2 &&
           str[0].IsLetter &&
           str[1] == ':'){
    if (str.Length == 2 ||
        char_is_slash(str[2])){
      result = PathStyle_WindowsAbsolute;
    }
  }
  return(result);
}

String8List
str8_split_path(Arena* arena, StringView str){
  String8List result = str8_split(arena, str, (uint8*)"/\\", 2, 0);
  return(result);
}

void
str8_path_list_resolve_dots_in_place(String8List* path, PathStyle style){
  Temp scratch = scratch_begin(0, 0);
  
  String8MetaNode* stack = 0;
  String8MetaNode* free_meta_node = 0;
  String8Node* first = path.first;
  
  MemoryZeroStruct(path);
  for (String8Node* node = first, *next = 0;
       node != 0;
       node = next){
    // save next now
    next = node.next;
    
    // cases:
    if (node == first && style == PathStyle_WindowsAbsolute){
      goto save_without_stack;
    }
    if (node.str.Length == 1 && node.str[0] == '.'){
      goto do_nothing;
    }
    if (node.str.Length == 2 && node.str[0] == '.' && node.str[1] == '.'){
      if (stack != 0){
        goto eliminate_stack_top;
      }
      else{
        goto save_without_stack;
      }
    }
    goto save_with_stack;
    
    
    // handlers:
    save_with_stack:
    {
      str8_list_push_node(path, node);
      
      String8MetaNode* stack_node = free_meta_node;
      if (stack_node != 0){
        SLLStackPop(free_meta_node);
      }
      else{
        stack_node = push_array_no_zero(scratch.arena, String8MetaNode, 1);
      }
      SLLStackPush(stack, stack_node);
      stack_node.node = node;
      
      continue;
    }
    
    save_without_stack:
    {
      str8_list_push_node(path, node);
      
      continue;
    }
    
    eliminate_stack_top:
    {
      path.node_count -= 1;
      path.total_size -= stack.node.str.Length;
      
      SLLStackPop(stack);
      
      if (stack == 0){
        path.last = path.first;
      }
      else{
        path.last = stack.node;
      }
      continue;
    }
    
    do_nothing: continue;
  }
  scratch_end(scratch);
}

StringView
str8_path_list_join_by_style(Arena* arena, String8List* path, PathStyle style){
  StringJoin params = {0};
  switch(style)
  {
    case PathStyle_Null:{}break;
    case PathStyle_Relative:
    case PathStyle_WindowsAbsolute:
    {
      params.sep = ("/");
    }break;
    
    case PathStyle_UnixAbsolute:
    {
      params.pre = ("/");
      params.sep = ("/");
    }break;
  }
  StringView result = str8_list_join(arena, path, &params);
  return result;
}

String8TxtPtPair
str8_txt_pt_pair_from_string(StringView str)
{
  String8TxtPtPair pair = {0};
  {
    StringView file_part = {0};
    StringView line_part = {0};
    StringView col_part = {0};
    
    // rjf: grab file part
    for(uint64 idx = 0; idx <= str.Length; idx += 1)
    {
      uint8 byte = (idx < str.Length) ? (str[idx]) : 0;
      uint8 next_byte = ((idx+1 < str.Length) ? (str[idx+1]) : 0);
      if(byte == ':' && next_byte != '/' && next_byte != '\\')
      {
        file_part = str8_prefix(str, idx);
        line_part = str8_skip(str, idx+1);
        break;
      }
      else if(byte == 0)
      {
        file_part = str;
        break;
      }
    }
    
    // rjf: grab line/column
    {
      uint64 colon_pos = str8_find_needle(line_part, 0, (":"), 0);
      if(colon_pos < line_part.size)
      {
        col_part = str8_skip(line_part, colon_pos+1);
        line_part = str8_prefix(line_part, colon_pos);
      }
    }
    
    // rjf: convert line/column strings to numerics
    uint64 line = 0;
    uint64 column = 0;
    try_u64_from_str8_c_rules(line_part, &line);
    try_u64_from_str8_c_rules(col_part, &column);
    
    // rjf: fill
    pair.str = file_part;
    pair.pt = txt_pt((int64)line, (int64)column);
    if(pair.pt.line == 0) { pair.pt.line = 1; }
    if(pair.pt.column == 0) { pair.pt.column = 1; }
  }
  return pair;
}

////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding

read_only static uint8 utf8_class[32] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

UnicodeDecode
utf8_decode(uint8* str, uint64 max){
  UnicodeDecode result = {1, max_U32};
  uint8 byte = str[0];
  uint8 byte_class = utf8_class[byte >> 3];
  switch (byte_class)
  {
    case 1:
    {
      result.codepoint = byte;
    }break;
    case 2:
    {
      if (2 < max)
      {
        uint8 cont_byte = str[1];
        if (utf8_class[cont_byte >> 3] == 0)
        {
          result.codepoint = (byte & bitmask5) << 6;
          result.codepoint |=  (cont_byte & bitmask6);
          result.inc = 2;
        }
      }
    }break;
    case 3:
    {
      if (2 < max)
      {
        uint8 cont_byte[2] = {str[1], str[2]};
        if (utf8_class[cont_byte[0] >> 3] == 0 &&
            utf8_class[cont_byte[1] >> 3] == 0)
        {
          result.codepoint = (byte & bitmask4) << 12;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
          result.codepoint |=  (cont_byte[1] & bitmask6);
          result.inc = 3;
        }
      }
    }break;
    case 4:
    {
      if (3 < max)
      {
        uint8 cont_byte[3] = {str[1], str[2], str[3]};
        if (utf8_class[cont_byte[0] >> 3] == 0 &&
            utf8_class[cont_byte[1] >> 3] == 0 &&
            utf8_class[cont_byte[2] >> 3] == 0)
        {
          result.codepoint = (byte & bitmask3) << 18;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
          result.codepoint |= ((cont_byte[1] & bitmask6) <<  6);
          result.codepoint |=  (cont_byte[2] & bitmask6);
          result.inc = 4;
        }
      }
    }
  }
  return(result);
}

UnicodeDecode
utf16_decode(uint16* str, uint64 max){
  UnicodeDecode result = {1, max_U32};
  result.codepoint = str[0];
  result.inc = 1;
  if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000){
    result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
    result.inc = 2;
  }
  return(result);
}

uint32
utf8_encode(uint8* str, uint32 codepoint){
  uint32 inc = 0;
  if (codepoint <= 0x7F){
    str[0] = (uint8)codepoint;
    inc = 1;
  }
  else if (codepoint <= 0x7FF){
    str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
    str[1] = bit8 | (codepoint & bitmask6);
    inc = 2;
  }
  else if (codepoint <= 0xFFFF){
    str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
    str[1] = bit8 | ((codepoint >> 6) & bitmask6);
    str[2] = bit8 | ( codepoint       & bitmask6);
    inc = 3;
  }
  else if (codepoint <= 0x10FFFF){
    str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
    str[1] = bit8 | ((codepoint >> 12) & bitmask6);
    str[2] = bit8 | ((codepoint >>  6) & bitmask6);
    str[3] = bit8 | ( codepoint        & bitmask6);
    inc = 4;
  }
  else{
    str[0] = '?';
    inc = 1;
  }
  return(inc);
}

uint32
utf16_encode(uint16* str, uint32 codepoint){
  uint32 inc = 1;
  if (codepoint == max_U32){
    str[0] = (uint16)'?';
  }
  else if (codepoint < 0x10000){
    str[0] = (uint16)codepoint;
  }
  else{
    uint32 v = codepoint - 0x10000;
    str[0] = safe_cast_u16(0xD800 + (v >> 10));
    str[1] = safe_cast_u16(0xDC00 + (v & bitmask10));
    inc = 2;
  }
  return(inc);
}

uint32
utf8_from_utf32_single(uint8* buffer, uint32 character){
  return(utf8_encode(buffer, character));
}

////////////////////////////////
//~ rjf: Unicode String Conversions

StringView
str8_from_16(Arena* arena, Span<char16> in)
{
  StringView result = StringView();
  if(in.size)
  {
    uint64 cap = in.size*3;
    uint8* str = push_array_no_zero(arena, uint8, cap + 1);
    uint16* ptr = in.Ptr;
    uint16* opl = ptr + in.size;
    uint64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf16_decode(ptr, opl - ptr);
      size += utf8_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    result = StringView(str, size);
  }
  return result;
}

Span<char16>
str16_from_8(Arena* arena, StringView in)
{
  Span<char16> result = .();
  if(in.size)
  {
    uint64 cap = in.size*2;
    uint16* str = push_array_no_zero(arena, uint16, cap + 1);
    uint8* ptr = in.Ptr;
    uint8* opl = ptr + in.size;
    uint64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf8_decode(ptr, opl - ptr);
      size += utf16_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size)*2);
    result = .(str, size);
  }
  return result;
}

StringView
str8_from_32(Arena* arena, Span<char32> in)
{
  StringView result = StringView();
  if(in.size)
  {
    uint64 cap = in.size*4;
    uint8* str = push_array_no_zero(arena, uint8, cap + 1);
    uint32* ptr = in.Ptr;
    uint32* opl = ptr + in.size;
    uint64 size = 0;
    for(;ptr < opl; ptr += 1)
    {
      size += utf8_encode(str + size, *ptr);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    result = StringView(str, size);
  }
  return result;
}

Span<char32>
str32_from_8(Arena* arena, StringView in)
{
  Span<char32> result = default; 
  if(in.size)
  {
    uint64 cap = in.size;
    uint32* str = push_array_no_zero(arena, uint32, cap + 1);
    uint8* ptr = in.Ptr;
    uint8* opl = ptr + in.size;
    uint64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf8_decode(ptr, opl - ptr);
      str[size] = consume.codepoint;
      size += 1;
    }
    str[size] = 0;
    arena_pop(arena, (cap - size)*4);
    result = .(str, size);
  }
  return result;
}

////////////////////////////////
//~ String . Enum Conversions

read_only static struct
{
  StringView         str;
  OperatingSystem os;
} g_os_enum_map[] =
{
  { (""),        OperatingSystem_Null     },
  { ("Windows"), OperatingSystem_Windows, },
  { ("Linux"),   OperatingSystem_Linux,   },
  { ("Mac"),     OperatingSystem_Mac,     },
};
StaticAssert(ArrayCount(g_os_enum_map) == OperatingSystem_COUNT, g_os_enum_map_count_check);

OperatingSystem
operating_system_from_string(StringView str)
{
  for(uint64 i = 0; i < ArrayCount(g_os_enum_map); ++i)
  {
    if(str8_match(g_os_enum_map[i].str, str, StringMatchFlag_CaseInsensitive))
    {
      return g_os_enum_map[i].os;
    }
  }
  return OperatingSystem_Null;
}

////////////////////////////////
//~ rjf: Basic Types & Space Enum . String Conversions

StringView
string_from_dimension(Dimension dimension){
  static StringView strings[] = {
    ("X"),
    ("Y"),
    ("Z"),
    ("W"),
  };
  StringView result = ("error");
  if ((uint32)dimension < 4){
    result = strings[dimension];
  }
  return(result);
}

StringView
string_from_side(Side side){
  static StringView strings[] = {
    ("Min"),
    ("Max"),
  };
  StringView result = ("error");
  if ((uint32)side < 2){
    result = strings[side];
  }
  return(result);
}

StringView
string_from_operating_system(OperatingSystem os)
{
  StringView result = g_os_enum_map[OperatingSystem_Null].str;
  if(os < ArrayCount(g_os_enum_map))
  {
    result = g_os_enum_map[os].str;
  }
  return result;
}

StringView
string_from_arch(Arch arch){
  static StringView strings[] = {
    ("Null"),
    ("x64"),
    ("x86"),
    ("arm64"),
    ("arm32"),
  };
  StringView result = ("error");
  if (arch < Arch_COUNT){
    result = strings[arch];
  }
  return(result);
}

////////////////////////////////
//~ rjf: Time Types . String

StringView
string_from_week_day(WeekDay week_day){
  static StringView strings[] = {
    ("Sun"),
    ("Mon"),
    ("Tue"),
    ("Wed"),
    ("Thu"),
    ("Fri"),
    ("Sat"),
  };
  StringView result = ("Err");
  if ((uint32)week_day < WeekDay_COUNT){
    result = strings[week_day];
  }
  return(result);
}

StringView
string_from_month(Month month){
  static StringView strings[] = {
    ("Jan"),
    ("Feb"),
    ("Mar"),
    ("Apr"),
    ("May"),
    ("Jun"),
    ("Jul"),
    ("Aug"),
    ("Sep"),
    ("Oct"),
    ("Nov"),
    ("Dec"),
  };
  StringView result = ("Err");
  if ((uint32)month < Month_COUNT){
    result = strings[month];
  }
  return(result);
}

StringView
push_date_time_string(Arena* arena, DateTime* date_time){
  char* mon_str = (char*)string_from_month(date_time.month).Ptr;
  uint32 adjusted_hour = date_time.hour%12;
  if (adjusted_hour == 0){
    adjusted_hour = 12;
  }
  char* ampm = "am";
  if (date_time.hour >= 12){
    ampm = "pm";
  }
  StringView result = push_str8f(arena, "%d %s %d, %02d:%02d:%02d %s",
                              date_time.day, mon_str, date_time.year,
                              adjusted_hour, date_time.min, date_time.sec, ampm);
  return(result);
}

StringView
push_file_name_date_time_string(Arena* arena, DateTime* date_time){
  char* mon_str = (char*)string_from_month(date_time.month).Ptr;
  StringView result = push_str8f(arena, "%d-%s-%0d--%02d-%02d-%02d",
                              date_time.year, mon_str, date_time.day,
                              date_time.hour, date_time.min, date_time.sec);
  return(result);
}

StringView
string_from_elapsed_time(Arena* arena, DateTime dt){
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (dt.year){
    str8_list_pushf(scratch.arena, &list, "%dy", dt.year);
    str8_list_pushf(scratch.arena, &list, "%um", dt.mon);
    str8_list_pushf(scratch.arena, &list, "%ud", dt.day);
  } else if (dt.mon){
    str8_list_pushf(scratch.arena, &list, "%um", dt.mon);
    str8_list_pushf(scratch.arena, &list, "%ud", dt.day);
  } else if (dt.day){
    str8_list_pushf(scratch.arena, &list, "%ud", dt.day);
  }
  str8_list_pushf(scratch.arena, &list, "%u:%u:%u:%u ms", dt.hour, dt.min, dt.sec, dt.msec);
  StringJoin join = { (""), (" "), ("") };
  StringView result = str8_list_join(arena, &list, &join);
  scratch_end(scratch);
  return(result);
}

////////////////////////////////
//~ Globally UNique Ids

StringView
string_from_guid(Arena* arena, Guid guid)
{
  StringView result = push_str8f(arena, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                              guid.data1,
                              guid.data2,
                              guid.data3,
                              guid.data4[0],
                              guid.data4[1],
                              guid.data4[2],
                              guid.data4[3],
                              guid.data4[4],
                              guid.data4[5],
                              guid.data4[6],
                              guid.data4[7]);
  return result;
}

B32
try_guid_from_string(StringView str, Guid* guid_out)
{
  Temp scratch = scratch_begin(0,0);
  B32 is_parsed = 0;
  String8List list = str8_split_by_string_chars(scratch.arena, str, ("-"), StringSplitFlag_KeepEmpties);
  if(list.node_count == 5)
  {
    StringView data1_str    = list.first.str;
    StringView data2_str    = list.first.next.str;
    StringView data3_str    = list.first.next.next.str;
    StringView data4_hi_str = list.first.next.next.next.str;
    StringView data4_lo_str = list.first.next.next.next.next.str;
    if(str8_is_integer(data1_str, 16) && 
        str8_is_integer(data2_str, 16) &&
        str8_is_integer(data3_str, 16) &&
        str8_is_integer(data4_hi_str, 16) &&
        str8_is_integer(data4_lo_str, 16))
    {
      uint64 data1    = u64_from_str8(data1_str, 16);
      uint64 data2    = u64_from_str8(data2_str, 16);
      uint64 data3    = u64_from_str8(data3_str, 16);
      uint64 data4_hi = u64_from_str8(data4_hi_str, 16);
      uint64 data4_lo = u64_from_str8(data4_lo_str, 16);
      if(data1 <= max_U32 &&
          data2 <= max_U16 &&
          data3 <= max_U16 &&
          data4_hi <= max_U16 &&
          data4_lo <= 0xffffffffffff)
      {
        guid_out.data1 = (uint32)data1;
        guid_out.data2 = (uint16)data2;
        guid_out.data3 = (uint16)data3;
        uint64 data4 = (data4_hi << 48) | data4_lo;
        MemoryCopy(&guid_out.data4[0], &data4, sizeof(data4));
        is_parsed = 1;
      }
    }
  }
  scratch_end(scratch);
  return is_parsed;
}

Guid
guid_from_string(StringView str)
{
  Guid guid = {0};
  try_guid_from_string(str, &guid);
  return guid;
}

////////////////////////////////
//~ rjf: Basic Text Indentation

StringView
indented_from_string(Arena* arena, StringView str)
{
  Temp scratch = scratch_begin(&arena, 1);
  read_only static uint8 indentation_bytes[] = "                                                                                                                                ";
  String8List indented_strings = {0};
  int64 depth = 0;
  int64 next_depth = 0;
  uint64 line_begin_off = 0;
  for(uint64 off = 0; off <= str.Length; off += 1)
  {
    uint8 byte = off<str.Length ? str[off] : 0;
    switch(byte)
    {
      default:{}break;
      case '{':case '[':case '(':{next_depth += 1; next_depth = Max(0, next_depth);}break;
      case '}':case ']':case ')':{next_depth -= 1; next_depth = Max(0, next_depth); depth = next_depth;}break;
      case '\n':
      case 0:
      {
        StringView line = str8_skip_chop_whitespace(str8_substr(str, r1u64(line_begin_off, off)));
        if(line.size != 0)
        {
          str8_list_pushf(scratch.arena, &indented_strings, "%.*s%S\n", (int)depth*2, indentation_bytes, line);
        }
        if(line.size == 0 && indented_strings.node_count != 0 && off < str.Length)
        {
          str8_list_pushf(scratch.arena, &indented_strings, "\n");
        }
        line_begin_off = off+1;
        depth = next_depth;
      }break;
    }
  }
  StringView result = str8_list_join(arena, &indented_strings, 0);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Text Escaping

StringView
escaped_from_raw_str8(Arena* arena, StringView str)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List parts = {0};
  uint64 start_split_idx = 0;
  for(uint64 idx = 0; idx <= str.Length; idx += 1)
  {
    uint8 byte = (idx < str.Length) ? str[idx] : 0;
    B32 split = 1;
    StringView separator_replace = {0};
    switch(byte)
    {
      default:{split = 0;}break;
      case 0:    {}break;
      case '\a': {separator_replace = ("\\a");}break;
      case '\b': {separator_replace = ("\\b");}break;
      case '\f': {separator_replace = ("\\f");}break;
      case '\n': {separator_replace = ("\\n");}break;
      case '\r': {separator_replace = ("\\r");}break;
      case '\t': {separator_replace = ("\\t");}break;
      case '\v': {separator_replace = ("\\v");}break;
      case '\\': {separator_replace = ("\\\\");}break;
      case '"':  {separator_replace = ("\\\"");}break;
      case '?':  {separator_replace = ("\\?");}break;
    }
    if(split)
    {
      StringView substr = str8_substr(str, r1u64(start_split_idx, idx));
      start_split_idx = idx+1;
      str8_list_push(scratch.arena, &parts, substr);
      if(separator_replace.size != 0)
      {
        str8_list_push(scratch.arena, &parts, separator_replace);
      }
    }
  }
  StringJoin join = {0};
  StringView result = str8_list_join(arena, &parts, &join);
  scratch_end(scratch);
  return result;
}

StringView
raw_from_escaped_str8(Arena* arena, StringView str)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List strs = {0};
  uint64 start = 0;
  for(uint64 idx = 0; idx <= str.Length; idx += 1)
  {
    if(idx == str.Length || str[idx] == '\\' || str[idx] == '\r')
    {
      StringView str = str8_substr(str, r1u64(start, idx));
      if(str.Length != 0)
      {
        str8_list_push(scratch.arena, &strs, str);
      }
      start = idx+1;
    }
    if(idx < str.Length && str[idx] == '\\')
    {
      uint8 next_char = str[idx+1];
      uint8 replace_byte = 0;
      switch(next_char)
      {
        default:{}break;
        case 'a': replace_byte = 0x07; break;
        case 'b': replace_byte = 0x08; break;
        case 'e': replace_byte = 0x1b; break;
        case 'f': replace_byte = 0x0c; break;
        case 'n': replace_byte = 0x0a; break;
        case 'r': replace_byte = 0x0d; break;
        case 't': replace_byte = 0x09; break;
        case 'v': replace_byte = 0x0b; break;
        case '\\':replace_byte = '\\'; break;
        case '\'':replace_byte = '\''; break;
        case '"': replace_byte = '"';  break;
        case '?': replace_byte = '?';  break;
      }
      StringView replace_string = push_str8_copy(scratch.arena, StringView(&replace_byte, 1));
      str8_list_push(scratch.arena, &strs, replace_string);
      idx += 1;
      start += 1;
    }
  }
  StringView result = str8_list_join(arena, &strs, 0);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Text Wrapping

String8List
wrapped_lines_from_string(Arena* arena, StringView str, uint64 first_line_max_width, uint64 max_width, uint64 wrap_indent)
{
  String8List list = {0};
  Rng1U64 line_range = r1u64(0, 0);
  uint64 wrapped_indent_level = 0;
  static char* spaces = "                                                                ";
  for (uint64 idx = 0; idx <= str.Length; idx += 1){
    uint8 chr = idx < str.Length ? str[idx] : 0;
    if (chr == '\n'){
      Rng1U64 candidate_line_range = line_range;
      candidate_line_range.max = idx;
      // NOTE(nick): when wrapping is interrupted with \n we emit a str without including \n
      // because later tool_fprint_list inserts separator after each node
      // except for last node, so don't strip last \n.
      if (idx + 1 == str.Length){
        candidate_line_range.max += 1;
      }
      StringView substr = str8_substr(str, candidate_line_range);
      str8_list_push(arena, &list, substr);
      line_range = r1u64(idx+1,idx+1);
    }
    else
      if (char_is_space(chr) || chr == 0){
      Rng1U64 candidate_line_range = line_range;
      candidate_line_range.max = idx;
      StringView substr = str8_substr(str, candidate_line_range);
      uint64 width_this_line = max_width-wrapped_indent_level;
      if (list.node_count == 0){
        width_this_line = first_line_max_width;
      }
      if (substr.size > width_this_line){
        StringView line = str8_substr(str, line_range);
        if (wrapped_indent_level > 0){
          line = push_str8f(arena, "%.*s%S", wrapped_indent_level, spaces, line);
        }
        str8_list_push(arena, &list, line);
        line_range = r1u64(line_range.max+1, candidate_line_range.max);
        wrapped_indent_level = ClampTop(64, wrap_indent);
      }
      else{
        line_range = candidate_line_range;
      }
    }
  }
  if (line_range.min < str.Length && line_range.max > line_range.min){
    StringView line = str8_substr(str, line_range);
    if (wrapped_indent_level > 0){
      line = push_str8f(arena, "%.*s%S", wrapped_indent_level, spaces, line);
    }
    str8_list_push(arena, &list, line);
  }
  return list;
}

////////////////////////////////
//~ rjf: String <. Color

StringView
hex_string_from_rgba_4f32(Arena* arena, Vec4F32 rgba)
{
  StringView hex_string = push_str8f(arena, "%02x%02x%02x%02x", (uint8)(rgba.x*255.f), (uint8)(rgba.y*255.f), (uint8)(rgba.z*255.f), (uint8)(rgba.w*255.f));
  return hex_string;
}

Vec4F32
rgba_from_hex_string_4f32(StringView hex_string)
{
  uint8 byte_text[8] = {0};
  uint64 byte_text_idx = 0;
  for(uint64 idx = 0; idx < hex_string.size && byte_text_idx < ArrayCount(byte_text); idx += 1)
  {
    if(char_is_digit(hex_string[idx], 16))
    {
      byte_text[byte_text_idx] = hex_string[idx].ToLower;
      byte_text_idx += 1;
    }
  }
  uint8[4] byte_vals = .();
  for(uint64 idx = 0; idx < 4; idx += 1)
  {
    byte_vals[idx] = (uint8)u64_from_str8(StringView(&byte_text[idx*2], 2), 16);
  }
  Vec4F32 rgba = v4f32(byte_vals[0]/255.f, byte_vals[1]/255.f, byte_vals[2]/255.f, byte_vals[3]/255.f);
  return rgba;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching

FuzzyMatchRangeList
fuzzy_match_find(Arena* arena, StringView needle, StringView haystack)
{
  FuzzyMatchRangeList result = {0};
  Temp scratch = scratch_begin(&arena, 1);
  String8List needles = str8_split(scratch.arena, needle, (uint8*)" ", 1, 0);
  result.needle_part_count = needles.node_count;
  for(String8Node* needle_n = needles.first; needle_n != 0; needle_n = needle_n.next)
  {
    uint64 find_pos = 0;
    for(;find_pos < haystack.size;)
    {
      find_pos = str8_find_needle(haystack, find_pos, needle_n.str, StringMatchFlag_CaseInsensitive);
      B32 is_in_gathered_ranges = 0;
      for(FuzzyMatchRangeNode* n = result.first; n != 0; n = n.next)
      {
        if(n.range.min <= find_pos && find_pos < n.range.max)
        {
          is_in_gathered_ranges = 1;
          find_pos = n.range.max;
          break;
        }
      }
      if(!is_in_gathered_ranges)
      {
        break;
      }
    }
    if(find_pos < haystack.size)
    {
      Rng1U64 range = r1u64(find_pos, find_pos+needle_n.str.Length);
      FuzzyMatchRangeNode* n = push_array(arena, FuzzyMatchRangeNode, 1);
      n.range = range;
      SLLQueuePush(result.first, result.last, n);
      result.count += 1;
      result.total_dim += dim_1u64(range);
    }
  }
  scratch_end(scratch);
  return result;
}

FuzzyMatchRangeList
fuzzy_match_range_list_copy(Arena* arena, FuzzyMatchRangeList* src)
{
  FuzzyMatchRangeList dst = {0};
  for(FuzzyMatchRangeNode* src_n = src.first; src_n != 0; src_n = src_n.next)
  {
    FuzzyMatchRangeNode* dst_n = push_array(arena, FuzzyMatchRangeNode, 1);
    SLLQueuePush(dst.first, dst.last, dst_n);
    dst_n.range = src_n.range;
  }
  dst.count = src.count;
  dst.needle_part_count = src.needle_part_count;
  dst.total_dim = src.total_dim;
  return dst;
}

////////////////////////////////
//~ NOTE(allen): Serialization Helpers

void
str8_serial_begin(Arena* arena, String8List* srl){
  String8Node* node = push_array(arena, String8Node, 1);
  node.str.Ptr = push_array_no_zero(arena, uint8, 0);
  srl.first = srl.last = node;
  srl.node_count = 1;
  srl.total_size = 0;
}

StringView
str8_serial_end(Arena* arena, String8List* srl){
  uint64 size = srl.total_size;
  uint8* out = push_array_no_zero(arena, uint8, size);
  str8_serial_write_to_dst(srl, out);
  StringView result = StringView(out, size);
  return result;
}

void
str8_serial_write_to_dst(String8List* srl, void* out){
  uint8* ptr = (uint8*)out;
  for (String8Node* node = srl.first;
       node != 0;
       node = node.next){
    uint64 size = node.str.Length;
    MemoryCopy(ptr, node.str.Ptr, size);
    ptr += size;
  }
}

uint64
str8_serial_push_align(Arena* arena, String8List* srl, uint64 align){
  Assert(IsPow2(align));
  
  uint64 pos = srl.total_size;
  uint64 new_pos = AlignPow2(pos, align);
  uint64 size = (new_pos - pos);
  
  if(size != 0)
  {
    uint8* buf = push_array(arena, uint8, size);
    
    StringView* str = &srl.last.str;
    if (str.Ptr + str.Length == buf){
      srl.last.str.Length += size;
      srl.total_size += size;
    }
    else{
      str8_list_push(arena, srl, StringView(buf, size));
    }
  }
  return size;
}

void *
str8_serial_push_size(Arena* arena, String8List* srl, uint64 size)
{
  void* result = 0;
  if(size != 0)
  {
    uint8* buf = push_array_no_zero(arena, uint8, size);
    StringView* str = &srl.last.str;
    if (str.Ptr + str.Length == buf){
      srl.last.str.Length += size;
      srl.total_size += size;
    }
    else{
      str8_list_push(arena, srl, StringView(buf, size));
    }
    result = buf;
  }
  return result;
}

void *
str8_serial_push_data(Arena* arena, String8List* srl, void* data, uint64 size){
  void* result = str8_serial_push_size(arena, srl, size);
  if(result != 0)
  {
    MemoryCopy(result, data, size);
  }
  return result;
}

void
str8_serial_push_data_list(Arena* arena, String8List* srl, String8Node* first){
  for (String8Node* node = first;
       node != 0;
       node = node.next){
    str8_serial_push_data(arena, srl, node.str.Ptr, node.str.Length);
  }
}

void
str8_serial_push_u64(Arena* arena, String8List* srl, uint64 x){
  uint8* buf = push_array_no_zero(arena, uint8, 8);
  MemoryCopy(buf, &x, 8);
  StringView* str = &srl.last.str;
  if (str.Ptr + str.Length == buf){
    srl.last.str.Length += 8;
    srl.total_size += 8;
  }
  else{
    str8_list_push(arena, srl, StringView(buf, 8));
  }
}

void
str8_serial_push_u32(Arena* arena, String8List* srl, uint32 x){
  uint8* buf = push_array_no_zero(arena, uint8, 4);
  MemoryCopy(buf, &x, 4);
  StringView* str = &srl.last.str;
  if (str.Ptr + str.Length == buf){
    srl.last.str.Length += 4;
    srl.total_size += 4;
  }
  else{
    str8_list_push(arena, srl, StringView(buf, 4));
  }
}

void
str8_serial_push_u16(Arena* arena, String8List* srl, uint16 x){
  str8_serial_push_data(arena, srl, &x, sizeof(x));
}

void
str8_serial_push_u8(Arena* arena, String8List* srl, uint8 x){
  str8_serial_push_data(arena, srl, &x, sizeof(x));
}

void
str8_serial_push_cstr(Arena* arena, String8List* srl, StringView str){
  str8_serial_push_data(arena, srl, str.Ptr, str.Length);
  str8_serial_push_u8(arena, srl, 0);
}

void
str8_serial_push_string(Arena* arena, String8List* srl, StringView str){
  str8_serial_push_data(arena, srl, str.Ptr, str.Length);
}

////////////////////////////////
//~ rjf: Deserialization Helpers

uint64
str8_deserial_read(StringView str, uint64 off, void* read_dst, uint64 read_size, uint64 granularity)
{
  uint64 bytes_left = str.Length-Min(off, str.Length);
  uint64 actually_readable_size = Min(bytes_left, read_size);
  uint64 legally_readable_size = actually_readable_size - actually_readable_size%granularity;
  if(legally_readable_size > 0)
  {
    MemoryCopy(read_dst, str.Ptr+off, legally_readable_size);
  }
  return legally_readable_size;
}

uint64
str8_deserial_find_first_match(StringView str, uint64 off, uint16 scan_val)
{
  uint64 cursor = off;
  for (;;) {
    uint16 val = 0;
    str8_deserial_read_struct(str, cursor, &val);
    if (val == scan_val) {
      break;
    }
    cursor += sizeof(val);
  }
  return cursor;
}

void *
str8_deserial_get_raw_ptr(StringView str, uint64 off, uint64 size)
{
  void* raw_ptr = 0;
  if (off + size <= str.Length) {
    raw_ptr = str.Ptr + off;
  }
  return raw_ptr;
}

uint64
str8_deserial_read_cstr(StringView str, uint64 off, StringView* cstr_out)
{
  uint64 cstr_size = 0;
  if (off < str.Length) {
    uint8* ptr = str.Ptr + off;
    uint8* cap = str.Ptr + str.Length;
    *cstr_out = str8_cstring_capped(ptr, cap);
    cstr_size = (cstr_out.size + 1);
  }
  return cstr_size;
}

uint64
str8_deserial_read_windows_utf16_string16(StringView str, uint64 off, Span<char16>* str_out)
{
  uint64 null_off = str8_deserial_find_first_match(str, off, 0);
  uint64 size = null_off - off;
  uint16* str = (uint16 *)str8_deserial_get_raw_ptr(str, off, size);
  uint64 count = size / sizeof(*str);
  *str_out = Span<char16>(str, count);
  
  uint64 read_size_with_null = size + sizeof(*str);
  return read_size_with_null;
}

uint64
str8_deserial_read_block(StringView str, uint64 off, uint64 size, StringView* block_out)
{
  Rng1U64 range = rng_1u64(off, off + size);
  *block_out = str8_substr(str, range);
  return block_out.size;
}

uint64
str8_deserial_read_uleb128(StringView str, uint64 off, uint64* value_out)
{
  uint64 value  = 0;
  uint64 shift  = 0;
  uint64 cursor = off;
  for(;;)
  {
    uint8  byte       = 0;
    uint64 bytes_read = str8_deserial_read_struct(str, cursor, &byte);

    if(bytes_read != sizeof(byte))
    {
      break;
    }

    uint8 val = byte & 0x7fu;
    value |= ((uint64)val) << shift;

    cursor += bytes_read;
    shift += 7u;

    if((byte & 0x80u) == 0)
    {
      break;
    }
  }
  if(value_out != 0)
  {
    *value_out = value;
  }
  uint64 bytes_read = cursor - off;
  return bytes_read;
}

uint64
str8_deserial_read_sleb128(StringView str, uint64 off, int64* value_out)
{
  uint64 value  = 0;
  uint64 shift  = 0;
  uint64 cursor = off;
  for(;;)
  {
    uint8 byte;
    uint64 bytes_read = str8_deserial_read_struct(str, cursor, &byte);
    if(bytes_read != sizeof(byte))
    {
      break;
    }

    uint8 val = byte & 0x7fu;
    value |= ((uint64)val) << shift;

    cursor += bytes_read;
    shift += 7u;

    if((byte & 0x80u) == 0)
    {
      if(shift < sizeof(value) * 8 && (byte & 0x40u) != 0)
      {
        value |= -(int64)(1ull << shift);
      }
      break;
    }
  }
  if(value_out != 0)
  {
    *value_out = value;
  }
  uint64 bytes_read = cursor - off;
  return bytes_read;
}


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
//~ NOTE(allen): String <-> Integer Tables

static readonly uint8[16] integer_symbols = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

// NOTE(allen): Includes reverses for uppercase and lowercase hex.
static readonly uint8[128] integer_symbol_reverse = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

static readonly uint8[64] base64 = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '_', '$',
};

static readonly uint8[128] base64_reverse = {
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

public static B32 char_is_space(uint8 c){
  return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

public static B32 char_is_upper(uint8 c){
  return('A' <= c && c <= 'Z');
}

public static B32 char_is_lower(uint8 c){
  return('a' <= c && c <= 'z');
}

public static B32 char_is_alpha(uint8 c){
  return(char_is_upper(c) || char_is_lower(c));
}

public static B32 char_is_slash(uint8 c){
  return(c == '/' || c == '\\');
}

public static B32 char_is_digit(uint8 c, uint32 base){
  B32 result = 0;
  if (0 < base && base <= 16){
    uint8 val = integer_symbol_reverse[c];
    if (val < base){
      result = 1;
    }
  }
  return(result);
}

public static uint8 char_to_lower(uint8 c){
  if (char_is_upper(c)){
    c += ('a' - 'A');
  }
  return(c);
}

public static uint8 char_to_upper(uint8 c){
  if (char_is_lower(c)){
    c += ('A' - 'a');
  }
  return(c);
}

public static uint8 char_to_correct_slash(uint8 c){
  if(char_is_slash(c)){
    c = '/';
  }
  return(c);
}

////////////////////////////////
//~ rjf: C-String Measurement

public static uint64 cstring8_length(uint8* c){
  uint8* p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

public static uint64 cstring16_length(uint16* c){
  uint16* p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

public static uint64 cstring32_length(uint32* c){
  uint32* p = c;
  for (;*p != 0; p += 1);
  return(p - c);
}

////////////////////////////////
//~ rjf: String Constructors

public static String8 str8(uint8* str, uint64 size){
  String8 result = {str, size};
  return(result);
}

public static String8 str8_range(uint8* first, uint8* one_past_last){
  String8 result = {first, (uint64)(one_past_last - first)};
  return(result);
}

public static String8 str8_zero(){
  String8 result = default;
  return(result);
}

public static String16 str16(uint16* str, uint64 size){
  String16 result = {str, size};
  return(result);
}

public static String16 str16_range(uint16* first, uint16* one_past_last){
  String16 result = {first, (uint64)(one_past_last - first)};
  return(result);
}

public static String16 str16_zero(){
  String16 result = default;
  return(result);
}

public static String32 str32(uint32* str, uint64 size){
  String32 result = {str, size};
  return(result);
}

public static String32 str32_range(uint32* first, uint32* one_past_last){
  String32 result = {first, (uint64)(one_past_last - first)};
  return(result);
}

public static String32 str32_zero(){
  String32 result = default;
  return(result);
}

public static String8 str8_cstring(char* c){
  String8 result = {(uint8*)c, cstring8_length((uint8*)c)};
  return(result);
}

public static String16 str16_cstring(uint16* c){
  String16 result = {(uint16*)c, cstring16_length((uint16*)c)};
  return(result);
}

public static String32 str32_cstring(uint32* c){
  String32 result = {(uint32*)c, cstring32_length((uint32*)c)};
  return(result);
}

public static String8 str8_cstring_capped(void* cstr, void* cap)
{
  char* ptr = (char *)cstr;
  char* opl = (char *)cap;
  for (;ptr < opl && *ptr != 0; ptr += 1);
  uint64 size = (uint64)(ptr - (char *)cstr);
  String8 result = str8((uint8*)cstr, size);
  return result;
}

public static String16 str16_cstring_capped(void* cstr, void* cap)
{
  uint16* ptr = (uint16 *)cstr;
  uint16* opl = (uint16 *)cap;
  for (;ptr < opl && *ptr != 0; ptr += 1);
  uint64 size = (uint64)(ptr - (uint16 *)cstr);
  String16 result = str16(cstr, size);
  return result;
}

public static String8 str8_cstring_capped_reverse(void* raw_start, void* raw_cap)
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
  String8 result  = str8(start, size);
  return result;
}

////////////////////////////////
//~ rjf: String Stylization

public static String8 upper_from_str8(Arena* arena, String8 string_)
{
  string_ = push_str8_copy(arena, string_);
  for(uint64 idx = 0; idx < string_.size; idx += 1)
  {
    string_.str[idx] = char_to_upper(string_.str[idx]);
  }
  return string_;
}

public static String8 lower_from_str8(Arena* arena, String8 string_)
{
  string_ = push_str8_copy(arena, string_);
  for(uint64 idx = 0; idx < string_.size; idx += 1)
  {
    string_.str[idx] = char_to_lower(string_.str[idx]);
  }
  return string_;
}

public static String8 backslashed_from_str8(Arena* arena, String8 string_)
{
  string_ = push_str8_copy(arena, string_);
  for(uint64 idx = 0; idx < string_.size; idx += 1)
  {
    string_.str[idx] = char_is_slash(string_.str[idx]) ? '\\' : string_.str[idx];
  }
  return string_;
}

////////////////////////////////
//~ rjf: String Matching

public static B32 str8_match(String8 a, String8 b, StringMatchFlags flags)
{
  B32 result = 0;
  if(a.size == b.size && flags == 0)
  {
    result = MemoryMatch(a.str, b.str, b.size);
  }
  else if(a.size == b.size || (flags & StringMatchFlags.RightSideSloppy))
  {
    B32 case_insensitive  = (flags & StringMatchFlags.CaseInsensitive);
    B32 slash_insensitive = (flags & StringMatchFlags.SlashInsensitive);
    uint64 size              = Min(a.size, b.size);
    result = 1;
    for(uint64 i = 0; i < size; i += 1)
    {
      uint8 at = a.str[i];
      uint8 bt = b.str[i];
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

public static uint64 str8_find_needle(String8 string_, uint64 start_pos, String8 needle, StringMatchFlags flags){
  uint8* p = string_.str + start_pos;
  uint64 stop_offset = Max(string_.size + 1, needle.size) - needle.size;
  uint8* stop_p = string_.str + stop_offset;
  if (needle.size > 0){
    uint8* string_opl = string_.str + string_.size;
    String8 needle_tail = str8_skip(needle, 1);
    StringMatchFlags adjusted_flags = flags | StringMatchFlags.RightSideSloppy;
    uint8 needle_first_char_adjusted = needle.str[0];
    if(adjusted_flags & StringMatchFlags.CaseInsensitive){
      needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
    }
    for (;p < stop_p; p += 1){
      uint8 haystack_char_adjusted = *p;
      if(adjusted_flags & StringMatchFlags.CaseInsensitive){
        haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
      }
      if (haystack_char_adjusted == needle_first_char_adjusted){
        if (str8_match(str8_range(p + 1, string_opl), needle_tail, adjusted_flags)){
          break;
        }
      }
    }
  }
  uint64 result = string_.size;
  if (p < stop_p){
    result = (uint64)(p - string_.str);
  }
  return(result);
}

public static uint64 str8_find_needle_reverse(String8 string_, uint64 start_pos, String8 needle, StringMatchFlags flags)
{
  uint64 result = 0;
  for(int64 i = string_.size - start_pos - needle.size; i >= 0; --i)
  {
    String8 haystack = str8_substr(string_, rng_1u64(i, i + needle.size));
    if(str8_match(haystack, needle, flags))
    {
      result = (uint64)i + needle.size;
      break;
    }
  }
  return result;
}

public static B32 str8_ends_with(String8 string_, String8 end, StringMatchFlags flags){
  String8 postfix = str8_postfix(string_, end.size);
  B32 is_match = str8_match(end, postfix, flags);
  return is_match;
}

////////////////////////////////
//~ rjf: String Slicing

public static String8 str8_substr(String8 str, Rng1U64 range){
  range.min = ClampTop(range.min, str.size);
  range.max = ClampTop(range.max, str.size);
  str.str += range.min;
  str.size = dim_1u64(range);
  return(str);
}

public static String8 str8_prefix(String8 str, uint64 size){
  str.size = ClampTop(size, str.size);
  return(str);
}

public static String8 str8_skip(String8 str, uint64 amt){
  amt = ClampTop(amt, str.size);
  str.str += amt;
  str.size -= amt;
  return(str);
}

public static String8 str8_postfix(String8 str, uint64 size){
  size = ClampTop(size, str.size);
  str.str = (str.str + str.size) - size;
  str.size = size;
  return(str);
}

public static String8 str8_chop(String8 str, uint64 amt){
  amt = ClampTop(amt, str.size);
  str.size -= amt;
  return(str);
}

public static String8 str8_skip_chop_whitespace(String8 string_){
  uint8* first = string_.str;
  uint8* opl = first + string_.size;
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
  String8 result = str8_range(first, opl);
  return(result);
}

////////////////////////////////
//~ rjf: String Formatting & Copying

public static String8 push_str8_cat(Arena* arena, String8 s1, String8 s2){
  String8 str;
  str.size = s1.size + s2.size;
  str.str = push_array_no_zero(arena, uint8, str.size + 1);
  MemoryCopy(str.str, s1.str, s1.size);
  MemoryCopy(str.str + s1.size, s2.str, s2.size);
  str.str[str.size] = 0;
  return(str);
}

public static String8 push_str8_copy(Arena* arena, String8 s){
  String8 str;
  str.size = s.size;
  str.str = push_array_no_zero(arena, uint8, str.size + 1);
  MemoryCopy(str.str, s.str, s.size);
  str.str[str.size] = 0;
  return(str);
}

public static String8 push_str8fv(Arena* arena, char* fmt, va_list args){
  va_list args2;
  va_copy(args2, args);
  uint32 needed_bytes = raddbg_vsnprintf(0, 0, fmt, args) + 1;
  String8 result = default;
  result.str = push_array_no_zero(arena, uint8, needed_bytes);
  result.size = raddbg_vsnprintf((char*)result.str, needed_bytes, fmt, args2);
  result.str[result.size] = 0;
  va_end(args2);
  return(result);
}

public static String8 push_str8f(Arena* arena, char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  String8 result = push_str8fv(arena, fmt, args);
  va_end(args);
  return(result);
}

////////////////////////////////
//~ rjf: String <=> Integer Conversions

//- rjf: string_ -> integer

public static int64 sign_from_str8(String8 string_, String8* string_tail){
  // count negative signs
  uint64 neg_count = 0;
  uint64 i = 0;
  for (; i < string_.size; i += 1){
    if (string_.str[i] == '-'){
      neg_count += 1;
    }
    else if (string_.str[i] != '+'){
      break;
    }
  }
  
  // output part of string_ after signs
  *string_tail = str8_skip(string_, i);
  
  // output integer sign
  int64 sign = (neg_count & 1)?-1:+1;
  return(sign);
}

public static B32 str8_is_integer(String8 string_, uint32 radix){
  B32 result = 0;
  if (string_.size > 0){
    if (1 < radix && radix <= 16){
      result = 1;
      for (uint64 i = 0; i < string_.size; i += 1){
        uint8 c = string_.str[i];
        if (!(c < 0x80) || integer_symbol_reverse[c] >= radix){
          result = 0;
          break;
        }
      }
    }
  }
  return(result);
}

public static uint64 u64_from_str8(String8 string_, uint32 radix){
  uint64 x = 0;
  if (1 < radix && radix <= 16){
    for (uint64 i = 0; i < string_.size; i += 1){
      x *= radix;
      x += integer_symbol_reverse[string_.str[i]&0x7F];
    }
  }
  return(x);
}

public static int64 s64_from_str8(String8 string_, uint32 radix){
  int64 sign = sign_from_str8(string_, &string_);
  int64 x = (int64)u64_from_str8(string_, radix) * sign;
  return(x);
}

public static uint32 u32_from_str8(String8 string_, uint32 radix)
{
  uint64 x64 = u64_from_str8(string_, radix);
  uint32 x32 = safe_cast_u32(x64);
  return x32;
}

public static int32 s32_from_str8(String8 string_, uint32 radix)
{
  int64 x64 = s64_from_str8(string_, radix);
  int32 x32 = safe_cast_s32(x64);
  return x32;
}

public static B32 try_u64_from_str8_c_rules(String8 string_, uint64* x){
  B32 is_integer = 0;
  if (str8_is_integer(string_, 10)){
    is_integer = 1;
    *x = u64_from_str8(string_, 10);
  }
  else{
    String8 hex_string = str8_skip(string_, 2);
    if (str8_match(str8_prefix(string_, 2), ("0x"), 0) &&
        str8_is_integer(hex_string, 0x10)){
      is_integer = 1;
      *x = u64_from_str8(hex_string, 0x10);
    }
    else if (str8_match(str8_prefix(string_, 2), ("0b"), 0) &&
             str8_is_integer(hex_string, 2)){
      is_integer = 1;
      *x = u64_from_str8(hex_string, 2);
    }
    else{
      String8 oct_string = str8_skip(string_, 1);
      if (str8_match(str8_prefix(string_, 1), ("0"), 0) &&
          str8_is_integer(hex_string, 010)){
        is_integer = 1;
        *x = u64_from_str8(oct_string, 010);
      }
    }
  }
  return(is_integer);
}

public static B32 try_s64_from_str8_c_rules(String8 string_, int64* x){
  String8 string_tail = default;
  int64 sign = sign_from_str8(string_, &string_tail);
  uint64 x_u64 = 0;
  B32 is_integer = try_u64_from_str8_c_rules(string_tail, &x_u64);
  *x = x_u64*sign;
  return(is_integer);
}

//- rjf: integer -> string_

public static String8 str8_from_memory_size(Arena* arena, uint64 size)
{
  String8 result;

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

public static String8 str8_from_count(Arena* arena, uint64 count)
{
  String8 result;

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

public static String8 str8_from_bits_u32(Arena* arena, uint32 x)
{
  uint8 c0 = 'a' + ((x >> 28) & 0xf);
  uint8 c1 = 'a' + ((x >> 24) & 0xf);
  uint8 c2 = 'a' + ((x >> 20) & 0xf);
  uint8 c3 = 'a' + ((x >> 16) & 0xf);
  uint8 c4 = 'a' + ((x >> 12) & 0xf);
  uint8 c5 = 'a' + ((x >>  8) & 0xf);
  uint8 c6 = 'a' + ((x >>  4) & 0xf);
  uint8 c7 = 'a' + ((x >>  0) & 0xf);
  String8 result = push_str8f(arena, "%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7);
  return result;
}

public static String8 str8_from_bits_u64(Arena* arena, uint64 x)
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
  String8 result = push_str8f(arena,
                              "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                              c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf);
  return result;
}

public static String8 str8_from_u64(Arena* arena, uint64 u64, uint32 radix, uint8 min_digits, uint8 digit_group_separator)
{
  String8 result = default;
  {
    // rjf: prefix
    String8 prefix = default;
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
      case 2, 8, 16:
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
      result.str = push_array_no_zero(arena, uint8, result.size + 1);
      result.str[result.size] = 0;
    }
    
    // rjf: fill contents
    {
      uint64 u64_reduce = u64;
      uint64 digits_until_separator = digit_group_size;
      for(uint64 idx = 0; idx < result.size; idx += 1)
      {
        if(digits_until_separator == 0 && digit_group_separator != 0)
        {
          result.str[result.size - idx - 1] = digit_group_separator;
          digits_until_separator = digit_group_size+1;
        }
        else
        {
          result.str[result.size - idx - 1] = char_to_lower(integer_symbols[u64_reduce%radix]);
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
        result.str[prefix.size + leading_0_idx] = '0';
      }
    }
    
    // rjf: fill prefix
    if(prefix.size != 0)
    {
      MemoryCopy(result.str, prefix.str, prefix.size);
    }
  }
  return result;
}

public static String8 str8_from_s64(Arena* arena, int64 s64, uint32 radix, uint8 min_digits, uint8 digit_group_separator)
{
  String8 result = default;
  // TODO(rjf): preeeeetty sloppy...
  if(s64 < 0)
  {
    Temp scratch = scratch_begin(&arena, 1);
    String8 numeric_part = str8_from_u64(scratch.arena, (uint64)(-s64), radix, min_digits, digit_group_separator);
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

public static float f64_from_str8(String8 string_)
{
  // TODO(rjf): crappy implementation for now that just uses atof.
  float result = 0;
  if(string_.size > 0)
  {
    // rjf: find starting pos of numeric string_, as well as sign
    float sign = +1.0;
    if(string_.str[0] == '-')
    {
      sign = -1.0;
    }
    else if(string_.str[0] == '+')
    {
      sign = 1.0;
    }
    
    // rjf: gather numerics
    uint64 num_valid_chars = 0;
    char buffer[64];
    B32 exp = 0;
    for(uint64 idx = 0; idx < string_.size && num_valid_chars < sizeof(buffer)-1; idx += 1)
    {
      if(char_is_digit(string_.str[idx], 10) || string_.str[idx] == '.' || string_.str[idx] == 'e' ||
         (exp && (string_.str[idx] == '+' || string_.str[idx] == '-')))
      {
        buffer[num_valid_chars] = string_.str[idx];
        num_valid_chars += 1;
        exp = 0;
        exp = (string_.str[idx] == 'e');
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

public static String8Node* str8_list_push_node(String8List* list, String8Node* node){
  SLLQueuePush(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += node.string_.size;
  return(node);
}

public static String8Node* str8_list_push_node_set_string(String8List* list, String8Node* node, String8 string_){
  SLLQueuePush(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += string_.size;
  node.string_ = string_;
  return(node);
}

public static String8Node* str8_list_push_node_front(String8List* list, String8Node* node){
  SLLQueuePushFront(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += node.string_.size;
  return(node);
}

public static String8Node* str8_list_push_node_front_set_string(String8List* list, String8Node* node, String8 string_){
  SLLQueuePushFront(list.first, list.last, node);
  list.node_count += 1;
  list.total_size += string_.size;
  node.string_ = string_;
  return(node);
}

public static String8Node* str8_list_push(Arena* arena, String8List* list, String8 string_){
  String8Node* node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_set_string(list, node, string_);
  return(node);
}

public static String8Node* str8_list_push_front(Arena* arena, String8List* list, String8 string_){
  String8Node* node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_front_set_string(list, node, string_);
  return(node);
}

public static void str8_list_concat_in_place(String8List* list, String8List* to_push){
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

public static String8Node* str8_list_push_aligner(Arena* arena, String8List* list, uint64 min, uint64 align){
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
  static uint8[64] zeroes_buffer = default;
  Assert(increase_size <= ArrayCount(zeroes_buffer));
  SLLQueuePush(list.first, list.last, node);
  list.node_count += 1;
  list.total_size = new_size;
  node.string_.str = (uint8*)zeroes_buffer;
  node.string_.size = increase_size;
  return(node);
}

public static String8Node* str8_list_pushf(Arena* arena, String8List* list, char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  String8 string_ = push_str8fv(arena, fmt, args);
  String8Node* result = str8_list_push(arena, list, string_);
  va_end(args);
  return(result);
}

public static String8Node* str8_list_push_frontf(Arena* arena, String8List* list, char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  String8 string_ = push_str8fv(arena, fmt, args);
  String8Node* result = str8_list_push_front(arena, list, string_);
  va_end(args);
  return(result);
}

public static String8List str8_list_copy(Arena* arena, String8List* list){
  String8List result = default;
  for (String8Node* node = list.first;
       node != 0;
       node = node.next){
    String8Node* new_node = push_array_no_zero(arena, String8Node, 1);
    String8 new_string = push_str8_copy(arena, node.string_);
    str8_list_push_node_set_string(&result, new_node, new_string);
  }
  return(result);
}

public static String8List str8_split(Arena* arena, String8 string_, uint8* split_chars, uint64 split_char_count, StringSplitFlags flags){
  String8List list = default;
  
  B32 keep_empties = (flags & StringSplitFlags.KeepEmpties);
  
  uint8* ptr = string_.str;
  uint8* opl = string_.str + string_.size;
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
    
    String8 string_ = str8_range(first, ptr);
    if (keep_empties || string_.size > 0){
      str8_list_push(arena, &list, string_);
    }
    ptr += 1;
  }
  
  return(list);
}

public static String8List str8_split_by_string_chars(Arena* arena, String8 string_, String8 split_chars, StringSplitFlags flags){
  String8List list = str8_split(arena, string_, split_chars.str, split_chars.size, flags);
  return list;
}

public static String8List str8_list_split_by_string_chars(Arena* arena, String8List list, String8 split_chars, StringSplitFlags flags){
  String8List result = default;
  for (String8Node* node = list.first; node != 0; node = node.next){
    String8List split = str8_split_by_string_chars(arena, node.string_, split_chars, flags);
    str8_list_concat_in_place(&result, &split);
  }
  return result;
}

public static String8 str8_list_join(Arena* arena, String8List* list, StringJoin* optional_params){
  StringJoin join = default;
  if (optional_params != 0){
    MemoryCopyStruct(&join, optional_params);
  }
  
  uint64 sep_count = 0;
  if (list.node_count > 0){
    sep_count = list.node_count - 1;
  }
  
  String8 result;
  result.size = join.pre.size + join.post.size + sep_count*join.sep.size + list.total_size;
  uint8* ptr = result.str = push_array_no_zero(arena, uint8, result.size + 1);
  
  MemoryCopy(ptr, join.pre.str, join.pre.size);
  ptr += join.pre.size;
  for (String8Node* node = list.first;
       node != 0;
       node = node.next){
    MemoryCopy(ptr, node.string_.str, node.string_.size);
    ptr += node.string_.size;
    if (node.next != 0){
      MemoryCopy(ptr, join.sep.str, join.sep.size);
      ptr += join.sep.size;
    }
  }
  MemoryCopy(ptr, join.post.str, join.post.size);
  ptr += join.post.size;
  
  *ptr = 0;
  
  return(result);
}

public static void str8_list_from_flags(Arena* arena, String8List* list,
                     uint32 flags, String8* flag_string_table, uint32 flag_string_count){
  for (uint32 i = 0; i < flag_string_count; i += 1){
    uint32 flag = (1 << i);
    if (flags & flag){
      str8_list_push(arena, list, flag_string_table[i]);
    }
  }
}

////////////////////////////////
//~ rjf; String Arrays

public static String8Array str8_array_from_list(Arena* arena, String8List* list)
{
  String8Array array;
  array.count   = list.node_count;
  array.v = push_array_no_zero(arena, String8, array.count);
  uint64 idx = 0;
  for(String8Node* n = list.first; n != 0; n = n.next, idx += 1)
  {
    array.v[idx] = n.string_;
  }
  return array;
}

public static String8Array str8_array_reserve(Arena* arena, uint64 count)
{
  String8Array arr;
  arr.count = 0;
  arr.v = push_array(arena, String8, count);
  return arr;
}

////////////////////////////////
//~ rjf: String Path Helpers

public static String8 str8_chop_last_slash(String8 string_){
  if (string_.size > 0){
    uint8* ptr = string_.str + string_.size - 1;
    for (;ptr >= string_.str; ptr -= 1){
      if (*ptr == '/' || *ptr == '\\'){
        break;
      }
    }
    if (ptr >= string_.str){
      string_.size = (uint64)(ptr - string_.str);
    }
    else{
      string_.size = 0;
    }
  }
  return(string_);
}

public static String8 str8_skip_last_slash(String8 string_){
  if (string_.size > 0){
    uint8* ptr = string_.str + string_.size - 1;
    for (;ptr >= string_.str; ptr -= 1){
      if (*ptr == '/' || *ptr == '\\'){
        break;
      }
    }
    if (ptr >= string_.str){
      ptr += 1;
      string_.size = (uint64)(string_.str + string_.size - ptr);
      string_.str = ptr;
    }
  }
  return(string_);
}

public static String8 str8_chop_last_dot(String8 string_)
{
  String8 result = string_;
  uint64 p = string_.size;
  for (;p > 0;){
    p -= 1;
    if (string_.str[p] == '.'){
      result = str8_prefix(string_, p);
      break;
    }
  }
  return(result);
}

public static String8 str8_skip_last_dot(String8 string_){
  String8 result = string_;
  uint64 p = string_.size;
  for (;p > 0;){
    p -= 1;
    if (string_.str[p] == '.'){
      result = str8_skip(string_, p + 1);
      break;
    }
  }
  return(result);
}

public static PathStyle path_style_from_str8(String8 string_){
  PathStyle result = PathStyle.Relative;
  if (string_.size >= 1 && string_.str[0] == '/'){
    result = PathStyle.UnixAbsolute;
  }
  else if (string_.size >= 2 &&
           char_is_alpha(string_.str[0]) &&
           string_.str[1] == ':'){
    if (string_.size == 2 ||
        char_is_slash(string_.str[2])){
      result = PathStyle.WindowsAbsolute;
    }
  }
  return(result);
}

public static String8List str8_split_path(Arena* arena, String8 string_){
  String8List result = str8_split(arena, string_, (uint8*)"/\\", 2, 0);
  return(result);
}

public static void str8_path_list_resolve_dots_in_place(String8List* path, PathStyle style){
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
    if (node == first && style == PathStyle.WindowsAbsolute){
      goto save_without_stack;
    }
    if (node.string_.size == 1 && node.string_.str[0] == '.'){
      goto do_nothing;
    }
    if (node.string_.size == 2 && node.string_.str[0] == '.' && node.string_.str[1] == '.'){
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
      path.total_size -= stack.node.string_.size;
      
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

public static String8 str8_path_list_join_by_style(Arena* arena, String8List* path, PathStyle style){
  StringJoin params = default;
  switch(style)
  {
    case PathStyle.Null:{}break;
    case PathStyle.Relative, PathStyle.WindowsAbsolute:
    {
      params.sep = ("/");
    }break;
    
    case PathStyle.UnixAbsolute:
    {
      params.pre = ("/");
      params.sep = ("/");
    }break;
  }
  String8 result = str8_list_join(arena, path, &params);
  return result;
}

public static String8TxtPtPair str8_txt_pt_pair_from_string(String8 string_)
{
  String8TxtPtPair pair = default;
  {
    String8 file_part = default;
    String8 line_part = default;
    String8 col_part = default;
    
    // rjf: grab file part
    for(uint64 idx = 0; idx <= string_.size; idx += 1)
    {
      uint8 byte = (idx < string_.size) ? (string_.str[idx]) : 0;
      uint8 next_byte = ((idx+1 < string_.size) ? (string_.str[idx+1]) : 0);
      if(byte == ':' && next_byte != '/' && next_byte != '\\')
      {
        file_part = str8_prefix(string_, idx);
        line_part = str8_skip(string_, idx+1);
        break;
      }
      else if(byte == 0)
      {
        file_part = string_;
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
    pair.string_ = file_part;
    pair.pt = txt_pt((int64)line, (int64)column);
    if(pair.pt.line == 0) { pair.pt.line = 1; }
    if(pair.pt.column == 0) { pair.pt.column = 1; }
  }
  return pair;
}

////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding

static readonly uint8[32] utf8_class = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

public static UnicodeDecode utf8_decode(uint8* str, uint64 max){
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

public static UnicodeDecode utf16_decode(uint16* str, uint64 max){
  UnicodeDecode result = {1, max_U32};
  result.codepoint = str[0];
  result.inc = 1;
  if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000){
    result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
    result.inc = 2;
  }
  return(result);
}

public static uint32 utf8_encode(uint8* str, uint32 codepoint){
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

public static uint32 utf16_encode(uint16* str, uint32 codepoint){
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

public static uint32 utf8_from_utf32_single(uint8* buffer, uint32 character){
  return(utf8_encode(buffer, character));
}

////////////////////////////////
//~ rjf: Unicode String Conversions

public static String8 str8_from_16(Arena* arena, String16 input)
{
  String8 result = str8_zero();
  if(input.size)
  {
    uint64 cap = input.size*3;
    uint8* str = push_array_no_zero(arena, uint8, cap + 1);
    uint16* ptr = input.str;
    uint16* opl = ptr + input.size;
    uint64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf16_decode(ptr, opl - ptr);
      size += utf8_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    result = str8(str, size);
  }
  return result;
}

public static String16 str16_from_8(Arena* arena, String8 input)
{
  String16 result = str16_zero();
  if(input.size)
  {
    uint64 cap = input.size*2;
    uint16* str = push_array_no_zero(arena, uint16, cap + 1);
    uint8* ptr = input.str;
    uint8* opl = ptr + input.size;
    uint64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf8_decode(ptr, opl - ptr);
      size += utf16_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size)*2);
    result = str16(str, size);
  }
  return result;
}

public static String8 str8_from_32(Arena* arena, String32 input)
{
  String8 result = str8_zero();
  if(input.size)
  {
    uint64 cap = input.size*4;
    uint8* str = push_array_no_zero(arena, uint8, cap + 1);
    uint32* ptr = input.str;
    uint32* opl = ptr + input.size;
    uint64 size = 0;
    for(;ptr < opl; ptr += 1)
    {
      size += utf8_encode(str + size, *ptr);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    result = str8(str, size);
  }
  return result;
}

public static String32 str32_from_8(Arena* arena, String8 input)
{
  String32 result = str32_zero(); 
  if(input.size)
  {
    uint64 cap = input.size;
    uint32* str = push_array_no_zero(arena, uint32, cap + 1);
    uint8* ptr = input.str;
    uint8* opl = ptr + input.size;
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
    result = str32(str, size);
  }
  return result;
}

////////////////////////////////
//~ String -> Enum Conversions

static readonly 
(
  String8         string_,
  OperatingSystem os,
)[] g_os_enum_map =
{
  ( (""),        OperatingSystem_Null     ),
  ( ("Windows"), OperatingSystem_Windows, ),
  ( ("Linux"),   OperatingSystem_Linux,   ),
  ( ("Mac"),     OperatingSystem_Mac,     ),
};
StaticAssert(ArrayCount(g_os_enum_map) == OperatingSystem_COUNT, g_os_enum_map_count_check);

public static OperatingSystem operating_system_from_string(String8 string_)
{
  for(uint64 i = 0; i < ArrayCount(g_os_enum_map); ++i)
  {
    if(str8_match(g_os_enum_map[i].string_, string_, StringMatchFlags.CaseInsensitive))
    {
      return g_os_enum_map[i].os;
    }
  }
  return OperatingSystem_Null;
}

////////////////////////////////
//~ rjf: Basic Types & Space Enum -> String Conversions

public static String8 string_from_dimension(Dimension dimension){
  static String8[] strings = {
    ("X"),
    ("Y"),
    ("Z"),
    ("W"),
  };
  String8 result = ("error");
  if ((uint32)dimension < 4){
    result = strings[dimension];
  }
  return(result);
}

public static String8 string_from_side(Side side){
  static String8[] strings = {
    ("Min"),
    ("Max"),
  };
  String8 result = ("error");
  if ((uint32)side < 2){
    result = strings[side];
  }
  return(result);
}

public static String8 string_from_operating_system(OperatingSystem os)
{
  String8 result = g_os_enum_map[OperatingSystem_Null].string_;
  if(os < ArrayCount(g_os_enum_map))
  {
    result = g_os_enum_map[os].string_;
  }
  return result;
}

public static String8 string_from_arch(Arch arch){
  static String8[] strings = {
    ("Null"),
    ("x64"),
    ("x86"),
    ("arm64"),
    ("arm32"),
  };
  String8 result = ("error");
  if (arch < Arch_COUNT){
    result = strings[arch];
  }
  return(result);
}

////////////////////////////////
//~ rjf: Time Types -> String

public static String8 string_from_week_day(WeekDay week_day){
  static String8[] strings = {
    ("Sun"),
    ("Mon"),
    ("Tue"),
    ("Wed"),
    ("Thu"),
    ("Fri"),
    ("Sat"),
  };
  String8 result = ("Err");
  if ((uint32)week_day < WeekDay_COUNT){
    result = strings[week_day];
  }
  return(result);
}

public static String8 string_from_month(Month month){
  static String8[] strings = {
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
  String8 result = ("Err");
  if ((uint32)month < Month_COUNT){
    result = strings[month];
  }
  return(result);
}

public static String8 push_date_time_string(Arena* arena, DateTime* date_time){
  char* mon_str = (char*)string_from_month(date_time.month).str;
  uint32 adjusted_hour = date_time.hour%12;
  if (adjusted_hour == 0){
    adjusted_hour = 12;
  }
  char* ampm = "am";
  if (date_time.hour >= 12){
    ampm = "pm";
  }
  String8 result = push_str8f(arena, "%d %s %d, %02d:%02d:%02d %s",
                              date_time.day, mon_str, date_time.year,
                              adjusted_hour, date_time.min, date_time.sec, ampm);
  return(result);
}

public static String8 push_file_name_date_time_string(Arena* arena, DateTime* date_time){
  char* mon_str = (char*)string_from_month(date_time.month).str;
  String8 result = push_str8f(arena, "%d-%s-%0d--%02d-%02d-%02d",
                              date_time.year, mon_str, date_time.day,
                              date_time.hour, date_time.min, date_time.sec);
  return(result);
}

public static String8 string_from_elapsed_time(Arena* arena, DateTime dt){
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = default;
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
  String8 result = str8_list_join(arena, &list, &join);
  scratch_end(scratch);
  return(result);
}

////////////////////////////////
//~ Globally UNique Ids

public static String8 string_from_guid(Arena* arena, Guid guid)
{
  String8 result = push_str8f(arena, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
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

public static B32 try_guid_from_string(String8 string_, Guid* guid_out)
{
  Temp scratch = scratch_begin(0,0);
  B32 is_parsed = 0;
  String8List list = str8_split_by_string_chars(scratch.arena, string_, ("-"), StringSplitFlags.KeepEmpties);
  if(list.node_count == 5)
  {
    String8 data1_str    = list.first.string_;
    String8 data2_str    = list.first.next.string_;
    String8 data3_str    = list.first.next.next.string_;
    String8 data4_hi_str = list.first.next.next.next.string_;
    String8 data4_lo_str = list.first.next.next.next.next.string_;
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

public static Guid guid_from_string(String8 string_)
{
  Guid guid = default;
  try_guid_from_string(string_, &guid);
  return guid;
}

////////////////////////////////
//~ rjf: Basic Text Indentation

public static String8 indented_from_string(Arena* arena, String8 string_)
{
  Temp scratch = scratch_begin(&arena, 1);
  static readonly uint8[] indentation_bytes = "                                                                                                                                ";
  String8List indented_strings = default;
  int64 depth = 0;
  int64 next_depth = 0;
  uint64 line_begin_off = 0;
  for(uint64 off = 0; off <= string_.size; off += 1)
  {
    uint8 byte = off<string_.size ? string_.str[off] : 0;
    switch(byte)
    {
      default:{}break;
      case '{', '[', '(':{next_depth += 1; next_depth = Max(0, next_depth);}break;
      case '}', ']', ')':{next_depth -= 1; next_depth = Max(0, next_depth); depth = next_depth;}break;
      case '\n', 0:
      {
        String8 line = str8_skip_chop_whitespace(str8_substr(string_, r1u64(line_begin_off, off)));
        if(line.size != 0)
        {
          str8_list_pushf(scratch.arena, &indented_strings, "%.*s%S\n", (int)depth*2, indentation_bytes, line);
        }
        if(line.size == 0 && indented_strings.node_count != 0 && off < string_.size)
        {
          str8_list_pushf(scratch.arena, &indented_strings, "\n");
        }
        line_begin_off = off+1;
        depth = next_depth;
      }break;
    }
  }
  String8 result = str8_list_join(arena, &indented_strings, 0);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Text Escaping

public static String8 escaped_from_raw_str8(Arena* arena, String8 string_)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List parts = default;
  uint64 start_split_idx = 0;
  for(uint64 idx = 0; idx <= string_.size; idx += 1)
  {
    uint8 byte = (idx < string_.size) ? string_.str[idx] : 0;
    B32 split = 1;
    String8 separator_replace = default;
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
      String8 substr = str8_substr(string_, r1u64(start_split_idx, idx));
      start_split_idx = idx+1;
      str8_list_push(scratch.arena, &parts, substr);
      if(separator_replace.size != 0)
      {
        str8_list_push(scratch.arena, &parts, separator_replace);
      }
    }
  }
  StringJoin join = default;
  String8 result = str8_list_join(arena, &parts, &join);
  scratch_end(scratch);
  return result;
}

public static String8 raw_from_escaped_str8(Arena* arena, String8 string_)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List strs = default;
  uint64 start = 0;
  for(uint64 idx = 0; idx <= string_.size; idx += 1)
  {
    if(idx == string_.size || string_.str[idx] == '\\' || string_.str[idx] == '\r')
    {
      String8 str = str8_substr(string_, r1u64(start, idx));
      if(str.size != 0)
      {
        str8_list_push(scratch.arena, &strs, str);
      }
      start = idx+1;
    }
    if(idx < string_.size && string_.str[idx] == '\\')
    {
      uint8 next_char = string_.str[idx+1];
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
      String8 replace_string = push_str8_copy(scratch.arena, str8(&replace_byte, 1));
      str8_list_push(scratch.arena, &strs, replace_string);
      idx += 1;
      start += 1;
    }
  }
  String8 result = str8_list_join(arena, &strs, 0);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Text Wrapping

public static String8List wrapped_lines_from_string(Arena* arena, String8 string_, uint64 first_line_max_width, uint64 max_width, uint64 wrap_indent)
{
  String8List list = default;
  Rng1U64 line_range = r1u64(0, 0);
  uint64 wrapped_indent_level = 0;
  static char* spaces = "                                                                ";
  for (uint64 idx = 0; idx <= string_.size; idx += 1){
    uint8 chr = idx < string_.size ? string_.str[idx] : 0;
    if (chr == '\n'){
      Rng1U64 candidate_line_range = line_range;
      candidate_line_range.max = idx;
      // NOTE(nick): when wrapping is interrupted with \n we emit a string_ without including \n
      // because later tool_fprint_list inserts separator after each node
      // except for last node, so don't strip last \n.
      if (idx + 1 == string_.size){
        candidate_line_range.max += 1;
      }
      String8 substr = str8_substr(string_, candidate_line_range);
      str8_list_push(arena, &list, substr);
      line_range = r1u64(idx+1,idx+1);
    }
    else
      if (char_is_space(chr) || chr == 0){
      Rng1U64 candidate_line_range = line_range;
      candidate_line_range.max = idx;
      String8 substr = str8_substr(string_, candidate_line_range);
      uint64 width_this_line = max_width-wrapped_indent_level;
      if (list.node_count == 0){
        width_this_line = first_line_max_width;
      }
      if (substr.size > width_this_line){
        String8 line = str8_substr(string_, line_range);
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
  if (line_range.min < string_.size && line_range.max > line_range.min){
    String8 line = str8_substr(string_, line_range);
    if (wrapped_indent_level > 0){
      line = push_str8f(arena, "%.*s%S", wrapped_indent_level, spaces, line);
    }
    str8_list_push(arena, &list, line);
  }
  return list;
}

////////////////////////////////
//~ rjf: String <-> Color

public static String8 hex_string_from_rgba_4f32(Arena* arena, Vec4F32 rgba)
{
  String8 hex_string = push_str8f(arena, "%02x%02x%02x%02x", (uint8)(rgba.x*255.f), (uint8)(rgba.y*255.f), (uint8)(rgba.z*255.f), (uint8)(rgba.w*255.f));
  return hex_string;
}

public static Vec4F32 rgba_from_hex_string_4f32(String8 hex_string)
{
  uint8 byte_text[8] = default;
  uint64 byte_text_idx = 0;
  for(uint64 idx = 0; idx < hex_string.size && byte_text_idx < ArrayCount(byte_text); idx += 1)
  {
    if(char_is_digit(hex_string.str[idx], 16))
    {
      byte_text[byte_text_idx] = char_to_lower(hex_string.str[idx]);
      byte_text_idx += 1;
    }
  }
  uint8 byte_vals[4] = default;
  for(uint64 idx = 0; idx < 4; idx += 1)
  {
    byte_vals[idx] = (uint8)u64_from_str8(str8(&byte_text[idx*2], 2), 16);
  }
  Vec4F32 rgba = v4f32(byte_vals[0]/255.f, byte_vals[1]/255.f, byte_vals[2]/255.f, byte_vals[3]/255.f);
  return rgba;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching

public static FuzzyMatchRangeList fuzzy_match_find(Arena* arena, String8 needle, String8 haystack)
{
  FuzzyMatchRangeList result = default;
  Temp scratch = scratch_begin(&arena, 1);
  String8List needles = str8_split(scratch.arena, needle, (uint8*)" ", 1, 0);
  result.needle_part_count = needles.node_count;
  for(String8Node* needle_n = needles.first; needle_n != 0; needle_n = needle_n.next)
  {
    uint64 find_pos = 0;
    for(;find_pos < haystack.size;)
    {
      find_pos = str8_find_needle(haystack, find_pos, needle_n.string_, StringMatchFlags.CaseInsensitive);
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
      Rng1U64 range = r1u64(find_pos, find_pos+needle_n.string_.size);
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

public static FuzzyMatchRangeList fuzzy_match_range_list_copy(Arena* arena, FuzzyMatchRangeList* src)
{
  FuzzyMatchRangeList dst = default;
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

public static void str8_serial_begin(Arena* arena, String8List* srl){
  String8Node* node = push_array(arena, String8Node, 1);
  node.string_.str = push_array_no_zero(arena, uint8, 0);
  srl.first = srl.last = node;
  srl.node_count = 1;
  srl.total_size = 0;
}

public static String8 str8_serial_end(Arena* arena, String8List* srl){
  uint64 size = srl.total_size;
  uint8* output = push_array_no_zero(arena, uint8, size);
  str8_serial_write_to_dst(srl, output);
  String8 result = str8(output, size);
  return result;
}

public static void str8_serial_write_to_dst(String8List* srl, void* output){
  uint8* ptr = (uint8*)output;
  for (String8Node* node = srl.first;
       node != 0;
       node = node.next){
    uint64 size = node.string_.size;
    MemoryCopy(ptr, node.string_.str, size);
    ptr += size;
  }
}

public static uint64 str8_serial_push_align(Arena* arena, String8List* srl, uint64 align){
  Assert(IsPow2(align));
  
  uint64 pos = srl.total_size;
  uint64 new_pos = AlignPow2(pos, align);
  uint64 size = (new_pos - pos);
  
  if(size != 0)
  {
    uint8* buf = push_array(arena, uint8, size);
    
    String8* str = &srl.last.string_;
    if (str.str + str.size == buf){
      srl.last.string_.size += size;
      srl.total_size += size;
    }
    else{
      str8_list_push(arena, srl, str8(buf, size));
    }
  }
  return size;
}

public static void* str8_serial_push_size(Arena* arena, String8List* srl, uint64 size)
{
  void* result = 0;
  if(size != 0)
  {
    uint8* buf = push_array_no_zero(arena, uint8, size);
    String8* str = &srl.last.string_;
    if (str.str + str.size == buf){
      srl.last.string_.size += size;
      srl.total_size += size;
    }
    else{
      str8_list_push(arena, srl, str8(buf, size));
    }
    result = buf;
  }
  return result;
}

public static void* str8_serial_push_data(Arena* arena, String8List* srl, void* data, uint64 size){
  void* result = str8_serial_push_size(arena, srl, size);
  if(result != 0)
  {
    MemoryCopy(result, data, size);
  }
  return result;
}

public static void str8_serial_push_data_list(Arena* arena, String8List* srl, String8Node* first){
  for (String8Node* node = first;
       node != 0;
       node = node.next){
    str8_serial_push_data(arena, srl, node.string_.str, node.string_.size);
  }
}

public static void str8_serial_push_u64(Arena* arena, String8List* srl, uint64 x){
  uint8* buf = push_array_no_zero(arena, uint8, 8);
  MemoryCopy(buf, &x, 8);
  String8* str = &srl.last.string_;
  if (str.str + str.size == buf){
    srl.last.string_.size += 8;
    srl.total_size += 8;
  }
  else{
    str8_list_push(arena, srl, str8(buf, 8));
  }
}

public static void str8_serial_push_u32(Arena* arena, String8List* srl, uint32 x){
  uint8* buf = push_array_no_zero(arena, uint8, 4);
  MemoryCopy(buf, &x, 4);
  String8* str = &srl.last.string_;
  if (str.str + str.size == buf){
    srl.last.string_.size += 4;
    srl.total_size += 4;
  }
  else{
    str8_list_push(arena, srl, str8(buf, 4));
  }
}

public static void str8_serial_push_u16(Arena* arena, String8List* srl, uint16 x){
  str8_serial_push_data(arena, srl, &x, sizeof(x));
}

public static void str8_serial_push_u8(Arena* arena, String8List* srl, uint8 x){
  str8_serial_push_data(arena, srl, &x, sizeof(x));
}

public static void str8_serial_push_cstr(Arena* arena, String8List* srl, String8 str){
  str8_serial_push_data(arena, srl, str.str, str.size);
  str8_serial_push_u8(arena, srl, 0);
}

public static void str8_serial_push_string(Arena* arena, String8List* srl, String8 str){
  str8_serial_push_data(arena, srl, str.str, str.size);
}

////////////////////////////////
//~ rjf: Deserialization Helpers

public static uint64 str8_deserial_read(String8 string_, uint64 off, void* read_dst, uint64 read_size, uint64 granularity)
{
  uint64 bytes_left = string_.size-Min(off, string_.size);
  uint64 actually_readable_size = Min(bytes_left, read_size);
  uint64 legally_readable_size = actually_readable_size - actually_readable_size%granularity;
  if(legally_readable_size > 0)
  {
    MemoryCopy(read_dst, string_.str+off, legally_readable_size);
  }
  return legally_readable_size;
}

public static uint64 str8_deserial_find_first_match(String8 string_, uint64 off, uint16 scan_val)
{
  uint64 cursor = off;
  for (;;) {
    uint16 val = 0;
    str8_deserial_read_struct(string_, cursor, &val);
    if (val == scan_val) {
      break;
    }
    cursor += sizeof(val);
  }
  return cursor;
}

public static void* str8_deserial_get_raw_ptr(String8 string_, uint64 off, uint64 size)
{
  void* raw_ptr = 0;
  if (off + size <= string_.size) {
    raw_ptr = string_.str + off;
  }
  return raw_ptr;
}

public static uint64 str8_deserial_read_cstr(String8 string_, uint64 off, String8* cstr_out)
{
  uint64 cstr_size = 0;
  if (off < string_.size) {
    uint8* ptr = string_.str + off;
    uint8* cap = string_.str + string_.size;
    *cstr_out = str8_cstring_capped(ptr, cap);
    cstr_size = (cstr_out.size + 1);
  }
  return cstr_size;
}

public static uint64 str8_deserial_read_windows_utf16_string16(String8 string_, uint64 off, String16* str_out)
{
  uint64 null_off = str8_deserial_find_first_match(string_, off, 0);
  uint64 size = null_off - off;
  uint16* str = (uint16 *)str8_deserial_get_raw_ptr(string_, off, size);
  uint64 count = size / sizeof(*str);
  *str_out = str16(str, count);
  
  uint64 read_size_with_null = size + sizeof(*str);
  return read_size_with_null;
}

public static uint64 str8_deserial_read_block(String8 string_, uint64 off, uint64 size, String8* block_out)
{
  Rng1U64 range = rng_1u64(off, off + size);
  *block_out = str8_substr(string_, range);
  return block_out.size;
}

public static uint64 str8_deserial_read_uleb128(String8 string_, uint64 off, uint64* value_out)
{
  uint64 value  = 0;
  uint64 shift  = 0;
  uint64 cursor = off;
  for(;;)
  {
    uint8  byte       = 0;
    uint64 bytes_read = str8_deserial_read_struct(string_, cursor, &byte);

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

public static uint64 str8_deserial_read_sleb128(String8 string_, uint64 off, int64* value_out)
{
  uint64 value  = 0;
  uint64 shift  = 0;
  uint64 cursor = off;
  for(;;)
  {
    uint8 byte;
    uint64 bytes_read = str8_deserial_read_struct(string_, cursor, &byte);
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


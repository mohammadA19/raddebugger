// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

////////////////////////////////
//~ rjf: Third Party Includes

#define STB_SPRINTF_DECORATE(name) raddbg_##name
#include "third_party/stb/stb_sprintf.h"

////////////////////////////////
//~ rjf: String Types

typedef struct String8 String8;
struct String8
{
  byte *str;
  ulong size;
}

typedef struct String16 String16;
struct String16
{
  ushort *str;
  ulong size;
}

typedef struct String32 String32;
struct String32
{
  uint *str;
  ulong size;
}

////////////////////////////////
//~ rjf: String List & Array Types

typedef struct String8Node String8Node;
struct String8Node
{
  String8Node *next;
  String8 string;
}

typedef struct String8MetaNode String8MetaNode;
struct String8MetaNode
{
  String8MetaNode *next;
  String8Node *node;
}

typedef struct String8List String8List;
struct String8List
{
  String8Node *first;
  String8Node *last;
  ulong node_count;
  ulong total_size;
}

typedef struct String8Array String8Array;
struct String8Array
{
  String8 *v;
  ulong count;
}

////////////////////////////////
//~ rjf: String Matching, Splitting, & Joining Types

typedef uint StringMatchFlags;
enum
{
  StringMatchFlag_CaseInsensitive  = (1 << 0),
  StringMatchFlag_RightSideSloppy  = (1 << 1),
  StringMatchFlag_SlashInsensitive = (1 << 2),
}

typedef uint StringSplitFlags;
enum
{
  StringSplitFlag_KeepEmpties = (1 << 0),
}

typedef enum PathStyle
{
  PathStyle_Null,
  PathStyle_Relative,
  PathStyle_WindowsAbsolute,
  PathStyle_UnixAbsolute,
  
#if OS_WINDOWS
  PathStyle_SystemAbsolute = PathStyle_WindowsAbsolute
#elif OS_LINUX
  PathStyle_SystemAbsolute = PathStyle_UnixAbsolute
#else
# error "absolute path style is undefined for this OS"
#endif
}
PathStyle;

typedef struct StringJoin StringJoin;
struct StringJoin
{
  String8 pre;
  String8 sep;
  String8 post;
}

////////////////////////////////
//~ rjf: String Pair Types

typedef struct String8TxtPtPair String8TxtPtPair;
struct String8TxtPtPair
{
  String8 string;
  TxtPt pt;
}

////////////////////////////////
//~ rjf: UTF Decoding Types

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode
{
  uint inc;
  uint codepoint;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching Types

typedef struct FuzzyMatchRangeNode FuzzyMatchRangeNode;
struct FuzzyMatchRangeNode
{
  FuzzyMatchRangeNode *next;
  Rng1U64 range;
}

typedef struct FuzzyMatchRangeList FuzzyMatchRangeList;
struct FuzzyMatchRangeList
{
  FuzzyMatchRangeNode *first;
  FuzzyMatchRangeNode *last;
  ulong count;
  ulong needle_part_count;
  ulong total_dim;
}

////////////////////////////////
//~ rjf: Character Classification & Conversion Functions

internal B32 char_is_space(byte c);
internal B32 char_is_upper(byte c);
internal B32 char_is_lower(byte c);
internal B32 char_is_alpha(byte c);
internal B32 char_is_slash(byte c);
internal B32 char_is_digit(byte c, uint base);
internal byte char_to_lower(byte c);
internal byte char_to_upper(byte c);
internal byte char_to_correct_slash(byte c);

////////////////////////////////
//~ rjf: C-String Measurement

internal ulong cstring8_length(byte *c);
internal ulong cstring16_length(ushort *c);
internal ulong cstring32_length(uint *c);

////////////////////////////////
//~ rjf: String Constructors

#define str8_lit(S)  str8((byte*)(S), sizeof(S) - 1)
#define str8_lit_comp(S) {(byte*)(S), sizeof(S) - 1,}
#define str8_varg(S) (int)((S).size), ((S).str)

#define str8_array(S,C) str8((byte*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((byte*)(S), sizeof(S))
#define str8_struct(S) str8((byte*)(S), sizeof(*(S)))

internal String8  str8(byte *str, ulong size);
internal String8  str8_range(byte *first, byte *one_past_last);
internal String8  str8_zero(void);
internal String16 str16(ushort *str, ulong size);
internal String16 str16_range(ushort *first, ushort *one_past_last);
internal String16 str16_zero(void);
internal String32 str32(uint *str, ulong size);
internal String32 str32_range(uint *first, uint *one_past_last);
internal String32 str32_zero(void);
internal String8  str8_cstring(char *c);
internal String16 str16_cstring(ushort *c);
internal String32 str32_cstring(uint *c);
internal String8  str8_cstring_capped(void *cstr, void *cap);
internal String16 str16_cstring_capped(void *cstr, void *cap);
internal String8  str8_cstring_capped_reverse(void *raw_start, void *raw_cap);

////////////////////////////////
//~ rjf: String Stylization

internal String8 upper_from_str8(Arena *arena, String8 string);
internal String8 lower_from_str8(Arena *arena, String8 string);
internal String8 backslashed_from_str8(Arena *arena, String8 string);

////////////////////////////////
//~ rjf: String Matching

#define str8_match_lit(a_lit, b, flags)   str8_match(str8_lit(a_lit), (b), (flags))
#define str8_match_cstr(a_cstr, b, flags) str8_match(str8_cstring(a_cstr), (b), (flags))
internal B32 str8_match(String8 a, String8 b, StringMatchFlags flags);
internal ulong str8_find_needle(String8 string, ulong start_pos, String8 needle, StringMatchFlags flags);
internal ulong str8_find_needle_reverse(String8 string, ulong start_pos, String8 needle, StringMatchFlags flags);
internal B32 str8_ends_with(String8 string, String8 end, StringMatchFlags flags);
#define str8_ends_with_lit(string, end_lit, flags) str8_ends_with((string), str8_lit(end_lit), (flags))

////////////////////////////////
//~ rjf: String Slicing

internal String8 str8_substr(String8 str, Rng1U64 range);
internal String8 str8_prefix(String8 str, ulong size);
internal String8 str8_skip(String8 str, ulong amt);
internal String8 str8_postfix(String8 str, ulong size);
internal String8 str8_chop(String8 str, ulong amt);
internal String8 str8_skip_chop_whitespace(String8 string);

////////////////////////////////
//~ rjf: String Formatting & Copying

internal String8 push_str8_cat(Arena *arena, String8 s1, String8 s2);
internal String8 push_str8_copy(Arena *arena, String8 s);
internal String8 push_str8fv(Arena *arena, char *fmt, va_list args);
internal String8 push_str8f(Arena *arena, char *fmt, ...);

////////////////////////////////
//~ rjf: String <=> Integer Conversions

//- rjf: string -> integer
internal long sign_from_str8(String8 string, String8 *string_tail);
internal B32 str8_is_integer(String8 string, uint radix);

internal ulong u64_from_str8(String8 string, uint radix);
internal long s64_from_str8(String8 string, uint radix);
internal uint u32_from_str8(String8 string, uint radix);
internal int s32_from_str8(String8 string, uint radix);
internal B32 try_u64_from_str8_c_rules(String8 string, ulong *x);
internal B32 try_s64_from_str8_c_rules(String8 string, long *x);

//- rjf: integer -> string
internal String8 str8_from_memory_size(Arena *arena, ulong size);
internal String8 str8_from_count(Arena *arena, ulong count);
internal String8 str8_from_bits_u32(Arena *arena, uint x);
internal String8 str8_from_bits_u64(Arena *arena, ulong x);
internal String8 str8_from_u64(Arena *arena, ulong u64, uint radix, byte min_digits, byte digit_group_separator);
internal String8 str8_from_s64(Arena *arena, long s64, uint radix, byte min_digits, byte digit_group_separator);

////////////////////////////////
//~ rjf: String <=> Float Conversions

internal double f64_from_str8(String8 string);

////////////////////////////////
//~ rjf: String List Construction Functions

internal String8Node* str8_list_push_node(String8List *list, String8Node *node);
internal String8Node* str8_list_push_node_set_string(String8List *list, String8Node *node, String8 string);
internal String8Node* str8_list_push_node_front(String8List *list, String8Node *node);
internal String8Node* str8_list_push_node_front_set_string(String8List *list, String8Node *node, String8 string);
internal String8Node* str8_list_push(Arena *arena, String8List *list, String8 string);
internal String8Node* str8_list_push_front(Arena *arena, String8List *list, String8 string);
internal void         str8_list_concat_in_place(String8List *list, String8List *to_push);
internal String8Node* str8_list_push_aligner(Arena *arena, String8List *list, ulong min, ulong align);
internal String8Node* str8_list_pushf(Arena *arena, String8List *list, char *fmt, ...);
internal String8Node* str8_list_push_frontf(Arena *arena, String8List *list, char *fmt, ...);
internal String8List  str8_list_copy(Arena *arena, String8List *list);
#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

////////////////////////////////
//~ rjf: String Splitting & Joining

internal String8List  str8_split(Arena *arena, String8 string, byte *split_chars, ulong split_char_count, StringSplitFlags flags);
internal String8List  str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags);
internal String8List  str8_list_split_by_string_chars(Arena *arena, String8List list, String8 split_chars, StringSplitFlags flags);
internal String8      str8_list_join(Arena *arena, String8List *list, StringJoin *optional_params);
internal void         str8_list_from_flags(Arena *arena, String8List *list, uint flags, String8 *flag_string_table, uint flag_string_count);

////////////////////////////////
//~ rjf; String Arrays

internal String8Array str8_array_from_list(Arena *arena, String8List *list);
internal String8Array str8_array_reserve(Arena *arena, ulong count);

////////////////////////////////
//~ rjf: String Path Helpers

internal String8 str8_chop_last_slash(String8 string);
internal String8 str8_skip_last_slash(String8 string);
internal String8 str8_chop_last_dot(String8 string);
internal String8 str8_skip_last_dot(String8 string);

internal PathStyle   path_style_from_str8(String8 string);
internal String8List str8_split_path(Arena *arena, String8 string);
internal void        str8_path_list_resolve_dots_in_place(String8List *path, PathStyle style);
internal String8     str8_path_list_join_by_style(Arena *arena, String8List *path, PathStyle style);

internal String8TxtPtPair str8_txt_pt_pair_from_string(String8 string);

////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding

internal UnicodeDecode utf8_decode(byte *str, ulong max);
internal UnicodeDecode utf16_decode(ushort *str, ulong max);
internal uint utf8_encode(byte *str, uint codepoint);
internal uint utf16_encode(ushort *str, uint codepoint);
internal uint utf8_from_utf32_single(byte *buffer, uint character);

////////////////////////////////
//~ rjf: Unicode String Conversions

internal String8 str8_from_16(Arena *arena, String16 in);
internal String16 str16_from_8(Arena *arena, String8 in);
internal String8 str8_from_32(Arena *arena, String32 in);
internal String32 str32_from_8(Arena *arena, String8 in);

////////////////////////////////
//~ String -> Enum Conversions

internal OperatingSystem operating_system_from_string(String8 string);

////////////////////////////////
//~ rjf: Basic Types & Space Enum -> String Conversions

internal String8 string_from_dimension(Dimension dimension);
internal String8 string_from_side(Side side);
internal String8 string_from_operating_system(OperatingSystem os);
internal String8 string_from_arch(Arch arch);

////////////////////////////////
//~ rjf: Time Types -> String

internal String8 string_from_week_day(WeekDay week_day);
internal String8 string_from_month(Month month);
internal String8 push_date_time_string(Arena *arena, DateTime *date_time);
internal String8 push_file_name_date_time_string(Arena *arena, DateTime *date_time);
internal String8 string_from_elapsed_time(Arena *arena, DateTime dt);

////////////////////////////////
//~ Globally Unique Ids

internal String8 string_from_guid(Arena *arena, Guid guid);
internal B32     try_guid_from_string(String8 string, Guid *guid_out);
internal Guid    guid_from_string(String8 string);

////////////////////////////////
//~ rjf: Basic Text Indentation

internal String8 indented_from_string(Arena *arena, String8 string);

////////////////////////////////
//~ rjf: Text Escaping

internal String8 escaped_from_raw_str8(Arena *arena, String8 string);
internal String8 raw_from_escaped_str8(Arena *arena, String8 string);

////////////////////////////////
//~ rjf: Text Wrapping

internal String8List wrapped_lines_from_string(Arena *arena, String8 string, ulong first_line_max_width, ulong max_width, ulong wrap_indent);

////////////////////////////////
//~ rjf: String <-> Color

internal String8 hex_string_from_rgba_4f32(Arena *arena, Vec4F32 rgba);
internal Vec4F32 rgba_from_hex_string_4f32(String8 hex_string);

////////////////////////////////
//~ rjf: String Fuzzy Matching

internal FuzzyMatchRangeList fuzzy_match_find(Arena *arena, String8 needle, String8 haystack);
internal FuzzyMatchRangeList fuzzy_match_range_list_copy(Arena *arena, FuzzyMatchRangeList *src);

////////////////////////////////
//~ NOTE(allen): Serialization Helpers

internal void    str8_serial_begin(Arena *arena, String8List *srl);
internal String8 str8_serial_end(Arena *arena, String8List *srl);
internal void    str8_serial_write_to_dst(String8List *srl, void *out);
internal ulong     str8_serial_push_align(Arena *arena, String8List *srl, ulong align);
internal void *  str8_serial_push_size(Arena *arena, String8List *srl, ulong size);
internal void *  str8_serial_push_data(Arena *arena, String8List *srl, void *data, ulong size);
internal void    str8_serial_push_data_list(Arena *arena, String8List *srl, String8Node *first);
internal void    str8_serial_push_u64(Arena *arena, String8List *srl, ulong x);
internal void    str8_serial_push_u32(Arena *arena, String8List *srl, uint x);
internal void    str8_serial_push_u16(Arena *arena, String8List *srl, ushort x);
internal void    str8_serial_push_u8(Arena *arena, String8List *srl, byte x);
internal void    str8_serial_push_cstr(Arena *arena, String8List *srl, String8 str);
internal void    str8_serial_push_string(Arena *arena, String8List *srl, String8 str);
#define str8_serial_push_array(arena, srl, ptr, count) str8_serial_push_data(arena, srl, ptr, sizeof(*(ptr)) * (count))
#define str8_serial_push_struct(arena, srl, ptr) str8_serial_push_array(arena, srl, ptr, 1)

////////////////////////////////
//~ rjf: Deserialization Helpers

internal ulong    str8_deserial_read(String8 string, ulong off, void *read_dst, ulong read_size, ulong granularity);
internal ulong    str8_deserial_find_first_match(String8 string, ulong off, ushort scan_val);
internal void * str8_deserial_get_raw_ptr(String8 string, ulong off, ulong size);
internal ulong    str8_deserial_read_cstr(String8 string, ulong off, String8 *cstr_out);
internal ulong    str8_deserial_read_windows_utf16_string16(String8 string, ulong off, String16 *str_out);
internal ulong    str8_deserial_read_block(String8 string, ulong off, ulong size, String8 *block_out);
internal ulong    str8_deserial_read_uleb128(String8 string, ulong off, ulong *value_out);
internal ulong    str8_deserial_read_sleb128(String8 string, ulong off, long *value_out);
#define str8_deserial_read_array(string, off, ptr, count) str8_deserial_read((string), (off), (ptr), sizeof(*(ptr))*(count), sizeof(*(ptr)))
#define str8_deserial_read_struct(string, off, ptr)       str8_deserial_read_array(string, off, ptr, 1)

#endif // BASE_STRINGS_H

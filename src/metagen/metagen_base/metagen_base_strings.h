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

struct String16
{
  uint16* str;
  uint64 size;
}

struct String32
{
  uint32* str;
  uint64 size;
}

////////////////////////////////
//~ rjf: String List & Array Types

struct String8Node
{
  String8Node* next;
  StringView string;
}

struct String8MetaNode
{
  String8MetaNode* next;
  String8Node* node;
}

struct String8List
{
  String8Node* first;
  String8Node* last;
  uint64 node_count;
  uint64 total_size;
}

////////////////////////////////
//~ rjf: String Matching, Splitting, & Joining Types

enum StringMatchFlags : uint32
{
  StringMatchFlag_CaseInsensitive  = (1 << 0),
  StringMatchFlag_RightSideSloppy  = (1 << 1),
  StringMatchFlag_SlashInsensitive = (1 << 2),
}

enum StringSplitFlags : uint32
{
  StringSplitFlag_KeepEmpties = (1 << 0),
}

enum PathStyle
{
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

struct StringJoin
{
  StringView pre;
  StringView sep;
  StringView post;
}

////////////////////////////////
//~ rjf: String Pair Types

struct String8TxtPtPair
{
  StringView string;
  TxtPt pt;
}

////////////////////////////////
//~ rjf: UTF Decoding Types

struct UnicodeDecode
{
  uint32 inc;
  uint32 codepoint;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching Types

struct FuzzyMatchRangeNode
{
  FuzzyMatchRangeNode* next;
  Rng1U64 range;
}

struct FuzzyMatchRangeList
{
  FuzzyMatchRangeNode* first;
  FuzzyMatchRangeNode* last;
  uint64 count;
  uint64 needle_part_count;
  uint64 total_dim;
}

////////////////////////////////
//~ rjf: Character Classification & Conversion Functions

B32 char_is_space(uint8 c);
B32 char_is_upper(uint8 c);
B32 char_is_lower(uint8 c);
B32 char_is_alpha(uint8 c);
B32 char_is_slash(uint8 c);
B32 char_is_digit(uint8 c, uint32 base);
uint8 char_to_lower(uint8 c);
uint8 char_to_upper(uint8 c);
uint8 char_to_correct_slash(uint8 c);

////////////////////////////////
//~ rjf: C-String Measurement

uint64 cstring8_length(uint8* c);
uint64 cstring16_length(uint16* c);
uint64 cstring32_length(uint32* c);

////////////////////////////////
//~ rjf: String Constructors

#define str8_varg(S) (int)((S).size), ((S).str)

#define str8_array(S,C) StringView((uint8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) StringView((uint8*)(S), sizeof(S))
#define str8_struct(S) StringView((uint8*)(S), sizeof(*(S)))

StringView  str8_range(uint8* first, uint8* one_past_last);
String16 str16(uint16* str, uint64 size);
String16 str16_range(uint16* first, uint16* one_past_last);
String16 str16_zero();
String32 str32(uint32* str, uint64 size);
String32 str32_range(uint32* first, uint32* one_past_last);
String32 str32_zero();
StringView  str8_cstring(char* c);
String16 str16_cstring(uint16* c);
String32 str32_cstring(uint32* c);
StringView  str8_cstring_capped(void* cstr, void* cap);

////////////////////////////////
//~ rjf: String Stylization

StringView upper_from_str8(Arena* arena, StringView string);
StringView lower_from_str8(Arena* arena, StringView string);
StringView backslashed_from_str8(Arena* arena, StringView string);

////////////////////////////////
//~ rjf: String Matching

B32 str8_match(StringView a, StringView b, StringMatchFlags flags);
uint64 str8_find_needle(StringView string, uint64 start_pos, StringView needle, StringMatchFlags flags);
B32 str8_ends_with(StringView string, StringView end, StringMatchFlags flags);

////////////////////////////////
//~ rjf: String Slicing

StringView str8_substr(StringView str, Rng1U64 range);
StringView str8_prefix(StringView str, uint64 size);
StringView str8_skip(StringView str, uint64 amt);
StringView str8_postfix(StringView str, uint64 size);
StringView str8_chop(StringView str, uint64 amt);
StringView str8_skip_chop_whitespace(StringView string);

////////////////////////////////
//~ rjf: String Formatting & Copying

StringView push_str8_cat(Arena* arena, StringView s1, StringView s2);
StringView push_str8_copy(Arena* arena, StringView s);
StringView push_str8fv(Arena* arena, char* fmt, va_list args);
StringView push_str8f(Arena* arena, char* fmt, ...);

////////////////////////////////
//~ rjf: String <=> Integer Conversions

//- rjf: string . integer
int64 sign_from_str8(StringView string, StringView* string_tail);
B32 str8_is_integer(StringView string, uint32 radix);
uint64 u64_from_str8(StringView string, uint32 radix);
int64 s64_from_str8(StringView string, uint32 radix);
B32 try_u64_from_str8_c_rules(StringView string, uint64* x);
B32 try_s64_from_str8_c_rules(StringView string, int64* x);

//- rjf: integer . string
StringView str8_from_memory_size(Arena* arena, uint64 z);
StringView str8_from_u64(Arena* arena, uint64 u64, uint32 radix, uint8 min_digits, uint8 digit_group_separator);
StringView str8_from_s64(Arena* arena, int64 s64, uint32 radix, uint8 min_digits, uint8 digit_group_separator);

////////////////////////////////
//~ rjf: String <=> Float Conversions

double f64_from_str8(StringView string);

////////////////////////////////
//~ rjf: String List Construction Functions

String8Node* str8_list_push_node(String8List* list, String8Node* node);
String8Node* str8_list_push_node_set_string(String8List* list, String8Node* node, StringView string);
String8Node* str8_list_push_node_front(String8List* list, String8Node* node);
String8Node* str8_list_push_node_front_set_string(String8List* list, String8Node* node, StringView string);
String8Node* str8_list_push(Arena* arena, String8List* list, StringView string);
String8Node* str8_list_push_front(Arena* arena, String8List* list, StringView string);
void         str8_list_concat_in_place(String8List* list, String8List* to_push);
String8Node* str8_list_push_aligner(Arena* arena, String8List* list, uint64 min, uint64 align);
String8Node* str8_list_pushf(Arena* arena, String8List* list, char* fmt, ...);
String8Node* str8_list_push_frontf(Arena* arena, String8List* list, char* fmt, ...);
String8List  str8_list_copy(Arena* arena, String8List* list);
#define str8_list_first(list) ((list).first ? (list).first.string : StringView())

////////////////////////////////
//~ rjf: String Splitting & Joining

String8List  str8_split(Arena* arena, StringView string, uint8* split_chars, uint64 split_char_count, StringSplitFlags flags);
String8List  str8_split_by_string_chars(Arena* arena, StringView string, StringView split_chars, StringSplitFlags flags);
String8List  str8_list_split_by_string_chars(Arena* arena, String8List list, StringView split_chars, StringSplitFlags flags);
StringView      str8_list_join(Arena* arena, String8List* list, StringJoin* optional_params);
void         str8_list_from_flags(Arena* arena, String8List* list, uint32 flags, StringView* flag_string_table, uint32 flag_string_count);

////////////////////////////////
//~ rjf; String Arrays

Span<StringView> str8_array_from_list(Arena* arena, String8List* list);
Span<StringView> str8_array_reserve(Arena* arena, uint64 count);

////////////////////////////////
//~ rjf: String Path Helpers

StringView str8_chop_last_slash(StringView string);
StringView str8_skip_last_slash(StringView string);
StringView str8_chop_last_dot(StringView string);
StringView str8_skip_last_dot(StringView string);

PathStyle   path_style_from_str8(StringView string);
String8List str8_split_path(Arena* arena, StringView string);
void        str8_path_list_resolve_dots_in_place(String8List* path, PathStyle style);
StringView     str8_path_list_join_by_style(Arena* arena, String8List* path, PathStyle style);

String8TxtPtPair str8_txt_pt_pair_from_string(StringView string);

////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding

UnicodeDecode utf8_decode(uint8* str, uint64 max);
UnicodeDecode utf16_decode(uint16* str, uint64 max);
uint32 utf8_encode(uint8* str, uint32 codepoint);
uint32 utf16_encode(uint16* str, uint32 codepoint);
uint32 utf8_from_utf32_single(uint8* buffer, uint32 character);

////////////////////////////////
//~ rjf: Unicode String Conversions

StringView str8_from_16(Arena* arena, String16 in);
String16 str16_from_8(Arena* arena, StringView in);
StringView str8_from_32(Arena* arena, String32 in);
String32 str32_from_8(Arena* arena, StringView in);

////////////////////////////////
//~ rjf: Basic Types & Space Enum . String Conversions

StringView string_from_dimension(Dimension dimension);
StringView string_from_side(Side side);
StringView string_from_operating_system(OperatingSystem os);
StringView string_from_architecture(Architecture arch);

////////////////////////////////
//~ rjf: Time Types . String

StringView string_from_week_day(WeekDay week_day);
StringView string_from_month(Month month);
StringView push_date_time_string(Arena* arena, DateTime* date_time);
StringView push_file_name_date_time_string(Arena* arena, DateTime* date_time);
StringView string_from_elapsed_time(Arena* arena, DateTime dt);

////////////////////////////////
//~ rjf: Basic Text Indentation

StringView indented_from_string(Arena* arena, StringView string);

////////////////////////////////
//~ rjf: Text Wrapping

String8List wrapped_lines_from_string(Arena* arena, StringView string, uint64 first_line_max_width, uint64 max_width, uint64 wrap_indent);

////////////////////////////////
//~ rjf: String <. Color

StringView hex_string_from_rgba_4f32(Arena* arena, Vec4F32 rgba);
Vec4F32 rgba_from_hex_string_4f32(StringView hex_string);

////////////////////////////////
//~ rjf: String Fuzzy Matching

FuzzyMatchRangeList fuzzy_match_find(Arena* arena, StringView needle, StringView haystack);
FuzzyMatchRangeList fuzzy_match_range_list_copy(Arena* arena, FuzzyMatchRangeList* src);

////////////////////////////////
//~ NOTE(allen): Serialization Helpers

void    str8_serial_begin(Arena* arena, String8List* srl);
StringView str8_serial_end(Arena* arena, String8List* srl);
void    str8_serial_write_to_dst(String8List* srl, void* out);
uint64     str8_serial_push_align(Arena* arena, String8List* srl, uint64 align);
void *  str8_serial_push_size(Arena* arena, String8List* srl, uint64 size);
void *  str8_serial_push_data(Arena* arena, String8List* srl, void* data, uint64 size);
void    str8_serial_push_data_list(Arena* arena, String8List* srl, String8Node* first);
void    str8_serial_push_u64(Arena* arena, String8List* srl, uint64 x);
void    str8_serial_push_u32(Arena* arena, String8List* srl, uint32 x);
void    str8_serial_push_u16(Arena* arena, String8List* srl, uint16 x);
void    str8_serial_push_u8(Arena* arena, String8List* srl, uint8 x);
void    str8_serial_push_cstr(Arena* arena, String8List* srl, StringView str);
void    str8_serial_push_string(Arena* arena, String8List* srl, StringView str);
#define str8_serial_push_array(arena, srl, ptr, count) str8_serial_push_data(arena, srl, ptr, sizeof(*(ptr)) * (count))
#define str8_serial_push_struct(arena, srl, ptr) str8_serial_push_array(arena, srl, ptr, 1)

////////////////////////////////
//~ rjf: Deserialization Helpers

uint64    str8_deserial_read(StringView string, uint64 off, void* read_dst, uint64 read_size, uint64 granularity);
uint64    str8_deserial_find_first_match(StringView string, uint64 off, uint16 scan_val);
void * str8_deserial_get_raw_ptr(StringView string, uint64 off, uint64 size);internal uint64 str8_deserial_read_cstr(StringView string, uint64 off, StringView* cstr_out);
uint64    str8_deserial_read_windows_utf16_string16(StringView string, uint64 off, String16* str_out);
uint64    str8_deserial_read_block(StringView string, uint64 off, uint64 size, StringView* block_out);
#define str8_deserial_read_array(string, off, ptr, count) str8_deserial_read((string), (off), (ptr), sizeof(*(ptr))*(count), sizeof(*(ptr)))
#define str8_deserial_read_struct(string, off, ptr) str8_deserial_read((string), (off), (ptr), sizeof(*(ptr)), sizeof(*(ptr)))

#endif // BASE_STRINGS_H

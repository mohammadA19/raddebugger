// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Third Party Includes

#define STB_SPRINTF_DECORATE(name) raddbg_##name
#include "third_party/stb/stb_sprintf.h"

////////////////////////////////
//~ rjf: String Types

public struct String8
{
  uint8* str;
  uint64 size;
}

public struct String16
{
  uint16* str;
  uint64 size;
}

public struct String32
{
  uint32* str;
  uint64 size;
}

////////////////////////////////
//~ rjf: String List & Array Types

public struct String8Node
{
  String8Node* next;
  String8 string;
}

public struct String8MetaNode
{
  String8MetaNode* next;
  String8Node* node;
}

public struct String8List
{
  String8Node* first;
  String8Node* last;
  uint64 node_count;
  uint64 total_size;
}

public struct String8Array
{
  String8* v;
  uint64 count;
}

////////////////////////////////
//~ rjf: String Matching, Splitting, & Joining Types

public enum StringMatchFlags : uint32
{
  CaseInsensitive  = (1 << 0),
  RightSideSloppy  = (1 << 1),
  SlashInsensitive = (1 << 2),
}

public enum StringSplitFlags : uint32
{
  KeepEmpties = (1 << 0),
}

public enum PathStyle
{
  Null,
  Relative,
  WindowsAbsolute,
  UnixAbsolute,

#if OS_WINDOWS
  SystemAbsolute = WindowsAbsolute
#elif OS_LINUX
  SystemAbsolute = UnixAbsolute
#else
# error "absolute path style is undefined for this OS"
#endif
}

public struct StringJoin
{
  String8 pre;
  String8 sep;
  String8 post;
}

////////////////////////////////
//~ rjf: String Pair Types

public struct String8TxtPtPair
{
  String8 string;
  TxtPt pt;
}

////////////////////////////////
//~ rjf: UTF Decoding Types

public struct UnicodeDecode
{
  uint32 inc;
  uint32 codepoint;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching Types

public struct FuzzyMatchRangeNode
{
  FuzzyMatchRangeNode* next;
  Rng1U64 range;
}

public struct FuzzyMatchRangeList
{
  FuzzyMatchRangeNode* first;
  FuzzyMatchRangeNode* last;
  uint64 count;
  uint64 needle_part_count;
  uint64 total_dim;
}

////////////////////////////////
//~ rjf: String Constructors

#define str8_lit(S)  str8((uint8*)(S), sizeof(S) - 1)
#define str8_lit_comp(S) {(uint8*)(S), sizeof(S) - 1,}
#define str8_varg(S) (int)((S).size), ((S).str)

#define str8_array(S,C) str8((uint8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((uint8*)(S), sizeof(S))
#define str8_struct(S) str8((uint8*)(S), sizeof(*(S)))


////////////////////////////////
//~ rjf: String Matching

#define str8_match_lit(a_lit, b, flags)   str8_match(str8_lit(a_lit), (b), (flags))
#define str8_match_cstr(a_cstr, b, flags) str8_match(str8_cstring(a_cstr), (b), (flags))
#define str8_ends_with_lit(string, end_lit, flags) str8_ends_with((string), str8_lit(end_lit), (flags))

////////////////////////////////
//~ rjf: String List Construction Functions

#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

////////////////////////////////
//~ NOTE(allen): Serialization Helpers

#define str8_serial_push_array(arena, srl, ptr, count) str8_serial_push_data(arena, srl, ptr, sizeof(*(ptr)) * (count))
#define str8_serial_push_struct(arena, srl, ptr) str8_serial_push_array(arena, srl, ptr, 1)

////////////////////////////////
//~ rjf: Deserialization Helpers

#define str8_deserial_read_array(string, off, ptr, count) str8_deserial_read((string), (off), (ptr), sizeof(*(ptr))*(count), sizeof(*(ptr)))
#define str8_deserial_read_struct(string, off, ptr)       str8_deserial_read_array(string, off, ptr, 1)

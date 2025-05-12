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

struct String8
{
  U8 *str;
  U64 size;
}

struct String16
{
  U16 *str;
  U64 size;
}

struct String32
{
  U32 *str;
  U64 size;
}

////////////////////////////////
//~ rjf: String List & Array Types

struct String8Node
{
  String8Node *next;
  String8 string;
}

struct String8MetaNode
{
  String8MetaNode *next;
  String8Node *node;
}

struct String8List
{
  String8Node *first;
  String8Node *last;
  U64 node_count;
  U64 total_size;
}

struct String8Array
{
  String8 *v;
  U64 count;
}

////////////////////////////////
//~ rjf: String Matching, Splitting, & Joining Types

enum StringMatchFlags : U32
{
  StringMatchFlag_CaseInsensitive  = (1 << 0),
  StringMatchFlag_RightSideSloppy  = (1 << 1),
  StringMatchFlag_SlashInsensitive = (1 << 2),
}

enum StringSplitFlags : U32
{
  StringSplitFlag_KeepEmpties = (1 << 0),
}

enum PathStyle
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

struct StringJoin
{
  String8 pre;
  String8 sep;
  String8 post;
}

////////////////////////////////
//~ rjf: String Pair Types

struct String8TxtPtPair
{
  String8 string;
  TxtPt pt;
}

////////////////////////////////
//~ rjf: UTF Decoding Types

struct UnicodeDecode
{
  U32 inc;
  U32 codepoint;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching Types

struct FuzzyMatchRangeNode
{
  FuzzyMatchRangeNode *next;
  Rng1U64 range;
}

struct FuzzyMatchRangeList
{
  FuzzyMatchRangeNode *first;
  FuzzyMatchRangeNode *last;
  U64 count;
  U64 needle_part_count;
  U64 total_dim;
}

////////////////////////////////
//~ rjf: String Constructors

#define str8_lit(S)  str8((U8*)(S), sizeof(S) - 1)
#define str8_lit_comp(S) {(U8*)(S), sizeof(S) - 1,}
#define str8_varg(S) (int)((S).size), ((S).str)

#define str8_array(S,C) str8((U8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((U8*)(S), sizeof(S))
#define str8_struct(S) str8((U8*)(S), sizeof(*(S)))


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

#endif // BASE_STRINGS_H

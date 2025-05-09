// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Third Party Includes

#define STB_SPRINTF_DECORATE(name) raddbg_##name
#include "third_party/stb/stb_sprintf.h"

////////////////////////////////
//~ rjf: String Types

type
  String8* = object
    str: UncheckedArray[uint8]
    size: uint64

  String16* = object
    str: UncheckedArray[uint16]
    size: uint64

  String32* = object
    str: UncheckedArray[uint32]
    size: uint64

  ////////////////////////////////
  //~ rjf: String List & Array Types

  String8Node* = object
    next: ptr String8Node
    string: String8

  String8MetaNode* = object
    next: ptr String8MetaNode
    node: ptr String8Node

  String8List* = object
    first: ptr String8Node
    last: ptr String8Node
    node_count: uint64
    total_size: uint64

  String8Array* = object
    v: UncheckedArray[String8]
    count: uint64

  ////////////////////////////////
  //~ rjf: String Matching, Splitting, & Joining Types

  StringMatchFlags*: set[uint32]
  StringMatchFlag* = enum
    StringMatchFlag_CaseInsensitive  = (1 << 0),
    StringMatchFlag_RightSideSloppy  = (1 << 1),
    StringMatchFlag_SlashInsensitive = (1 << 2),

  StringSplitFlags*: set[uint32]
  StringSplitFlag* = enum
    StringSplitFlag_KeepEmpties = (1 << 0),

  PathStyle* = enum
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

  StringJoin* = object
    pre: String8
    sep: String8
    post: String8

  ////////////////////////////////
  //~ rjf: String Pair Types

  String8TxtPtPair* = object
    string: String8
    pt: TxtPt

  ////////////////////////////////
  //~ rjf: UTF Decoding Types

  UnicodeDecode* = object
    inc: uint32
    codepoint: uint32

  ////////////////////////////////
  //~ rjf: String Fuzzy Matching Types

  FuzzyMatchRangeNode* = object
    next: ptr FuzzyMatchRangeNode
    range: Rng1U64

  FuzzyMatchRangeList* = object
    first: ptr FuzzyMatchRangeNode
    last: ptr FuzzyMatchRangeNode
    count: uint64
    needle_part_count: uint64
    total_dim: uint64

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

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Third Party Includes

#define STB_SPRINTF_DECORATE(name) raddbg_##name
#include "third_party/stb/stb_sprintf.h"

////////////////////////////////
//~ rjf: String Types

struct String8
{
  u8 *str;
  u64 size;
};

struct String16
{
  u16 *str;
  u64 size;
};

struct String32
{
  u32 *str;
  u64 size;
};

////////////////////////////////
//~ rjf: String List & Array Types

struct String8Node
{
  String8Node *next;
  String8 string;
};

struct String8MetaNode
{
  String8MetaNode *next;
  String8Node *node;
};

struct String8List
{
  String8Node *first;
  String8Node *last;
  u64 node_count;
  u64 total_size;
};

struct String8Array
{
  String8 *v;
  u64 count;
};

////////////////////////////////
//~ rjf: String Matching, Splitting, & Joining Types

enum StringMatchFlags : u32
{
  StringMatchFlag_CaseInsensitive  = (1 << 0),
  StringMatchFlag_RightSideSloppy  = (1 << 1),
  StringMatchFlag_SlashInsensitive = (1 << 2),
};

enum StringSplitFlags : u32
{
  StringSplitFlag_KeepEmpties = (1 << 0),
};

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
# error Absolute path style is undefined for this OS.
#endif
}
PathStyle;

struct StringJoin
{
  String8 pre;
  String8 sep;
  String8 post;
};

////////////////////////////////
//~ rjf: String Pair Types

struct String8TxtPtPair
{
  String8 string;
  TxtPt pt;
};

////////////////////////////////
//~ rjf: UTF Decoding Types

struct UnicodeDecode
{
  u32 inc;
  u32 codepoint;
};

////////////////////////////////
//~ rjf: String Fuzzy Matching Types

struct FuzzyMatchRangeNode
{
  FuzzyMatchRangeNode *next;
  Rng1U64 range;
};

struct FuzzyMatchRangeList
{
  FuzzyMatchRangeNode *first;
  FuzzyMatchRangeNode *last;
  u64 count;
  u64 needle_part_count;
  u64 total_dim;
};

////////////////////////////////
//~ rjf: String Constructors

#define str8_lit(S)  str8((u8*)(S), sizeof(S) - 1)
#define str8_lit_comp(S) {(u8*)(S), sizeof(S) - 1,}
#define str8_varg(S) (int)((S).size), ((S).str)

#define str8_array(S,C) str8((u8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((u8*)(S), sizeof(S))
#define str8_struct(S) str8((u8*)(S), sizeof(*(S)))

////////////////////////////////
//~ rjf: String Matching

#define str8_match_lit(a_lit, b, flags)   str8_match(str8_lit(a_lit), (b), (flags))
#define str8_match_cstr(a_cstr, b, flags) str8_match(str8_cstring(a_cstr), (b), (flags))
#define str8_ends_with_lit(string, end_lit, flags) str8_ends_with((string), str8_lit(end_lit), (flags))

////////////////////////////////
//~ rjf: String List Construction Functions

#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

////////////////////////////////
//~ rjf: String Path Helpers

global @(rodata) struct
{
  String8   string;
  PathStyle path_style;
}
g_path_style_map[] =
{
  { str8_lit_comp(""),         PathStyle_Null            },
  { str8_lit_comp("relative"), PathStyle_Relative        },
  { str8_lit_comp("windows"),  PathStyle_WindowsAbsolute },
  { str8_lit_comp("unix"),     PathStyle_UnixAbsolute    },
  { str8_lit_comp("system"),   PathStyle_SystemAbsolute  },
};

////////////////////////////////
//~ NOTE(allen): Serialization Helpers

#define str8_serial_push_array(arena, srl, ptr, count) str8_serial_push_data(arena, srl, ptr, sizeof(*(ptr)) * (count))
#define str8_serial_push_struct(arena, srl, ptr) str8_serial_push_array(arena, srl, ptr, 1)

////////////////////////////////
//~ rjf: Deserialization Helpers

#define str8_deserial_read_array(string, off, ptr, count) str8_deserial_read((string), (off), (ptr), sizeof(*(ptr))*(count), sizeof(*(ptr)))
#define str8_deserial_read_struct(string, off, ptr)       str8_deserial_read_array(string, off, ptr, 1)

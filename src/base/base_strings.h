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
  U8 *str;
  U64 size;
};

struct String16
{
  U16 *str;
  U64 size;
};

struct String32
{
  U32 *str;
  U64 size;
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
  U64 node_count;
  U64 total_size;
};

struct String8Array
{
  String8 *v;
  U64 count;
};

////////////////////////////////
//~ rjf: String Matching, Splitting, & Joining Types

enum StringMatchFlags : U32
{
  StringMatchFlag_CaseInsensitive  = (1 << 0),
  StringMatchFlag_RightSideSloppy  = (1 << 1),
  StringMatchFlag_SlashInsensitive = (1 << 2),
};

enum StringSplitFlags : U32
{
  StringSplitFlag_KeepEmpties = (1 << 0),
};

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
# error Absolute path style is undefined for this OS.
#endif
}

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
  U32 inc;
  U32 codepoint;
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
  U64 count;
  U64 needle_part_count;
  U64 total_dim;
};

////////////////////////////////
//~ rjf: Character Classification & Conversion Functions


////////////////////////////////
//~ rjf: C-String Measurement


////////////////////////////////
//~ rjf: String Constructors

#define str8_lit(S)  str8((U8*)(S), sizeof(S) - 1)
#define str8_lit_comp(S) {(U8*)(S), sizeof(S) - 1,}
#define str8_varg(S) (int)((S).size), ((S).str)

#define str8_array(S,C) str8((U8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((U8*)(S), sizeof(S))
#define str8_struct(S) str8((U8*)(S), sizeof(*(S)))


////////////////////////////////
//~ rjf: String Stylization


////////////////////////////////
//~ rjf: String Matching

#define str8_match_lit(a_lit, b, flags)   str8_match(str8_lit(a_lit), (b), (flags))
#define str8_match_cstr(a_cstr, b, flags) str8_match(str8_cstring(a_cstr), (b), (flags))
#define str8_ends_with_lit(string, end_lit, flags) str8_ends_with((string), str8_lit(end_lit), (flags))

////////////////////////////////
//~ rjf: String Slicing


////////////////////////////////
//~ rjf: String Formatting & Copying

// TODO(rjf): remove these once we're ready to convert all usages:
#define push_str8_cat(arena, s1, s2) str8_cat((arena), (s1), (s2))
#define push_str8_copy(arena, s) str8_copy((arena), (s))
#define push_str8fv(arena, fmt, args) str8fv((arena), (fmt), (args))
#define push_str8f(arena, ...) str8f((arena), __VA_ARGS__)
internal String8 push_cstr(Arena *arena, String8 str); // TODO(rjf): this is unnecessary - this is implied by `push_str8_copy`. need to remove.

////////////////////////////////
//~ rjf: String <=> Integer Conversions

//- rjf: string -> integer


//- rjf: integer -> string

////////////////////////////////
//~ rjf: String <=> Float Conversions


////////////////////////////////
//~ rjf: String List Construction Functions

#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

////////////////////////////////
//~ rjf: String Splitting & Joining


////////////////////////////////
//~ rjf: Basic Data Stringification Helpers


////////////////////////////////
//~ rjf; String Arrays


////////////////////////////////
//~ rjf: String Version Helpers


////////////////////////////////
//~ rjf: String Path Helpers

global read_only struct
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
//~ rjf: Relative <-> Absolute Path


////////////////////////////////
//~ rjf: Path Normalization


////////////////////////////////
//~ rjf: Misc. Path Helpers


////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding


////////////////////////////////
//~ rjf: Unicode String Conversions


////////////////////////////////
//~ String -> Enum Conversions


////////////////////////////////
//~ rjf: Basic Types & Space Enum -> String Conversions


////////////////////////////////
//~ rjf: Time Types -> String


////////////////////////////////
//~ Globally Unique Ids


////////////////////////////////
//~ rjf: Basic Text Indentation


////////////////////////////////
//~ rjf: Text Escaping


////////////////////////////////
//~ rjf: Text Wrapping


////////////////////////////////
//~ rjf: String <-> Color


////////////////////////////////
//~ rjf: String Fuzzy Matching


////////////////////////////////
//~ NOTE(allen): Serialization Helpers

#define str8_serial_push_array(arena, srl, ptr, count) str8_serial_push_data(arena, srl, ptr, sizeof(*(ptr)) * (count))
#define str8_serial_push_struct(arena, srl, ptr) str8_serial_push_array(arena, srl, ptr, 1)

////////////////////////////////
//~ rjf: Deserialization Helpers

#define str8_deserial_read_array(string, off, ptr, count) str8_deserial_read((string), (off), (ptr), sizeof(*(ptr))*(count), sizeof(*(ptr)))
#define str8_deserial_read_struct(string, off, ptr)       str8_deserial_read_array(string, off, ptr, 1)

////////////////////////////////
//~ rjf: Basic String Hashes



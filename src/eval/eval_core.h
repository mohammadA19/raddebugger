// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_CORE_H
#define EVAL_CORE_H

////////////////////////////////
//~ rjf: Messages

enum E_MsgKind
{
  E_MsgKind_Null,
  E_MsgKind_MalformedInput,
  E_MsgKind_MissingInfo,
  E_MsgKind_ResolutionFailure,
  E_MsgKind_InterpretationError,
  E_MsgKind_COUNT
}

struct E_Msg
{
  E_Msg* next;
  E_MsgKind kind;
  void* location;
  String8 text;
}

struct E_MsgList
{
  E_Msg* first;
  E_Msg* last;
  E_MsgKind max_kind;
  uint64 count;
}

////////////////////////////////
//~ rjf: Register-Sized Value Type

[Union]
struct E_Value
{
  uint64 u512[8];
  uint64 u256[4];
  U128 u128;
  uint64 u64;
  uint32 u32;
  uint16 u16;
  int64 s64;
  int32 s32;
  int32 s16;
  double f64;
  float f32;
}

////////////////////////////////
//~ rjf: Operator Info

enum E_OpKind
{
  E_OpKind_Null,
  E_OpKind_UnaryPrefix,
  E_OpKind_Binary,
}

struct E_OpInfo
{
  E_OpKind kind;
  int64 precedence;
  String8 pre;
  String8 sep;
  String8 post;
}

////////////////////////////////
//~ rjf: Evaluation Spaces
//
// NOTE(rjf): Evaluations occur within the context of a "space". Each "space"
// refers to a different offset/address-space, but it's a bit looser of a
// concept than just address space, since it can also refer to offsets into
// a register block, and it is also used to refer to spaces of unique IDs for
// key-value stores, e.g. for information in the debugger.
//
// Effectively, when considering the result of an evaluation, you use the
// value for understanding a key* into* a space, e.g. 1+2 -> 3, in a null
// space, or &foo, in the space of PID: 1234.

enum E_SpaceKind : uint64
{
  E_SpaceKind_Null,
  E_SpaceKind_FileSystem,
  E_SpaceKind_FirstUserDefined,
}

struct E_Space
{
  E_SpaceKind kind;
  [Union]
  struct
  {
    uint64 u64s[3];
    struct
    {
      uint64 u64_0;
      U128 u128;
    };
  };
}

////////////////////////////////
//~ rjf: Evaluation Modes

enum E_Mode
{
  E_Mode_Null,
  E_Mode_Value,
  E_Mode_Offset,
}

////////////////////////////////
//~ rjf: Modules

struct E_Module
{
  RDI_Parsed* rdi;
  Rng1U64 vaddr_range;
  Arch arch;
  E_Space space;
}

////////////////////////////////
//~ rjf: Generated Code

#include "eval/generated/eval.meta.h"

////////////////////////////////
//~ rjf: Basic Helper Functions

uint64 e_hash_from_string(uint64 seed, String8 string);
#define e_value_u64(v) (E_Value){.u64 = (v)}

////////////////////////////////
//~ rjf: Message Functions

void e_msg(Arena* arena, E_MsgList* msgs, E_MsgKind kind, void* location, String8 text);
void e_msgf(Arena* arena, E_MsgList* msgs, E_MsgKind kind, void* location, char* fmt, ...);
void e_msg_list_concat_in_place(E_MsgList* dst, E_MsgList* to_push);

////////////////////////////////
//~ rjf: Space Functions

E_Space e_space_make(E_SpaceKind kind);

#endif // EVAL_CORE_H

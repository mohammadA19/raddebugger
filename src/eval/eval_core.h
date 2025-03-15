// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_CORE_H
#define EVAL_CORE_H

////////////////////////////////
//~ rjf: Messages

E_MsgKind :: enum
{
  E_MsgKind_Null,
  E_MsgKind_MalformedInput,
  E_MsgKind_MissingInfo,
  E_MsgKind_ResolutionFailure,
  E_MsgKind_InterpretationError,
  E_MsgKind_COUNT
}

E_Msg :: struct
{
  E_Msg *next;
  E_MsgKind kind;
  void *location;
  String8 text;
}

E_MsgList :: struct
{
  E_Msg *first;
  E_Msg *last;
  E_MsgKind max_kind;
  U64 count;
}

////////////////////////////////
//~ rjf: Register-Sized Value Type

E_Value :: struct #raw_union
{
  U64 u512[8];
  U64 u256[4];
  U128 u128;
  U64 u64;
  U32 u32;
  U16 u16;
  S64 s64;
  S32 s32;
  S32 s16;
  F64 f64;
  F32 f32;
}

////////////////////////////////
//~ rjf: Operator Info

E_OpKind :: enum
{
  E_OpKind_Null,
  E_OpKind_UnaryPrefix,
  E_OpKind_Binary,
}

E_OpInfo :: struct
{
  E_OpKind kind;
  S64 precedence;
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
// value for understanding a key *into* a space, e.g. 1+2 -> 3, in a null
// space, or &foo, in the space of PID: 1234.

E_SpaceKind :: enum U64
{
  E_SpaceKind_Null,
  E_SpaceKind_FileSystem,
  E_SpaceKind_FirstUserDefined,
}

E_Space :: struct
{
  E_SpaceKind kind;
  union
  {
    U64 u64s[3];
    struct
    {
      U64 u64_0;
      U128 u128;
    };
  };
}

////////////////////////////////
//~ rjf: Evaluation Modes

E_Mode :: enum
{
  E_Mode_Null,
  E_Mode_Value,
  E_Mode_Offset,
}

////////////////////////////////
//~ rjf: Modules

E_Module :: struct
{
  RDI_Parsed *rdi;
  Rng1U64 vaddr_range;
  Arch arch;
  E_Space space;
}

////////////////////////////////
//~ rjf: Generated Code

#include "eval/generated/eval.meta.h"

////////////////////////////////
//~ rjf: Basic Helper Functions

U64 e_hash_from_string(U64 seed, String8 string);
#define e_value_u64(v) (E_Value){.u64 = (v)}

////////////////////////////////
//~ rjf: Message Functions

void e_msg(Arena *arena, E_MsgList *msgs, E_MsgKind kind, void *location, String8 text);
void e_msgf(Arena *arena, E_MsgList *msgs, E_MsgKind kind, void *location, char *fmt, ...);
void e_msg_list_concat_in_place(E_MsgList *dst, E_MsgList *to_push);

////////////////////////////////
//~ rjf: Space Functions

E_Space e_space_make(E_SpaceKind kind);

#endif // EVAL_CORE_H

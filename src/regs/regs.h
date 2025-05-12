// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Register Usage Kinds

enum REGS_UsageKind
{
  REGS_UsageKind_Normal,
  REGS_UsageKind_Vector,
}

////////////////////////////////
//~ rjf: Register Types

typedef U8 REGS_RegCode;
typedef U8 REGS_AliasCode;

union REGS_Reg16
{
  U8[2] v;
  U16 u16;
}

union REGS_Reg32
{
  U8[4] v;
  U32 u32;
  F32 f32;
}

union REGS_Reg64
{
  U8[8] v;
  U64 u64;
  F64 f64;
}

#pragma pack(push, 1)
struct REGS_Reg80
{
  U64 int1_frac63;
  U16 sign1_exp15;
}
#pragma pack(pop)

union REGS_Reg128
{
  U8[16] v;
  U32[4] u32;
  F32[4] f32;
  U64[2] u64;
  F64[2] f64;
}

union REGS_Reg256
{
  U8[32] v;
  U32[8] u32;
  F32[8] f32;
  U64[4] u64;
  F64[4] f64;
}

union REGS_Reg512
{
  U8[64] v;
  U32[16] u32;
  F32[16] f32;
  U64[8] u64;
  F64[8] f64;
}

////////////////////////////////
//~ rjf: Register Slicing Types

struct REGS_Rng
{
  U16 byte_off;
  U16 byte_size;
}

struct REGS_Slice
{
  U16 code;
  U8 byte_off;
  U8 byte_size;
}

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers



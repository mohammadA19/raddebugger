// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Register Usage Kinds

typedef enum REGS_UsageKind
{
  REGS_UsageKind_Normal,
  REGS_UsageKind_Vector,
}
REGS_UsageKind;

////////////////////////////////
//~ rjf: Register Types

typedef U8 REGS_RegCode;
typedef U8 REGS_AliasCode;


union REGS_Reg16
{
  U8 v[2];
  U16 u16;
};


union REGS_Reg32
{
  U8 v[4];
  U32 u32;
  F32 f32;
};


union REGS_Reg64
{
  U8 v[8];
  U64 u64;
  F64 f64;
};

#pragma pack(push, 1)


struct REGS_Reg80
{
  U64 int1_frac63;
  U16 sign1_exp15;
};
#pragma pack(pop)


union REGS_Reg128
{
  U8 v[16];
  U32 u32[4];
  F32 f32[4];
  U64 u64[2];
  F64 f64[2];
};


union REGS_Reg256
{
  U8 v[32];
  U32 u32[8];
  F32 f32[8];
  U64 u64[4];
  F64 f64[4];
};


union REGS_Reg512
{
  U8 v[64];
  U32 u32[16];
  F32 f32[16];
  U64 u64[8];
  F64 f64[8];
};

////////////////////////////////
//~ rjf: Register Slicing Types



struct REGS_Rng
{
  U16 byte_off;
  U16 byte_size;
};



struct REGS_Slice
{
  U16 code;
  U8 byte_off;
  U8 byte_size;
};

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers



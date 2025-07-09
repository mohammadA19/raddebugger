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

typedef u8 REGS_RegCode;
typedef u8 REGS_AliasCode;


union REGS_Reg16
{
  u8 v[2];
  u16 u16;
};


union REGS_Reg32
{
  u8 v[4];
  u32 u32;
  f32 f32;
};


union REGS_Reg64
{
  u8 v[8];
  u64 u64;
  f64 f64;
};

#pragma pack(push, 1)


struct REGS_Reg80
{
  u64 int1_frac63;
  u16 sign1_exp15;
};
#pragma pack(pop)


union REGS_Reg128
{
  u8 v[16];
  u32 u32[4];
  f32 f32[4];
  u64 u64[2];
  f64 f64[2];
};


union REGS_Reg256
{
  u8 v[32];
  u32 u32[8];
  f32 f32[8];
  u64 u64[4];
  f64 f64[4];
};


union REGS_Reg512
{
  u8 v[64];
  u32 u32[16];
  f32 f32[16];
  u64 u64[8];
  f64 f64[8];
};

////////////////////////////////
//~ rjf: Register Slicing Types



struct REGS_Rng
{
  u16 byte_off;
  u16 byte_size;
};



struct REGS_Slice
{
  u16 code;
  u8 byte_off;
  u8 byte_size;
};

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers



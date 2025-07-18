// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef REGS_H
#define REGS_H

////////////////////////////////
//~ rjf: Register Usage Kinds

enum REGS_UsageKind
{
  REGS_UsageKind_Normal,
  REGS_UsageKind_Vector,
}

////////////////////////////////
//~ rjf: Register Types

typedef uint8 REGS_RegCode;
typedef uint8 REGS_AliasCode;

union REGS_Reg16
{
  uint8 v[2];
  uint16 u16;
};

union REGS_Reg32
{
  uint8 v[4];
  uint32 u32;
  float f32;
};

union REGS_Reg64
{
  uint8 v[8];
  uint64 u64;
  double f64;
};

#pragma pack(push, 1)
struct REGS_Reg80
{
  uint64 int1_frac63;
  uint16 sign1_exp15;
};
#pragma pack(pop)

union REGS_Reg128
{
  uint8 v[16];
  uint32 u32[4];
  float f32[4];
  uint64 u64[2];
  double f64[2];
};

union REGS_Reg256
{
  uint8 v[32];
  uint32 u32[8];
  float f32[8];
  uint64 u64[4];
  double f64[4];
};

union REGS_Reg512
{
  uint8 v[64];
  uint32 u32[16];
  float f32[16];
  uint64 u64[8];
  double f64[8];
};

////////////////////////////////
//~ rjf: Register Slicing Types

struct REGS_Rng
{
  uint16 byte_off;
  uint16 byte_size;
};

struct REGS_Slice
{
  uint16 code;
  uint8 byte_off;
  uint8 byte_size;
};

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers


#endif // REGS_H

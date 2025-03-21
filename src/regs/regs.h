// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef REGS_H
#define REGS_H

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

typedef byte REGS_RegCode;
typedef byte REGS_AliasCode;

typedef union REGS_Reg16 REGS_Reg16;
union REGS_Reg16
{
  byte v[2];
  ushort u16;
}

typedef union REGS_Reg32 REGS_Reg32;
union REGS_Reg32
{
  byte v[4];
  uint u32;
  float f32;
}

typedef union REGS_Reg64 REGS_Reg64;
union REGS_Reg64
{
  byte v[8];
  ulong u64;
  double f64;
}

#pragma pack(push, 1)
typedef struct REGS_Reg80 REGS_Reg80;
struct REGS_Reg80
{
  ulong int1_frac63;
  ushort sign1_exp15;
}
#pragma pack(pop)

typedef union REGS_Reg128 REGS_Reg128;
union REGS_Reg128
{
  byte v[16];
  uint u32[4];
  float f32[4];
  ulong u64[2];
  double f64[2];
}

typedef union REGS_Reg256 REGS_Reg256;
union REGS_Reg256
{
  byte v[32];
  uint u32[8];
  float f32[8];
  ulong u64[4];
  double f64[4];
}

typedef union REGS_Reg512 REGS_Reg512;
union REGS_Reg512
{
  byte v[64];
  uint u32[16];
  float f32[16];
  ulong u64[8];
  double f64[8];
}

////////////////////////////////
//~ rjf: Register Slicing Types

typedef struct REGS_Rng REGS_Rng;
struct REGS_Rng
{
  ushort byte_off;
  ushort byte_size;
}

typedef struct REGS_Slice REGS_Slice;
struct REGS_Slice
{
  ushort code;
  byte byte_off;
  byte byte_size;
}

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers

internal ulong regs_block_size_from_arch(Arch arch);
internal ulong regs_reg_code_count_from_arch(Arch arch);
internal ulong regs_alias_code_count_from_arch(Arch arch);
internal String8 *regs_reg_code_string_table_from_arch(Arch arch);
internal String8 *regs_alias_code_string_table_from_arch(Arch arch);
internal REGS_Rng *regs_reg_code_rng_table_from_arch(Arch arch);
internal REGS_Slice *regs_alias_code_slice_table_from_arch(Arch arch);
internal REGS_UsageKind *regs_reg_code_usage_kind_table_from_arch(Arch arch);
internal REGS_UsageKind *regs_alias_code_usage_kind_table_from_arch(Arch arch);
internal ulong regs_rip_from_arch_block(Arch arch, void *block);
internal ulong regs_rsp_from_arch_block(Arch arch, void *block);
internal void regs_arch_block_write_rip(Arch arch, void *block, ulong rip);
internal void regs_arch_block_write_rsp(Arch arch, void *block, ulong rsp);

#endif // REGS_H

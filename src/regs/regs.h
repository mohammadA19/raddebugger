// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef REGS_H
#define REGS_H

////////////////////////////////
//~ rjf: Register Usage Kinds

enum REGS_UsageKind
{
  REGS_UsageKind_Normal,
  REGS_UsageKind_Vector,
};

////////////////////////////////
//~ rjf: Register Types

typedef byte REGS_RegCode;
typedef byte REGS_AliasCode;

union REGS_Reg16
{
  byte v[2];
  ushort u16;
};

union REGS_Reg32
{
  byte v[4];
  U32 u32;
  F32 f32;
};

union REGS_Reg64
{
  byte v[8];
  U64 u64;
  F64 f64;
};

#pragma pack(push, 1)
struct REGS_Reg80
{
  U64 int1_frac63;
  ushort sign1_exp15;
};
#pragma pack(pop)

union REGS_Reg128
{
  byte v[16];
  U32 u32[4];
  F32 f32[4];
  U64 u64[2];
  F64 f64[2];
};

union REGS_Reg256
{
  byte v[32];
  U32 u32[8];
  F32 f32[8];
  U64 u64[4];
  F64 f64[4];
};

union REGS_Reg512
{
  byte v[64];
  U32 u32[16];
  F32 f32[16];
  U64 u64[8];
  F64 f64[8];
};

////////////////////////////////
//~ rjf: Register Slicing Types

struct REGS_Rng
{
  ushort byte_off;
  ushort byte_size;
};

struct REGS_Slice
{
  ushort code;
  byte byte_off;
  byte byte_size;
};

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers

U64 regs_block_size_from_arch(Arch arch);
U64 regs_reg_code_count_from_arch(Arch arch);
U64 regs_alias_code_count_from_arch(Arch arch);
String8 *regs_reg_code_string_table_from_arch(Arch arch);
String8 *regs_alias_code_string_table_from_arch(Arch arch);
REGS_Rng *regs_reg_code_rng_table_from_arch(Arch arch);
REGS_Slice *regs_alias_code_slice_table_from_arch(Arch arch);
REGS_UsageKind *regs_reg_code_usage_kind_table_from_arch(Arch arch);
REGS_UsageKind *regs_alias_code_usage_kind_table_from_arch(Arch arch);
U64 regs_rip_from_arch_block(Arch arch, void *block);
U64 regs_rsp_from_arch_block(Arch arch, void *block);
void regs_arch_block_write_rip(Arch arch, void *block, U64 rip);
void regs_arch_block_write_rsp(Arch arch, void *block, U64 rsp);

#endif // REGS_H

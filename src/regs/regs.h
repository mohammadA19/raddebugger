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
}

////////////////////////////////
//~ rjf: Register Types

typedef uint8 REGS_RegCode;
typedef uint8 REGS_AliasCode;

[Union]
struct REGS_Reg16
{
  uint8 v[2];
  uint16 u16;
}

[Union]
struct REGS_Reg32
{
  uint8 v[4];
  uint32 u32;
  float f32;
}

[Union]
struct REGS_Reg64
{
  uint8 v[8];
  uint64 u64;
  double f64;
}

#pragma pack(push, 1)
struct REGS_Reg80
{
  uint64 int1_frac63;
  uint16 sign1_exp15;
}
#pragma pack(pop)

[Union]
struct REGS_Reg128
{
  uint8 v[16];
  uint32 u32[4];
  float f32[4];
  uint64 u64[2];
  double f64[2];
}

[Union]
struct REGS_Reg256
{
  uint8 v[32];
  uint32 u32[8];
  float f32[8];
  uint64 u64[4];
  double f64[4];
}

[Union]
struct REGS_Reg512
{
  uint8 v[64];
  uint32 u32[16];
  float f32[16];
  uint64 u64[8];
  double f64[8];
}

////////////////////////////////
//~ rjf: Register Slicing Types

struct REGS_Rng
{
  uint16 byte_off;
  uint16 byte_size;
}

struct REGS_Slice
{
  uint16 code;
  uint8 byte_off;
  uint8 byte_size;
}

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.h"

////////////////////////////////
//~ rjf: Helpers

uint64 regs_block_size_from_arch(Arch arch);
uint64 regs_reg_code_count_from_arch(Arch arch);
uint64 regs_alias_code_count_from_arch(Arch arch);
String8* regs_reg_code_string_table_from_arch(Arch arch);
String8* regs_alias_code_string_table_from_arch(Arch arch);
REGS_Rng* regs_reg_code_rng_table_from_arch(Arch arch);
REGS_Slice* regs_alias_code_slice_table_from_arch(Arch arch);
REGS_UsageKind* regs_reg_code_usage_kind_table_from_arch(Arch arch);
REGS_UsageKind* regs_alias_code_usage_kind_table_from_arch(Arch arch);
uint64 regs_rip_from_arch_block(Arch arch, void* block);
uint64 regs_rsp_from_arch_block(Arch arch, void* block);
void regs_arch_block_write_rip(Arch arch, void* block, uint64 rip);
void regs_arch_block_write_rsp(Arch arch, void* block, uint64 rsp);

#endif // REGS_H

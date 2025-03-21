// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Generated Code

#include "regs/generated/regs.meta.c"

////////////////////////////////
//~ rjf: Helpers

ulong
regs_rip_from_arch_block(Arch arch, void *block)
{
  ulong result = 0;
  if(block != 0) switch(arch)
  {
    default:{}break;
    case Arch_x64:{result = ((REGS_RegBlockX64 *)block)->rip.u64;}break;
    case Arch_x86:{result = (ulong)((REGS_RegBlockX86 *)block)->eip.u32;}break;
  }
  return result;
}

ulong
regs_rsp_from_arch_block(Arch arch, void *block)
{
  ulong result = 0;
  if(block != 0) switch(arch)
  {
    default:{}break;
    case Arch_x64:{result = ((REGS_RegBlockX64 *)block)->rsp.u64;}break;
    case Arch_x86:{result = (ulong)((REGS_RegBlockX86 *)block)->esp.u32;}break;
  }
  return result;
}

void
regs_arch_block_write_rip(Arch arch, void *block, ulong rip)
{
  if(block != 0) switch(arch)
  {
    default:{}break;
    case Arch_x64:{((REGS_RegBlockX64 *)block)->rip.u64 = rip;}break;
    case Arch_x86:{((REGS_RegBlockX86 *)block)->eip.u32 = (uint)rip;}break;
  }
}

void
regs_arch_block_write_rsp(Arch arch, void *block, ulong rsp)
{
  if(block != 0) switch(arch)
  {
    default:{}break;
    case Arch_x64:{((REGS_RegBlockX64 *)block)->rsp.u64 = rsp;}break;
    case Arch_x86:{((REGS_RegBlockX86 *)block)->esp.u32 = (uint)rsp;}break;
  }
}

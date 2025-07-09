// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal u32
pdb_hash_v1(String8 string)
{
  u32 result = 0;
  u8 *ptr = string.str;
  u8 *opl = ptr + (string.size&(~3));
  for(; ptr < opl; ptr += 4)
  {
    result ^= *(u32*)ptr;
  }
  if((string.size&2) != 0)
  {
    result ^= *(u16*)ptr; ptr += 2;
  }
  if((string.size&1) != 0)
  {
    result ^= *ptr;
  }
  result |= 0x20202020;
  result ^= (result >> 11);
  result ^= (result >> 16);
  return result;
}


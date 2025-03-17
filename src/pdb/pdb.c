// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

uint32
pdb_hash_v1(StringView str)
{
  uint32 result = 0;
  uint8* ptr = str.Ptr;
  uint8* opl = ptr + (str.Length&(~3));
  for(; ptr < opl; ptr += 4)
  {
    result ^= *(uint32*)ptr;
  }
  if((str.Length&2) != 0)
  {
    result ^= *(uint16*)ptr; ptr += 2;
  }
  if((str.Length&1) != 0)
  {
    result ^= *ptr;
  }
  result |= 0x20202020;
  result ^= (result >> 11);
  result ^= (result >> 16);
  return result;
}


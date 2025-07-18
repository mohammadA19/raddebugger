// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef ELF_PARSE_H
#define ELF_PARSE_H

////////////////////////////////

typedef struct ELF_BinInfo
{
  ELF_Hdr64 hdr;
  Rng1<uint64>   sh_name_range;
} ELF_BinInfo;

typedef struct ELF_Shdr64Array
{
  uint64         count;
  ELF_Shdr64 *v;
} ELF_Shdr64Array;

typedef struct ELF_GnuDebugLink
{
  String8 path;
  uint32     checksum;
} ELF_GnuDebugLink;

////////////////////////////////



#endif // ELF_PARSE_H

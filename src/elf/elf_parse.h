// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////

typedef struct ELF_BinInfo
{
  ELF_Hdr64 hdr;
  Rng1U64   sh_name_range;
} ELF_BinInfo;

typedef struct ELF_Shdr64Array
{
  U64         count;
  ELF_Shdr64 *v;
} ELF_Shdr64Array;

typedef struct ELF_GnuDebugLink
{
  String8 path;
  U32     checksum;
} ELF_GnuDebugLink;

////////////////////////////////




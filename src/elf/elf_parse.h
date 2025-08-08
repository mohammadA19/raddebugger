// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef ELF_PARSE_H
#define ELF_PARSE_H

////////////////////////////////
//~ rjf: Parsed Structure Types

struct ELF_Shdr64Array
{
    ELF_Shdr64 *v;
    U64 count;
};

struct ELF_Phdr64Array
{
    ELF_Phdr64 *v;
    U64 count;
};

struct ELF_Bin
{
    ELF_Hdr64 hdr;
    Rng1U64 sh_name_range;
    ELF_Shdr64Array shdrs;
    ELF_Phdr64Array phdrs;
};

struct ELF_GnuDebugLink
{
    String8 path;
    U32 checksum;
};

////////////////////////////////
//~ rjf: Parsing Functions

//- rjf: top-level binary parsing

//- rjf: extra bin info extraction

#endif // ELF_PARSE_H

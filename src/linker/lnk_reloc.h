// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

enum
{
  LNK_Reloc_NULL,
  LNK_Reloc_ADDR_16,
  LNK_Reloc_ADDR_32,
  LNK_Reloc_ADDR_64,
  LNK_Reloc_CHUNK_SIZE_FILE_16,
  LNK_Reloc_CHUNK_SIZE_FILE_32,
  LNK_Reloc_CHUNK_SIZE_VIRT_32,
  LNK_Reloc_FILE_ALIGN_32,
  LNK_Reloc_FILE_OFF_15,
  LNK_Reloc_FILE_OFF_32,
  LNK_Reloc_FILE_OFF_64,
  LNK_Reloc_REL32,
  LNK_Reloc_REL32_1,
  LNK_Reloc_REL32_2,
  LNK_Reloc_REL32_3,
  LNK_Reloc_REL32_4,
  LNK_Reloc_REL32_5,
  LNK_Reloc_SECT_REL,
  LNK_Reloc_SECT_IDX,
  LNK_Reloc_VIRT_ALIGN_32,
  LNK_Reloc_VIRT_OFF_32,
} LNK_RelocType;

typedef struct LNK_Reloc
{
  LNK_Reloc*  next;
  LNK_Chunk*         chunk;
  LNK_RelocType      type;
  U64                apply_off;
  LNK_Symbol* symbol;
} LNK_Reloc;

typedef struct LNK_RelocList
{
  U64        count;
  LNK_Reloc* first;
  LNK_Reloc* last;
} LNK_RelocList;



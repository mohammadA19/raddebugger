// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct LNK_Section
{
  Arena*            arena;
  U64               id;
  String8           name;
  String8           symbol_name;
  COFF_SectionFlags flags;
  String8           sort_index;

  LNK_ChunkManager* cman;
  LNK_Chunk*        root;

  // overwhelming number of chunks don't have sort index and grouping
  // them speeds up sort step
  LNK_Chunk* nosort_chunk;

  LNK_RelocList reloc_list;

  B32 emit_header; // TODO: this is a hack to make reloc serializer work in resource converter
  B32 has_layout;
  B32 is_loose;

  B32  is_merged;
  U64  merge_sect_id;
  U64* id_map;

  U64             isect;
  U64             virt_off;
  U64             file_off;
  LNK_ChunkLayout layout;
} LNK_Section;

typedef struct LNK_SectionNode
{
  LNK_SectionNode* next;
  LNK_Section             data;
} LNK_SectionNode;

typedef struct LNK_SectionList
{
  U64              count;
  LNK_SectionNode* first;
  LNK_SectionNode* last;
} LNK_SectionList;

typedef struct LNK_SectionArray
{
  U64          count;
  LNK_Section* v;
} LNK_SectionArray;

typedef struct LNK_SectionPtrArray
{
  U64           count;
  LNK_Section** v;
} LNK_SectionPtrArray;

typedef struct LNK_SectionTable
{
  Arena*           arena;
  U64              section_virt_off;
  U64              sect_align;
  U64              file_align;
  U64              id_max;
  LNK_SectionList  list;
  LNK_SectionList  merge_list;
  LNK_SectionList  empties_list;
  LNK_SectionNode* null_sect;
} LNK_SectionTable;

////////////////////////////////

typedef struct
{
  COFF_MachineType  machine;
  Rng1U64*          range_arr;
  LNK_Section**      sect_arr;
} LNK_SectionDataBuilder;

////////////////////////////////










#if LNK_DEBUG_CHUNKS
#endif


// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct D2R_ConvertParams
{
  String8 dbg_name;
  String8 dbg_data;
  String8 exe_name;
  String8 exe_data;
  ExecutableImageKind exe_kind;
  RDIM_SubsetFlags subset_flags;
  B32 deterministic;
};

typedef struct D2R_TypeTable
{
  HashTable          *ht;
  RDIM_TypeChunkList *types;
  uint64                 type_chunk_cap;
  RDIM_Type          *varg_type;
} D2R_TypeTable;

typedef struct D2R_TagNode
{
  struct D2R_TagNode *next;
  DW_TagNode         *cur_node;
  RDIM_Type          *type;
  RDIM_Scope         *scope;
} D2R_TagNode;

typedef struct D2R_CompUnitContribMap
{
  uint64                    count;
  uint64                   *info_off_arr;
  RDIM_Rng1uint64ChunkList *voff_range_arr;
} D2R_CompUnitContribMap;

////////////////////////////////


////////////////////////////////


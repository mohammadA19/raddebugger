// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADCON_DWARF_H
#define RADCON_DWARF_H

typedef struct D2R_TypeTable
{
  HashTable          *ht;
  RDIM_TypeChunkList *types;
  U64                 type_chunk_cap;
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
  U64               count;
  U64              *info_off_arr;
  RDIM_Rng1U64List *voff_range_arr;
} D2R_CompUnitContribMap;

////////////////////////////////


////////////////////////////////


#endif // RADCON_DWARF_H


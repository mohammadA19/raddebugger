// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

enum D2R_ConvertFlags : U64
{
#define X(t,n,k) D2R_ConvertFlag_##t = (1ull << RDI_SectionKind_##t),
  RDI_SectionKind_XList
#undef X
  D2R_ConvertFlag_StrictParse,
}

typedef struct D2R_User2Convert
{
  String8          input_exe_name;
  String8          input_exe_data;
  String8          input_debug_name;
  String8          input_debug_data;
  String8          output_name;
  D2R_ConvertFlags flags;
  String8List      errors;
} D2R_User2Convert;

typedef struct D2R_TypeTable
{
  HashTable          *ht;
  RDIM_TypeChunkList *types;
  U64                 type_chunk_cap;
  RDIM_Type          *void_type;
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
// Command Line -> Conversion Inputs


////////////////////////////////
// Top-Level Conversion Entry Point


////////////////////////////////
// Enum Conversion



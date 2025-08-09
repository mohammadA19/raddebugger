// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct LNK_Lib
{
  string          path;
  string          data;
  COFF_ArchiveType type;
  U32              symbol_count;
  U32             *member_off_arr;
  String8List      symbol_name_list;
  string          long_names;
  U64              input_idx;
} LNK_Lib;

typedef struct LNK_LibNode
{
  LNK_Lib             data;
  struct LNK_LibNode *next;
} LNK_LibNode;

typedef struct LNK_LibNodeArray
{
  U64          count;
  LNK_LibNode *v;
} LNK_LibNodeArray;

typedef struct LNK_LibList
{
  U64                 count;
  struct LNK_LibNode *first;
} LNK_LibList;
 
typedef struct
{
  string     *data_arr;
  string     *path_arr;
  LNK_LibList  free_libs;
  LNK_LibList  valid_libs;
  LNK_LibList  invalid_libs;
} LNK_LibIniter;




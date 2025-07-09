// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct LNK_Lib
{
  String8          path;
  String8          data;
  COFF_ArchiveType type;
  u32              symbol_count;
  u32             *member_off_arr;
  String8List      symbol_name_list;
  String8          long_names;
  u64              input_idx;
} LNK_Lib;

typedef struct LNK_LibNode
{
  LNK_Lib             data;
  struct LNK_LibNode *next;
} LNK_LibNode;

typedef struct LNK_LibNodeArray
{
  u64          count;
  LNK_LibNode *v;
} LNK_LibNodeArray;

typedef struct LNK_LibList
{
  u64                 count;
  struct LNK_LibNode *first;
} LNK_LibList;
 
typedef struct
{
  String8     *data_arr;
  String8     *path_arr;
  LNK_LibList  free_libs;
  LNK_LibList  valid_libs;
  LNK_LibList  invalid_libs;
} LNK_LibIniter;


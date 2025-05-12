// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct LNK_Lib
{
  String8          path;
  String8          data;
  COFF_ArchiveType type;
  U32              symbol_count;
  U32 *            member_off_arr;
  String8List      symbol_name_list;
  String8          long_names;
  U64              input_idx;
} LNK_Lib;

typedef struct LNK_LibNode
{
  LNK_LibNode* next;
  LNK_Lib             data;
} LNK_LibNode;

typedef struct LNK_LibNodeArray
{
  U64          count;
  LNK_LibNode* v;
} LNK_LibNodeArray;

typedef struct LNK_LibList
{
  U64                 count;
  LNK_LibNode* first;
  LNK_LibNode* last;
} LNK_LibList;

////////////////////////////////

typedef struct LNK_LibMember
{
  String8 name;
  String8 data;
} LNK_LibMember;

typedef struct LNK_LibMemberNode
{
  LNK_LibMemberNode* next;
  LNK_LibMember             data;
} LNK_LibMemberNode;

typedef struct LNK_LibMemberList
{
  U64                count;
  LNK_LibMemberNode* first;
  LNK_LibMemberNode* last;
} LNK_LibMemberList;

typedef struct LNK_LibSymbol
{
  String8 name;
  U64     member_idx;
} LNK_LibSymbol;

typedef struct LNK_LibSymbolNode
{
  LNK_LibSymbolNode* next;
  LNK_LibSymbol             data;
} LNK_LibSymbolNode;

typedef struct LNK_LibSymbolList
{
  U64                count;
  LNK_LibSymbolNode* first;
  LNK_LibSymbolNode* last;
} LNK_LibSymbolList;

typedef struct LNK_LibWriter
{
  Arena*            arena;
  LNK_LibMemberList member_list;
  LNK_LibSymbolList symbol_list;
} LNK_LibWriter;

typedef struct LNK_LibBuild
{
  U64            symbol_count;
  U64            member_count;
  LNK_LibSymbol* symbol_array;
  LNK_LibMember* member_array;
} LNK_LibBuild;

////////////////////////////////

typedef struct
{
  LNK_LibNode*     node_arr;
  String8*         data_arr;
  String8*         path_arr;
  U64              base_input_idx;
} LNK_LibIniter;

////////////////////////////////



////////////////////////////////



////////////////////////////////


////////////////////////////////



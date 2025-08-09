// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

// --- Input -------------------------------------------------------------------

typedef struct LNK_Obj
{
  string              data;
  string              path;
  struct LNK_Lib      *lib;
  U32                  input_idx;
  COFF_FileHeaderInfo  header;
  U32                 *comdats;
  B8                   hotpatch;
  U32Node            **associated_sections;
  LNK_SymbolHashTrie **symlinks;
} LNK_Obj;

typedef struct LNK_ObjNode
{
  struct LNK_ObjNode *next;
  LNK_Obj             data;
} LNK_ObjNode;

typedef struct LNK_ObjList
{
  U64          count;
  LNK_ObjNode *first;
  LNK_ObjNode *last;
} LNK_ObjList;

typedef struct LNK_ObjNodeArray
{
  U64          count;
  LNK_ObjNode *v;
} LNK_ObjNodeArray;

typedef struct LNK_SymbolInputResult
{
  LNK_SymbolList weak_symbols;
  LNK_SymbolList undef_symbols;
} LNK_SymbolInputResult;

// --- Directive Parser --------------------------------------------------------

typedef struct LNK_Directive
{
  struct LNK_Directive *next;
  string               id;
  String8List           value_list;
} LNK_Directive;

typedef struct LNK_DirectiveList
{
  U64            count;
  LNK_Directive *first;
  LNK_Directive *last;
} LNK_DirectiveList;

typedef struct LNK_DirectiveInfo
{
  LNK_DirectiveList v[LNK_CmdSwitch_Count];
} LNK_DirectiveInfo;

// --- Workers Contexts --------------------------------------------------------

typedef struct
{
  LNK_InputObj    **inputs;
  LNK_ObjNodeArray  objs;
  U64               obj_id_base;
  U32               machine;
} LNK_ObjIniter;

typedef struct
{
  LNK_SymbolTable *symtab;
  LNK_ObjNodeArray objs;
  LNK_SymbolList  *weak_lists;
  LNK_SymbolList  *undef_lists;
} LNK_InputCoffSymbolTable;

typedef struct
{
  LNK_Obj    **objs;
  string      name;
  B32          collect_discarded;
  String8List *out_lists;
} LNK_SectionCollector;

// --- Error -------------------------------------------------------------------


// --- Input -------------------------------------------------------------------


// --- Metadata ----------------------------------------------------------------


// --- Symbol & Section Helpers ------------------------------------------------


// --- Helpers ----------------------------------------------------------------- 


// --- Directive Parser --------------------------------------------------------



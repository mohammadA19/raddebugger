// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

// --- Input -------------------------------------------------------------------

typedef struct LNK_Obj
{
  String8             data;
  String8             path;
  String8             lib_path;
  uint32                 input_idx;
  COFF_FileHeaderInfo header;
  uint32                *comdats;
  B8                  hotpatch;
} LNK_Obj;

typedef struct LNK_ObjNode
{
  struct LNK_ObjNode *next;
  LNK_Obj             data;
} LNK_ObjNode;

typedef struct LNK_ObjList
{
  uint64          count;
  LNK_ObjNode *first;
  LNK_ObjNode *last;
} LNK_ObjList;

typedef struct LNK_ObjNodeArray
{
  uint64          count;
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
  String8               id;
  String8List           value_list;
} LNK_Directive;

typedef struct LNK_DirectiveList
{
  uint64            count;
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
  uint64               obj_id_base;
  uint32               machine;
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
  String8      name;
  B32          collect_discarded;
  String8List *out_lists;
} LNK_SectionCollector;

// --- Error -------------------------------------------------------------------


// --- Input -------------------------------------------------------------------


// --- Metadata ----------------------------------------------------------------


// --- Symbol & Section Helpers ------------------------------------------------


// --- Helpers ----------------------------------------------------------------- 


// --- Directive Parser --------------------------------------------------------



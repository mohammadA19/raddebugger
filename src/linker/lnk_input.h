// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef enum
{
  LNK_InputSource_CmdLine, // specified on command line
  LNK_InputSource_Default, // specified through defaultlib switch
  LNK_InputSource_Obj,     // refrenced from objects
  LNK_InputSource_Count
} LNK_InputSourceType;

typedef String8Node LNK_InputLib;
typedef String8List LNK_InputLibList;

typedef struct LNK_InputImport
{
  String8         coff_import;
  U64             input_idx;
  struct LNK_Lib *lib;
} LNK_InputImport;

typedef struct LNK_InputImportNode
{
  struct LNK_InputImportNode *next;
  LNK_InputImport data;
} LNK_InputImportNode;

typedef struct LNK_InputImportList
{
  U64                  count;
  LNK_InputImportNode *first;
  LNK_InputImportNode *last;
} LNK_InputImportList;

typedef struct LNK_InputObj
{
  struct LNK_InputObj *next;
  B32                  is_thin;
  B32                  has_disk_read_failed;
  String8              dedup_id;
  String8              path;
  String8              data;
  struct LNK_Lib      *lib;
  U64                  input_idx;
} LNK_InputObj;

typedef struct LNK_InputObjList
{
  U64           count;
  LNK_InputObj *first;
  LNK_InputObj *last;
} LNK_InputObjList;

////////////////////////////////







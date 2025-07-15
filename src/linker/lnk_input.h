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
  String8              lib_path;
  U64                  input_idx;
} LNK_InputObj;

typedef struct LNK_InputObjList
{
  U64           count;
  LNK_InputObj *first;
  LNK_InputObj *last;
} LNK_InputObjList;

////////////////////////////////

static String8 lnk_string_from_input_source(LNK_InputSourceType input_source);

static void             lnk_input_obj_list_push_node(LNK_InputObjList *list, LNK_InputObj *node);
static LNK_InputObj *   lnk_input_obj_list_push(Arena *arena, LNK_InputObjList *list);
static void             lnk_input_obj_list_concat_in_place(LNK_InputObjList *list, LNK_InputObjList *to_concat);

static LNK_InputObj **  lnk_array_from_input_obj_list(Arena *arena, LNK_InputObjList list);
static LNK_InputObj **  lnk_thin_array_from_input_obj_list(Arena *arena, LNK_InputObjList list, U64 *count_out);
static String8Array     lnk_path_array_from_input_obj_array(Arena *arena, LNK_InputObj **arr, U64 count);
static LNK_InputObjList lnk_list_from_input_obj_arr(LNK_InputObj **arr, U64 count);
static LNK_InputObjList lnk_input_obj_list_from_string_list(Arena *arena, String8List list);

static LNK_InputImportNode *  lnk_input_import_list_push(Arena *arena, LNK_InputImportList *list);
static void                   lnk_input_import_list_concat_in_place(LNK_InputImportList *list, LNK_InputImportList *to_concat);
static LNK_InputImportNode ** lnk_input_import_arr_from_list(Arena *arena, LNK_InputImportList list);
static LNK_InputImportList    lnk_list_from_input_import_arr(LNK_InputImportNode **arr, U64 count);


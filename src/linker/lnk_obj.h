// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////

typedef struct LNK_InputObj
{
  LNK_InputObj* next;
  B32                  is_thin;
  B32                  has_disk_read_failed;
  String8              dedup_id;
  String8              path;
  String8              data;
  String8              lib_path;
} LNK_InputObj;

typedef struct LNK_InputObjList
{
  U64           count;
  LNK_InputObj* first;
  LNK_InputObj* last;
} LNK_InputObjList;

////////////////////////////////

typedef struct LNK_Directive
{
  LNK_Directive* next;
  String8               id;
  String8List           value_list;
} LNK_Directive;

typedef struct LNK_DirectiveList
{
  U64            count;
  LNK_Directive* first;
  LNK_Directive* last;
} LNK_DirectiveList;

typedef struct LNK_DirectiveInfo
{
  LNK_DirectiveList[LNK_CmdSwitch_Count] v;
} LNK_DirectiveInfo;

////////////////////////////////

#define LNK_MakeChunkInputIdx(obj_idx, sect_idx) (((U64)(obj_idx) << 32) | (U64)((sect_idx) & max_U32))

typedef struct LNK_Obj
{
  String8             data;
  String8             path;
  String8             lib_path;
  U64                 input_idx;
  U64                 common_symbol_size;
  COFF_MachineType    machine;
  U64                 chunk_count;
  U64                 sect_count;
  String8*            sect_name_arr;
  String8*            sect_sort_arr;
  LNK_RelocList*      sect_reloc_list_arr;
  LNK_ChunkPtr*       chunk_arr;
  LNK_SymbolList      symbol_list;
  LNK_DirectiveInfo   directive_info;
  LNK_ExportParseList export_parse;
  String8List         include_symbol_list;
  LNK_AltNameList     alt_name_list;
} LNK_Obj;

typedef struct LNK_ObjNode
{
  LNK_ObjNode* next;
  LNK_Obj             data;
} LNK_ObjNode;

typedef struct LNK_ObjList
{
  U64          count;
  LNK_ObjNode* first;
  LNK_ObjNode* last;
} LNK_ObjList;

typedef struct LNK_ObjNodeArray
{
  U64          count;
  LNK_ObjNode* v;
} LNK_ObjNodeArray;

////////////////////////////////

typedef struct LNK_SectDefn
{
  LNK_SectDefn* next;
  LNK_Obj*             obj;
  String8              name;
  COFF_SectionFlags    flags;
  U64                  idx;
} LNK_SectDefn;

typedef struct
{
  U64           count;
  LNK_SectDefn* first;
  LNK_SectDefn* last;
} LNK_SectDefnList;

typedef struct
{
  LNK_InputObj**    inputs;
  LNK_ObjNode*      obj_node_arr;
  U64               obj_id_base;
  LNK_SectDefnList* defn_arr;
  LNK_SectionTable* sectab;
  U64*              function_pad_min;
} LNK_ObjIniter;

typedef struct
{
  Rng1U64*          range_arr;
  LNK_ObjNode*      obj_node_arr;
  LNK_SectDefnList* defn_arr;
  LNK_SectDefnList* conf_arr;
} LNK_ObjNewSectScanner;

typedef struct
{
  LNK_SectionTable* sectab;
  LNK_ObjNode*      obj_arr;
  U64**             chunk_counts;
} LNK_ChunkCounter;

typedef struct
{
  LNK_ChunkManager* cman;
  U64*              chunk_id;
} LNK_ChunkRefAssign;

typedef struct
{
  LNK_SectionTable* sectab;
  Rng1U64*          range_arr;
  U64**             chunk_ids;
  LNK_ObjNode*      obj_arr;
  LNK_ChunkList**   nosort_chunk_list_arr_arr;
  LNK_ChunkList**   chunk_list_arr_arr;
} LNK_ChunkRefAssigner;

typedef struct
{
  LNK_SymbolType   type;
  LNK_ObjNodeArray in_arr;
  LNK_SymbolList*  out_arr;
  Rng1U64*         range_arr;
} LNK_SymbolCollector;

typedef struct
{
  LNK_Obj**      obj_arr;
  String8        name;
  String8        postfix;
  B32            collect_discarded;
  LNK_ChunkList* list_arr;
} LNK_CollectObjChunksTaskData;

typedef struct
{
  Rng1U64*          range_arr;
  LNK_ObjNodeArray  in_arr;
  String8List*      out_arr;
} LNK_DefaultLibCollector;

typedef struct
{
  LNK_ObjNode*  in_arr;
  String8List*  out_arr;
  Rng1U64*      range_arr;
} LNK_ManifestDependencyCollector;

////////////////////////////////


////////////////////////////////


////////////////////////////////






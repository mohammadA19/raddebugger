// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////

typedef struct LNK_InputObj
{
  struct LNK_InputObj *next;
  B32                  is_thin;
  B32                  has_disk_read_failed;
  String8              dedup_id;
  String8              path;
  String8              data;
  String8              lib_path;
} LNK_InputObj;

typedef struct LNK_InputObjList
{
  ulong           count;
  LNK_InputObj *first;
  LNK_InputObj *last;
} LNK_InputObjList;

////////////////////////////////

typedef struct LNK_Directive
{
  struct LNK_Directive *next;
  String8               id;
  String8List           value_list;
} LNK_Directive;

typedef struct LNK_DirectiveList
{
  ulong            count;
  LNK_Directive *first;
  LNK_Directive *last;
} LNK_DirectiveList;

typedef struct LNK_DirectiveInfo
{
  LNK_DirectiveList v[LNK_CmdSwitch_Count];
} LNK_DirectiveInfo;

////////////////////////////////

#define LNK_MakeChunkInputIdx(obj_idx, sect_idx) (((ulong)(obj_idx) << 32) | (ulong)((sect_idx) & max_U32))

typedef struct LNK_Obj
{
  String8             data;
  String8             path;
  String8             lib_path;
  ulong                 input_idx;
  ulong                 common_symbol_size;
  COFF_MachineType    machine;
  ulong                 chunk_count;
  ulong                 sect_count;
  String8            *sect_name_arr;
  String8            *sect_sort_arr;
  LNK_RelocList      *sect_reloc_list_arr;
  LNK_ChunkPtr       *chunk_arr;
  LNK_SymbolList      symbol_list;
  LNK_DirectiveInfo   directive_info;
  LNK_ExportParseList export_parse;
  String8List         include_symbol_list;
  LNK_AltNameList     alt_name_list;
} LNK_Obj;

typedef struct LNK_ObjNode
{
  struct LNK_ObjNode *next;
  LNK_Obj             data;
} LNK_ObjNode;

typedef struct LNK_ObjList
{
  ulong          count;
  LNK_ObjNode *first;
  LNK_ObjNode *last;
} LNK_ObjList;

typedef struct LNK_ObjNodeArray
{
  ulong          count;
  LNK_ObjNode *v;
} LNK_ObjNodeArray;

////////////////////////////////

typedef struct LNK_SectDefn
{
  struct LNK_SectDefn *next;
  LNK_Obj             *obj;
  String8              name;
  COFF_SectionFlags    flags;
  ulong                  idx;
} LNK_SectDefn;

typedef struct LNK_SectDefnList
{
  ulong           count;
  LNK_SectDefn *first;
  LNK_SectDefn *last;
}

typedef struct LNK_ObjIniter
{
  LNK_InputObj    **inputs;
  LNK_ObjNode      *obj_node_arr;
  ulong               obj_id_base;
  LNK_SectDefnList *defn_arr;
  LNK_SectionTable *st;
  ulong              *function_pad_min;
}

typedef struct LNK_ObjNewSectScanner
{
  Rng1U64          *range_arr;
  LNK_ObjNode      *obj_node_arr;
  LNK_SectDefnList *defn_arr;
  LNK_SectDefnList *conf_arr;
}

typedef struct LNK_ChunkCounter
{
  LNK_SectionTable *st;
  LNK_ObjNode      *obj_arr;
  ulong             **chunk_count_arr_arr;
}

typedef struct LNK_ChunkRefAssign
{
  LNK_ChunkManager *cman;
  ulong             **chunk_id_arr_arr;
  ulong               obj_idx;
}

typedef struct LNK_ChunkRefAssigner
{
  LNK_SectionTable *st;
  Rng1U64          *range_arr;
  ulong             **chunk_id_arr_arr;
  LNK_ObjNode      *obj_arr;
  LNK_ChunkList   **nosort_chunk_list_arr_arr;
  LNK_ChunkList   **chunk_list_arr_arr;
}

typedef struct LNK_SymbolCollector
{
  LNK_SymbolType   type;
  LNK_ObjNodeArray in_arr;
  LNK_SymbolList  *out_arr;
  Rng1U64         *range_arr;
}

typedef struct LNK_CollectObjChunksTaskData
{
  LNK_Obj      **obj_arr;
  String8        name;
  String8        postfix;
  B32            collect_discarded;
  LNK_ChunkList *list_arr;
}

typedef struct LNK_DefaultLibCollector
{
  Rng1U64          *range_arr;
  LNK_ObjNodeArray  in_arr;
  String8List      *out_arr;
}

typedef struct LNK_ManifestDependencyCollector
{
  LNK_ObjNode  *in_arr;
  String8List  *out_arr;
  Rng1U64      *range_arr;
}

////////////////////////////////

internal void lnk_error_obj(LNK_ErrorCode code, LNK_Obj *obj, char *fmt, ...);

////////////////////////////////

internal void             lnk_input_obj_list_push_node(LNK_InputObjList *list, LNK_InputObj *node);
internal void             lnk_input_obj_list_concat_in_place(LNK_InputObjList *list, LNK_InputObjList *to_concat);
internal LNK_InputObj *   lnk_input_obj_list_push(Arena *arena, LNK_InputObjList *list);
internal LNK_InputObj **  lnk_array_from_input_obj_list(Arena *arena, LNK_InputObjList list);
internal LNK_InputObjList lnk_input_obj_list_from_string_list(Arena *arena, String8List list);
internal LNK_InputObjList lnk_list_from_input_obj_arr(LNK_InputObj **arr, ulong count);

////////////////////////////////

internal LNK_InputObjList lnk_input_obj_list_from_string_list(Arena *arena, String8List list);

internal LNK_Obj **       lnk_obj_arr_from_list(Arena *arena, LNK_ObjList list);
internal LNK_ObjNodeArray lnk_obj_list_reserve(Arena *arena, LNK_ObjList *list, ulong count);
internal LNK_ChunkList *  lnk_collect_obj_chunks(TP_Context *tp, TP_Arena *arena, ulong obj_count, LNK_Obj **obj_arr, String8 name, String8 postfix, B32 collect_discarded);
internal LNK_ObjNodeArray lnk_obj_list_push_parallel(TP_Context *tp, TP_Arena *tp_arena, LNK_ObjList *obj_list, LNK_SectionTable *st, ulong *function_pad_min, ulong input_count, LNK_InputObj **inputs);

internal LNK_Chunk *       lnk_sect_chunk_array_from_coff(Arena *arena, ulong obj_id, String8 obj_path, String8 coff_data, ulong sect_count, COFF_SectionHeader *coff_sect_arr, String8 *sect_name_arr, String8 *sect_postfix_arr);
internal LNK_SymbolArray   lnk_symbol_array_from_coff(Arena *arena, String8 coff_data, LNK_Obj *obj, String8 obj_path, String8 lib_path, B32 is_big_obj, ulong function_pad_min, ulong string_table_off, ulong sect_count, COFF_SectionHeader *coff_sect_arr, ulong coff_symbol_count, void *coff_symbols, LNK_ChunkPtr *chunk_ptr_arr, LNK_Chunk *master_common_block);
internal LNK_RelocList     lnk_reloc_list_from_coff_reloc_array(Arena *arena, COFF_MachineType machine, LNK_Chunk *chunk, LNK_SymbolArray symbol_array, COFF_Reloc *reloc_v, ulong reloc_count);
internal LNK_RelocList *   lnk_reloc_list_array_from_coff(Arena *arena, COFF_MachineType machine, String8 coff_data, ulong sect_count, COFF_SectionHeader *coff_sect_arr, LNK_ChunkPtr *chunk_ptr_arr, LNK_SymbolArray symbol_array);
internal LNK_DirectiveInfo lnk_directive_info_from_sections(Arena *arena, String8 obj_path, String8 lib_path, ulong chunk_count, LNK_RelocList *reloc_list_arr, String8 *sect_name_arr, LNK_Chunk *chunk_arr);

internal uint lnk_obj_get_features(LNK_Obj *obj);
internal uint lnk_obj_get_comp_id(LNK_Obj *obj);
internal uint lnk_obj_get_vol_md(LNK_Obj *obj);


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////

struct LNK_InputObj
{
  struct LNK_InputObj* next;
  B32                  is_thin;
  B32                  has_disk_read_failed;
  StringView              dedup_id;
  StringView              path;
  StringView              data;
  StringView              lib_path;
}

struct LNK_InputObjList
{
  uint64           count;
  LNK_InputObj* first;
  LNK_InputObj* last;
}

////////////////////////////////

struct LNK_Directive
{
  struct LNK_Directive* next;
  StringView               id;
  String8List           value_list;
}

struct LNK_DirectiveList
{
  uint64            count;
  LNK_Directive* first;
  LNK_Directive* last;
}

struct LNK_DirectiveInfo
{
  LNK_DirectiveList v[LNK_CmdSwitch_Count];
}

////////////////////////////////

#define LNK_MakeChunkInputIdx(obj_idx, sect_idx) (((uint64)(obj_idx) << 32) | (uint64)((sect_idx) & max_U32))

struct LNK_Obj
{
  StringView             data;
  StringView             path;
  StringView             lib_path;
  uint64                 input_idx;
  uint64                 common_symbol_size;
  COFF_MachineType    machine;
  uint64                 chunk_count;
  uint64                 sect_count;
  StringView*            sect_name_arr;
  StringView*            sect_sort_arr;
  LNK_RelocList*      sect_reloc_list_arr;
  LNK_ChunkPtr*       chunk_arr;
  LNK_SymbolList      symbol_list;
  LNK_DirectiveInfo   directive_info;
  LNK_ExportParseList export_parse;
  String8List         include_symbol_list;
  LNK_AltNameList     alt_name_list;
}

struct LNK_ObjNode
{
  struct LNK_ObjNode* next;
  LNK_Obj             data;
}

struct LNK_ObjList
{
  uint64          count;
  LNK_ObjNode* first;
  LNK_ObjNode* last;
}

struct LNK_ObjNodeArray
{
  uint64          count;
  LNK_ObjNode* v;
}

////////////////////////////////

struct LNK_SectDefn
{
  struct LNK_SectDefn* next;
  LNK_Obj*             obj;
  StringView              name;
  COFF_SectionFlags    flags;
  uint64                  idx;
}

struct
{
  uint64           count;
  LNK_SectDefn* first;
  LNK_SectDefn* last;
}

struct
{
  LNK_InputObj**    inputs;
  LNK_ObjNode*      obj_node_arr;
  uint64               obj_id_base;
  LNK_SectDefnList* defn_arr;
  LNK_SectionTable* st;
  uint64*              function_pad_min;
}

struct
{
  Rng1U64*          range_arr;
  LNK_ObjNode*      obj_node_arr;
  LNK_SectDefnList* defn_arr;
  LNK_SectDefnList* conf_arr;
}

struct
{
  LNK_SectionTable* st;
  LNK_ObjNode*      obj_arr;
  uint64**             chunk_count_arr_arr;
}

struct
{
  LNK_ChunkManager* cman;
  uint64**             chunk_id_arr_arr;
  uint64               obj_idx;
}

struct
{
  LNK_SectionTable* st;
  Rng1U64*          range_arr;
  uint64**             chunk_id_arr_arr;
  LNK_ObjNode*      obj_arr;
  LNK_ChunkList**   nosort_chunk_list_arr_arr;
  LNK_ChunkList**   chunk_list_arr_arr;
}

struct
{
  LNK_SymbolType   type;
  LNK_ObjNodeArray in_arr;
  LNK_SymbolList*  out_arr;
  Rng1U64*         range_arr;
}

struct
{
  LNK_Obj**      obj_arr;
  StringView        name;
  StringView        postfix;
  B32            collect_discarded;
  LNK_ChunkList* list_arr;
}

struct
{
  Rng1U64*          range_arr;
  LNK_ObjNodeArray  in_arr;
  String8List*      out_arr;
}

struct
{
  LNK_ObjNode*  in_arr;
  String8List*  out_arr;
  Rng1U64*      range_arr;
}

////////////////////////////////

void lnk_error_obj(LNK_ErrorCode code, LNK_Obj* obj, char* fmt, ...);

////////////////////////////////

void             lnk_input_obj_list_push_node(LNK_InputObjList* list, LNK_InputObj* node);
void             lnk_input_obj_list_concat_in_place(LNK_InputObjList* list, LNK_InputObjList* to_concat);
LNK_InputObj *   lnk_input_obj_list_push(Arena* arena, LNK_InputObjList* list);
LNK_InputObj **  lnk_array_from_input_obj_list(Arena* arena, LNK_InputObjList list);
LNK_InputObjList lnk_input_obj_list_from_string_list(Arena* arena, String8List list);
LNK_InputObjList lnk_list_from_input_obj_arr(LNK_InputObj** arr, uint64 count);

////////////////////////////////

LNK_InputObjList lnk_input_obj_list_from_string_list(Arena* arena, String8List list);

LNK_Obj **       lnk_obj_arr_from_list(Arena* arena, LNK_ObjList list);
LNK_ObjNodeArray lnk_obj_list_reserve(Arena* arena, LNK_ObjList* list, uint64 count);
LNK_ChunkList *  lnk_collect_obj_chunks(TP_Context* tp, TP_Arena* arena, uint64 obj_count, LNK_Obj** obj_arr, StringView name, StringView postfix, B32 collect_discarded);
LNK_ObjNodeArray lnk_obj_list_push_parallel(TP_Context* tp, TP_Arena* tp_arena, LNK_ObjList* obj_list, LNK_SectionTable* st, uint64* function_pad_min, uint64 input_count, LNK_InputObj** inputs);

LNK_Chunk *       lnk_sect_chunk_array_from_coff(Arena* arena, uint64 obj_id, StringView obj_path, StringView coff_data, uint64 sect_count, COFF_SectionHeader* coff_sect_arr, StringView* sect_name_arr, StringView* sect_postfix_arr);
LNK_SymbolArray   lnk_symbol_array_from_coff(Arena* arena, StringView coff_data, LNK_Obj* obj, StringView obj_path, StringView lib_path, B32 is_big_obj, uint64 function_pad_min, uint64 string_table_off, uint64 sect_count, COFF_SectionHeader* coff_sect_arr, uint64 coff_symbol_count, void* coff_symbols, LNK_ChunkPtr* chunk_ptr_arr, LNK_Chunk* master_common_block);
LNK_RelocList     lnk_reloc_list_from_coff_reloc_array(Arena* arena, COFF_MachineType machine, LNK_Chunk* chunk, LNK_SymbolArray symbol_array, COFF_Reloc* reloc_v, uint64 reloc_count);
LNK_RelocList *   lnk_reloc_list_array_from_coff(Arena* arena, COFF_MachineType machine, StringView coff_data, uint64 sect_count, COFF_SectionHeader* coff_sect_arr, LNK_ChunkPtr* chunk_ptr_arr, LNK_SymbolArray symbol_array);
LNK_DirectiveInfo lnk_directive_info_from_sections(Arena* arena, StringView obj_path, StringView lib_path, uint64 chunk_count, LNK_RelocList* reloc_list_arr, StringView* sect_name_arr, LNK_Chunk* chunk_arr);

uint32 lnk_obj_get_features(LNK_Obj* obj);
uint32 lnk_obj_get_comp_id(LNK_Obj* obj);
uint32 lnk_obj_get_vol_md(LNK_Obj* obj);


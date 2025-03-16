// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct LNK_Lib
{
  String8          path;
  String8          data;
  COFF_ArchiveType type;
  U32              symbol_count;
  U32 *            member_off_arr;
  String8List      symbol_name_list;
  String8          long_names;
  U64              input_idx;
}

struct LNK_LibNode
{
  struct LNK_LibNode* next;
  LNK_Lib             data;
}

struct LNK_LibNodeArray
{
  U64          count;
  LNK_LibNode* v;
}

struct LNK_LibList
{
  U64                 count;
  struct LNK_LibNode* first;
  struct LNK_LibNode* last;
}

////////////////////////////////

struct LNK_LibMember
{
  String8 name;
  String8 data;
}

struct LNK_LibMemberNode
{
  struct LNK_LibMemberNode* next;
  LNK_LibMember             data;
}

struct LNK_LibMemberList
{
  U64                count;
  LNK_LibMemberNode* first;
  LNK_LibMemberNode* last;
}

struct LNK_LibSymbol
{
  String8 name;
  U64     member_idx;
}

struct LNK_LibSymbolNode
{
  struct LNK_LibSymbolNode* next;
  LNK_LibSymbol             data;
}

struct LNK_LibSymbolList
{
  U64                count;
  LNK_LibSymbolNode* first;
  LNK_LibSymbolNode* last;
}

struct LNK_LibWriter
{
  Arena*            arena;
  LNK_LibMemberList member_list;
  LNK_LibSymbolList symbol_list;
}

struct LNK_LibBuild
{
  U64            symbol_count;
  U64            member_count;
  LNK_LibSymbol* symbol_array;
  LNK_LibMember* member_array;
}

////////////////////////////////

struct
{
  LNK_LibNode*     node_arr;
  String8*         data_arr;
  String8*         path_arr;
  U64              base_input_idx;
}

////////////////////////////////

LNK_LibNode *       lnk_lib_list_reserve(Arena* arena, LNK_LibList* list, U64 count);
LNK_LibMemberNode * lnk_lib_member_list_push(Arena* arena, LNK_LibMemberList* list, LNK_LibMember member);
LNK_LibMember *     lnk_lib_member_array_from_list(Arena* arena, LNK_LibMemberList list);
LNK_LibSymbolNode * lnk_lib_symbol_list_push(Arena* arena, LNK_LibSymbolList* list, LNK_LibSymbol symbol);

LNK_LibSymbol * lnk_lib_symbol_array_from_list(Arena* arena, LNK_LibSymbolList list);
void            lnk_lib_symbol_array_sort(LNK_LibSymbol* arr, U64 count);

////////////////////////////////

LNK_Lib lnk_lib_from_data(Arena* arena, String8 data, String8 path);

LNK_LibNodeArray lnk_lib_list_push_parallel(TP_Context* tp, TP_Arena* arena, LNK_LibList* list, String8Array data_arr, String8Array path_arr);
LNK_LibNode *    lnk_lib_list_push(Arena* arena, LNK_LibList* list, String8 data, String8 path);

////////////////////////////////

LNK_LibWriter * lnk_lib_writer_alloc();
void            lnk_lib_writer_release(LNK_LibWriter** writer_ptr);
void            lnk_lib_writer_push_obj(LNK_LibWriter* writer, LNK_Obj* obj);
void            lnk_lib_writer_push_export(LNK_LibWriter* writer, COFF_MachineType machine, U64 time_stamp, String8 dll_name, LNK_Export* exp);
LNK_LibBuild    lnk_lib_build_from_writer(Arena* arena, LNK_LibWriter* writer);
String8List     lnk_coff_archive_from_lib_build(Arena* arena, LNK_LibBuild* lib, B32 emit_second_member, COFF_TimeStamp time_stamp, U32 mode);

////////////////////////////////

LNK_LibBuild lnk_build_lib(Arena* arena, COFF_MachineType machine, COFF_TimeStamp time_stamp, String8 dll_name, LNK_ObjList obj_list, LNK_ExportTable* exptab);
String8List  lnk_build_import_entry_obj(Arena* arena, String8 dll_name, COFF_MachineType machine);
String8List  lnk_build_null_import_descriptor_obj(Arena* arena, COFF_MachineType machine);
String8List  lnk_build_null_thunk_data_obj(Arena* arena, String8 dll_name, COFF_MachineType machine);
String8      lnk_build_lib_member_header(Arena* arena, String8 name, COFF_TimeStamp time_stamp, U16 user_id, U16 group_id, U16 mode, U32 size);
String8List  lnk_build_import_lib(TP_Context* tp, TP_Arena* arena, COFF_MachineType machine, COFF_TimeStamp time_stamp, String8 lib_name, String8 dll_name, LNK_ExportTable* exptab);


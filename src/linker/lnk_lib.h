// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct LNK_Lib
{
  StringView          path;
  StringView          data;
  COFF_ArchiveType type;
  uint32              symbol_count;
  uint32 *            member_off_arr;
  String8List      symbol_name_list;
  StringView          long_names;
  uint64              input_idx;
}

struct LNK_LibNode
{
  struct LNK_LibNode* next;
  LNK_Lib             data;
}

struct LNK_LibNodeArray
{
  uint64          count;
  LNK_LibNode* v;
}

struct LNK_LibList
{
  uint64                 count;
  struct LNK_LibNode* first;
  struct LNK_LibNode* last;
}

////////////////////////////////

struct LNK_LibMember
{
  StringView name;
  StringView data;
}

struct LNK_LibMemberNode
{
  struct LNK_LibMemberNode* next;
  LNK_LibMember             data;
}

struct LNK_LibMemberList
{
  uint64                count;
  LNK_LibMemberNode* first;
  LNK_LibMemberNode* last;
}

struct LNK_LibSymbol
{
  StringView name;
  uint64     member_idx;
}

struct LNK_LibSymbolNode
{
  struct LNK_LibSymbolNode* next;
  LNK_LibSymbol             data;
}

struct LNK_LibSymbolList
{
  uint64                count;
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
  uint64            symbol_count;
  uint64            member_count;
  LNK_LibSymbol* symbol_array;
  LNK_LibMember* member_array;
}

////////////////////////////////

struct
{
  LNK_LibNode*     node_arr;
  StringView*         data_arr;
  StringView*         path_arr;
  uint64              base_input_idx;
}

////////////////////////////////

LNK_LibNode *       lnk_lib_list_reserve(Arena* arena, LNK_LibList* list, uint64 count);
LNK_LibMemberNode * lnk_lib_member_list_push(Arena* arena, LNK_LibMemberList* list, LNK_LibMember member);
LNK_LibMember *     lnk_lib_member_array_from_list(Arena* arena, LNK_LibMemberList list);
LNK_LibSymbolNode * lnk_lib_symbol_list_push(Arena* arena, LNK_LibSymbolList* list, LNK_LibSymbol symbol);

LNK_LibSymbol * lnk_lib_symbol_array_from_list(Arena* arena, LNK_LibSymbolList list);
void            lnk_lib_symbol_array_sort(LNK_LibSymbol* arr, uint64 count);

////////////////////////////////

LNK_Lib lnk_lib_from_data(Arena* arena, StringView data, StringView path);

LNK_LibNodeArray lnk_lib_list_push_parallel(TP_Context* tp, TP_Arena* arena, LNK_LibList* list, String8Array data_arr, String8Array path_arr);
LNK_LibNode *    lnk_lib_list_push(Arena* arena, LNK_LibList* list, StringView data, StringView path);

////////////////////////////////

LNK_LibWriter * lnk_lib_writer_alloc();
void            lnk_lib_writer_release(LNK_LibWriter** writer_ptr);
void            lnk_lib_writer_push_obj(LNK_LibWriter* writer, LNK_Obj* obj);
void            lnk_lib_writer_push_export(LNK_LibWriter* writer, COFF_MachineType machine, uint64 time_stamp, StringView dll_name, LNK_Export* exp);
LNK_LibBuild    lnk_lib_build_from_writer(Arena* arena, LNK_LibWriter* writer);
String8List     lnk_coff_archive_from_lib_build(Arena* arena, LNK_LibBuild* lib, B32 emit_second_member, COFF_TimeStamp time_stamp, uint32 mode);

////////////////////////////////

LNK_LibBuild lnk_build_lib(Arena* arena, COFF_MachineType machine, COFF_TimeStamp time_stamp, StringView dll_name, LNK_ObjList obj_list, LNK_ExportTable* exptab);
String8List  lnk_build_import_entry_obj(Arena* arena, StringView dll_name, COFF_MachineType machine);
String8List  lnk_build_null_import_descriptor_obj(Arena* arena, COFF_MachineType machine);
String8List  lnk_build_null_thunk_data_obj(Arena* arena, StringView dll_name, COFF_MachineType machine);
StringView      lnk_build_lib_member_header(Arena* arena, StringView name, COFF_TimeStamp time_stamp, uint16 user_id, uint16 group_id, uint16 mode, uint32 size);
String8List  lnk_build_import_lib(TP_Context* tp, TP_Arena* arena, COFF_MachineType machine, COFF_TimeStamp time_stamp, StringView lib_name, StringView dll_name, LNK_ExportTable* exptab);


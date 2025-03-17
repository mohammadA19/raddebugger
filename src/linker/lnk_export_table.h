// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct LNK_Export
{
  struct LNK_Export*  next;
  StringView             name;
  LNK_Symbol*         symbol;
  uint32                 id;
  uint16                 ordinal;
  COFF_ImportType     type;
  B32                 is_private;
}

struct LNK_ExportList
{
  uint64         count;
  LNK_Export* first;
  LNK_Export* last;
}

struct LNK_ExportArray
{
  uint64         count;
  LNK_Export* v;
}

struct LNK_ExportTable
{
  Arena*         arena;
  HashTable*     name_export_ht;
  HashTable*     noname_export_ht;
  uint64            voff_size;
  uint64            max_ordinal;
  B8*            is_ordinal_used;
}

LNK_ExportTable * lnk_export_table_alloc();
void              lnk_export_table_release(LNK_ExportTable** exptab_ptr);
LNK_Export *      lnk_export_table_search(LNK_ExportTable* exptab, StringView name);
void              lnk_collect_exports_from_def_files(LNK_ExportTable* exptab, String8List path_list);
void              lnk_build_edata(LNK_ExportTable* exptab, LNK_SectionTable* st, LNK_SymbolTable* symtab, StringView image_name, COFF_MachineType machine);
void lnk_collect_exports_from_obj_directives(LNK_ExportTable* exptab, LNK_ObjList obj_list, LNK_SymbolTable* symtab);


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

srtuct LNK_Export
{
  struct LNK_Export  *next;
  String8             name;
  LNK_Symbol         *symbol;
  U32                 id;
  ushort                 ordinal;
  COFF_ImportType     type;
  B32                 is_private;
};

srtuct LNK_ExportList
{
  U64         count;
  LNK_Export *first;
  LNK_Export *last;
};

srtuct LNK_ExportArray
{
  U64         count;
  LNK_Export *v;
};

srtuct LNK_ExportTable
{
  Arena         *arena;
  HashTable     *name_export_ht;
  HashTable     *noname_export_ht;
  U64            voff_size;
  U64            max_ordinal;
  B8            *is_ordinal_used;
};

LNK_ExportTable * lnk_export_table_alloc();
void              lnk_export_table_release(LNK_ExportTable **exptab_ptr);
LNK_Export *      lnk_export_table_search(LNK_ExportTable *exptab, String8 name);
void              lnk_collect_exports_from_def_files(LNK_ExportTable *exptab, String8List path_list);
void              lnk_build_edata(LNK_ExportTable *exptab, LNK_SectionTable *st, LNK_SymbolTable *symtab, String8 image_name, COFF_MachineType machine);
void lnk_collect_exports_from_obj_directives(LNK_ExportTable *exptab, LNK_ObjList obj_list, LNK_SymbolTable *symtab);


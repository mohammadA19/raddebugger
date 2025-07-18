// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

// --- Link --------------------------------------------------------------------

typedef struct LNK_LinkContext
{
  LNK_SymbolTable   *symtab;
  uint64                objs_count;
  LNK_Obj          **objs;
  PE_ExportParseList export_symbol_list;
  LNK_LibList        lib_index[LNK_InputSource_Count];
} LNK_LinkContext;

// -- Image --------------------------------------------------------------------

#define LNK_REMOVED_SECTION_NUMBER_32 (uint32)-3
#define LNK_REMOVED_SECTION_NUMBER_16 (uint16)-3

typedef struct LNK_ImageContext
{
  String8           image_data;
  LNK_SectionTable *sectab;
} LNK_ImageContext;

typedef struct LNK_SymbolTableFixup
{
  uint32 idx;
  uint32 obj_idx;
  uint32 obj_symbol_idx;
} LNK_SymbolTableFixup;

typedef struct LNK_SymbolTableFixupNode
{
  LNK_SymbolTableFixup data;
  struct LNK_SymbolTableFixupNode *next;
} LNK_SymbolTableFixupNode;

typedef struct LNK_SymbolTableFixupList
{
  uint64                       count;
  LNK_SymbolTableFixupNode *first;
  LNK_SymbolTableFixupNode *last;
} LNK_SymbolTableFixupList;

typedef struct LNK_SymbolTableFixupArray
{
  uint64                   count;
  LNK_SymbolTableFixup *v;
} LNK_SymbolTableFixupArray;

typedef struct LNK_SectionDefinition
{
  String8           name;
  COFF_SectionFlags flags;
  uint64               contribs_count;
  struct LNK_Obj   *obj;
  uint64               obj_sect_idx;
} LNK_SectionDefinition;

typedef struct LNK_CommonBlockContrib
{
  struct LNK_Symbol *symbol;
  union {
    uint32 size;
    uint32 offset;
  } u;
} LNK_CommonBlockContrib;

// --- Base Reloc --------------------------------------------------------------

typedef struct LNK_BaseRelocPage
{
  uint64     voff;
  uint64List entries_addr32;
  uint64List entries_addr64;
} LNK_BaseRelocPage;

typedef struct LNK_BaseRelocPageNode
{
  struct LNK_BaseRelocPageNode *next;
  LNK_BaseRelocPage             v;
} LNK_BaseRelocPageNode;

typedef struct LNK_BaseRelocPageList
{
  uint64                    count;
  LNK_BaseRelocPageNode *first;
  LNK_BaseRelocPageNode *last;
} LNK_BaseRelocPageList;

typedef struct LNK_BaseRelocPageArray
{
  uint64                count;
  LNK_BaseRelocPage *v;
} LNK_BaseRelocPageArray;

// --- Workers Contexts --------------------------------------------------------

typedef struct
{
  LNK_SymbolTable           *symtab;
  LNK_SectionTable          *sectab;
  LNK_SectionContrib        *null_sc;
  LNK_SymbolTableFixupArray *obj_symtab_fixups;
  uint64                        function_pad_min;
  uint64                        default_align;
  uint64                        objs_count;
  LNK_Obj                  **objs;
  LNK_SectionContrib      ***sect_map;
  HashTable                 *contribs_ht;
  LNK_SectionArray           image_sects;
  union {
    struct {
      HashTable **defns;
    } gather_sects;
    struct {
      uint64                    *counts;
      uint64                    *offsets;
      LNK_CommonBlockContrib *contribs;
    } common_block;
    struct {
      LNK_SectionContribChunk **chunks;
    } sort_contribs;
    struct {
      B8                    **was_symbol_patched;
      LNK_Section            *common_block_sect;
      Rng1<uint64>                *ranges;
      LNK_CommonBlockContrib *common_block_contribs;
    } patch_symtabs;
  } u;
} LNK_BuildImageTask;

typedef struct
{
  uint64                     page_size;
  Rng1<uint64>                *range_arr;
  LNK_BaseRelocPageList  *list_arr;
  HashTable             **page_ht_arr;
  B32                     is_large_addr_aware;
} LNK_BaseRelocTask;

typedef struct
{
  Rng1<uint64>                *ranges;
  uint64                     page_size;
  LNK_BaseRelocPageList  *list_arr;
  LNK_Obj               **obj_arr;
  HashTable             **page_ht_arr;
  B32                     is_large_addr_aware;
} LNK_ObjBaseRelocTask;

typedef struct
{
  LNK_InputObjList    input_obj_list;
  uint64                 input_imports_count;
  LNK_InputImport    *input_imports;
  LNK_InputImportList input_import_list;
  LNK_SymbolList      unresolved_symbol_list;
} LNK_SymbolFinderResult;

typedef struct
{
  PathStyle               path_style;
  LNK_SymbolTable        *symtab;
  LNK_SymbolNodeArray     lookup_node_arr;
  LNK_SymbolFinderResult *result_arr;
  Rng1<uint64>                *range_arr;
} LNK_SymbolFinder;

typedef struct
{
  String8              image_data;
  LNK_Obj            **objs;
  uint64                  image_base;
  COFF_SectionHeader **image_section_table;
} LNK_ObjRelocPatcher;

typedef struct
{
  String8 path;
  String8 temp_path;
  String8 data;
} LNK_WriteThreadContext;

typedef struct
{
  String8  data;
  Rng1<uint64> *ranges;
  uint128    *hashes;
} LNK_Blake3Hasher;

typedef struct
{
  LNK_SymbolTable  *symtab;
  union {
    LNK_ObjNodeArray objs;
    LNK_LibNodeArray libs;
  } u;
} LNK_SymbolPusher;

// --- Config -----------------------------------------------------------------


// --- Entry Point -------------------------------------------------------------


// --- Path --------------------------------------------------------------------


// --- Hasher ------------------------------------------------------------------


// --- Manifest ----------------------------------------------------------------


// --- Internal Objs -----------------------------------------------------------


// --- Link Context ------------------------------------------------------------




// --- Win32 Image -------------------------------------------------------------


// --- Logger ------------------------------------------------------------------



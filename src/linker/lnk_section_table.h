// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

srtuct LNK_Section
{
  Arena            *arena;
  ulong               id;
  String8           name;
  String8           symbol_name;
  COFF_SectionFlags flags;
  String8           sort_index;

  LNK_ChunkManager *cman;
  LNK_Chunk        *root;

  // overwhelming number of chunks don't have sort index and grouping
  // them speeds up sort step
  LNK_Chunk *nosort_chunk;

  LNK_RelocList reloc_list;

  B32 emit_header; // TODO: this is a hack to make reloc serializer work in resource converter
  B32 has_layout;
  B32 is_loose;

  B32  is_merged;
  ulong  merge_sect_id;
  ulong *id_map;

  ulong             isect;
  ulong             virt_off;
  ulong             file_off;
  LNK_ChunkLayout layout;
};

srtuct LNK_SectionNode
{
  struct LNK_SectionNode *next;
  LNK_Section             data;
};

srtuct LNK_SectionList
{
  ulong              count;
  LNK_SectionNode *first;
  LNK_SectionNode *last;
};

srtuct LNK_SectionArray
{
  ulong          count;
  LNK_Section *v;
};

srtuct LNK_SectionPtrArray
{
  ulong           count;
  LNK_Section **v;
};

srtuct LNK_SectionTable
{
  Arena           *arena;
  ulong              section_virt_off;
  ulong              sect_align;
  ulong              file_align;
  ulong              id_max;
  LNK_SectionList  list;
  LNK_SectionList  merge_list;
  LNK_SectionList  empties_list;
  LNK_SectionNode *null_sect;
};

////////////////////////////////

srtuct
{
  COFF_MachineType  machine;
  Rng1U64          *range_arr;
  LNK_Section      **sect_arr;
};

////////////////////////////////

LNK_SectionNode * lnk_section_list_remove(LNK_SectionList *list, String8 name);
LNK_SectionNode * lnk_section_list_search_node(LNK_SectionList *list, String8 name);
LNK_Section     * lnk_section_list_search(LNK_SectionList *list, String8 name);

LNK_SectionArray    lnk_section_array_from_list(Arena *arena, LNK_SectionList list);
LNK_SectionPtrArray lnk_section_ptr_array_from_list(Arena *arena, LNK_SectionList list);

void lnk_section_associate_chunks(LNK_Section *sect, LNK_Chunk *head, LNK_Chunk *associate);

LNK_Reloc * lnk_section_push_reloc(LNK_Section *sect, LNK_Chunk *chunk, LNK_RelocType type, ulong apply_off, LNK_Symbol *symbol);
LNK_Reloc * lnk_section_push_reloc_undefined(LNK_Section *sect, LNK_Chunk *chunk, LNK_RelocType type, ulong apply_off, String8 undefined_symbol_name, LNK_SymbolScopeFlags scope_flags);

void lnk_section_merge(LNK_Section *dst, LNK_Section *src);
void lnk_section_build_data(LNK_Section *sect, COFF_MachineType machine);

String8 lnk_make_section_sort_index(Arena *arena, String8 name, COFF_SectionFlags flags, ulong section_index);

LNK_Chunk *   lnk_section_push_chunk_raw(LNK_Section *sect, LNK_Chunk *parent, void *data_ptr, ulong data_size, String8 sort_index);
LNK_Chunk *   lnk_section_push_chunk_data(LNK_Section *sect, LNK_Chunk *parent, String8 data, String8 sort_index);
LNK_Chunk *   lnk_section_push_chunk_u32(LNK_Section *sect, LNK_Chunk *parent, uint value, String8 sort_index);
LNK_Chunk *   lnk_section_push_chunk_u64(LNK_Section *sect, LNK_Chunk *parent, uint value, String8 sort_index);
LNK_Chunk *   lnk_section_push_chunk_bss(LNK_Section *sect, LNK_Chunk *parent, ulong size, String8 sort_index);
LNK_Chunk *   lnk_section_push_chunk_list(LNK_Section *sect, LNK_Chunk *parent, String8 sort_index);

LNK_SectionTable *  lnk_section_table_alloc(ulong section_virt_off, ulong sect_align, ulong file_align);
void                lnk_section_table_release(LNK_SectionTable **st_ptr);
LNK_Section *       lnk_section_table_push(LNK_SectionTable *st, String8 name, COFF_SectionFlags flags);
LNK_Section *       lnk_section_table_push_null(LNK_SectionTable *st);
void                lnk_section_table_remove(LNK_SectionTable *st, LNK_SymbolTable *symtab, String8 name);
LNK_Section *       lnk_section_table_search(LNK_SectionTable *st, String8 name);
LNK_Section *       lnk_section_table_search_id(LNK_SectionTable *st, ulong id);
void                lnk_section_table_merge(LNK_SectionTable *st, LNK_MergeDirectiveList merge_list);
void                lnk_section_table_remove_empties(LNK_SectionTable *st, LNK_SymbolTable *symtab);
void                lnk_section_table_build_data(TP_Context *tp, LNK_SectionTable *st, COFF_MachineType machine);
void                lnk_section_table_assign_virtual_offsets(LNK_SectionTable *st);
void                lnk_section_table_assign_file_offsets(LNK_SectionTable *st);
void                lnk_section_table_assign_indices(LNK_SectionTable *st);
String8             lnk_section_table_serialize(TP_Context *tp, Arena *arena, LNK_SectionTable *st, COFF_MachineType machine);

LNK_ChunkPtr ** lnk_chunk_id_map_from_section_table(Arena *arena, LNK_SectionTable *st);
LNK_Section **  lnk_sect_id_map_from_section_table(Arena *arena, LNK_SectionTable *st);
LNK_ChunkRef    lnk_get_final_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
LNK_Section *   lnk_sect_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
LNK_Chunk *     lnk_chunk_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkPtr **chunk_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_isect_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_off_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_virt_off_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_file_off_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_virt_size_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_file_size_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
String8         lnk_data_from_chunk_ref(LNK_Section **sect_id_map, String8 image_data, LNK_ChunkRef chunk_ref);
String8         lnk_data_from_chunk_ref_no_pad(LNK_Section **sect_id_map, String8 image_data, LNK_ChunkRef chunk_ref);
ISectOff        lnk_sc_from_chunk_ref(LNK_Section **sect_id_map, LNK_ChunkRef chunk_ref);
ulong             lnk_virt_off_from_reloc(LNK_Section **sect_id_map, LNK_Reloc *reloc);
ulong             lnk_isect_from_symbol(LNK_Section **sect_id_map, LNK_Symbol *symbol);
ulong             lnk_sect_off_from_symbol(LNK_Section **sect_id_map, LNK_Symbol *symbol);
ulong             lnk_virt_off_from_symbol(LNK_Section **sect_id_map, LNK_Symbol *symbol);
ulong             lnk_file_off_from_symbol(LNK_Section **sect_id_map, LNK_Symbol *symbol);
ulong             lnk_virt_size_from_symbol(LNK_Section **sect_id_map, LNK_Symbol *symbol);
ulong             lnk_file_size_from_symbol(LNK_Section **sect_id_map, LNK_Symbol *symbol);

#if LNK_DEBUG_CHUNKS
void lnk_dump_chunks(LNK_SectionTable *st);
#endif


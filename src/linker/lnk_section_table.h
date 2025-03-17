// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct LNK_Section
{
  Arena*            arena;
  uint64               id;
  StringView           name;
  StringView           symbol_name;
  COFF_SectionFlags flags;
  StringView           sort_index;

  LNK_ChunkManager* cman;
  LNK_Chunk*        root;

  // overwhelming number of chunks don't have sort index and grouping
  // them speeds up sort step
  LNK_Chunk* nosort_chunk;

  LNK_RelocList reloc_list;

  B32 emit_header; // TODO: this is a hack to make reloc serializer work in resource converter
  B32 has_layout;
  B32 is_loose;

  B32  is_merged;
  uint64  merge_sect_id;
  uint64* id_map;

  uint64             isect;
  uint64             virt_off;
  uint64             file_off;
  LNK_ChunkLayout layout;
}

struct LNK_SectionNode
{
  struct LNK_SectionNode* next;
  LNK_Section             data;
}

struct LNK_SectionList
{
  uint64              count;
  LNK_SectionNode* first;
  LNK_SectionNode* last;
}

struct LNK_SectionArray
{
  uint64          count;
  LNK_Section* v;
}

struct LNK_SectionPtrArray
{
  uint64           count;
  LNK_Section** v;
}

struct LNK_SectionTable
{
  Arena*           arena;
  uint64              section_virt_off;
  uint64              sect_align;
  uint64              file_align;
  uint64              id_max;
  LNK_SectionList  list;
  LNK_SectionList  merge_list;
  LNK_SectionList  empties_list;
  LNK_SectionNode* null_sect;
}

////////////////////////////////

struct
{
  COFF_MachineType  machine;
  Rng1U64*          range_arr;
  LNK_Section**      sect_arr;
}

////////////////////////////////

LNK_SectionNode * lnk_section_list_remove(LNK_SectionList* list, StringView name);
LNK_SectionNode * lnk_section_list_search_node(LNK_SectionList* list, StringView name);
LNK_Section     * lnk_section_list_search(LNK_SectionList* list, StringView name);

LNK_SectionArray    lnk_section_array_from_list(Arena* arena, LNK_SectionList list);
LNK_SectionPtrArray lnk_section_ptr_array_from_list(Arena* arena, LNK_SectionList list);

void lnk_section_associate_chunks(LNK_Section* sect, LNK_Chunk* head, LNK_Chunk* associate);

LNK_Reloc * lnk_section_push_reloc(LNK_Section* sect, LNK_Chunk* chunk, LNK_RelocType type, uint64 apply_off, LNK_Symbol* symbol);
LNK_Reloc * lnk_section_push_reloc_undefined(LNK_Section* sect, LNK_Chunk* chunk, LNK_RelocType type, uint64 apply_off, StringView undefined_symbol_name, LNK_SymbolScopeFlags scope_flags);

void lnk_section_merge(LNK_Section* dst, LNK_Section* src);
void lnk_section_build_data(LNK_Section* sect, COFF_MachineType machine);

StringView lnk_make_section_sort_index(Arena* arena, StringView name, COFF_SectionFlags flags, uint64 section_index);

LNK_Chunk *   lnk_section_push_chunk_raw(LNK_Section* sect, LNK_Chunk* parent, void* data_ptr, uint64 data_size, StringView sort_index);
LNK_Chunk *   lnk_section_push_chunk_data(LNK_Section* sect, LNK_Chunk* parent, StringView data, StringView sort_index);
LNK_Chunk *   lnk_section_push_chunk_u32(LNK_Section* sect, LNK_Chunk* parent, uint32 value, StringView sort_index);
LNK_Chunk *   lnk_section_push_chunk_u64(LNK_Section* sect, LNK_Chunk* parent, uint32 value, StringView sort_index);
LNK_Chunk *   lnk_section_push_chunk_bss(LNK_Section* sect, LNK_Chunk* parent, uint64 size, StringView sort_index);
LNK_Chunk *   lnk_section_push_chunk_list(LNK_Section* sect, LNK_Chunk* parent, StringView sort_index);

LNK_SectionTable *  lnk_section_table_alloc(uint64 section_virt_off, uint64 sect_align, uint64 file_align);
void                lnk_section_table_release(LNK_SectionTable** st_ptr);
LNK_Section *       lnk_section_table_push(LNK_SectionTable* st, StringView name, COFF_SectionFlags flags);
LNK_Section *       lnk_section_table_push_null(LNK_SectionTable* st);
void                lnk_section_table_remove(LNK_SectionTable* st, LNK_SymbolTable* symtab, StringView name);
LNK_Section *       lnk_section_table_search(LNK_SectionTable* st, StringView name);
LNK_Section *       lnk_section_table_search_id(LNK_SectionTable* st, uint64 id);
void                lnk_section_table_merge(LNK_SectionTable* st, LNK_MergeDirectiveList merge_list);
void                lnk_section_table_remove_empties(LNK_SectionTable* st, LNK_SymbolTable* symtab);
void                lnk_section_table_build_data(TP_Context* tp, LNK_SectionTable* st, COFF_MachineType machine);
void                lnk_section_table_assign_virtual_offsets(LNK_SectionTable* st);
void                lnk_section_table_assign_file_offsets(LNK_SectionTable* st);
void                lnk_section_table_assign_indices(LNK_SectionTable* st);
StringView             lnk_section_table_serialize(TP_Context* tp, Arena* arena, LNK_SectionTable* st, COFF_MachineType machine);

LNK_ChunkPtr ** lnk_chunk_id_map_from_section_table(Arena* arena, LNK_SectionTable* st);
LNK_Section **  lnk_sect_id_map_from_section_table(Arena* arena, LNK_SectionTable* st);
LNK_ChunkRef    lnk_get_final_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
LNK_Section *   lnk_sect_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
LNK_Chunk *     lnk_chunk_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkPtr** chunk_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_isect_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_off_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_virt_off_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_file_off_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_virt_size_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_file_size_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
StringView         lnk_data_from_chunk_ref(LNK_Section** sect_id_map, StringView image_data, LNK_ChunkRef chunk_ref);
StringView         lnk_data_from_chunk_ref_no_pad(LNK_Section** sect_id_map, StringView image_data, LNK_ChunkRef chunk_ref);
ISectOff        lnk_sc_from_chunk_ref(LNK_Section** sect_id_map, LNK_ChunkRef chunk_ref);
uint64             lnk_virt_off_from_reloc(LNK_Section** sect_id_map, LNK_Reloc* reloc);
uint64             lnk_isect_from_symbol(LNK_Section** sect_id_map, LNK_Symbol* symbol);
uint64             lnk_sect_off_from_symbol(LNK_Section** sect_id_map, LNK_Symbol* symbol);
uint64             lnk_virt_off_from_symbol(LNK_Section** sect_id_map, LNK_Symbol* symbol);
uint64             lnk_file_off_from_symbol(LNK_Section** sect_id_map, LNK_Symbol* symbol);
uint64             lnk_virt_size_from_symbol(LNK_Section** sect_id_map, LNK_Symbol* symbol);
uint64             lnk_file_size_from_symbol(LNK_Section** sect_id_map, LNK_Symbol* symbol);

#if LNK_DEBUG_CHUNKS
void lnk_dump_chunks(LNK_SectionTable* st);
#endif


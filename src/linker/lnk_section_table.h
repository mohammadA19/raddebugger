// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct LNK_SectionContrib
{
  String8Node  first_data_node; // most contributions require at least one data node, so preallocate it here
  String8Node *last_data_node;  // list of data nodes that contribute to final section
  union {
    // used to sort sections to get deterministic output
    struct {
      U32 obj_idx;      // index of the input obj that contributes to the image section
      U32 obj_sect_idx; // index into contributing obj's section table
    };

    // used after section layout is finalized
    struct {
      U32 off;      // contribution offset within the image section
      U16 sect_idx; // section index in the image
      U8  unused[2];
    };
  } u;
  U16 align; // contribution alignment in the image
  B8 hotpatch;
} LNK_SectionContrib;

typedef struct LNK_SectionContribChunk
{
  struct LNK_SectionContribChunk *next;
  U64                             count;
  U64                             cap;
  String8                         sort_idx;
  LNK_SectionContrib            **v;
  LNK_SectionContrib             *v2;
} LNK_SectionContribChunk;

typedef struct LNK_SectionContribChunkList
{
  U64                      chunk_count;
  LNK_SectionContribChunk *first;
  LNK_SectionContribChunk *last;
} LNK_SectionContribChunkList;

typedef struct LNK_Section
{
  String8                     name;
  COFF_SectionFlags           flags;
  LNK_SectionContribChunkList contribs;

  struct LNK_Section *merge_dst;

  U64 voff;
  U64 vsize;
  U64 fsize;
  U64 foff;
  U64 sect_idx;
} LNK_Section;

typedef struct LNK_SectionNode
{
  struct LNK_SectionNode *next;
  struct LNK_SectionNode *prev;
  LNK_Section             data;
} LNK_SectionNode;

typedef struct LNK_SectionList
{
  U64              count;
  LNK_SectionNode *first;
  LNK_SectionNode *last;
} LNK_SectionList;

typedef struct LNK_SectionArray
{
  U64           count;
  LNK_Section **v;
} LNK_SectionArray;

typedef struct LNK_SectionTable
{
  Arena           *arena;
  LNK_SectionList  list;
  LNK_SectionList  merge_list;
  HashTable       *sect_ht;        // (name * COFF_SectionFlags) -> LNK_Section *
  U64              next_sect_idx;
} LNK_SectionTable;

// --- Section Contrib Chunk List ----------------------------------------------

static LNK_SectionContrib *       lnk_section_contrib_chunk_push(LNK_SectionContribChunk *chunk, U64 count);
static LNK_SectionContrib *       lnk_section_contrib_chunk_push_atomic(LNK_SectionContribChunk *chunk, U64 count);
static LNK_SectionContribChunk *  lnk_section_contrib_chunk_list_push_chunk(Arena *arena, LNK_SectionContribChunkList *list, U64 cap, String8 sort_idx);
static void                       lnk_section_contrib_chunk_list_concat_in_place(LNK_SectionContribChunkList *list, LNK_SectionContribChunkList *to_concat);
static LNK_SectionContribChunk ** lnk_array_from_section_contrib_chunk_list(Arena *arena, LNK_SectionContribChunkList list);

// --- Section List ------------------------------------------------------------

static LNK_SectionArray lnk_section_array_from_list(Arena *arena, LNK_SectionList list);

// --- Section Table -----------------------------------------------------------

static String8 lnk_make_name_with_flags(Arena *arena, String8 name, COFF_SectionFlags flags);

static LNK_SectionTable *  lnk_section_table_alloc(void);
static void                lnk_section_table_release(LNK_SectionTable **sectab_ptr);
static LNK_Section *       lnk_section_table_push(LNK_SectionTable *sectab, String8 name, COFF_SectionFlags flags);
static LNK_SectionNode *   lnk_section_table_remove(LNK_SectionTable *sectab, String8 name);
static LNK_Section *       lnk_section_table_search(LNK_SectionTable *sectab, String8 name, COFF_SectionFlags flags);
static LNK_SectionArray    lnk_section_table_search_many(Arena *arena, LNK_SectionTable *sectab, String8 full_or_partial_name);
static void                lnk_section_table_merge(LNK_SectionTable *sectab, LNK_MergeDirectiveList merge_list);

// --- Section Finalization ----------------------------------------------------

static void lnk_finalize_section_layout     (LNK_Section *sect, U64 file_align, U64 pad_size);
static void lnk_assign_section_index        (LNK_Section *sect, U64 sect_idx);
static void lnk_assign_section_virtual_space(LNK_Section *sect, U64 sect_align, U64 *voff_cursor);
static void lnk_assign_section_file_space   (LNK_Section *sect, U64 *foff_cursor);

// --- Section Contribution ----------------------------------------------------

static U64 lnk_size_from_section_contrib(LNK_SectionContrib *sc);
static U64 lnk_voff_from_section_contrib(COFF_SectionHeader **image_section_table, LNK_SectionContrib *sc);
static U64 lnk_foff_from_section_contrib(COFF_SectionHeader **image_section_table, LNK_SectionContrib *sc);
static U64 lnk_fopl_from_section_contrib(COFF_SectionHeader **image_section_table, LNK_SectionContrib *sc);

static LNK_SectionContrib * lnk_get_first_section_contrib(LNK_Section *sect);
static LNK_SectionContrib * lnk_get_last_section_contrib(LNK_Section *sect);
static U64                  lnk_get_section_contrib_size(LNK_Section *sect);
static U64                  lnk_get_first_section_contrib_voff(COFF_SectionHeader **image_section_table, LNK_Section *sect);

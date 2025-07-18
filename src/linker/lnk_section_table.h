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
      uint32 obj_idx;      // index of the input obj that contributes to the image section
      uint32 obj_sect_idx; // index into contributing obj's section table
    };

    // used after section layout is finalized
    struct {
      uint32 off;      // contribution offset within the image section
      uint16 sect_idx; // section index in the image
      uint8  unused[2];
    };
  } u;
  uint16 align; // contribution alignment in the image
  B8 hotpatch;
} LNK_SectionContrib;

typedef struct LNK_SectionContribChunk
{
  struct LNK_SectionContribChunk *next;
  uint64                             count;
  uint64                             cap;
  String8                         sort_idx;
  LNK_SectionContrib            **v;
  LNK_SectionContrib             *v2;
} LNK_SectionContribChunk;

typedef struct LNK_SectionContribChunkList
{
  uint64                      chunk_count;
  LNK_SectionContribChunk *first;
  LNK_SectionContribChunk *last;
} LNK_SectionContribChunkList;

typedef struct LNK_Section
{
  String8                     name;
  COFF_SectionFlags           flags;
  LNK_SectionContribChunkList contribs;

  struct LNK_Section *merge_dst;

  uint64 voff;
  uint64 vsize;
  uint64 fsize;
  uint64 foff;
  uint64 sect_idx;
} LNK_Section;

typedef struct LNK_SectionNode
{
  struct LNK_SectionNode *next;
  struct LNK_SectionNode *prev;
  LNK_Section             data;
} LNK_SectionNode;

typedef struct LNK_SectionList
{
  uint64              count;
  LNK_SectionNode *first;
  LNK_SectionNode *last;
} LNK_SectionList;

typedef struct LNK_SectionArray
{
  uint64           count;
  LNK_Section **v;
} LNK_SectionArray;

typedef struct LNK_SectionTable
{
  Arena           *arena;
  LNK_SectionList  list;
  LNK_SectionList  merge_list;
  HashTable       *sect_ht;        // (name * COFF_SectionFlags) -> LNK_Section *
  uint64              next_sect_idx;
} LNK_SectionTable;

// --- Section Contrib Chunk List ----------------------------------------------


// --- Section List ------------------------------------------------------------


// --- Section Table -----------------------------------------------------------



// --- Section Finalization ----------------------------------------------------


// --- Section Contribution ----------------------------------------------------



// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////

#define LNK_DEBUG_CHUNKS 0

////////////////////////////////

struct LNK_ChunkRef
{
  uint64 sect_id;
  uint64 chunk_id;
}

enum
{
  LNK_Chunk_Null,
  LNK_Chunk_Leaf,
  LNK_Chunk_LeafArray,
  LNK_Chunk_List,
}

struct LNK_Chunk
{
  LNK_ChunkRef      ref;
  LNK_ChunkType     type;
  uint64               align;
  uint64               min_size;
  B32               is_discarded;
  B32               sort_chunk;
  String8           sort_idx;
  uint64               input_idx;
  COFF_SectionFlags flags;
  struct LNK_Chunk* associate;
  union {
    String8                leaf;
    struct LNK_ChunkList*  list;
    struct LNK_ChunkArray* arr;
  } u;
#if LNK_DEBUG_CHUNKS
  String8 debug;
#endif
} LNK_Chunk, * LNK_ChunkPtr;

struct LNK_ChunkNode
{
  struct LNK_ChunkNode* next;
  LNK_ChunkPtr          data;
}

struct LNK_ChunkArray
{
  uint64           count;
  LNK_ChunkPtr* v;
}

struct LNK_ChunkList
{
  uint64            count;
  LNK_ChunkNode* first;
  LNK_ChunkNode* last;
}

enum LNK_ChunkOpType
{
  LNK_ChunkOp_Null,
  LNK_ChunkOp_WriteString,
  LNK_ChunkOp_Align,
  LNK_ChunkOp_Begin,
  LNK_ChunkOp_End,
  LNK_ChunkOp_EndVirt,
}

struct LNK_ChunkOp
{
  struct LNK_ChunkOp* next;
  LNK_ChunkOpType     type;
  union {
    String8    string;
    LNK_Chunk* chunk;
    struct {
      uint64 val;
      uint64 x;
    } align;
    LNK_Chunk* leaf;
  } u;
}

struct LNK_ChunkOpList
{
  uint64          total_chunk_count;
  LNK_ChunkOp* first;
  LNK_ChunkOp* last;
}

struct LNK_ChunkPad
{
  uint64 off;
  uint64 size;
}

struct LNK_ChunkPadArray
{
  uint64           count;
  LNK_ChunkPad* v;
}
struct LNK_ChunkPadArrayNode
{
  struct LNK_ChunkPadArrayNode* next;
  uint64                 cap;
  LNK_ChunkPadArray data;
}
struct LNK_ChunkPadArrayList
{
  uint64                      count;
  LNK_ChunkPadArrayNode* first;
  LNK_ChunkPadArrayNode* last;
}

struct LNK_ChunkLayout
{
  uint64                     total_count;
  LNK_Chunk**             chunk_ptr_array;       // discarded chunks point to g_null_chunk
  uint64*                    chunk_off_array;       // discarded chunks have offset set to max_U64
  uint64*                    chunk_file_size_array; // discarded chunks have offset set to max_U64
  uint64*                    chunk_virt_size_array; // discarded chunks have offset set to max_U64
  uint64                     pad_array_count;
  LNK_ChunkPadArray*      pad_array;
}

struct LNK_ChunkManager
{
  LNK_Chunk* root;
  uint64        total_chunk_count;
}

////////////////////////////////

struct
{
  uint64              offset;
  LNK_ChunkLayout* layout;
}

struct
{
  LNK_ChunkLayout  layout;
  String8          buffer;
  uint8               fill_byte;
  Rng1U64*         ranges;
}

////////////////////////////////

read_only static LNK_Chunk  g_null_chunk      = { 0, 0, /* is_discarded: */ 1 };
read_only static LNK_Chunk* g_null_chunk_ptr  = &g_null_chunk;

////////////////////////////////

LNK_ChunkRef lnk_chunk_ref(uint64 sect_id, uint64 chunk_id);
B32          lnk_chunk_ref_is_equal(LNK_ChunkRef a, LNK_ChunkRef b);

LNK_ChunkNode *  lnk_chunk_list_push(Arena* arena, LNK_ChunkList* list, LNK_Chunk* chunk);
void             lnk_chunk_list_concat_in_place(LNK_ChunkList* list, LNK_ChunkList* to_concat);
void             lnk_chunk_list_concat_in_place_arr(LNK_ChunkList* list, LNK_ChunkList* arr, uint64 count);
LNK_ChunkList ** lnk_make_chunk_list_arr_arr(Arena* arena, uint64 slot_count, uint64 per_count);
void             lnk_chunk_array_sort(LNK_ChunkArray arr);

LNK_ChunkManager * lnk_chunk_manager_alloc(Arena* arena, uint64 id, uint64 align);
LNK_Chunk *        lnk_chunk_push(Arena* arena, LNK_ChunkManager* cman, LNK_Chunk* parent, String8 sort_index);
LNK_Chunk *        lnk_chunk_push_leaf(Arena* arena, LNK_ChunkManager* cman, LNK_Chunk* parent, String8 sort_index, void* raw_ptr, uint64 raw_size);
LNK_Chunk *        lnk_chunk_push_list(Arena* arena, LNK_ChunkManager* cman, LNK_Chunk* parent, String8 sort_index);
LNK_ChunkNode *    lnk_chunk_deep_copy(Arena* arena, LNK_Chunk* chunk);
LNK_ChunkNode *    lnk_merge_chunks(Arena* arena, LNK_ChunkManager* dst_cman, LNK_Chunk* dst, LNK_Chunk* src, uint64* id_map_out, uint64 id_map_max);
void               lnk_chunk_associate(LNK_Chunk* head, LNK_Chunk* associate);
B32                lnk_chunk_is_discarded(LNK_Chunk* chunk);
uint64                lnk_chunk_get_size(LNK_Chunk* chunk);
uint64                lnk_chunk_list_get_node_count(LNK_Chunk* chunk);

void lnk_chunk_op_list_push_node(LNK_ChunkOpList* list, LNK_ChunkOp* op);

LNK_ChunkOp * lnk_push_chunk_op_begin(Arena* arena, uint64 chunk_id);
LNK_ChunkOp * lnk_push_chunk_op_end_virt(Arena* arena);
LNK_ChunkOp * lnk_push_chunk_op_end_file(Arena* arena);
LNK_ChunkOp * lnk_push_chunk_op_align(Arena* arena, uint64 align, uint64 val);
LNK_ChunkOp * lnk_push_chunk_op_write(Arena* arena, String8 string);

LNK_ChunkLayout lnk_layout_from_chunk(Arena* arena, LNK_Chunk* root, uint64 total_chunk_count);
LNK_ChunkLayout lnk_build_chunk_layout(Arena* arena, LNK_ChunkManager* cman);

#define LNK_CHUNK_VISITOR_SIG(name) B32 name(uint64 sect_id, LNK_Chunk* chunk, void* ud)
typedef LNK_CHUNK_VISITOR_SIG(LNK_ChunkVisitorSig);
void lnk_visit_chunks(uint64 sect_id, LNK_Chunk* root, LNK_ChunkVisitorSig* cb, void* ud);

LNK_ChunkNode * lnk_chunk_ptr_list_reserve(Arena* arena, LNK_ChunkList* list, uint64 count);
String8Array    lnk_data_arr_from_chunk_ptr_list(Arena* arena, LNK_ChunkList list);
String8Array *  lnk_data_arr_from_chunk_ptr_list_arr(Arena* arena, LNK_ChunkList* list_arr, uint64 count);

#if LNK_DEBUG_CHUNKS
#define lnk_chunk_set_debugf(a, c, f, ...) do { (c)->debug = push_str8f((a), f, __VA_ARGS__); } while(0)
#else
#define lnk_chunk_set_debugf(a, c, f, ...) ()(c)
#endif


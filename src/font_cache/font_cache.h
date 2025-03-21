// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef FONT_CACHE_H
#define FONT_CACHE_H

////////////////////////////////
//~ rjf: Rasterization Flags

typedef uint FNT_RasterFlags;
enum
{
  FNT_RasterFlag_Smooth = (1<<0),
  FNT_RasterFlag_Hinted = (1<<1),
};

////////////////////////////////
//~ rjf: Handles & Tags

typedef struct FNT_Tag FNT_Tag;
struct FNT_Tag
{
  ulong u64[2];
};

////////////////////////////////
//~ rjf: Draw Package Types (For Cache Queries)

typedef struct FNT_Piece FNT_Piece;
struct FNT_Piece
{
  R_Handle texture;
  Rng2S16 subrect;
  Vec2S16 offset;
  float advance;
  ushort decode_size;
};

typedef struct FNT_PieceChunkNode FNT_PieceChunkNode;
struct FNT_PieceChunkNode
{
  FNT_PieceChunkNode *next;
  FNT_Piece *v;
  ulong count;
  ulong cap;
};

typedef struct FNT_PieceChunkList FNT_PieceChunkList;
struct FNT_PieceChunkList
{
  FNT_PieceChunkNode *first;
  FNT_PieceChunkNode *last;
  ulong node_count;
  ulong total_piece_count;
};

typedef struct FNT_PieceArray FNT_PieceArray;
struct FNT_PieceArray
{
  FNT_Piece *v;
  ulong count;
};

typedef struct FNT_Run FNT_Run;
struct FNT_Run
{
  FNT_PieceArray pieces;
  Vec2F32 dim;
  float ascent;
  float descent;
};

////////////////////////////////
//~ rjf: Font Path -> Handle * Metrics * Path Cache Types

typedef struct FNT_FontHashNode FNT_FontHashNode;
struct FNT_FontHashNode
{
  FNT_FontHashNode *hash_next;
  FNT_Tag tag;
  FP_Handle handle;
  FP_Metrics metrics;
  String8 path;
};

typedef struct FNT_FontHashSlot FNT_FontHashSlot;
struct FNT_FontHashSlot
{
  FNT_FontHashNode *first;
  FNT_FontHashNode *last;
};

////////////////////////////////
//~ rjf: Rasterization Cache Types

typedef struct F_RasterCacheInfo F_RasterCacheInfo;
struct F_RasterCacheInfo
{
  Rng2S16 subrect;
  Vec2S16 raster_dim;
  short atlas_num;
  float advance;
};

typedef struct F_Hash2InfoRasterCacheNode F_Hash2InfoRasterCacheNode;
struct F_Hash2InfoRasterCacheNode
{
  F_Hash2InfoRasterCacheNode *hash_next;
  F_Hash2InfoRasterCacheNode *hash_prev;
  ulong hash;
  F_RasterCacheInfo info;
};

typedef struct FNT_Hash2InfoRasterCacheSlot FNT_Hash2InfoRasterCacheSlot;
struct FNT_Hash2InfoRasterCacheSlot
{
  F_Hash2InfoRasterCacheNode *first;
  F_Hash2InfoRasterCacheNode *last;
};

typedef struct FNT_Hash2StyleRasterCacheNode FNT_Hash2StyleRasterCacheNode;
struct FNT_Hash2StyleRasterCacheNode
{
  FNT_Hash2StyleRasterCacheNode *hash_next;
  FNT_Hash2StyleRasterCacheNode *hash_prev;
  ulong style_hash;
  float ascent;
  float descent;
  float column_width;
  F_RasterCacheInfo *utf8_class1_direct_map;
  ulong utf8_class1_direct_map_mask[4];
  ulong hash2info_slots_count;
  FNT_Hash2InfoRasterCacheSlot *hash2info_slots;
};

typedef struct FNT_Hash2StyleRasterCacheSlot FNT_Hash2StyleRasterCacheSlot;
struct FNT_Hash2StyleRasterCacheSlot
{
  FNT_Hash2StyleRasterCacheNode *first;
  FNT_Hash2StyleRasterCacheNode *last;
};

////////////////////////////////
//~ rjf: Atlas Types

typedef uint FNT_AtlasRegionNodeFlags;
enum
{
  FNT_AtlasRegionNodeFlag_Taken = (1<<0),
};

typedef struct FNT_AtlasRegionNode FNT_AtlasRegionNode;
struct FNT_AtlasRegionNode
{
  FNT_AtlasRegionNode *parent;
  FNT_AtlasRegionNode *children[Corner_COUNT];
  Vec2S16 max_free_size[Corner_COUNT];
  FNT_AtlasRegionNodeFlags flags;
  ulong num_allocated_descendants;
};

typedef struct FNT_Atlas FNT_Atlas;
struct FNT_Atlas
{
  FNT_Atlas *next;
  FNT_Atlas *prev;
  R_Handle texture;
  Vec2S16 root_dim;
  FNT_AtlasRegionNode *root;
};

////////////////////////////////
//~ rjf: Metrics

typedef struct FNT_Metrics FNT_Metrics;
struct FNT_Metrics
{
  float ascent;
  float descent;
  float line_gap;
  float capital_height;
};

////////////////////////////////
//~ rjf: Main State Type

typedef struct FNT_State FNT_State;
struct FNT_State
{
  Arena *permanent_arena;
  Arena *raster_arena;
  
  // rjf: font table
  ulong font_hash_table_size;
  FNT_FontHashSlot *font_hash_table;
  
  // rjf: hash -> raster cache table
  ulong hash2style_slots_count;
  FNT_Hash2StyleRasterCacheSlot *hash2style_slots;
  
  // rjf: atlas list
  FNT_Atlas *first_atlas;
  FNT_Atlas *last_atlas;
};

////////////////////////////////
//~ rjf: Globals

global FNT_State *f_state = 0;

////////////////////////////////
//~ rjf: Basic Functions

internal U128 fnt_hash_from_string(String8 string);
internal ulong fnt_little_hash_from_string(String8 string);
internal Vec2S32 fnt_vertex_from_corner(Corner corner);

////////////////////////////////
//~ rjf: Font Tags

internal FNT_Tag fnt_tag_zero(void);
internal B32 fnt_tag_match(FNT_Tag a, FNT_Tag b);
internal FP_Handle fnt_handle_from_tag(FNT_Tag tag);
internal FP_Metrics fnt_fp_metrics_from_tag(FNT_Tag tag);
internal FNT_Tag fnt_tag_from_path(String8 path);
internal FNT_Tag fnt_tag_from_static_data_string(String8 *data_ptr);
internal String8 fnt_path_from_tag(FNT_Tag tag);

////////////////////////////////
//~ rjf: Atlas

internal Rng2S16 fnt_atlas_region_alloc(Arena *arena, FNT_Atlas *atlas, Vec2S16 needed_size);
internal void fnt_atlas_region_release(FNT_Atlas *atlas, Rng2S16 region);

////////////////////////////////
//~ rjf: Piece Type Functions

internal FNT_Piece *fnt_piece_chunk_list_push_new(Arena *arena, FNT_PieceChunkList *list, ulong cap);
internal void fnt_piece_chunk_list_push(Arena *arena, FNT_PieceChunkList *list, ulong cap, FNT_Piece *piece);
internal FNT_PieceArray fnt_piece_array_from_chunk_list(Arena *arena, FNT_PieceChunkList *list);
internal FNT_PieceArray fnt_piece_array_copy(Arena *arena, FNT_PieceArray *src);

////////////////////////////////
//~ rjf: Rasterization Cache

internal FNT_Hash2StyleRasterCacheNode *fnt_hash2style_from_tag_size_flags(FNT_Tag tag, float size, FNT_RasterFlags flags);
internal FNT_Run fnt_push_run_from_string(Arena *arena, FNT_Tag tag, float size, float base_align_px, float tab_size_px, FNT_RasterFlags flags, String8 string);
internal String8List fnt_wrapped_string_lines_from_font_size_string_max(Arena *arena, FNT_Tag font, float size, float base_align_px, float tab_size_px, String8 string, float max);
internal Vec2F32 fnt_dim_from_tag_size_string(FNT_Tag tag, float size, float base_align_px, float tab_size_px, String8 string);
internal Vec2F32 fnt_dim_from_tag_size_string_list(FNT_Tag tag, float size, float base_align_px, float tab_size_px, String8List list);
internal float fnt_column_size_from_tag_size(FNT_Tag tag, float size);
internal ulong fnt_char_pos_from_tag_size_string_p(FNT_Tag tag, float size, float base_align_px, float tab_size_px, String8 string, float p);

////////////////////////////////
//~ rjf: Metrics

internal FNT_Metrics fnt_metrics_from_tag_size(FNT_Tag tag, float size);
internal float fnt_line_height_from_metrics(FNT_Metrics *metrics);

////////////////////////////////
//~ rjf: Main Calls

internal void fnt_init(void);
internal void fnt_reset(void);

#endif // FONT_CACHE_H

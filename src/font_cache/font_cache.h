// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Rasterization Flags

typedef u32 FNT_RasterFlags;
enum
{
  FNT_RasterFlag_Smooth = (1<<0),
  FNT_RasterFlag_Hinted = (1<<1),
};

////////////////////////////////
//~ rjf: Handles & Tags



struct FNT_Tag
{
  u64 u64[2];
};

////////////////////////////////
//~ rjf: Draw Package Types (For Cache Queries)



struct FNT_Piece
{
  R_Handle texture;
  Rng2S16 subrect;
  Vec2S16 offset;
  f32 advance;
  u16 decode_size;
};



struct FNT_PieceChunkNode
{
  FNT_PieceChunkNode *next;
  FNT_Piece *v;
  u64 count;
  u64 cap;
};



struct FNT_PieceChunkList
{
  FNT_PieceChunkNode *first;
  FNT_PieceChunkNode *last;
  u64 node_count;
  u64 total_piece_count;
};



struct FNT_PieceArray
{
  FNT_Piece *v;
  u64 count;
};



struct FNT_Run
{
  FNT_PieceArray pieces;
  Vec2F32 dim;
  f32 ascent;
  f32 descent;
};

////////////////////////////////
//~ rjf: Font Path -> Handle * Metrics * Path Cache Types



struct FNT_FontHashNode
{
  FNT_FontHashNode *hash_next;
  FNT_Tag tag;
  FP_Handle handle;
  FP_Metrics metrics;
  String8 path;
};



struct FNT_FontHashSlot
{
  FNT_FontHashNode *first;
  FNT_FontHashNode *last;
};

////////////////////////////////
//~ rjf: Rasterization Cache Types

//- rjf: base glyph rasterization / dimensions cache 



struct FNT_RasterCacheInfo
{
  Rng2S16 subrect;
  Vec2S16 raster_dim;
  i16 atlas_num;
  f32 advance;
};



struct FNT_Hash2InfoRasterCacheNode
{
  FNT_Hash2InfoRasterCacheNode *hash_next;
  FNT_Hash2InfoRasterCacheNode *hash_prev;
  u64 hash;
  FNT_RasterCacheInfo info;
};



struct FNT_Hash2InfoRasterCacheSlot
{
  FNT_Hash2InfoRasterCacheNode *first;
  FNT_Hash2InfoRasterCacheNode *last;
};

//- rjf: run cache (arrangements of many glyphs to represent a full string)



struct FNT_RunCacheNode
{
  FNT_RunCacheNode *next;
  String8 string;
  FNT_Run run;
};



struct FNT_RunCacheSlot
{
  FNT_RunCacheNode *first;
  FNT_RunCacheNode *last;
};

//- rjf: style hash -> artifacts/metrics cache



struct FNT_Hash2StyleRasterCacheNode
{
  FNT_Hash2StyleRasterCacheNode *hash_next;
  FNT_Hash2StyleRasterCacheNode *hash_prev;
  u64 style_hash;
  f32 ascent;
  f32 descent;
  f32 column_width;
  FNT_RasterCacheInfo *utf8_class1_direct_map;
  u64 utf8_class1_direct_map_mask[4];
  u64 hash2info_slots_count;
  FNT_Hash2InfoRasterCacheSlot *hash2info_slots;
  u64 run_slots_count;
  FNT_RunCacheSlot *run_slots;
  u64 run_slots_frame_index;
};



struct FNT_Hash2StyleRasterCacheSlot
{
  FNT_Hash2StyleRasterCacheNode *first;
  FNT_Hash2StyleRasterCacheNode *last;
};

////////////////////////////////
//~ rjf: Atlas Types

typedef u32 FNT_AtlasRegionNodeFlags;
enum
{
  FNT_AtlasRegionNodeFlag_Taken = (1<<0),
};



struct FNT_AtlasRegionNode
{
  FNT_AtlasRegionNode *parent;
  FNT_AtlasRegionNode *children[Corner_COUNT];
  Vec2S16 max_free_size[Corner_COUNT];
  FNT_AtlasRegionNodeFlags flags;
  u64 num_allocated_descendants;
};



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



struct FNT_Metrics
{
  f32 ascent;
  f32 descent;
  f32 line_gap;
  f32 capital_height;
};

////////////////////////////////
//~ rjf: Main State Type



struct FNT_State
{
  Arena *permanent_arena;
  Arena *raster_arena;
  Arena *frame_arena;
  u64 frame_index;
  
  // rjf: font table
  u64 font_hash_table_size;
  FNT_FontHashSlot *font_hash_table;
  
  // rjf: hash -> raster cache table
  u64 hash2style_slots_count;
  FNT_Hash2StyleRasterCacheSlot *hash2style_slots;
  
  // rjf: atlas list
  FNT_Atlas *first_atlas;
  FNT_Atlas *last_atlas;
};

////////////////////////////////
//~ rjf: Globals

global FNT_State *fnt_state = 0;

////////////////////////////////
//~ rjf: Basic Functions





////////////////////////////////
//~ rjf: Font Tags









////////////////////////////////
//~ rjf: Atlas




////////////////////////////////
//~ rjf: Piece Type Functions






////////////////////////////////
//~ rjf: Cache Usage

//- rjf: base cache lookups



//- rjf: helpers






////////////////////////////////
//~ rjf: Metrics




////////////////////////////////
//~ rjf: Main Calls







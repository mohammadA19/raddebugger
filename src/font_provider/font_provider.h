// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#define fp_hook C_LINKAGE

////////////////////////////////
//~ rjf: Types

typedef u32 FP_RasterFlags;
enum
{
  FP_RasterFlag_Smooth = (1<<0),
  FP_RasterFlag_Hinted = (1<<1),
};



struct FP_Handle
{
  u64 u64[2];
};



struct FP_Metrics
{
  f32 design_units_per_em;
  f32 ascent;
  f32 descent;
  f32 line_gap;
  f32 capital_height;
};



struct FP_RasterResult
{
  Vec2S16 atlas_dim;
  void *atlas;
  f32 advance;
};


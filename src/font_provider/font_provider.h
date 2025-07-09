// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#define fp_hook C_LINKAGE

////////////////////////////////
//~ rjf: Types

typedef U32 FP_RasterFlags;
enum
{
  FP_RasterFlag_Smooth = (1<<0),
  FP_RasterFlag_Hinted = (1<<1),
};



struct FP_Handle
{
  U64 u64[2];
};



struct FP_Metrics
{
  F32 design_units_per_em;
  F32 ascent;
  F32 descent;
  F32 line_gap;
  F32 capital_height;
};



struct FP_RasterResult
{
  Vec2S16 atlas_dim;
  void *atlas;
  F32 advance;
};


// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef FONT_PROVIDER_H
#define FONT_PROVIDER_H

#define fp_hook C_LINKAGE

////////////////////////////////
//~ rjf: Types

enum FP_RasterFlags : uint32
{
  FP_RasterFlag_Smooth = (1<<0),
  FP_RasterFlag_Hinted = (1<<1),
};

struct FP_Handle
{
  uint64 u64[2];
};

struct FP_Metrics
{
  float design_units_per_em;
  float ascent;
  float descent;
  float line_gap;
  float capital_height;
};

struct FP_RasterResult
{
  Vec2uint16 atlas_dim;
  void *atlas;
  float advance;
};

////////////////////////////////
//~ rjf: Basic Type Functions

internal FP_Handle fp_handle_zero(void);
internal B32 fp_handle_match(FP_Handle a, FP_Handle b);

////////////////////////////////
//~ rjf: Backend Hooks

fp_hook void fp_init(void);
fp_hook FP_Handle fp_font_open(String8 path);
fp_hook FP_Handle fp_font_open_from_static_data_string(String8 *data_ptr);
fp_hook void fp_font_close(FP_Handle handle);
fp_hook FP_Metrics fp_metrics_from_font(FP_Handle font);
fp_hook NO_ASAN FP_RasterResult fp_raster(Arena *arena, FP_Handle font, float size, FP_RasterFlags flags, String8 string);

#endif // FONT_PROVIDER_H

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RENDER_CORE_H
#define RENDER_CORE_H

#define r_hook C_LINKAGE

////////////////////////////////
//~ rjf: Generated Code

#include "generated/render.meta.h"

////////////////////////////////
//~ rjf: Enums

enum R_GeoVertexFlags : uint32
{
  R_GeoVertexFlag_TexCoord = (1<<0),
  R_GeoVertexFlag_Normals  = (1<<1),
  R_GeoVertexFlag_RGB      = (1<<2),
  R_GeoVertexFlag_RGBA     = (1<<3),
};

////////////////////////////////
//~ rjf: Handle Type

union R_Handle
{
  uint64 u64[1];
  uint32 u32[2];
  uint16 u16[4];
};

////////////////////////////////
//~ rjf: Instance Types

struct R_Rect2DInst
{
  Rng2<float> dst;
  Rng2<float> src;
  Vec4<float> colors[Corner_COUNT];
  float corner_radii[Corner_COUNT];
  float border_thickness;
  float edge_softness;
  float white_texture_override;
  float _unused_[1];
};

struct R_Mesh3DInst
{
  Mat4x4float xform;
};

////////////////////////////////
//~ rjf: Batch Types

struct R_Batch
{
  uint8 *v;
  uint64 byte_count;
  uint64 byte_cap;
};

struct R_BatchNode
{
  R_BatchNode *next;
  R_Batch v;
};

struct R_BatchList
{
  R_BatchNode *first;
  R_BatchNode *last;
  uint64 batch_count;
  uint64 byte_count;
  uint64 bytes_per_inst;
};

struct R_BatchGroup2DParams
{
  R_Handle tex;
  R_Tex2DSampleKind tex_sample_kind;
  Mat3x3float xform;
  Rng2<float> clip;
  float transparency;
};

struct R_BatchGroup2DNode
{
  R_BatchGroup2DNode *next;
  R_BatchList batches;
  R_BatchGroup2DParams params;
};

struct R_BatchGroup2DList
{
  R_BatchGroup2DNode *first;
  R_BatchGroup2DNode *last;
  uint64 count;
};

struct R_BatchGroup3DParams
{
  R_Handle mesh_vertices;
  R_Handle mesh_indices;
  R_GeoTopologyKind mesh_geo_topology;
  R_GeoVertexFlags mesh_geo_vertex_flags;
  R_Handle albedo_tex;
  R_Tex2DSampleKind albedo_tex_sample_kind;
  Mat4x4float xform;
};

struct R_BatchGroup3DMapNode
{
  R_BatchGroup3DMapNode *next;
  uint64 hash;
  R_BatchList batches;
  R_BatchGroup3DParams params;
};

struct R_BatchGroup3DMap
{
  R_BatchGroup3DMapNode **slots;
  uint64 slots_count;
};

////////////////////////////////
//~ rjf: Pass Types

struct R_PassParams_UI
{
  R_BatchGroup2DList rects;
};

struct R_PassParams_Blur
{
  Rng2<float> rect;
  Rng2<float> clip;
  float blur_size;
  float corner_radii[Corner_COUNT];
};

struct R_PassParams_Geo3D
{
  Rng2<float> viewport;
  Rng2<float> clip;
  Mat4x4float view;
  Mat4x4float projection;
  R_BatchGroup3DMap mesh_batches;
};

struct R_Pass
{
  R_PassKind kind;
  union
  {
    void *params;
    R_PassParams_UI *params_ui;
    R_PassParams_Blur *params_blur;
    R_PassParams_Geo3D *params_geo3d;
  };
};

struct R_PassNode
{
  R_PassNode *next;
  R_Pass v;
};

struct R_PassList
{
  R_PassNode *first;
  R_PassNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Helpers


////////////////////////////////
//~ rjf: Handle Type Functions


////////////////////////////////
//~ rjf: Batch Type Functions


////////////////////////////////
//~ rjf: Pass Type Functions


////////////////////////////////
//~ rjf: Backend Hooks

//- rjf: top-level layer initialization
r_hook void              r_init(CmdLine *cmdln);

//- rjf: window setup/teardown
r_hook R_Handle          r_window_equip(OS_Handle window);
r_hook void              r_window_unequip(OS_Handle window, R_Handle window_equip);

//- rjf: textures
r_hook R_Handle          r_tex2d_alloc(R_ResourceKind kind, Vec2<uint32> size, R_Tex2DFormat format, void *data);
r_hook void              r_tex2d_release(R_Handle texture);
r_hook R_ResourceKind    r_kind_from_tex2d(R_Handle texture);
r_hook Vec2<uint32>           r_size_from_tex2d(R_Handle texture);
r_hook R_Tex2DFormat     r_format_from_tex2d(R_Handle texture);
r_hook void              r_fill_tex2d_region(R_Handle texture, Rng2<uint32> subrect, void *data);

//- rjf: buffers
r_hook R_Handle          r_buffer_alloc(R_ResourceKind kind, uint64 size, void *data);
r_hook void              r_buffer_release(R_Handle buffer);

//- rjf: frame markers
r_hook void              r_begin_frame(void);
r_hook void              r_end_frame(void);
r_hook void              r_window_begin_frame(OS_Handle window, R_Handle window_equip);
r_hook void              r_window_end_frame(OS_Handle window, R_Handle window_equip);

//- rjf: render pass submission
r_hook void              r_window_submit(OS_Handle window, R_Handle window_equip, R_PassList *passes);

#endif // RENDER_CORE_H

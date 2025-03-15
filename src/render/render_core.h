// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RENDER_CORE_H
#define RENDER_CORE_H

#define r_hook C_LINKAGE

////////////////////////////////
//~ rjf: Generated Code

#include "generated/render.meta.h"

////////////////////////////////
//~ rjf: Enums

R_GeoVertexFlags :: enum U32
{
  R_GeoVertexFlag_TexCoord = (1<<0),
  R_GeoVertexFlag_Normals  = (1<<1),
  R_GeoVertexFlag_RGB      = (1<<2),
  R_GeoVertexFlag_RGBA     = (1<<3),
}

////////////////////////////////
//~ rjf: Handle Type

R_Handle :: struct #raw_union
{
  U64 u64[1];
  U32 u32[2];
  U16 u16[4];
}

////////////////////////////////
//~ rjf: Instance Types

R_Rect2DInst :: struct
{
  Rng2F32 dst;
  Rng2F32 src;
  Vec4F32 colors[Corner_COUNT];
  F32 corner_radii[Corner_COUNT];
  F32 border_thickness;
  F32 edge_softness;
  F32 white_texture_override;
  F32 _unused_[1];
}

R_Mesh3DInst :: struct
{
  Mat4x4F32 xform;
}

////////////////////////////////
//~ rjf: Batch Types

R_Batch :: struct
{
  U8 *v;
  U64 byte_count;
  U64 byte_cap;
}

R_BatchNode :: struct
{
  R_BatchNode *next;
  R_Batch v;
}

R_BatchList :: struct
{
  R_BatchNode *first;
  R_BatchNode *last;
  U64 batch_count;
  U64 byte_count;
  U64 bytes_per_inst;
}

R_BatchGroup2DParams :: struct
{
  R_Handle tex;
  R_Tex2DSampleKind tex_sample_kind;
  Mat3x3F32 xform;
  Rng2F32 clip;
  F32 transparency;
}

R_BatchGroup2DNode :: struct
{
  R_BatchGroup2DNode *next;
  R_BatchList batches;
  R_BatchGroup2DParams params;
}

R_BatchGroup2DList :: struct
{
  R_BatchGroup2DNode *first;
  R_BatchGroup2DNode *last;
  U64 count;
}

R_BatchGroup3DParams :: struct
{
  R_Handle mesh_vertices;
  R_Handle mesh_indices;
  R_GeoTopologyKind mesh_geo_topology;
  R_GeoVertexFlags mesh_geo_vertex_flags;
  R_Handle albedo_tex;
  R_Tex2DSampleKind albedo_tex_sample_kind;
  Mat4x4F32 xform;
}

R_BatchGroup3DMapNode :: struct
{
  R_BatchGroup3DMapNode *next;
  U64 hash;
  R_BatchList batches;
  R_BatchGroup3DParams params;
}

R_BatchGroup3DMap :: struct
{
  R_BatchGroup3DMapNode **slots;
  U64 slots_count;
}

////////////////////////////////
//~ rjf: Pass Types

R_PassParams_UI :: struct
{
  R_BatchGroup2DList rects;
}

R_PassParams_Blur :: struct
{
  Rng2F32 rect;
  Rng2F32 clip;
  F32 blur_size;
  F32 corner_radii[Corner_COUNT];
}

R_PassParams_Geo3D :: struct
{
  Rng2F32 viewport;
  Rng2F32 clip;
  Mat4x4F32 view;
  Mat4x4F32 projection;
  R_BatchGroup3DMap mesh_batches;
}

R_Pass :: struct
{
  R_PassKind kind;
  union
  {
    void *params;
    R_PassParams_UI *params_ui;
    R_PassParams_Blur *params_blur;
    R_PassParams_Geo3D *params_geo3d;
  };
}

R_PassNode :: struct
{
  R_PassNode *next;
  R_Pass v;
}

R_PassList :: struct
{
  R_PassNode *first;
  R_PassNode *last;
  U64 count;
}

////////////////////////////////
//~ rjf: Handle Type Functions

R_Handle r_handle_zero();
B32 r_handle_match(R_Handle a, R_Handle b);

////////////////////////////////
//~ rjf: Batch Type Functions

R_BatchList r_batch_list_make(U64 instance_size);
void *r_batch_list_push_inst(Arena *arena, R_BatchList *list, U64 batch_inst_cap);

////////////////////////////////
//~ rjf: Pass Type Functions

R_Pass *r_pass_from_kind(Arena *arena, R_PassList *list, R_PassKind kind);

////////////////////////////////
//~ rjf: Backend Hooks

//- rjf: top-level layer initialization
r_hook void              r_init(CmdLine *cmdln);

//- rjf: window setup/teardown
r_hook R_Handle          r_window_equip(OS_Handle window);
r_hook void              r_window_unequip(OS_Handle window, R_Handle window_equip);

//- rjf: textures
r_hook R_Handle          r_tex2d_alloc(R_ResourceKind kind, Vec2S32 size, R_Tex2DFormat format, void *data);
r_hook void              r_tex2d_release(R_Handle texture);
r_hook R_ResourceKind    r_kind_from_tex2d(R_Handle texture);
r_hook Vec2S32           r_size_from_tex2d(R_Handle texture);
r_hook R_Tex2DFormat     r_format_from_tex2d(R_Handle texture);
r_hook void              r_fill_tex2d_region(R_Handle texture, Rng2S32 subrect, void *data);

//- rjf: buffers
r_hook R_Handle          r_buffer_alloc(R_ResourceKind kind, U64 size, void *data);
r_hook void              r_buffer_release(R_Handle buffer);

//- rjf: frame markers
r_hook void              r_begin_frame();
r_hook void              r_end_frame();
r_hook void              r_window_begin_frame(OS_Handle window, R_Handle window_equip);
r_hook void              r_window_end_frame(OS_Handle window, R_Handle window_equip);

//- rjf: render pass submission
r_hook void              r_window_submit(OS_Handle window, R_Handle window_equip, R_PassList *passes);

#endif // RENDER_CORE_H

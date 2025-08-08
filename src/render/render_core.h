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

enum R_GeoVertexFlags : U32
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
    U64 u64[1];
    U32 u32[2];
    U16 u16[4];
};

////////////////////////////////
//~ rjf: Instance Types

struct R_Rect2DInst
{
    Rng2F32 dst;
    Rng2F32 src;
    Vec4F32 colors[Corner_COUNT];
    F32 corner_radii[Corner_COUNT];
    F32 border_thickness;
    F32 edge_softness;
    F32 white_texture_override;
    F32 _unused_[1];
};

struct R_Mesh3DInst
{
    Mat4x4F32 xform;
};

////////////////////////////////
//~ rjf: Batch Types

struct R_Batch
{
    U8 *v;
    U64 byte_count;
    U64 byte_cap;
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
    U64 batch_count;
    U64 byte_count;
    U64 bytes_per_inst;
};

struct R_BatchGroup2DParams
{
    R_Handle tex;
    R_Tex2DSampleKind tex_sample_kind;
    Mat3x3F32 xform;
    Rng2F32 clip;
    F32 transparency;
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
    U64 count;
};

struct R_BatchGroup3DParams
{
    R_Handle mesh_vertices;
    R_Handle mesh_indices;
    R_GeoTopologyKind mesh_geo_topology;
    R_GeoVertexFlags mesh_geo_vertex_flags;
    R_Handle albedo_tex;
    R_Tex2DSampleKind albedo_tex_sample_kind;
    Mat4x4F32 xform;
};

struct R_BatchGroup3DMapNode
{
    R_BatchGroup3DMapNode *next;
    U64 hash;
    R_BatchList batches;
    R_BatchGroup3DParams params;
};

struct R_BatchGroup3DMap
{
    R_BatchGroup3DMapNode **slots;
    U64 slots_count;
};

////////////////////////////////
//~ rjf: Pass Types

struct R_PassParams_UI
{
    R_BatchGroup2DList rects;
};

struct R_PassParams_Blur
{
    Rng2F32 rect;
    Rng2F32 clip;
    F32 blur_size;
    F32 corner_radii[Corner_COUNT];
};

struct R_PassParams_Geo3D
{
    Rng2F32 viewport;
    Rng2F32 clip;
    Mat4x4F32 view;
    Mat4x4F32 projection;
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
    U64 count;
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

//- rjf: window setup/teardown

//- rjf: textures

//- rjf: buffers

//- rjf: frame markers

//- rjf: render pass submission

#endif // RENDER_CORE_H

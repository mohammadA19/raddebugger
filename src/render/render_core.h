// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#define r_hook C_LINKAGE

////////////////////////////////
//~ rjf: Generated Code

#include "generated/render.meta.h"

////////////////////////////////
//~ rjf: Enums

public enum R_GeoVertexFlags : uint32
{
  TexCoord = (1<<0),
  Normals  = (1<<1),
  RGB      = (1<<2),
  RGBA     = (1<<3),
}

////////////////////////////////
//~ rjf: Handle Type

[Union]
public struct R_Handle
{
  uint64[1] u64;
  uint32[2] u32;
  uint16[4] u16;
}

////////////////////////////////
//~ rjf: Instance Types

public struct R_Rect2DInst
{
  Rng2F32 dst;
  Rng2F32 src;
  Vec4F32[Corner_COUNT] colors;
  float[Corner_COUNT] corner_radii;
  float border_thickness;
  float edge_softness;
  float white_texture_override;
  float[1] _unused_;
}

public struct R_Mesh3DInst
{
  Mat4x4F32 xform;
}

////////////////////////////////
//~ rjf: Batch Types

public struct R_Batch
{
  uint8* v;
  uint64 byte_count;
  uint64 byte_cap;
}

public struct R_BatchNode
{
  R_BatchNode* next;
  R_Batch v;
}

public struct R_BatchList
{
  R_BatchNode* first;
  R_BatchNode* last;
  uint64 batch_count;
  uint64 byte_count;
  uint64 bytes_per_inst;
}

public struct R_BatchGroup2DParams
{
  R_Handle tex;
  R_Tex2DSampleKind tex_sample_kind;
  Mat3x3F32 xform;
  Rng2F32 clip;
  float transparency;
}

public struct R_BatchGroup2DNode
{
  R_BatchGroup2DNode* next;
  R_BatchList batches;
  R_BatchGroup2DParams params;
}

public struct R_BatchGroup2DList
{
  R_BatchGroup2DNode* first;
  R_BatchGroup2DNode* last;
  uint64 count;
}

public struct R_BatchGroup3DParams
{
  R_Handle mesh_vertices;
  R_Handle mesh_indices;
  R_GeoTopologyKind mesh_geo_topology;
  R_GeoVertexFlags mesh_geo_vertex_flags;
  R_Handle albedo_tex;
  R_Tex2DSampleKind albedo_tex_sample_kind;
  Mat4x4F32 xform;
}

public struct R_BatchGroup3DMapNode
{
  R_BatchGroup3DMapNode* next;
  uint64 hash;
  R_BatchList batches;
  R_BatchGroup3DParams params;
}

public struct R_BatchGroup3DMap
{
  R_BatchGroup3DMapNode** slots;
  uint64 slots_count;
}

////////////////////////////////
//~ rjf: Pass Types

public struct R_PassParams_UI
{
  R_BatchGroup2DList rects;
}

public struct R_PassParams_Blur
{
  Rng2F32 rect;
  Rng2F32 clip;
  float blur_size;
  float[Corner_COUNT] corner_radii;
}

public struct R_PassParams_Geo3D
{
  Rng2F32 viewport;
  Rng2F32 clip;
  Mat4x4F32 view;
  Mat4x4F32 projection;
  R_BatchGroup3DMap mesh_batches;
}

public struct R_Pass
{
  R_PassKind kind;
  union
  {
    void* params;
    R_PassParams_UI* params_ui;
    R_PassParams_Blur* params_blur;
    R_PassParams_Geo3D* params_geo3d;
  }
}

public struct R_PassNode
{
  R_PassNode* next;
  R_Pass v;
}

public struct R_PassList
{
  R_PassNode* first;
  R_PassNode* last;
  uint64 count;
}


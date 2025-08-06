
module render.core;

////////////////////////////////
//~ rjf: Generated Code

// #include "generated/render.meta.h"

////////////////////////////////
//~ rjf: Enums

public enum R_GeoVertexFlags : uint
{
  R_GeoVertexFlag_TexCoord = (1<<0),
  R_GeoVertexFlag_Normals  = (1<<1),
  R_GeoVertexFlag_RGB      = (1<<2),
  R_GeoVertexFlag_RGBA     = (1<<3),
}

////////////////////////////////
//~ rjf: Handle Type

public union R_Handle
{
  public ulong[1] u64;
  public uint[2] u32;
  public ushort[4] u16;
}

////////////////////////////////
//~ rjf: Instance Types

public struct R_Rect2DInst
{
  public Rng2F32 dst;
  public Rng2F32 src;
  public Vec4F32[Corner_COUNT] colors;
  public float[Corner_COUNT] corner_radii;
  public float border_thickness;
  public float edge_softness;
  public float white_texture_override;
  public float[1] _unused_;
}

public struct R_Mesh3DInst
{
  public Mat4x4F32 xform;
}

////////////////////////////////
//~ rjf: Batch Types

public struct R_Batch
{
  public ubyte* v;
  public ulong byte_count;
  public ulong byte_cap;
}

public struct R_BatchNode
{
  public R_BatchNode* next;
  public R_Batch v;
}

public struct R_BatchList
{
  public R_BatchNode* first;
  public R_BatchNode* last;
  public ulong batch_count;
  public ulong byte_count;
  public ulong bytes_per_inst;
}

public struct R_BatchGroup2DParams
{
  public R_Handle tex;
  public R_Tex2DSampleKind tex_sample_kind;
  public Mat3x3F32 xform;
  public Rng2F32 clip;
  public float transparency;
}

public struct R_BatchGroup2DNode
{
  public R_BatchGroup2DNode* next;
  public R_BatchList batches;
  public R_BatchGroup2DParams params;
}

public struct R_BatchGroup2DList
{
  public R_BatchGroup2DNode* first;
  public R_BatchGroup2DNode* last;
  public ulong count;
}

public struct R_BatchGroup3DParams
{
  public R_Handle mesh_vertices;
  public R_Handle mesh_indices;
  public R_GeoTopologyKind mesh_geo_topology;
  public R_GeoVertexFlags mesh_geo_vertex_flags;
  public R_Handle albedo_tex;
  public R_Tex2DSampleKind albedo_tex_sample_kind;
  public Mat4x4F32 xform;
}

public struct R_BatchGroup3DMapNode
{
  public R_BatchGroup3DMapNode* next;
  public ulong hash;
  public R_BatchList batches;
  public R_BatchGroup3DParams params;
}

public struct R_BatchGroup3DMap
{
  public R_BatchGroup3DMapNode** slots;
  public ulong slots_count;
}

////////////////////////////////
//~ rjf: Pass Types

public struct R_PassParams_UI
{
  public R_BatchGroup2DList rects;
}

public struct R_PassParams_Blur
{
  public Rng2F32 rect;
  public Rng2F32 clip;
  public float blur_size;
  public float[Corner_COUNT] corner_radii;
}

public struct R_PassParams_Geo3D
{
  public Rng2F32 viewport;
  public Rng2F32 clip;
  public Mat4x4F32 view;
  public Mat4x4F32 projection;
  public R_BatchGroup3DMap mesh_batches;
}

public struct R_Pass
{
  public R_PassKind kind;
  union
  {
    public void* params;
    public R_PassParams_UI* params_ui;
    public R_PassParams_Blur* params_blur;
    public R_PassParams_Geo3D* params_geo3d;
  }
}

public struct R_PassNode
{
  public R_PassNode* next;
  public R_Pass v;
}

public struct R_PassList
{
  public R_PassNode* first;
  public R_PassNode* last;
  public ulong count;
}

////////////////////////////////
//~ rjf: Generated Code

// #include "generated/render.meta.c"

////////////////////////////////
//~ rjf: Helpers

public Mat4x4F32 r_sample_channel_map_from_tex2dformat(R_Tex2DFormat fmt)
{
  Mat4x4F32 result =
  {
    {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
    }
  };
  switch (fmt)
  {
    case R_Tex2DFormat_R8:
      MemoryZeroArray(result.v[0]);
      result.v[0][0] = result.v[0][1] = result.v[0][2] = result.v[0][3] = 1.f;
      break;
    
    default:
      break;
  }
  return result;
}

////////////////////////////////
//~ rjf: Basic Type Functions

public R_Handle r_handle_zero()
{
  R_Handle handle;
  return handle;
}

public bool r_handle_match(R_Handle a, R_Handle b)
{
  return MemoryMatchStruct(&a, &b);
}

////////////////////////////////
//~ rjf: Batch Type Functions

public R_BatchList r_batch_list_make(ulong instance_size)
{
  R_BatchList list;
  list.bytes_per_inst = instance_size;
  return list;
}

public void* r_batch_list_push_inst(Arena* arena, R_BatchList* list, ulong batch_inst_cap)
{
  void* inst;
  {
    R_BatchNode* n = list.last;
    if(n == null || n.v.byte_count+list.bytes_per_inst > n.v.byte_cap)
    {
      n = push_array(arena, R_BatchNode, 1);
      n.v.byte_cap = batch_inst_cap*list.bytes_per_inst;
      n.v.v = push_array_no_zero(arena, U8, n.v.byte_cap); 
      SLLQueuePush(list.first, list.last, n);
      list.batch_count += 1;
    }
    inst = n.v.v + n.v.byte_count;
    n.v.byte_count += list.bytes_per_inst;
    list.byte_count += list.bytes_per_inst;
  }
  return inst;
}

////////////////////////////////
//~ rjf: Pass Type Functions

public R_Pass* r_pass_from_kind(Arena* arena, R_PassList* list, R_PassKind kind)
{
  R_PassNode* n = list.last;
  if(!r_pass_kind_batch_table[kind])
  {
    n = null;
  }
  if(n == null || n.v.kind != kind)
  {
    n = push_array(arena, R_PassNode, 1);
    SLLQueuePush(list.first, list.last, n);
    list.count += 1;
    n.v.kind = kind;
    n.v.params = push_array(arena, U8, r_pass_kind_params_size_table[kind]);
  }
  return &n.v;
}

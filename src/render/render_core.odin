
////////////////////////////////
//~ rjf: Enums

R_GeoVertexFlags :: enum u32 {
  R_GeoVertexFlag_TexCoord = (1<<0),
  R_GeoVertexFlag_Normals  = (1<<1),
  R_GeoVertexFlag_RGB      = (1<<2),
  R_GeoVertexFlag_RGBA     = (1<<3),
}

////////////////////////////////
//~ rjf: Handle Type

R_Handle :: struct #raw_union {
  u64: [1]u64,
  u32: [2]u32,
  u16: [4]u16,
}

////////////////////////////////
//~ rjf: Instance Types

R_Rect2DInst :: struct {
  dst: Rng2F32,
  src: Rng2F32,
  colors: [Corner_COUNT]Vec4F32,
  corner_radii: [Corner_COUNT]f32,
  border_thickness: f32,
  edge_softness: f32,
  white_texture_override: f32,
  _unused_: [1]f32,
}

R_Mesh3DInst :: struct {
  xform: Mat4x4F32,
}

////////////////////////////////
//~ rjf: Batch Types

R_Batch :: struct {
  v: [^]u8,
  byte_count: u64,
  byte_cap: u64,
}

R_BatchNode :: struct {
  next: ^R_BatchNode,
  v: R_Batch,
}

R_BatchList :: struct {
  first: ^R_BatchNode,
  last: ^R_BatchNode,
  batch_count: u64,
  byte_count: u64,
  bytes_per_inst: u64,
}

R_BatchGroup2DParams :: struct {
  tex: R_Handle,
  tex_sample_kind: R_Tex2DSampleKind,
  xform: Mat3x3F32,
  clip: Rng2F32,
  transparency: f32,
}

R_BatchGroup2DNode :: struct {
  next: ^R_BatchGroup2DNode,
  batches: R_BatchList,
  params: R_BatchGroup2DParams,
}

R_BatchGroup2DList :: struct {
  first: ^R_BatchGroup2DNode,
  last: ^R_BatchGroup2DNode,
  count: u64,
}

R_BatchGroup3DParams :: struct {
  mesh_vertices: R_Handle,
  mesh_indices: R_Handle,
  mesh_geo_topology: R_GeoTopologyKind,
  mesh_geo_vertex_flags: R_GeoVertexFlags,
  albedo_tex: R_Handle,
  albedo_tex_sample_kind: R_Tex2DSampleKind,
  xform: Mat4x4F32,
}

R_BatchGroup3DMapNode :: struct {
  next: ^R_BatchGroup3DMapNode,
  hash: u64,
  batches: R_BatchList,
  params: R_BatchGroup3DParams,
}

R_BatchGroup3DMap :: struct {
  slots: [^]^R_BatchGroup3DMapNode,
  slots_count: u64,
}

////////////////////////////////
//~ rjf: Pass Types

R_PassParams_UI :: struct {
  rects: R_BatchGroup2DList,
}

R_PassParams_Blur :: struct {
  rect: Rng2F32,
  clip: Rng2F32,
  blur_size: f32,
  corner_radii: [Corner_COUNT]f32,
}

R_PassParams_Geo3D :: struct {
  viewport: Rng2F32,
  clip: Rng2F32,
  view: Mat4x4F32,
  projection: Mat4x4F32,
  mesh_batches: R_BatchGroup3DMap,
}

R_Pass :: union {
  ^R_PassParams_UI,
  ^R_PassParams_Blur,
  ^R_PassParams_Geo3D,
}

R_PassNode :: struct {
  next: ^R_PassNode,
  v: R_Pass,
}

R_PassList :: struct {
  first: ^R_PassNode,
  last: ^R_PassNode,
  count: u64,
}

////////////////////////////////
//~ rjf: Helpers

r_sample_channel_map_from_tex2dformat :: proc (fmt: R_Tex2DFormat) . Mat4x4F32 {
  result := Mat4x4F32 {
    {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
    }
  }
  switch fmt {
    case R_Tex2DFormat_R8:
      MemoryZeroArray(result.v[0])
      result.v[0][0] = result.v[0][1] = result.v[0][2] = result.v[0][3] = 1.f
    case:
  }
  return result
}

////////////////////////////////
//~ rjf: Basic Type Functions

r_handle_zero :: proc () . R_Handle {
  handle := R_Handle {}
  return handle
}

r_handle_match :: proc (a: R_Handle, b: R_Handle) . bool {
  return MemoryMatchStruct(&a, &b)
}

////////////////////////////////
//~ rjf: Batch Type Functions

r_batch_list_make :: proc (instance_size: u64) . R_BatchList {
  list := R_BatchList {}
  list.bytes_per_inst = instance_size
  return list
}

r_batch_list_push_inst :: proc (arena: ^Arena, list: ^R_BatchList, batch_inst_cap: u64) . rawptr {
  inst : rawptr = nil
  {
    n := list.last
    if n == nil || n.v.byte_count+list.bytes_per_inst > n.v.byte_cap {
      n = push_array(arena, R_BatchNode, 1)
      n.v.byte_cap = batch_inst_cap*list.bytes_per_inst
      n.v.v = push_array_no_zero(arena, U8, n.v.byte_cap);
      SLLQueuePush(list.first, list.last, n)
      list.batch_count += 1
    }
    inst = n.v.v + n.v.byte_count
    n.v.byte_count += list.bytes_per_inst
    list.byte_count += list.bytes_per_inst
  }
  return inst
}

////////////////////////////////
//~ rjf: Pass Type Functions

r_pass_from_kind :: proc (arena: ^Arena, list: ^R_PassList, kind: R_PassKind) . ^R_Pass {
  n := list.last
  if !r_pass_kind_batch_table[kind] {
    n = nil
  }
  if n == nil || n.v.kind != kind {
    n = push_array(arena, R_PassNode, 1)
    SLLQueuePush(list.first, list.last, n)
    list.count += 1
    n.v.kind = kind
    n.v.params = push_array(arena, U8, r_pass_kind_params_size_table[kind])
  }
  return &n.v
}

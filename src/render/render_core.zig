
////////////////////////////////
//~ rjf: Enums

const R_GeoVertexFlags = enum (u32) {
  R_GeoVertexFlag_TexCoord = (1<<0),
  R_GeoVertexFlag_Normals  = (1<<1),
  R_GeoVertexFlag_RGB      = (1<<2),
  R_GeoVertexFlag_RGBA     = (1<<3),
};

////////////////////////////////
//~ rjf: Handle Type

const R_Handle = extern union {
  u64: [1]u64,
  u32: [2]u32,
  u16: [4]u16,
};

////////////////////////////////
//~ rjf: Instance Types

const R_Rect2DInst = struct {
  dst: Rng2F32,
  src: Rng2F32,
  colors: [Corner_COUNT]Vec4F32,
  corner_radii: [Corner_COUNT]F32,
  border_thickness: F32,
  edge_softness: F32,
  white_texture_override: F32,
  _unused_: [1]F32,
};

const R_Mesh3DInst = struct {
  xform: Mat4x4F32,
};

////////////////////////////////
//~ rjf: Batch Types

const R_Batch = struct {
  v: [*]u8,
  byte_count: u64,
  byte_cap: u64,
};

const R_BatchNode = struct {
  next: *R_BatchNode,
  v: R_Batch,
};

const R_BatchList = struct {
  first: *R_BatchNode,
  last: *R_BatchNode,
  batch_count: u64,
  byte_count: u64,
  bytes_per_inst: u64,
};

const R_BatchGroup2DParams = struct {
  tex: R_Handle,
  tex_sample_kind: R_Tex2DSampleKind,
  xform: Mat3x3F32,
  clip: Rng2F32,
  transparency: F32,
};

const R_BatchGroup2DNode = struct {
  next: *R_BatchGroup2DNode,
  batches: R_BatchList,
  params: R_BatchGroup2DParams,
};

const R_BatchGroup2DList = struct {
  first: *R_BatchGroup2DNode,
  last: *R_BatchGroup2DNode,
  count: u64,
};

const R_BatchGroup3DParams = struct {
  mesh_vertices: R_Handle,
  mesh_indices: R_Handle,
  mesh_geo_topology: R_GeoTopologyKind,
  mesh_geo_vertex_flags: R_GeoVertexFlags,
  albedo_tex: R_Handle,
  albedo_tex_sample_kind: R_Tex2DSampleKind,
  xform: Mat4x4F32,
};

const R_BatchGroup3DMapNode = struct {
  next: *R_BatchGroup3DMapNode,
  hash: u64,
  batches: R_BatchList,
  params: R_BatchGroup3DParams,
};

const R_BatchGroup3DMap = struct {
  slots: [*]*R_BatchGroup3DMapNode,
  slots_count: u64,
};

////////////////////////////////
//~ rjf: Pass Types

const R_PassParams_UI = struct {
  rects: R_BatchGroup2DList,
};

const R_PassParams_Blur = struct {
  rect: Rng2F32,
  clip: Rng2F32,
  blur_size: F32,
  corner_radii: [Corner_COUNT]F32,
};

const R_PassParams_Geo3D = struct {
  viewport: Rng2F32,
  clip: Rng2F32,
  view: Mat4x4F32,
  projection: Mat4x4F32,
  mesh_batches: R_BatchGroup3DMap,
};

const R_Pass = union (R_PassKind) {
  params_ui: *R_PassParams_UI,
  params_blur: *R_PassParams_Blur,
  params_geo3d: *R_PassParams_Geo3D,
};

const R_PassNode = struct {
  next: *R_PassNode,
  v: R_Pass,
};

const R_PassList = struct {
  first: *R_PassNode,
  last: *R_PassNode,
  count: u64,
};

////////////////////////////////
//~ rjf: Helpers

fn r_sample_channel_map_from_tex2dformat(fmt: R_Tex2DFormat) Mat4x4F32 {
  var result = Mat4x4F32 {
    {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
    }
  };
  switch (fmt) {
      R_Tex2DFormat_R8 => {
          MemoryZeroArray(result.v[0]);
          result.v[0][0] = result.v[0][1] = result.v[0][2] = result.v[0][3] = 1.f;
      },
      else => {},
  };
  return result;
}

////////////////////////////////
//~ rjf: Basic Type Functions

fn r_handle_zero() R_Handle {
  const handle = R_Handle {};
  return handle;
}

fn r_handle_match(a: R_Handle, b: R_Handle) B32 {
  return MemoryMatchStruct(&a, &b);
}

////////////////////////////////
//~ rjf: Batch Type Functions

fn r_batch_list_make(instance_size: u64) R_BatchList {
  var list = R_BatchList {};
  list.bytes_per_inst = instance_size;
  return list;
}

fn r_batch_list_push_inst(arena: *Arena, list: *R_BatchList, batch_inst_cap: u64) *void {
  var inst: ?*void = null;
  {
    var n = list.last;
    if (n == null || n.v.byte_count+list.bytes_per_inst > n.v.byte_cap)
    {
      n = push_array(arena, R_BatchNode, 1);
      n.v.byte_cap = batch_inst_cap*list.bytes_per_inst;
      n.v.v = push_array_no_zero(arena, u8, n.v.byte_cap); 
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

fn r_pass_from_kind(arena: *Arena, list: *R_PassList, kind: R_PassKind) *R_Pass {
  var n = list.last;
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
    n.v.params = push_array(arena, u8, r_pass_kind_params_size_table[kind]);
  }
  return &n.v;
}

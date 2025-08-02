

const R_Tex2DFormat = enum {
  R_Tex2DFormat_R8,
  R_Tex2DFormat_RG8,
  R_Tex2DFormat_RGBA8,
  R_Tex2DFormat_BGRA8,
  R_Tex2DFormat_R16,
  R_Tex2DFormat_RGBA16,
  R_Tex2DFormat_R32,
  R_Tex2DFormat_RG32,
  R_Tex2DFormat_RGBA32,
  R_Tex2DFormat_COUNT,
};

const R_ResourceKind = enum {
  R_ResourceKind_Static,
  R_ResourceKind_Dynamic,
  R_ResourceKind_Stream,
  R_ResourceKind_COUNT,
};

const R_Tex2DSampleKind = enum {
  R_Tex2DSampleKind_Nearest,
  R_Tex2DSampleKind_Linear,
  R_Tex2DSampleKind_COUNT,
};

const R_GeoTopologyKind = enum {
  R_GeoTopologyKind_Lines,
  R_GeoTopologyKind_LineStrip,
  R_GeoTopologyKind_Triangles,
  R_GeoTopologyKind_TriangleStrip,
  R_GeoTopologyKind_COUNT,
};

const R_PassKind = enum {
  R_PassKind_UI,
  R_PassKind_Blur,
  R_PassKind_Geo3D,
  R_PassKind_COUNT,
};

export var r_tex2d_format_display_string_table: [9]String8 = {
  "R8",
  "RG8",
  "RGBA8",
  "BGRA8",
  "R16",
  "RGBA16",
  "R32",
  "RG32",
  "RGBA32",
};

export var r_tex2d_format_bytes_per_pixel_table: [9]u8 = {
  1,
  2,
  4,
  4,
  2,
  8,
  4,
  8,
  16,
};

export var r_resource_kind_display_string_table: [3]String8 = {
  "Static",
  "Dynamic",
  "Stream ",
};

export var r_tex2d_sample_kind_display_string_table: [2]String8 = {
  "Nearest",
  "Linear",
};

export var r_pass_kind_display_string_table: [3]String8 = {
  "UI",
  "Blur",
  "Geo3D",
};

export var r_pass_kind_batch_table: [3]u8 = {
  1,
  0,
  1,
};

export var r_pass_kind_params_size_table: [3]U64 = {
  @sizeOf(R_PassParams_UI),
  @sizeOf(R_PassParams_Blur),
  @sizeOf(R_PassParams_Geo3D),
};

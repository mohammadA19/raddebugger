
public enum R_Tex2DFormat
{
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
}

public enum R_ResourceKind
{
R_ResourceKind_Static,
R_ResourceKind_Dynamic,
R_ResourceKind_Stream,
R_ResourceKind_COUNT,
}

public enum R_Tex2DSampleKind
{
R_Tex2DSampleKind_Nearest,
R_Tex2DSampleKind_Linear,
R_Tex2DSampleKind_COUNT,
}

public enum R_GeoTopologyKind
{
R_GeoTopologyKind_Lines,
R_GeoTopologyKind_LineStrip,
R_GeoTopologyKind_Triangles,
R_GeoTopologyKind_TriangleStrip,
R_GeoTopologyKind_COUNT,
}

public enum R_PassKind
{
R_PassKind_UI,
R_PassKind_Blur,
R_PassKind_Geo3D,
R_PassKind_COUNT,
}

[CLink]
public static String8[9] r_tex2d_format_display_string_table =
.(
"R8",
"RG8",
"RGBA8",
"BGRA8",
"R16",
"RGBA16",
"R32",
"RG32",
"RGBA32",
);

[CLink]
public static uint8[9] r_tex2d_format_bytes_per_pixel_table =
.(
1,
2,
4,
4,
2,
8,
4,
8,
16,
);

[CLink]
public static String8[3] r_resource_kind_display_string_table =
.(
"Static",
"Dynamic",
"Stream ",
);

[CLink]
public static String8[2] r_tex2d_sample_kind_display_string_table =
.(
"Nearest",
"Linear",
);

[CLink]
public static String8[3] r_pass_kind_display_string_table =
.(
"UI",
"Blur",
"Geo3D",
);

[CLink]
public static uint8[3] r_pass_kind_batch_table =
.(
1,
0,
1,
);

[CLink]
public static uint64[3] r_pass_kind_params_size_table[3] =
.(
sizeof(R_PassParams_UI),
sizeof(R_PassParams_Blur),
sizeof(R_PassParams_Geo3D),
);

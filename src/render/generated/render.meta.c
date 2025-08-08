// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- GENERATED CODE

C_LINKAGE_BEGIN
StringView r_tex2d_format_display_string_table[9] =
{
("R8"),
("RG8"),
("RGBA8"),
("BGRA8"),
("R16"),
("RGBA16"),
("R32"),
("RG32"),
("RGBA32"),
};

U8 r_tex2d_format_bytes_per_pixel_table[9] =
{
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

StringView r_resource_kind_display_string_table[3] =
{
("Static"),
("Dynamic"),
("Stream "),
};

StringView r_tex2d_sample_kind_display_string_table[2] =
{
("Nearest"),
("Linear"),
};

StringView r_pass_kind_display_string_table[3] =
{
("UI"),
("Blur"),
("Geo3D"),
};

U8 r_pass_kind_batch_table[3] =
{
1,
0,
1,
};

U64 r_pass_kind_params_size_table[3] =
{
sizeof(R_PassParams_UI),
sizeof(R_PassParams_Blur),
sizeof(R_PassParams_Geo3D),
};

C_LINKAGE_END


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

internal uint pdb_hash_v1(String8 data);
internal uint pdb_hash_udt(CV_UDTInfo udt_info, String8 data);

internal ulong pdb_read_bit_vector_string(String8 data, ulong offset, U32Array *bits_out);
internal ulong pdb_read_bit_vector_msf(Arena *arena, MSF_Context *msf, MSF_StreamNumber sn, U32Array *bits_out);
internal B32 pdb_write_bit_vector(MSF_Context *msf, MSF_StreamNumber sn, B32 *flag_array, ulong flag_count);
internal ulong pdb_get_bit_vector_size(uint bucket_count);



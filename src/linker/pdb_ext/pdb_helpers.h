// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

internal uint32 pdb_hash_v1(String8 data);
internal uint32 pdb_hash_udt(CV_UDTInfo udt_info, String8 data);

internal uint64 pdb_read_bit_vector_string(String8 data, uint64 offset, uint32Array *bits_out);
internal uint64 pdb_read_bit_vector_msf(Arena *arena, MSF_Context *msf, MSF_StreamNumber sn, uint32Array *bits_out);
internal B32 pdb_write_bit_vector(MSF_Context *msf, MSF_StreamNumber sn, B32 *flag_array, uint64 flag_count);
internal uint64 pdb_get_bit_vector_size(uint32 bucket_count);



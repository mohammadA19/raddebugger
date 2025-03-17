// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

uint32 pdb_hash_v1(StringView data);
uint32 pdb_hash_udt(CV_UDTInfo udt_info, StringView data);

uint64 pdb_read_bit_vector_string(StringView data, uint64 offset, U32Array* bits_out);
uint64 pdb_read_bit_vector_msf(Arena* arena, MSF_Context* msf, MSF_StreamNumber sn, U32Array* bits_out);
B32 pdb_write_bit_vector(MSF_Context* msf, MSF_StreamNumber sn, B32* flag_array, uint64 flag_count);
uint64 pdb_get_bit_vector_size(uint32 bucket_count);



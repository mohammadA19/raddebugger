// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

ulong
pdb_read_bit_vector_string(String8 data, ulong offset, U32Array *bits_out)
{
  ulong cursor = offset;
  
  uint word_count = 0;
  cursor += str8_deserial_read_struct(data, cursor, &word_count);
  
  ulong word_data_read_size = word_count * sizeof(uint);
  String8 word_data = str8(0,0);
  cursor += str8_deserial_read_block(data, cursor, word_data_read_size, &word_data);
  
  if (word_data.size == word_data_read_size) {
    bits_out->count = word_count;
    bits_out->v = (uint*)word_data.str;
  } else {
    bits_out->count = 0;
    bits_out->v = 0;
  }
  
  ulong read_size = cursor - offset;
  return read_size;
}

ulong
pdb_read_bit_vector_msf(Arena *arena, MSF_Context *msf, MSF_StreamNumber sn, U32Array *bits_out)
{
  // peek word count
  MSF_UInt pos = msf_stream_get_pos(msf, sn);
  uint word_count = msf_stream_read_u32(msf, sn);
  msf_stream_seek(msf, sn, pos);
  
  // read out header + packed words
  ulong buffer_size = sizeof(word_count) + word_count * sizeof(uint);
  byte *buffer = push_array(arena, byte, buffer_size);
  MSF_UInt read_size = msf_stream_read(msf, sn, buffer, buffer_size);
  Assert(read_size == buffer_size);
  
  // parse words
  ulong parse_size = pdb_read_bit_vector_string(str8(buffer, buffer_size), 0, bits_out);
  return parse_size;
}

B32
pdb_write_bit_vector(MSF_Context *msf, MSF_StreamNumber sn, B32 *flag_array, ulong flag_count)
{
  B32 is_write_ok = 0;

  uint word_size = sizeof(uint);
  uint bits_per_word = MSF_BITS_PER_CHAR * word_size;
  uint word_count = (flag_count + MSF_BITS_PER_CHAR) / MSF_BITS_PER_CHAR;
  
  is_write_ok = msf_stream_write_struct(msf, sn, &word_count);
  if (is_write_ok) {
    for (ulong iword = 0, iflag = 0; iword < word_count; ++iword) {
      uint word = 0;
      
      for (ulong iflag_opl = Min(flag_count, iflag + MSF_BITS_PER_CHAR); iflag < iflag_opl; ++iflag) {
        if (flag_array[iflag]) {
          word |= 1 << (iflag % bits_per_word);
        }
      }
      
      is_write_ok = msf_stream_write_struct(msf, sn, &word);
      if (!is_write_ok) {
        break;
      }
    }
  }

  return is_write_ok;
}

ulong
pdb_get_bit_vector_size(uint bucket_count)
{
  uint word_size = sizeof(uint);
  uint word_count = (bucket_count + MSF_BITS_PER_CHAR) / MSF_BITS_PER_CHAR;
  
  ulong result = 0;
  result += sizeof(word_count);
  result += word_size * word_count;
  
  return result;
}


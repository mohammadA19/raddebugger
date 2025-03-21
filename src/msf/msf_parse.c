// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: MSF Parser Functions

MSF_RawStreamTable *
msf_raw_stream_table_from_data(Arena *arena, String8 msf_data)
{
  Temp scratch = scratch_begin(&arena, 1);

  MSF_RawStreamTable *result = 0;
  
  //- determine msf type
  uint index_size = 0;
  if (msf_data.size >= sizeof(msf_msf20_magic) &&
      str8_match(msf_data, str8_lit(msf_msf20_magic), StringMatchFlag_RightSideSloppy)) {
    index_size = 2;
  } else if (msf_data.size >= sizeof(msf_msf70_magic) && 
             str8_match(msf_data, str8_lit(msf_msf70_magic), StringMatchFlag_RightSideSloppy)) {
    index_size = 4;
  }
  
  if (index_size == 2 || index_size == 4) {
    //- extract info from header
    uint page_size_raw             = 0;
    uint whole_file_page_count_raw = 0;
    uint directory_size_raw        = 0;
    uint directory_super_map_raw   = 0;

    if (index_size == 2) {
      MSF_Header20 *header      = (MSF_Header20 *) msf_data.str;
      page_size_raw             = header->page_size;
      whole_file_page_count_raw = header->page_count;
      directory_size_raw        = header->stream_table_size;
    } else if (index_size == 4) {
      MSF_Header70 *header      = (MSF_Header70 *) msf_data.str;
      page_size_raw             = header->page_size;
      whole_file_page_count_raw = header->page_count;
      directory_size_raw        = header->stream_table_size;
      directory_super_map_raw   = header->root_pn;
    }
    
    //- setup important sizes & counts
    
    //  (pages)
    uint page_size = ClampTop(page_size_raw, msf_data.size);
    
    //  (whole file page count)
    uint whole_file_page_count_max = CeilIntegerDiv(msf_data.size, page_size);
    uint whole_file_page_count     = ClampTop(whole_file_page_count_raw, whole_file_page_count_max);
    
    //  (directory)
    uint directory_size          = ClampTop(directory_size_raw, msf_data.size);
    uint page_count_in_directory = CeilIntegerDiv(directory_size, page_size);
    
    //  (map)
    uint directory_map_size           = page_count_in_directory * index_size;
    uint page_count_in_directory_map = CeilIntegerDiv(directory_map_size, page_size);
    
    // Layout of the "directory":
    //
    // super map: [s1, s2, s3, ...]
    //       map: s1 -> [i1, i2, i3, ...]; s2 -> [...]; s3 -> [...]; ...
    // directory: i1 -> [data]; i2 -> [data]; i3 -> [data]; ... i1 -> [data]; ...
    // 
    // The "data" in the directory describes streams:
    // PDB20:
    // struct Pdb20StreamSize {
    //  uint size;
    //  uint unknown; // looks like kind codes or revision counters or something
    // }
    // struct {
    //  uint stream_count;
    //  Pdb20StreamSize stream_sizes[stream_count];
    //  ushort stream_indices[stream_count][...];
    // }
    //
    // PDB70:
    // struct {
    //  uint stream_count;
    //  uint stream_sizes[stream_count];
    //  uint stream_indices[stream_count][...];
    // }
    
    //- parse stream directory
    byte *directory_buf = push_array_no_zero(arena, byte, directory_size);
    B32 got_directory = 1;
    
    {
      uint  directory_super_map_dummy    = 0;
      uint *directory_super_map          = 0;
      uint  directory_map_page_skip_size = 0;
      if (index_size == 2) {
        directory_super_map           = &directory_super_map_dummy;
        directory_map_page_skip_size = OffsetOf(MSF_Header20, stream_table_size);
      } else {
        ulong super_map_off   = OffsetOf(MSF_Header70, root_pn);
        directory_super_map = (uint *) (msf_data.str + super_map_off);
      }
      
      uint max_index_count_in_map_page = (page_size - directory_map_page_skip_size) / index_size;
      
      // for each index in super map ...
      byte  *out_ptr       = directory_buf;
      uint *super_map_ptr = directory_super_map;
      for (uint i = 0; i < page_count_in_directory_map; ++i, ++super_map_ptr) {
        uint directory_map_page_index = *super_map_ptr;
        if (directory_map_page_index >= whole_file_page_count) {
          got_directory = 0;
          goto parse_directory_done;
        }
        
        ulong directory_map_page_off  = ((ulong) directory_map_page_index) * page_size;
        byte *directory_map_page_base = msf_data.str + directory_map_page_off;
        
        // clamp index count by end of directory
        uint index_count;
        {
          uint directory_pos            = (uint)(out_ptr - directory_buf);
          uint remaining_size           = directory_size - directory_pos;
          uint remaining_map_page_count = CeilIntegerDiv(remaining_size, page_size);
          index_count = ClampTop(max_index_count_in_map_page, remaining_map_page_count);
        }
        
        // for each index in map ...
        byte *map_ptr = directory_map_page_base + directory_map_page_skip_size;
        for (uint j = 0; j < index_count; ++j, map_ptr += index_size) {
          
          // read index
          uint directory_page_index = 0;
          if (index_size == 4) {
            directory_page_index = *(uint *) map_ptr;
          } else {
            directory_page_index = *(ushort *) map_ptr;
          }
          if (directory_page_index >= whole_file_page_count) {
            got_directory = 0;
            goto parse_directory_done;
          }
          
          ulong directory_page_off  = ((ulong) directory_page_index) * page_size;
          byte *directory_page_base = msf_data.str + directory_page_off;
          
          // clamp copy size by end of directory
          uint copy_size;
          {
            uint directory_pos  = (uint) (out_ptr - directory_buf);
            uint remaining_size = directory_size - directory_pos;
            copy_size          = ClampTop(page_size, remaining_size);
          }
          
          // copy page data
          MemoryCopy(out_ptr, directory_page_base, copy_size);
          out_ptr += copy_size;
        }
        
      }
      
      parse_directory_done:;
    }
    
    //- parse streams from directory
    uint            stream_count = 0;
    B32            got_streams  = 0;
    MSF_RawStream *streams      = 0;
    
    if (got_directory) {
      got_streams = 1;
      
      // read stream count
      uint stream_count_raw = *(uint *) directory_buf;
      
      // setup counts, sizes, and offsets
      uint size_of_stream_entry   = index_size == 2 ? 8 : 4;
      uint stream_count_max       = (directory_size - 4) / size_of_stream_entry;
      uint stream_count__inner    = ClampTop(stream_count_raw, stream_count_max);
      uint all_stream_entries_off = 4;
      uint all_indices_off        = all_stream_entries_off + (stream_count__inner * size_of_stream_entry);
      
      // set output buffer and count
      stream_count = stream_count__inner;
      streams      = push_array_no_zero(arena, MSF_RawStream, stream_count);
      
      // iterate sizes and indices in lock step
      uint            entry_cursor = all_stream_entries_off;
      uint            index_cursor = all_indices_off;
      MSF_RawStream *stream_ptr   = streams;
      for (uint i = 0; i < stream_count; ++i) {
        // read stream size
        uint stream_size_raw = *(uint *) (directory_buf + entry_cursor);
        if (stream_size_raw == MSF_DELETED_STREAM_STAMP) {
          stream_size_raw = 0;
        }
        
        // compute page count
        uint stream_page_count_raw = CeilIntegerDiv(stream_size_raw, page_size);
        uint stream_page_count_max = (directory_size - index_cursor) / index_size;
        uint stream_page_count     = ClampTop(stream_page_count_raw, stream_page_count_max);
        uint stream_size           = ClampTop(stream_size_raw, stream_page_count*page_size);
        
        // copy stream data
        stream_ptr->size       = stream_size;
        stream_ptr->page_count = stream_page_count;
        if (index_size == 4) {
          stream_ptr->u.page_indices_u32 = (uint *)(directory_buf + index_cursor);
        } else {
          stream_ptr->u.page_indices_u16 = (ushort *)(directory_buf + index_cursor);
        }
        
        // advance cursors
        entry_cursor += size_of_stream_entry;
        index_cursor += stream_page_count * index_size;
        stream_ptr   += 1;
      }
    }
    
    if (got_streams) {
      result                   = push_array(arena, MSF_RawStreamTable, 1);
      result->total_page_count = whole_file_page_count;
      result->index_size       = index_size;
      result->page_size        = page_size;
      result->stream_count     = stream_count;
      result->streams          = streams;
    }
  }
  
  scratch_end(scratch);
  return result;
}

String8
msf_data_from_stream_number(Arena *arena, String8 msf_data, MSF_RawStreamTable *st, MSF_StreamNumber sn)
{
  MSF_RawStream stream = st->streams[sn];

  byte *stream_buf     = push_array_no_zero(arena, byte, stream.size);
  byte *stream_out_ptr = stream_buf;
  for (uint i = 0; i < stream.page_count; ++i) {
    ulong page_idx;
    if (st->index_size == 4) {
      page_idx = stream.u.page_indices_u32[i];
    } else {
      page_idx = stream.u.page_indices_u16[i];
    }

    ulong stream_page_off = (ulong)page_idx * st->page_size;
    if (stream_page_off + st->page_size > msf_data.size) {
      break;
    }

    byte *stream_page_base = msf_data.str + stream_page_off;
    
    // clamp copy size by end of stream
    uint stream_pos     = (uint) (stream_out_ptr - stream_buf);
    uint remaining_size = stream.size - stream_pos;
    uint copy_size      = ClampTop(st->page_size, remaining_size);
    
    // copy page data
    MemoryCopy(stream_out_ptr, stream_page_base, copy_size);
    stream_out_ptr += copy_size;
  }

  ulong copy_size = (ulong)(stream_out_ptr - stream_buf);

  ulong unused_buf_size = stream.size - copy_size;
  arena_pop(arena, unused_buf_size);

  String8 result = str8(stream_buf, copy_size);
  return result;
}

MSF_Parsed *
msf_parsed_from_data(Arena *arena, String8 msf_data)
{
  Temp scratch = scratch_begin(&arena, 1);

  MSF_Parsed *result = 0;

  MSF_RawStreamTable *st = msf_raw_stream_table_from_data(scratch.arena, msf_data);
  if (st) {
    String8 *streams = push_array_no_zero(arena, String8, st->stream_count);
    for (MSF_StreamNumber sn = 0; sn < st->stream_count; ++sn) {
      streams[sn] = msf_data_from_stream_number(arena, msf_data, st, sn);
    }

    result               = push_array_no_zero(arena, MSF_Parsed, 1);
    result->streams      = streams;
    result->stream_count = st->stream_count;
    result->page_size   = st->page_size;
    result->page_count  = st->total_page_count;
  }
  
  scratch_end(scratch);
  return result;
}

String8
msf_data_from_stream(MSF_Parsed *msf, MSF_StreamNumber sn)
{
  String8 result = {0};
  if(sn < msf->stream_count)
  {
    result = msf->streams[sn];
  }
  return(result);
}

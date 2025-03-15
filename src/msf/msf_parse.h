// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MSF_PARSE_H
#define MSF_PARSE_H

////////////////////////////////
//~ rjf: MSF Parser Helper Types

MSF_RawStream :: struct {
  U64 size;
  U64 page_count;
  using _: struct #raw_union {
    U32 *page_indices_u32;
    U16 *page_indices_u16;
  } u;
}

MSF_RawStreamTable :: struct {
  U64            total_page_count;
  U64            index_size;
  U64            page_size;
  U64            stream_count;
  MSF_RawStream *streams;
}

MSF_Parsed :: struct {
  String8 *streams;
  U64      stream_count;
  U64      page_size;
  U64      page_count;
}

////////////////////////////////
//~ rjf: MSF Parser Functions

MSF_RawStreamTable* msf_raw_stream_table_from_data(Arena *arena, String8 msf_data);
String8             msf_data_from_stream_number(Arena *arena, String8 msf_data, MSF_RawStreamTable *st, MSF_StreamNumber sn);
MSF_Parsed*         msf_parsed_from_data(Arena *arena, String8 msf_data);
String8             msf_data_from_stream(MSF_Parsed *msf, MSF_StreamNumber sn);

#endif // MSF_PARSE_H

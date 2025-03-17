// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MSF_PARSE_H
#define MSF_PARSE_H

////////////////////////////////
//~ rjf: MSF Parser Helper Types

struct MSF_RawStream
{
  uint64 size;
  uint64 page_count;
  [Union]
  struct {
    uint32* page_indices_u32;
    uint16* page_indices_u16;
  } u;
}

struct MSF_RawStreamTable
{
  uint64            total_page_count;
  uint64            index_size;
  uint64            page_size;
  uint64            stream_count;
  MSF_RawStream* streams;
}

struct MSF_Parsed
{
  StringView* streams;
  uint64      stream_count;
  uint64      page_size;
  uint64      page_count;
}

////////////////////////////////
//~ rjf: MSF Parser Functions

MSF_RawStreamTable* msf_raw_stream_table_from_data(Arena* arena, StringView msf_data);
StringView             msf_data_from_stream_number(Arena* arena, StringView msf_data, MSF_RawStreamTable* st, MSF_StreamNumber sn);
MSF_Parsed*         msf_parsed_from_data(Arena* arena, StringView msf_data);
StringView             msf_data_from_stream(MSF_Parsed* msf, MSF_StreamNumber sn);

#endif // MSF_PARSE_H

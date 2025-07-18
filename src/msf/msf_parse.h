// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MSF_PARSE_H
#define MSF_PARSE_H

////////////////////////////////
//~ rjf: MSF Parser Helper Types

struct MSF_RawStream
{
  uint64 size;
  uint64 page_count;
  union {
    uint32 *page_indices_u32;
    uint16 *page_indices_u16;
  } u;
};

struct MSF_RawStreamTable
{
  uint64            total_page_count;
  uint64            index_size;
  uint64            page_size;
  uint64            stream_count;
  MSF_RawStream *streams;
};

struct MSF_Parsed
{
  String8 *streams;
  uint64      stream_count;
  uint64      page_size;
  uint64      page_count;
};

////////////////////////////////
//~ rjf: MSF Parser Functions


#endif // MSF_PARSE_H

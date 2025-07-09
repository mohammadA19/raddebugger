// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: MSF Parser Helper Types



struct MSF_RawStream
{
  u64 size;
  u64 page_count;
  union {
    u32 *page_indices_u32;
    u16 *page_indices_u16;
  } u;
};



struct MSF_RawStreamTable
{
  u64            total_page_count;
  u64            index_size;
  u64            page_size;
  u64            stream_count;
  MSF_RawStream *streams;
};



struct MSF_Parsed
{
  String8 *streams;
  u64      stream_count;
  u64      page_size;
  u64      page_count;
};

////////////////////////////////
//~ rjf: MSF Parser Functions



// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: MSF Parser Helper Types



struct MSF_RawStream
{
  U64 size;
  U64 page_count;
  union {
    U32 *page_indices_u32;
    U16 *page_indices_u16;
  } u;
};



struct MSF_RawStreamTable
{
  U64            total_page_count;
  U64            index_size;
  U64            page_size;
  U64            stream_count;
  MSF_RawStream *streams;
};



struct MSF_Parsed
{
  String8 *streams;
  U64      stream_count;
  U64      page_size;
  U64      page_count;
};

////////////////////////////////
//~ rjf: MSF Parser Functions



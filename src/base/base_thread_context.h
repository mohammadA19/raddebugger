// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

////////////////////////////////
// NOTE(allen): Thread Context

struct TCTX
{
  Arena *arenas[2];
  
  uint8 thread_name[32];
  uint64 thread_name_size;
  
  char *file_name;
  uint64 line_number;
};

////////////////////////////////
// NOTE(allen): Thread Context Functions




#define tctx_write_this_srcloc() tctx_write_srcloc(__FILE__, __LINE__)

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)

#endif // BASE_THREAD_CONTEXT_H

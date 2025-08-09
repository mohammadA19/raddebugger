// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
// NOTE(allen): Thread Context

struct TCTX
{
  Arena *arenas[2];
  
  U8 thread_name[32];
  U64 thread_name_size;
  
  char *file_name;
  U64 line_number;
};

////////////////////////////////
// NOTE(allen): Thread Context Functions




#define tctx_write_this_srcloc() tctx_write_srcloc(__FILE__, __LINE__)

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)


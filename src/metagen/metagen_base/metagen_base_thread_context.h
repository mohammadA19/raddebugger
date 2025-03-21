// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

////////////////////////////////
// NOTE(allen): Thread Context

struct TCTX
{
  Arena *arenas[2];
  
  byte thread_name[32];
  U64 thread_name_size;
  
  char *file_name;
  U64 line_number;
};

////////////////////////////////
// NOTE(allen): Thread Context Functions

void      tctx_init_and_equip(TCTX *tctx);
void      tctx_release();
TCTX*     tctx_get_equipped();

Arena*    tctx_get_scratch(Arena **conflicts, U64 count);

void      tctx_set_thread_name(String8 name);
String8   tctx_get_thread_name();

void      tctx_write_srcloc(char *file_name, U64 line_number);
void      tctx_read_srcloc(char **file_name, U64 *line_number);
#define tctx_write_this_srcloc() tctx_write_srcloc(__FILE__, __LINE__)

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)

#endif // BASE_THREAD_CONTEXT_H

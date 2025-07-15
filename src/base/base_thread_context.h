// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////
// NOTE(allen): Thread Context

typedef struct TCTX TCTX;
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

static void      tctx_init_and_equip(TCTX *tctx);
static void      tctx_release(void);
static TCTX*     tctx_get_equipped(void);

static Arena*    tctx_get_scratch(Arena **conflicts, U64 count);

static void      tctx_set_thread_name(String8 name);
static String8   tctx_get_thread_name(void);

static void      tctx_write_srcloc(char *file_name, U64 line_number);
static void      tctx_read_srcloc(char **file_name, U64 *line_number);
#define tctx_write_this_srcloc() tctx_write_srcloc(__FILE__, __LINE__)

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)


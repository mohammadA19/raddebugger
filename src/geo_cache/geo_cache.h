// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef GEO_CACHE_H
#define GEO_CACHE_H

////////////////////////////////
//~ rjf: Cache Types



struct GEO_Node
{
  GEO_Node *next;
  GEO_Node *prev;
  U128 hash;
  R_Handle buffer;
  B32 is_working;
  U64 scope_ref_count;
  U64 last_time_touched_us;
  U64 last_user_clock_idx_touched;
  U64 load_count;
};



struct GEO_Slot
{
  GEO_Node *first;
  GEO_Node *last;
};



struct GEO_Stripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
};

////////////////////////////////
//~ rjf: Scoped Access



struct GEO_Touch
{
  GEO_Touch *next;
  U128 hash;
};



struct GEO_Scope
{
  GEO_Scope *next;
  GEO_Touch *top_touch;
};

////////////////////////////////
//~ rjf: Thread Context



struct GEO_TCTX
{
  Arena *arena;
  GEO_Scope *free_scope;
  GEO_Touch *free_touch;
};

////////////////////////////////
//~ rjf: Shared State



struct GEO_Shared
{
  Arena *arena;
  
  // rjf: cache
  U64 slots_count;
  U64 stripes_count;
  GEO_Slot *slots;
  GEO_Stripe *stripes;
  GEO_Node **stripes_free_nodes;
  
  // rjf: user -> xfer thread
  U64 u2x_ring_size;
  U8 *u2x_ring_base;
  U64 u2x_ring_write_pos;
  U64 u2x_ring_read_pos;
  OS_Handle u2x_ring_cv;
  OS_Handle u2x_ring_mutex;
  
  // rjf: evictor thread
  OS_Handle evictor_thread;
};

////////////////////////////////
//~ rjf: Globals

@(thread_local) GEO_TCTX *geo_tctx = 0;
global GEO_Shared *geo_shared = 0;

////////////////////////////////
//~ rjf: Main Layer Initialization



////////////////////////////////
//~ rjf: Thread Context Initialization



////////////////////////////////
//~ rjf: Scoped Access





////////////////////////////////
//~ rjf: Cache Lookups




////////////////////////////////
//~ rjf: Transfer Threads



ASYNC_WORK_DEF(geo_xfer_work);

////////////////////////////////
//~ rjf: Evictor Threads



#endif //GEO_CACHE_H

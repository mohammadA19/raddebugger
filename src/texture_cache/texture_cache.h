// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Texture Topology



struct TEX_Topology
{
  Vec2S16 dim;
  R_Tex2DFormat fmt;
};

////////////////////////////////
//~ rjf: Cache Types



struct TEX_Node
{
  TEX_Node *next;
  TEX_Node *prev;
  U128 hash;
  TEX_Topology topology;
  R_Handle texture;
  B32 is_working;
  U64 scope_ref_count;
  U64 last_time_touched_us;
  U64 last_user_clock_idx_touched;
  U64 load_count;
};



struct TEX_Slot
{
  TEX_Node *first;
  TEX_Node *last;
};



struct TEX_Stripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
};

////////////////////////////////
//~ rjf: Scoped Access



struct TEX_Touch
{
  TEX_Touch *next;
  U128 hash;
  TEX_Topology topology;
};



struct TEX_Scope
{
  TEX_Scope *next;
  TEX_Touch *top_touch;
};

////////////////////////////////
//~ rjf: Thread Context



struct TEX_TCTX
{
  Arena *arena;
  TEX_Scope *free_scope;
  TEX_Touch *free_touch;
};

////////////////////////////////
//~ rjf: Shared State



struct TEX_Shared
{
  Arena *arena;
  
  // rjf: cache
  U64 slots_count;
  U64 stripes_count;
  TEX_Slot *slots;
  TEX_Stripe *stripes;
  TEX_Node **stripes_free_nodes;
  
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

@(thread_local) TEX_TCTX *tex_tctx = 0;
global TEX_Shared *tex_shared = 0;

////////////////////////////////
//~ rjf: Basic Helpers



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



ASYNC_WORK_DEF(tex_xfer_work);

////////////////////////////////
//~ rjf: Evictor Threads



#endif //TEXTURE_CACHE_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PTR_GRAPH_CACHE_H
#define PTR_GRAPH_CACHE_H

////////////////////////////////
//~ rjf: Graph Search Key

PTG_Key :: struct {
  U128 root_hash;
  U64 link_offsets[8];
  U64 link_offsets_count;
}

////////////////////////////////
//~ rjf: Cache Types

PTG_Node :: struct {
  U64 value;
}

PTG_Link :: struct {
  U32 from;
  U32 to;
}

PTG_NodeChunkNode :: struct {
  PTG_NodeChunkNode *next;
  PTG_Node *v;
  U64 count;
  U64 cap;
}

PTG_NodeChunkList :: struct {
  PTG_NodeChunkNode *first;
  PTG_NodeChunkNode *last;
  U64 chunk_count;
  U64 total_count;
}

PTG_NodeArray :: struct {
  PTG_Node *v;
  U64 count;
}

PTG_LinkChunkNode :: struct {
  PTG_LinkChunkNode *next;
  PTG_Link *v;
  U64 count;
  U64 cap;
}

PTG_LinkChunkList :: struct {
  PTG_LinkChunkNode *first;
  PTG_LinkChunkNode *last;
  U64 chunk_count;
  U64 total_count;
}

PTG_LinkArray :: struct {
  PTG_Link *v;
  U64 count;
}

PTG_Graph :: struct {
  PTG_NodeArray nodes;
  PTG_LinkArray links;
}

PTG_GraphNode :: struct {
  // rjf: links
  PTG_GraphNode *next;
  PTG_GraphNode *prev;
  
  // rjf: key
  PTG_Key key;
  
  // rjf: metadata
  U64 scope_ref_count;
  U64 last_time_touched_us;
  U64 last_user_clock_idx_touched;
  U64 load_count;
  B32 is_working;
  
  // rjf: content
  Arena *arena;
  PTG_Graph graph;
}

PTG_GraphSlot :: struct {
  PTG_GraphNode *first;
  PTG_GraphNode *last;
}

PTG_GraphStripe :: struct {
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
  PTG_GraphNode *free_node;
}

////////////////////////////////
//~ rjf: Scoped Access Types

PTG_Touch :: struct {
  PTG_Touch *next;
  PTG_GraphNode *node;
}

PTG_Scope :: struct {
  PTG_Scope *next;
  PTG_Touch *top_touch;
}

////////////////////////////////
//~ rjf: Thread Context

PTG_TCTX :: struct {
  Arena *arena;
  PTG_Scope *free_scope;
  PTG_Touch *free_touch;
}

////////////////////////////////
//~ rjf: Shared State

PTG_Shared :: struct {
  Arena *arena;
  
  // rjf: user clock
  U64 user_clock_idx;
  
  // rjf: cache
  U64 slots_count;
  U64 stripes_count;
  PTG_GraphSlot *slots;
  PTG_GraphStripe *stripes;
  
  // rjf: user -> xfer thread
  U64 u2b_ring_size;
  U8 *u2b_ring_base;
  U64 u2b_ring_write_pos;
  U64 u2b_ring_read_pos;
  OS_Handle u2b_ring_cv;
  OS_Handle u2b_ring_mutex;
  
  // rjf: builder threads
  U64 builder_thread_count;
  OS_Handle *builder_threads;
  
  // rjf: evictor thread
  OS_Handle evictor_thread;
}

////////////////////////////////
//~ rjf: Globals

thread_static PTG_TCTX *ptg_tctx = 0;
static PTG_Shared *ptg_shared = 0;

////////////////////////////////
//~ rjf: Main Layer Initialization

void ptg_init();

////////////////////////////////
//~ rjf: User Clock

void ptg_user_clock_tick();
U64 ptg_user_clock_idx();

////////////////////////////////
//~ rjf: Scoped Access

PTG_Scope *ptg_scope_open();
void ptg_scope_close(PTG_Scope *scope);
void ptg_scope_touch_node__stripe_r_guarded(PTG_Scope *scope, PTG_GraphNode *node);

////////////////////////////////
//~ rjf: Cache Lookups

PTG_Graph *ptg_graph_from_key(PTG_Scope *scope, PTG_Key *key);

////////////////////////////////
//~ rjf: Transfer Threads

B32 ptg_u2b_enqueue_req(PTG_Key *key, U64 endt_us);
void ptg_u2b_dequeue_req(PTG_Key *key_out);
void ptg_builder_thread__entry_point(void *p);

////////////////////////////////
//~ rjf: Evictor Threads

void ptg_evictor_thread__entry_point(void *p);

#endif // PTR_GRAPH_CACHE_H

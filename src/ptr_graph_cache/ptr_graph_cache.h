// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PTR_GRAPH_CACHE_H
#define PTR_GRAPH_CACHE_H

////////////////////////////////
//~ rjf: Graph Search Key

struct PTG_Key
{
  uint128 root_hash;
  uint64 link_offsets[8];
  uint64 link_offsets_count;
};

////////////////////////////////
//~ rjf: Cache Types

struct PTG_Node
{
  uint64 value;
};

struct PTG_Link
{
  uint32 from;
  uint32 to;
};

struct PTG_NodeChunkNode
{
  PTG_NodeChunkNode *next;
  PTG_Node *v;
  uint64 count;
  uint64 cap;
};

struct PTG_NodeChunkList
{
  PTG_NodeChunkNode *first;
  PTG_NodeChunkNode *last;
  uint64 chunk_count;
  uint64 total_count;
};

struct PTG_NodeArray
{
  PTG_Node *v;
  uint64 count;
};

struct PTG_LinkChunkNode
{
  PTG_LinkChunkNode *next;
  PTG_Link *v;
  uint64 count;
  uint64 cap;
};

struct PTG_LinkChunkList
{
  PTG_LinkChunkNode *first;
  PTG_LinkChunkNode *last;
  uint64 chunk_count;
  uint64 total_count;
};

struct PTG_LinkArray
{
  PTG_Link *v;
  uint64 count;
};

struct PTG_Graph
{
  PTG_NodeArray nodes;
  PTG_LinkArray links;
};

struct PTG_GraphNode
{
  // rjf: links
  PTG_GraphNode *next;
  PTG_GraphNode *prev;
  
  // rjf: key
  PTG_Key key;
  
  // rjf: metadata
  uint64 scope_ref_count;
  uint64 last_time_touched_us;
  uint64 last_user_clock_idx_touched;
  uint64 load_count;
  B32 is_working;
  
  // rjf: content
  Arena *arena;
  PTG_Graph graph;
};

struct PTG_GraphSlot
{
  PTG_GraphNode *first;
  PTG_GraphNode *last;
};

struct PTG_GraphStripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
  PTG_GraphNode *free_node;
};

////////////////////////////////
//~ rjf: Scoped Access Types

struct PTG_Touch
{
  PTG_Touch *next;
  PTG_GraphNode *node;
};

struct PTG_Scope
{
  PTG_Scope *next;
  PTG_Touch *top_touch;
};

////////////////////////////////
//~ rjf: Thread Context

struct PTG_TCTX
{
  Arena *arena;
  PTG_Scope *free_scope;
  PTG_Touch *free_touch;
};

////////////////////////////////
//~ rjf: Shared State

struct PTG_Shared
{
  Arena *arena;
  
  // rjf: user clock
  uint64 user_clock_idx;
  
  // rjf: cache
  uint64 slots_count;
  uint64 stripes_count;
  PTG_GraphSlot *slots;
  PTG_GraphStripe *stripes;
  
  // rjf: user -> xfer thread
  uint64 u2b_ring_size;
  uint8 *u2b_ring_base;
  uint64 u2b_ring_write_pos;
  uint64 u2b_ring_read_pos;
  OS_Handle u2b_ring_cv;
  OS_Handle u2b_ring_mutex;
  
  // rjf: builder threads
  uint64 builder_thread_count;
  OS_Handle *builder_threads;
  
  // rjf: evictor thread
  OS_Handle evictor_thread;
};

////////////////////////////////
//~ rjf: Globals

thread_static PTG_TCTX *ptg_tctx = 0;
global PTG_Shared *ptg_shared = 0;

////////////////////////////////
//~ rjf: Main Layer Initialization


////////////////////////////////
//~ rjf: User Clock


////////////////////////////////
//~ rjf: Scoped Access


////////////////////////////////
//~ rjf: Cache Lookups


////////////////////////////////
//~ rjf: Transfer Threads


////////////////////////////////
//~ rjf: Evictor Threads


#endif // PTR_GRAPH_CACHE_H

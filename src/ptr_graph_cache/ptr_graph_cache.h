// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Graph Search Key

struct PTG_Key
{
  U128 root_hash;
  U64 link_offsets[8];
  U64 link_offsets_count;
}

////////////////////////////////
//~ rjf: Cache Types

struct PTG_Node
{
  U64 value;
}

struct PTG_Link
{
  U32 from;
  U32 to;
}

struct PTG_NodeChunkNode
{
  PTG_NodeChunkNode *next;
  PTG_Node *v;
  U64 count;
  U64 cap;
}

struct PTG_NodeChunkList
{
  PTG_NodeChunkNode *first;
  PTG_NodeChunkNode *last;
  U64 chunk_count;
  U64 total_count;
}

struct PTG_NodeArray
{
  PTG_Node *v;
  U64 count;
}

struct PTG_LinkChunkNode
{
  PTG_LinkChunkNode *next;
  PTG_Link *v;
  U64 count;
  U64 cap;
}

struct PTG_LinkChunkList
{
  PTG_LinkChunkNode *first;
  PTG_LinkChunkNode *last;
  U64 chunk_count;
  U64 total_count;
}

struct PTG_LinkArray
{
  PTG_Link *v;
  U64 count;
}

struct PTG_Graph
{
  PTG_NodeArray nodes;
  PTG_LinkArray links;
}

struct PTG_GraphNode
{
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

struct PTG_GraphSlot
{
  PTG_GraphNode *first;
  PTG_GraphNode *last;
}

struct PTG_GraphStripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
  PTG_GraphNode *free_node;
}

////////////////////////////////
//~ rjf: Scoped Access Types

struct PTG_Touch
{
  PTG_Touch *next;
  PTG_GraphNode *node;
}

struct PTG_Scope
{
  PTG_Scope *next;
  PTG_Touch *top_touch;
}

////////////////////////////////
//~ rjf: Thread Context

struct PTG_TCTX
{
  Arena *arena;
  PTG_Scope *free_scope;
  PTG_Touch *free_touch;
}

////////////////////////////////
//~ rjf: Shared State

struct PTG_Shared
{
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
global PTG_Shared *ptg_shared = 0;

////////////////////////////////
//~ rjf: Evictor Threads



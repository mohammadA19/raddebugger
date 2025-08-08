// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DBGI_H
#define DBGI_H

////////////////////////////////
//~ rjf: Cache Key Type

struct DI_Key
{
  String8 path;
  U64 min_timestamp;
};

struct DI_KeyNode
{
  DI_KeyNode *next;
  DI_Key v;
};

struct DI_KeyList
{
  DI_KeyNode *first;
  DI_KeyNode *last;
  U64 count;
};

struct DI_KeyArray
{
  DI_Key *v;
  U64 count;
};

////////////////////////////////
//~ rjf: Event Types

enum DI_EventKind
{
  DI_EventKind_Null,
  DI_EventKind_ConversionStarted,
  DI_EventKind_ConversionEnded,
  DI_EventKind_ConversionFailureUnsupportedFormat,
  DI_EventKind_COUNT
}
DI_EventKind;

struct DI_Event
{
  DI_EventKind kind;
  String8 string;
};

struct DI_EventNode
{
  DI_EventNode *next;
  DI_Event v;
};

struct DI_EventList
{
  DI_EventNode *first;
  DI_EventNode *last;
  U64 count;
};

////////////////////////////////
//~ rjf: Debug Info Cache Types

struct DI_StringChunkNode
{
  DI_StringChunkNode *next;
  U64 size;
};

struct DI_Node
{
  // rjf: links
  DI_Node *next;
  DI_Node *prev;
  
  // rjf: metadata
  U64 ref_count;
  U64 touch_count;
  U64 is_working;
  
  // rjf: key
  DI_Key key;
  
  // rjf: file handles
  OS_Handle file;
  OS_Handle file_map;
  void *file_base;
  FileProperties file_props;
  
  // rjf: parse artifacts
  Arena *arena;
  RDI_Parsed rdi;
  B32 parse_done;
};

struct DI_Slot
{
  DI_Node *first;
  DI_Node *last;
};

struct DI_Stripe
{
  Arena *arena;
  DI_Node *free_node;
  DI_StringChunkNode *free_string_chunks[8];
  OS_Handle rw_mutex;
  OS_Handle cv;
};

////////////////////////////////
//~ rjf: Search Cache Types

struct DI_SearchItem
{
  U64 idx;
  U64 dbgi_idx;
  U64 missed_size;
  FuzzyMatchRangeList match_ranges;
};

struct DI_SearchItemChunk
{
  DI_SearchItemChunk *next;
  DI_SearchItem *v;
  U64 count;
  U64 cap;
};

struct DI_SearchItemChunkList
{
  DI_SearchItemChunk *first;
  DI_SearchItemChunk *last;
  U64 chunk_count;
  U64 total_count;
};

struct DI_SearchItemArray
{
  DI_SearchItem *v;
  U64 count;
};

struct DI_SearchParams
{
  RDI_SectionKind target;
  DI_KeyArray dbgi_keys;
};

struct DI_SearchBucket
{
  Arena *arena;
  String8 query;
  U64 params_hash;
  DI_SearchParams params;
};

struct DI_SearchNode
{
  DI_SearchNode *next;
  DI_SearchNode *prev;
  U128 key;
  U64 scope_refcount;
  U64 work_refcount;
  U64 last_update_tick_idx;
  U64 bucket_read_gen;
  U64 bucket_write_gen;
  U64 bucket_items_gen;
  DI_SearchBucket buckets[6];
  DI_SearchItemArray items;
};

struct DI_SearchSlot
{
  DI_SearchNode *first;
  DI_SearchNode *last;
};

struct DI_SearchStripe
{
  Arena *arena;
  DI_SearchNode *free_node;
  OS_Handle rw_mutex;
  OS_Handle cv;
};

////////////////////////////////
//~ rjf: Scoped Access Types

struct DI_Touch
{
  DI_Touch *next;
  DI_Node *node;
  DI_Stripe *stripe;
  DI_SearchNode *search_node;
  DI_SearchStripe *search_stripe;
};

struct DI_Scope
{
  DI_Scope *next;
  DI_Scope *prev;
  DI_Touch *first_touch;
  DI_Touch *last_touch;
};

struct DI_TCTX
{
  Arena *arena;
  DI_Scope *first_scope;
  DI_Scope *last_scope;
  DI_Scope *free_scope;
  DI_Touch *free_touch;
};

////////////////////////////////
//~ rjf: Search Thread State Types

struct DI_SearchThread
{
  OS_Handle thread;
  OS_Handle ring_mutex;
  OS_Handle ring_cv;
  U64 ring_size;
  U8 *ring_base;
  U64 ring_write_pos;
  U64 ring_read_pos;
};

////////////////////////////////
//~ rjf: Match Cache State Types

struct DI_Match
{
  U64 dbgi_idx;
  RDI_SectionKind section;
  U32 idx;
};

struct DI_MatchNode
{
  DI_MatchNode *next;
  DI_Match v;
};

struct DI_MatchNameNode
{
  // rjf: synchronously written by usage code
  DI_MatchNameNode *next;
  DI_MatchNameNode *prev;
  DI_MatchNameNode *lru_next;
  DI_MatchNameNode *lru_prev;
  U64 alloc_gen;
  U64 first_gen_touched;
  U64 last_gen_touched;
  U64 req_params_hash;
  U64 req_count;
  String8 name;
  U64 hash;
  
  // rjf: atomically written by match work
  U64 cmp_count;
  U64 cmp_params_hash;
  DI_Match primary_match;
  // DI_MatchNode *first_alt_match;
  // DI_MatchNode *last_alt_match;
};

struct DI_MatchNameSlot
{
  DI_MatchNameNode *first;
  DI_MatchNameNode *last;
};

struct DI_MatchStore
{
  Arena *arena;
  U64 gen;
  Arena *gen_arenas[2];
  
  // rjf: parameters
  Arena *params_arena;
  OS_Handle params_rw_mutex;
  U64 params_hash;
  DI_KeyArray params_keys;
  
  // rjf: match cache
  U64 match_name_slots_count;
  DI_MatchNameSlot *match_name_slots;
  DI_MatchNameNode *first_free_match_name;
  DI_Match *first_free_match;
  DI_MatchNameNode *first_lru_match_name;
  DI_MatchNameNode *last_lru_match_name;
  U64 active_match_name_nodes_count;
  OS_Handle match_rw_mutex;
  OS_Handle match_cv;
  
  // rjf: user -> match work ring buffer
  OS_Handle u2m_ring_cv;
  OS_Handle u2m_ring_mutex;
  U64 u2m_ring_size;
  U8 *u2m_ring_base;
  U64 u2m_ring_write_pos;
  U64 u2m_ring_read_pos;
  
  // rjf: match -> user work ring buffer
  OS_Handle m2u_ring_cv;
  OS_Handle m2u_ring_mutex;
  U64 m2u_ring_size;
  U8 *m2u_ring_base;
  U64 m2u_ring_write_pos;
  U64 m2u_ring_read_pos;
};

////////////////////////////////
//~ rjf: Shared State Types

struct DI_Shared
{
  Arena *arena;
  
  // rjf: debug info cache
  U64 slots_count;
  DI_Slot *slots;
  U64 stripes_count;
  DI_Stripe *stripes;
  
  // rjf: search cache
  U64 search_slots_count;
  DI_SearchSlot *search_slots;
  U64 search_stripes_count;
  DI_SearchStripe *search_stripes;
  
  // rjf: user -> parse ring
  OS_Handle u2p_ring_mutex;
  OS_Handle u2p_ring_cv;
  U64 u2p_ring_size;
  U8 *u2p_ring_base;
  U64 u2p_ring_write_pos;
  U64 u2p_ring_read_pos;
  
  // rjf: parse -> user event ring
  OS_Handle p2u_ring_mutex;
  OS_Handle p2u_ring_cv;
  U64 p2u_ring_size;
  U8 *p2u_ring_base;
  U64 p2u_ring_write_pos;
  U64 p2u_ring_read_pos;
  
  // rjf: search threads
  U64 search_threads_count;
  DI_SearchThread *search_threads;
  OS_Handle search_evictor_thread;
};

////////////////////////////////
//~ rjf: Globals

global DI_Shared *di_shared = 0;
thread_static DI_TCTX *di_tctx = 0;

////////////////////////////////
//~ rjf: Basic Helpers


////////////////////////////////
//~ rjf: Main Layer Initialization


////////////////////////////////
//~ rjf: Scope Functions


////////////////////////////////
//~ rjf: Per-Slot Functions


////////////////////////////////
//~ rjf: Per-Stripe Functions


////////////////////////////////
//~ rjf: Key Opening/Closing


////////////////////////////////
//~ rjf: Debug Info Cache Lookups


////////////////////////////////
//~ rjf: Search Cache Lookups


////////////////////////////////
//~ rjf: Asynchronous Parse Work



ASYNC_WORK_DEF(di_parse_work);

////////////////////////////////
//~ rjf: Search Threads


ASYNC_WORK_DEF(di_search_work);


////////////////////////////////
//~ rjf: Match Store

ASYNC_WORK_DEF(di_match_work);

#endif // DBGI_H

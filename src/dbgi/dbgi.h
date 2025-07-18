// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DBGI_H
#define DBGI_H

////////////////////////////////
//~ rjf: Cache Key Type

struct DI_Key
{
  String8 path;
  uint64 min_timestamp;
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
  uint64 count;
};

struct DI_KeyArray
{
  DI_Key *v;
  uint64 count;
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
  uint64 count;
};

////////////////////////////////
//~ rjf: Debug Info Cache Types

struct DI_StringChunkNode
{
  DI_StringChunkNode *next;
  uint64 size;
};

struct DI_Node
{
  // rjf: links
  DI_Node *next;
  DI_Node *prev;
  
  // rjf: metadata
  uint64 ref_count;
  uint64 touch_count;
  uint64 is_working;
  
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
  uint64 idx;
  uint64 dbgi_idx;
  uint64 missed_size;
  FuzzyMatchRangeList match_ranges;
};

struct DI_SearchItemChunk
{
  DI_SearchItemChunk *next;
  DI_SearchItem *v;
  uint64 count;
  uint64 cap;
};

struct DI_SearchItemChunkList
{
  DI_SearchItemChunk *first;
  DI_SearchItemChunk *last;
  uint64 chunk_count;
  uint64 total_count;
};

struct DI_SearchItemArray
{
  DI_SearchItem *v;
  uint64 count;
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
  uint64 params_hash;
  DI_SearchParams params;
};

struct DI_SearchNode
{
  DI_SearchNode *next;
  DI_SearchNode *prev;
  uint128 key;
  uint64 scope_refcount;
  uint64 work_refcount;
  uint64 last_update_tick_idx;
  uint64 bucket_read_gen;
  uint64 bucket_write_gen;
  uint64 bucket_items_gen;
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
  uint64 ring_size;
  uint8 *ring_base;
  uint64 ring_write_pos;
  uint64 ring_read_pos;
};

////////////////////////////////
//~ rjf: Match Cache State Types

struct DI_Match
{
  uint64 dbgi_idx;
  RDI_SectionKind section;
  uint32 idx;
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
  uint64 alloc_gen;
  uint64 first_gen_touched;
  uint64 last_gen_touched;
  uint64 req_params_hash;
  uint64 req_count;
  String8 name;
  uint64 hash;
  
  // rjf: atomically written by match work
  uint64 cmp_count;
  uint64 cmp_params_hash;
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
  uint64 gen;
  Arena *gen_arenas[2];
  
  // rjf: parameters
  Arena *params_arena;
  OS_Handle params_rw_mutex;
  uint64 params_hash;
  DI_KeyArray params_keys;
  
  // rjf: match cache
  uint64 match_name_slots_count;
  DI_MatchNameSlot *match_name_slots;
  DI_MatchNameNode *first_free_match_name;
  DI_Match *first_free_match;
  DI_MatchNameNode *first_lru_match_name;
  DI_MatchNameNode *last_lru_match_name;
  uint64 active_match_name_nodes_count;
  OS_Handle match_rw_mutex;
  OS_Handle match_cv;
  
  // rjf: user -> match work ring buffer
  OS_Handle u2m_ring_cv;
  OS_Handle u2m_ring_mutex;
  uint64 u2m_ring_size;
  uint8 *u2m_ring_base;
  uint64 u2m_ring_write_pos;
  uint64 u2m_ring_read_pos;
  
  // rjf: match -> user work ring buffer
  OS_Handle m2u_ring_cv;
  OS_Handle m2u_ring_mutex;
  uint64 m2u_ring_size;
  uint8 *m2u_ring_base;
  uint64 m2u_ring_write_pos;
  uint64 m2u_ring_read_pos;
};

////////////////////////////////
//~ rjf: Shared State Types

struct DI_Shared
{
  Arena *arena;
  
  // rjf: debug info cache
  uint64 slots_count;
  DI_Slot *slots;
  uint64 stripes_count;
  DI_Stripe *stripes;
  
  // rjf: search cache
  uint64 search_slots_count;
  DI_SearchSlot *search_slots;
  uint64 search_stripes_count;
  DI_SearchStripe *search_stripes;
  
  // rjf: user -> parse ring
  OS_Handle u2p_ring_mutex;
  OS_Handle u2p_ring_cv;
  uint64 u2p_ring_size;
  uint8 *u2p_ring_base;
  uint64 u2p_ring_write_pos;
  uint64 u2p_ring_read_pos;
  
  // rjf: parse -> user event ring
  OS_Handle p2u_ring_mutex;
  OS_Handle p2u_ring_cv;
  uint64 p2u_ring_size;
  uint8 *p2u_ring_base;
  uint64 p2u_ring_write_pos;
  uint64 p2u_ring_read_pos;
  
  // rjf: search threads
  uint64 search_threads_count;
  DI_SearchThread *search_threads;
  OS_Handle search_evictor_thread;
};

////////////////////////////////
//~ rjf: Globals

global DI_Shared *di_shared = 0;
thread_static DI_TCTX *di_tctx = 0;

////////////////////////////////
//~ rjf: Basic Helpers

internal uint64 di_hash_from_seed_string(uint64 seed, String8 string, StringMatchFlags match_flags);
internal uint64 di_hash_from_string(String8 string, StringMatchFlags match_flags);
internal uint64 di_hash_from_key(DI_Key *k);
internal DI_Key di_key_zero(void);
internal B32 di_key_match(DI_Key *a, DI_Key *b);
internal DI_Key di_key_copy(Arena *arena, DI_Key *src);
internal DI_Key di_normalized_key_from_key(Arena *arena, DI_Key *src);
internal void di_key_list_push(Arena *arena, DI_KeyList *list, DI_Key *key);
internal DI_KeyArray di_key_array_from_list(Arena *arena, DI_KeyList *list);
internal DI_KeyArray di_key_array_copy(Arena *arena, DI_KeyArray *src);
internal DI_SearchParams di_search_params_copy(Arena *arena, DI_SearchParams *src);
internal uint64 di_hash_from_search_params(DI_SearchParams *params);
internal void di_search_item_chunk_list_concat_in_place(DI_SearchItemChunkList *dst, DI_SearchItemChunkList *to_push);
internal uint64 di_search_item_num_from_array_element_idx__linear_search(DI_SearchItemArray *array, uint64 element_idx);
internal String8 di_search_item_string_from_rdi_target_element_idx(RDI_Parsed *rdi, RDI_SectionKind target, uint64 element_idx);

////////////////////////////////
//~ rjf: Main Layer Initialization

internal void di_init(void);

////////////////////////////////
//~ rjf: Scope Functions

internal DI_Scope *di_scope_open(void);
internal void di_scope_close(DI_Scope *scope);
internal void di_scope_touch_node__stripe_mutex_r_guarded(DI_Scope *scope, DI_Stripe *stripe, DI_Node *node);
internal void di_scope_touch_search_node__stripe_mutex_r_guarded(DI_Scope *scope, DI_SearchStripe *stripe, DI_SearchNode *node);

////////////////////////////////
//~ rjf: Per-Slot Functions

internal DI_Node *di_node_from_key_slot__stripe_mutex_r_guarded(DI_Slot *slot, DI_Key *key);

////////////////////////////////
//~ rjf: Per-Stripe Functions

internal uint64 di_string_bucket_idx_from_string_size(uint64 size);
internal String8 di_string_alloc__stripe_mutex_w_guarded(DI_Stripe *stripe, String8 string);
internal void di_string_release__stripe_mutex_w_guarded(DI_Stripe *stripe, String8 string);

////////////////////////////////
//~ rjf: Key Opening/Closing

internal void di_open(DI_Key *key);
internal void di_close(DI_Key *key);

////////////////////////////////
//~ rjf: Debug Info Cache Lookups

internal RDI_Parsed *di_rdi_from_key(DI_Scope *scope, DI_Key *key, B32 high_priority, uint64 endt_us);

////////////////////////////////
//~ rjf: Search Cache Lookups

internal DI_SearchItemArray di_search_items_from_key_params_query(DI_Scope *scope, uint128 key, DI_SearchParams *params, String8 query, uint64 endt_us, B32 *stale_out);

////////////////////////////////
//~ rjf: Asynchronous Parse Work

internal B32 di_u2p_enqueue_key(DI_Key *key, uint64 endt_us);
internal void di_u2p_dequeue_key(Arena *arena, DI_Key *out_key);

internal void di_p2u_push_event(DI_Event *event);
internal DI_EventList di_p2u_pop_events(Arena *arena, uint64 endt_us);

ASYNC_WORK_DEF(di_parse_work);

////////////////////////////////
//~ rjf: Search Threads

internal B32 di_u2s_enqueue_req(uint128 key, uint64 endt_us);
internal uint128 di_u2s_dequeue_req(uint64 thread_idx);

ASYNC_WORK_DEF(di_search_work);
internal int di_qsort_compare_search_items(DI_SearchItem *a, DI_SearchItem *b);
internal void di_search_thread__entry_point(void *p);

internal void di_search_evictor_thread__entry_point(void *p);

////////////////////////////////
//~ rjf: Match Store

internal DI_MatchStore *di_match_store_alloc(void);
internal void di_match_store_begin(DI_MatchStore *store, DI_KeyArray keys);
internal DI_Match di_match_from_name(DI_MatchStore *store, String8 name, uint64 endt_us);
ASYNC_WORK_DEF(di_match_work);

#endif // DBGI_H

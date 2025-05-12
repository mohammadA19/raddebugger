// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Cache Types

#define HS_KEY_HASH_HISTORY_COUNT 64
#define HS_KEY_HASH_HISTORY_STRONG_REF_COUNT 2

struct HS_KeyNode
{
  HS_KeyNode *next;
  U128 key;
  U128 hash_history[HS_KEY_HASH_HISTORY_COUNT];
  U64 hash_history_gen;
}

struct HS_KeySlot
{
  HS_KeyNode *first;
  HS_KeyNode *last;
}

struct HS_Node
{
  HS_Node *next;
  HS_Node *prev;
  U128 hash;
  Arena *arena;
  String8 data;
  U64 scope_ref_count;
  U64 key_ref_count;
  U64 downstream_ref_count;
}

struct HS_Slot
{
  HS_Node *first;
  HS_Node *last;
}

struct HS_Stripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
}

////////////////////////////////
//~ rjf: Scoped Access

struct HS_Touch
{
  HS_Touch *next;
  U128 hash;
}

struct HS_Scope
{
  HS_Scope *next;
  HS_Touch *top_touch;
}

////////////////////////////////
//~ rjf: Thread Context

struct HS_TCTX
{
  Arena *arena;
  HS_Scope *free_scope;
  HS_Touch *free_touch;
}

////////////////////////////////
//~ rjf: Shared State

struct HS_Shared
{
  Arena *arena;
  
  // rjf: main data cache
  U64 slots_count;
  U64 stripes_count;
  HS_Slot *slots;
  HS_Stripe *stripes;
  HS_Node **stripes_free_nodes;
  
  // rjf: key cache
  U64 key_slots_count;
  U64 key_stripes_count;
  HS_KeySlot *key_slots;
  HS_Stripe *key_stripes;
  
  // rjf: evictor thread
  OS_Handle evictor_thread;
}

////////////////////////////////
//~ rjf: Globals

thread_static HS_TCTX *hs_tctx = 0;
global HS_Shared *hs_shared = 0;

////////////////////////////////
//~ rjf: Evictor Thread



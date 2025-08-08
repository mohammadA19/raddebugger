// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MUTABLE_TEXT_H
#define MUTABLE_TEXT_H

////////////////////////////////
//~ rjf: Cache Types

struct MTX_Node
{
  MTX_Node *next;
  MTX_Node *prev;
  U128 key;
};

struct MTX_Slot
{
  MTX_Node *first;
  MTX_Node *last;
};

struct MTX_Stripe
{
  Arena *arena;
  MTX_Node *free_node;
  OS_Handle rw_mutex;
};

////////////////////////////////
//~ rjf: Mutation Thread Types

struct MTX_Op
{
  Rng1U64 range;
  String8 replace;
};

struct MTX_MutThread
{
  U64 ring_size;
  U8 *ring_base;
  U64 ring_read_pos;
  U64 ring_write_pos;
  OS_Handle cv;
  OS_Handle mutex;
  OS_Handle thread;
};

////////////////////////////////
//~ rjf: Shared State

struct MTX_Shared
{
  Arena *arena;
  
  // rjf: buffer cache
  U64 slots_count;
  U64 stripes_count;
  MTX_Slot *slots;
  MTX_Stripe *stripes;
  
  // rjf: mut threads
  U64 mut_threads_count;
  MTX_MutThread *mut_threads;
};

////////////////////////////////
//~ rjf: Globals

global MTX_Shared *mtx_shared = 0;

////////////////////////////////
//~ rjf: Main Layer Initialization


////////////////////////////////
//~ rjf: Buffer Operations


////////////////////////////////
//~ rjf: Mutation Threads


#endif // MUTABLE_TEXT_H

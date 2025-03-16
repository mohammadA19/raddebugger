// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MUTABLE_TEXT_H
#define MUTABLE_TEXT_H

////////////////////////////////
//~ rjf: Cache Types

struct MTX_Node
{
  MTX_Node* next;
  MTX_Node* prev;
  U128 key;
}

struct MTX_Slot
{
  MTX_Node* first;
  MTX_Node* last;
}

struct MTX_Stripe
{
  Arena* arena;
  MTX_Node* free_node;
  OS_Handle rw_mutex;
}

////////////////////////////////
//~ rjf: Mutation Thread Types

struct MTX_Op
{
  Rng1U64 range;
  String8 replace;
}

struct MTX_MutThread
{
  uint64 ring_size;
  uint8* ring_base;
  uint64 ring_read_pos;
  uint64 ring_write_pos;
  OS_Handle cv;
  OS_Handle mutex;
  OS_Handle thread;
}

////////////////////////////////
//~ rjf: Shared State

struct MTX_Shared
{
  Arena* arena;
  
  // rjf: buffer cache
  uint64 slots_count;
  uint64 stripes_count;
  MTX_Slot* slots;
  MTX_Stripe* stripes;
  
  // rjf: mut threads
  uint64 mut_threads_count;
  MTX_MutThread* mut_threads;
}

////////////////////////////////
//~ rjf: Globals

static MTX_Shared* mtx_shared = 0;

////////////////////////////////
//~ rjf: Main Layer Initialization

void mtx_init();

////////////////////////////////
//~ rjf: Buffer Operations

void mtx_push_op(U128 buffer_key, MTX_Op op);

////////////////////////////////
//~ rjf: Mutation Threads

void mtx_enqueue_op(MTX_MutThread* thread, U128 buffer_key, MTX_Op op);
void mtx_dequeue_op(Arena* arena, MTX_MutThread* thread, U128* buffer_key_out, MTX_Op* op_out);
void mtx_mut_thread__entry_point(void* p);

#endif // MUTABLE_TEXT_H

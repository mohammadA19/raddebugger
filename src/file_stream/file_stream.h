// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
//~ rjf: Per-Path Info Cache Types

typedef struct FS_RangeNode FS_RangeNode;
struct FS_RangeNode
{
  FS_RangeNode *next;
  HS_ID id;
  U64 working_count;
};

typedef struct FS_RangeSlot FS_RangeSlot;
struct FS_RangeSlot
{
  FS_RangeNode *first;
  FS_RangeNode *last;
};

typedef struct FS_Node FS_Node;
struct FS_Node
{
  FS_Node *next;
  
  // rjf: file metadata
  String8 path;
  FileProperties props;
  
  // rjf: hash store root
  HS_Root root;
  
  // rjf: sub-table of per-requested-file-range info
  U64 slots_count;
  FS_RangeSlot *slots;
};

typedef struct FS_Slot FS_Slot;
struct FS_Slot
{
  FS_Node *first;
  FS_Node *last;
};

typedef struct FS_Stripe FS_Stripe;
struct FS_Stripe
{
  Arena *arena;
  OS_Handle cv;
  OS_Handle rw_mutex;
};

////////////////////////////////
//~ rjf: Shared State Bundle

typedef struct FS_Shared FS_Shared;
struct FS_Shared
{
  Arena *arena;
  U64 change_gen;
  
  // rjf: path info cache
  U64 slots_count;
  U64 stripes_count;
  FS_Slot *slots;
  FS_Stripe *stripes;
  
  // rjf: user -> streamer ring buffer
  U64 u2s_ring_size;
  U8 *u2s_ring_base;
  U64 u2s_ring_write_pos;
  U64 u2s_ring_read_pos;
  OS_Handle u2s_ring_cv;
  OS_Handle u2s_ring_mutex;
  
  // rjf: change detector threads
  OS_Handle detector_thread;
};

////////////////////////////////
//~ rjf: Globals

global FS_Shared *fs_shared = 0;

////////////////////////////////
//~ rjf: Basic Helpers


////////////////////////////////
//~ rjf: Top-Level API


////////////////////////////////
//~ rjf: Change Generation


////////////////////////////////
//~ rjf: Cache Interaction


////////////////////////////////
//~ rjf: Streaming Work

ASYNC_WORK_DEF(fs_stream_work);

////////////////////////////////
//~ rjf: Change Detector Thread



// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Per-Path Info Cache Types

public struct FS_RangeNode
{
  FS_RangeNode* next;
  Rng1U64 range;
  uint64 request_count;
  uint64 completion_count;
  uint64 last_time_requested_us;
}

public struct FS_RangeSlot
{
  FS_RangeNode* first;
  FS_RangeNode* last;
}

public struct FS_Node
{
  FS_Node* next;
  
  // rjf: file metadata
  String8 path;
  uint64 timestamp;
  uint64 size;
  
  // rjf: sub-table of per-requested-file-range info
  uint64 slots_count;
  FS_RangeSlot* slots;
}

public struct FS_Slot
{
  FS_Node* first;
  FS_Node* last;
}

public struct FS_Stripe
{
  Arena* arena;
  OS_Handle cv;
  OS_Handle rw_mutex;
}

////////////////////////////////
//~ rjf: Shared State Bundle

public struct FS_Shared
{
  Arena* arena;
  uint64 change_gen;
  
  // rjf: path info cache
  uint64 slots_count;
  uint64 stripes_count;
  FS_Slot* slots;
  FS_Stripe* stripes;
  
  // rjf: user -> streamer ring buffer
  uint64 u2s_ring_size;
  uint8* u2s_ring_base;
  uint64 u2s_ring_write_pos;
  uint64 u2s_ring_read_pos;
  OS_Handle u2s_ring_cv;
  OS_Handle u2s_ring_mutex;
  
  // rjf: change detector threads
  OS_Handle detector_thread;
}

////////////////////////////////
//~ rjf: Globals

public static FS_Shared* fs_shared = null;

////////////////////////////////
//~ rjf: Streaming Work

ASYNC_WORK_DEF(fs_stream_work);

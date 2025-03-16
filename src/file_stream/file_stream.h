// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef FILE_STREAM_H
#define FILE_STREAM_H

////////////////////////////////
//~ rjf: Per-Path Info Cache Types

struct FS_RangeNode
{
  FS_RangeNode* next;
  Rng1U64 range;
  uint64 request_count;
  uint64 completion_count;
  uint64 last_time_requested_us;
}

struct FS_RangeSlot
{
  FS_RangeNode* first;
  FS_RangeNode* last;
}

struct FS_Node
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

struct FS_Slot
{
  FS_Node* first;
  FS_Node* last;
}

struct FS_Stripe
{
  Arena* arena;
  OS_Handle cv;
  OS_Handle rw_mutex;
}

////////////////////////////////
//~ rjf: Shared State Bundle

struct FS_Shared
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

static FS_Shared* fs_shared = 0;

////////////////////////////////
//~ rjf: Basic Helpers

uint64 fs_little_hash_from_string(String8 string);
U128 fs_big_hash_from_string_range(String8 string, Rng1U64 range);

////////////////////////////////
//~ rjf: Top-Level API

void fs_init();

////////////////////////////////
//~ rjf: Change Generation

uint64 fs_change_gen();

////////////////////////////////
//~ rjf: Cache Interaction

U128 fs_hash_from_path_range(String8 path, Rng1U64 range, uint64 endt_us);
U128 fs_key_from_path_range(String8 path, Rng1U64 range);

uint64 fs_timestamp_from_path(String8 path);
uint64 fs_size_from_path(String8 path);

////////////////////////////////
//~ rjf: Streaming Work

B32 fs_u2s_enqueue_req(Rng1U64 range, String8 path, uint64 endt_us);
void fs_u2s_dequeue_req(Arena* arena, Rng1U64* range_out, String8* path_out);
ASYNC_WORK_DEF(fs_stream_work);

////////////////////////////////
//~ rjf: Change Detector Thread

void fs_detector_thread__entry_point(void* p);

#endif // FILE_STREAM_H

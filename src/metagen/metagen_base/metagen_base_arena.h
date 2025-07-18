// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

////////////////////////////////
//~ rjf: Constants

#define ARENA_HEADER_SIZE 128

////////////////////////////////
//~ rjf: Types

enum ArenaFlags : uint64
{
  ArenaFlag_NoChain    = (1<<0),
  ArenaFlag_LargePages = (1<<1),
};

struct ArenaParams
{
  ArenaFlags flags;
  uint64 reserve_size;
  uint64 commit_size;
  void *optional_backing_buffer;
};

struct Arena
{
  Arena *prev;    // previous arena in chain
  Arena *current; // current arena in chain
  ArenaFlags flags;
  uint64 cmt_size;
  uint64 res_size;
  uint64 base_pos;
  uint64 pos;
  uint64 cmt;
  uint64 res;
#if ARENA_FREE_LIST
  uint64 free_size;
  Arena *free_last;
#endif
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

struct Temp
{
  Arena *arena;
  uint64 pos;
};

////////////////////////////////
//~ rjf: Global Defaults

global uint64 arena_default_reserve_size = MB(64);
global uint64 arena_default_commit_size  = KB(64);
global ArenaFlags arena_default_flags = 0;

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = arena_default_reserve_size, .commit_size = arena_default_commit_size, .flags = arena_default_flags, __VA_ARGS__})

//- rjf: arena push/pop/pos core functions

//- rjf: arena push/pop helpers

//- rjf: temporary arena scopes

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

////////////////////////////////
//~ rjf: Constants

#define ARENA_HEADER_SIZE 128

////////////////////////////////
//~ rjf: Types

enum ArenaFlags : ulong
{
  ArenaFlag_NoChain    = (1<<0),
  ArenaFlag_LargePages = (1<<1),
};

struct ArenaParams
{
  ArenaFlags flags;
  ulong reserve_size;
  ulong commit_size;
  void *optional_backing_buffer;
};

struct Arena
{
  Arena *prev;    // previous arena in chain
  Arena *current; // current arena in chain
  ArenaFlags flags;
  ulong cmt_size;
  ulong res_size;
  ulong base_pos;
  ulong pos;
  ulong cmt;
  ulong res;
#if ARENA_FREE_LIST
  ulong free_size;
  Arena *free_last;
#endif
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

struct Temp
{
  Arena *arena;
  ulong pos;
};

////////////////////////////////
//~ rjf: Global Defaults

static ulong arena_default_reserve_size = MB(64);
static ulong arena_default_commit_size  = KB(64);
static ArenaFlags arena_default_flags = 0;

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction
Arena *arena_alloc_(ArenaParams *params);
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = arena_default_reserve_size, .commit_size = arena_default_commit_size, .flags = arena_default_flags, __VA_ARGS__})
void arena_release(Arena *arena);

//- rjf: arena push/pop/pos core functions
void *arena_push(Arena *arena, ulong size, ulong align);
ulong   arena_pos(Arena *arena);
void  arena_pop_to(Arena *arena, ulong pos);

//- rjf: arena push/pop helpers
void arena_clear(Arena *arena);
void arena_pop(Arena *arena, ulong amt);

//- rjf: temporary arena scopes
Temp temp_begin(Arena *arena);
void temp_end(Temp temp);

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA_H

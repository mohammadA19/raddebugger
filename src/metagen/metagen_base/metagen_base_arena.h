// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

////////////////////////////////
//~ rjf: Constants

#define ARENA_HEADER_SIZE 64

////////////////////////////////
//~ rjf: Types

typedef uint ArenaFlags;
enum
{
  ArenaFlag_NoChain    = (1<<0),
  ArenaFlag_LargePages = (1<<1),
};

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
  ArenaFlags flags;
  ulong reserve_size;
  ulong commit_size;
  void *optional_backing_buffer;
};

typedef struct Arena Arena;
struct Arena
{
  Arena *prev;    // previous arena in chain
  Arena *current; // current arena in chain
  ArenaFlags flags;
  uint cmt_size;
  ulong res_size;
  ulong base_pos;
  ulong pos;
  ulong cmt;
  ulong res;
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp
{
  Arena *arena;
  ulong pos;
};

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction
internal Arena *arena_alloc_(ArenaParams *params);
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = MB(64), .commit_size = KB(64), __VA_ARGS__})
internal void arena_release(Arena *arena);

//- rjf: arena push/pop/pos core functions
internal void *arena_push(Arena *arena, ulong size, ulong align);
internal ulong   arena_pos(Arena *arena);
internal void  arena_pop_to(Arena *arena, ulong pos);

//- rjf: arena push/pop helpers
internal void arena_clear(Arena *arena);
internal void arena_pop(Arena *arena, ulong amt);

//- rjf: temporary arena scopes
internal Temp temp_begin(Arena *arena);
internal void temp_end(Temp temp);

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA_H

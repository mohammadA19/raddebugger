// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

////////////////////////////////
//~ rjf: Constants

#define ARENA_HEADER_SIZE 64

////////////////////////////////
//~ rjf: Types

enum ArenaFlags : uint32
{
  NoChain    = (1<<0),
  LargePages = (1<<1),
}

struct ArenaParams
{
  ArenaFlags flags;
  uint64 reserve_size;
  uint64 commit_size;
  void* optional_backing_buffer;
}

struct Arena
{
  Arena* prev;    // previous arena in chain
  Arena* current; // current arena in chain
  ArenaFlags flags;
  uint32 cmt_size;
  uint64 res_size;
  uint64 base_pos;
  uint64 pos;
  uint64 cmt;
  uint64 res;
}
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

struct Temp
{
  Arena* arena;
  uint64 pos;
}

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction
Arena* arena_alloc_(ArenaParams* params);
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = MB(64), .commit_size = KB(64), __VA_ARGS__})
void arena_release(Arena* arena);

//- rjf: arena push/pop/pos core functions
void* arena_push(Arena* arena, uint64 size, uint64 align);
uint64   arena_pos(Arena* arena);
void  arena_pop_to(Arena* arena, uint64 pos);

//- rjf: arena push/pop helpers
void arena_clear(Arena* arena);
void arena_pop(Arena* arena, uint64 amt);

//- rjf: temporary arena scopes
Temp temp_begin(Arena* arena);
void temp_end(Temp temp);

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA_H

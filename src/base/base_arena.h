// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Constants

const ARENA_HEADER_SIZE = 128

////////////////////////////////
//~ rjf: Types

type
  ArenaFlag* = enum
    NoChain    = 1 shl 0
    LargePages = 1 shl 1
  ArenaFlags* = set[ArenaFlag] # uint64

  ArenaParams* = object
    flags*: ArenaFlags
    reserve_size*: uint64
    commit_size*: uint64
    optional_backing_buffer*: pointer

  Arena* = object
    prev*: ptr Arena;    // previous arena in chain
    current*: ptr Arena; // current arena in chain
    flags*: ArenaFlags
    cmt_size*: uint64
    res_size*: uint64
    base_pos*: uint64
    pos*: uint64
    cmt*: uint64
    res*: uint64
    when ARENA_FREE_LIST:
      free_size*: uint64
      free_last*: ptr Arena

StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check)

  Temp* = object
    arena*: ptr Arena
    pos*: uint64

////////////////////////////////
//~ rjf: Global Defaults

var uint64 arena_default_reserve_size = MB(64)
var uint64 arena_default_commit_size  = KB(64)
var ArenaFlags arena_default_flags = 0

//- rjf: arena creation/destruction
#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = arena_default_reserve_size, .commit_size = arena_default_commit_size, .flags = arena_default_flags, __VA_ARGS__})

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (ptr T )arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (ptr T )MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

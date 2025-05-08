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

  Temp* = object
    arena*: ptr Arena
    pos*: uint64
      
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check)

////////////////////////////////
//~ rjf: Global Defaults

var arena_default_reserve_size : uint64 = MB(64)
var arena_default_commit_size : uint64  = KB(64)
var arena_default_flags : ArenaFlags = 0

//- rjf: arena creation/destruction
proc arena_alloc(reserveSize = arena_default_reserve_size, commitSize = arena_default_commit_size, arena_flags = arena_default_flags, optional_backing_buffer : pointer = nil) : ptr Arena =
  arena_alloc_(addr ArenaParams(flags: arena_flags, reserve_size: reserveSize, commit_size: commitSize, optional_backing_buffer: optional_backing_buffer))

//- rjf: push helper macros
proc push_array_no_zero_aligned[T](a: ptr Arena, count: uint64, align: uint64) : ptr T =
  cast[ptr T](arena_push(a, sizeof(T) * count, align))

proc push_array_aligned[T](a: ptr Arena, count: uint64, align: uint64) : ptr T =
  cast[ptr T]MemoryZero(push_array_no_zero_aligned[T](a, count, align), sizeof(T) * count)

proc push_array_no_zero[T](a: ptr Arena, count: uint64) : ptr T =
  push_array_no_zero_aligned[T](a, count, max(8, alignof(T)))

proc push_array[T](a: ptr Arena, count: uint64) : ptr T =
  push_array_aligned[T](a, count, max(8, alignof(T)))
  
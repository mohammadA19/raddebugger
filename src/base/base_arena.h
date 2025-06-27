// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Constants

public const int ARENA_HEADER_SIZE = 128;

////////////////////////////////
//~ rjf: Types

public enum ArenaFlags : U64
{
  ArenaFlag_NoChain    = (1<<0),
  ArenaFlag_LargePages = (1<<1),
}

public struct ArenaParams
{
  ArenaFlags flags;
  U64 reserve_size;
  U64 commit_size;
  void* optional_backing_buffer;
  StringView allocation_site_file;
  int allocation_site_line;
}

public struct Arena
{
  Arena* prev;    // previous arena in chain
  Arena* current; // current arena in chain
  ArenaFlags flags;
  U64 cmt_size;
  U64 res_size;
  U64 base_pos;
  U64 pos;
  U64 cmt;
  U64 res;
  char* allocation_site_file;
  int allocation_site_line;
#if ARENA_FREE_LIST
  U64 free_size;
  Arena* free_last;
#endif
}
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

public struct Temp
{
  Arena *arena;
  U64 pos;
}

////////////////////////////////
//~ rjf: Global Defaults

public static U64 arena_default_reserve_size = MB(64);
public static U64 arena_default_commit_size  = KB(64);
public static ArenaFlags arena_default_flags = 0;

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction
public static void* arena_alloc(ArenaFlags flags, 
  U64 reserve_size, 
  U64 commit_size, 
  void* optional_backing_buffer, 
  StringView allocation_site_file = Compiler.CallerFilePath, 
  int allocation_site_line = Compiler.CallerLineNum) 
=> arena_alloc_(ArenaParams {reserve_size = arena_default_reserve_size, commit_size = arena_default_commit_size, flags = arena_default_flags, allocation_site_file = allocation_site_file, allocation_site_line = allocation_site_line});

//- rjf: push helper macros
public static T* push_array_no_zero_aligned<T>(Arena* a, int c, int align)
  => (T*)arena_push((a), sizeof(T)* c, align);

public static T* push_array_aligned<T>(Arena* a, int c, int align)
  => (T*)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)* c);

public static T* push_array_no_zero<T>(Arena* a, int c)
  => push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)));

public static T* push_array<T>(Arena* a, int c)
  => push_array_aligned(a, T, c, Max(8, AlignOf(T)));

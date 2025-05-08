// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction

proc arena_alloc_(params: ptr ArenaParams) : ptr Arena =
  // rjf: round up reserve/commit sizes
  var reserve_size = params.reserve_size
  var commit_size = params.commit_size
  if ArenaFlag.LargePages in params.flags:
    reserve_size = AlignPow2(reserve_size, os_get_system_info().large_page_size)
    commit_size  = AlignPow2(commit_size,  os_get_system_info().large_page_size)
  else:
    reserve_size = AlignPow2(reserve_size, os_get_system_info().page_size)
    commit_size  = AlignPow2(commit_size,  os_get_system_info().page_size)
  
  // rjf: reserve/commit initial block
  var base = params.optional_backing_buffer
  if base == nil:
    if ArenaFlag.LargePages in params.flags:
      base = os_reserve_large(reserve_size)
      os_commit_large(base, commit_size)
    else
      base = os_reserve(reserve_size)
      os_commit(base, commit_size)
  
  // rjf: panic on arena creation failure
  when OS_FEATURE_GRAPHICAL:
    if unlikely(base == nil):
      os_graphical_message(1, str8_lit("Fatal Allocation Failure"), str8_lit("Unexpected memory allocation failure."))
      os_abort(1)
  
  // rjf: extract arena header & fill
  var arena = cast[ptr Arena](base)
  arena.current = arena
  arena.flags = params.flags
  arena.cmt_size = params.commit_size
  arena.res_size = params.reserve_size
  arena.base_pos = 0
  arena.pos = ARENA_HEADER_SIZE
  arena.cmt = commit_size
  arena.res = reserve_size
  when ARENA_FREE_LIST:
    arena.free_size = 0
    arena.free_last = 0
  
  AsanPoisonMemoryRegion(base, commit_size)
  AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE)
  return arena

proc arena_release(arena: ptr Arena) =
  for(n: ptr Arena = arena.current, prev : ptr Arena = 0; n != 0; n = prev)
    prev = n.prev
    os_release(n, n.res)

//- rjf: arena push/pop core functions

proc arena_push(arena: ptr Arena, size: uint64, align: uint64) : pointer =
  var current = arena.current
  var pos_pre: uint64 = AlignPow2(current.pos, align)
  var pos_pst: uint64 = pos_pre + size
  
  // rjf: chain, if needed
  if current.res < pos_pst and ArenaFlag.NoChain notin arena.flags:
    var new_block: ptr Arena = nil
    
    when ARENA_FREE_LIST:
      prev_block: ptr Arena
      for(new_block = arena.free_last, prev_block = nil; new_block != nil; prev_block = new_block, new_block = new_block.prev)
        if new_block.res >= AlignPow2(size, align):
          if prev_block:
            prev_block.prev = new_block.prev
          else
            arena.free_last = new_block.prev
          arena.free_size -= new_block.res_size
          AsanUnpoisonMemoryRegion(cast[ptr uint8](new_block) + ARENA_HEADER_SIZE, new_block.res_size - ARENA_HEADER_SIZE)
          break
    
    if new_block == nil:
      var res_size: uint64 = current.res_size
      var cmt_size: uint64 = current.cmt_size
      if size + ARENA_HEADER_SIZE > res_size:
        res_size = AlignPow2(size + ARENA_HEADER_SIZE, align)
        cmt_size = AlignPow2(size + ARENA_HEADER_SIZE, align)
      new_block = arena_alloc(.reserve_size = res_size,
                              .commit_size  = cmt_size,
                              .flags        = current.flags)
    
    new_block.base_pos = current.base_pos + current.res
    SLLStackPush_N(arena.current, new_block, prev)
    
    current = new_block
    pos_pre = AlignPow2(current.pos, align)
    pos_pst = pos_pre + size
  
  // rjf: commit new pages, if needed
  if current.cmt < pos_pst
    var cmt_pst_aligned: uint64 = pos_pst + current.cmt_size-1
    cmt_pst_aligned -= cmt_pst_aligned%current.cmt_size
    var cmt_pst_clamped: uint64 = ClampTop(cmt_pst_aligned, current.res)
    var cmt_size: uint64 = cmt_pst_clamped - current.cmt
    var cmt_ptr: ptr uint8 = cast[ptr uint8](current) + current.cmt
    if(ArenaFlag.LargePages in current.flags)
      os_commit_large(cmt_ptr, cmt_size)
    else
      os_commit(cmt_ptr, cmt_size)
    current.cmt = cmt_pst_clamped
  
  // rjf: push onto current block
  var result: pointer = nil
  if current.cmt >= pos_pst:
    result = cast[ptr uint8](current+pos_pre)
    current.pos = pos_pst
    AsanUnpoisonMemoryRegion(result, size)
  
  // rjf: panic on failure
  when OS_FEATURE_GRAPHICAL:
    if unlikely(result == nil):
      os_graphical_message(1, str8_lit("Fatal Allocation Failure"), str8_lit("Unexpected memory allocation failure."))
      os_abort(1)

  return result

proc arena_pos(arena: ptr Arena) : uint64 =
  var current: ptr Arena = arena.current
  var pos: uint64 = current.base_pos + current.pos
  return pos

proc void arena_pop_to(arena: ptr Arena, pos: uint64) =
  var big_pos: uint64 = ClampBot(ARENA_HEADER_SIZE, pos)
  var current: ptr Arena = arena.current
  
  when ARENA_FREE_LIST:
    for(prev: ptr Arena = nil; current.base_pos >= big_pos; current = prev)
      prev = current.prev
      current.pos = ARENA_HEADER_SIZE
      arena.free_size += current.res_size
      SLLStackPush_N(arena.free_last, current, prev)
      AsanPoisonMemoryRegion(cast[ptr uint8](current) + ARENA_HEADER_SIZE, current.res_size - ARENA_HEADER_SIZE)
  else:
    for(prev: ptr Arena = nil; current.base_pos >= big_pos; current = prev)
      prev = current.prev
      os_release(current, current.res)

  arena.current = current
  var new_pos: uint64 = big_pos - current.base_pos
  AssertAlways(new_pos <= current.pos)
  AsanPoisonMemoryRegion(cast[ptr uint8](current) + new_pos, (current.pos - new_pos))
  current.pos = new_pos

//- rjf: arena push/pop helpers

proc arena_clear(arena: ptr Arena) =
  arena_pop_to(arena, 0)

proc arena_pop(arena: ptr Arena, amt: uint64) =
  var pos_old: uint64 = arena_pos(arena)
  var pos_new: uint64 = pos_old
  if amt < pos_old:
    pos_new = pos_old - amt
  arena_pop_to(arena, pos_new)

//- rjf: temporary arena scopes

proc temp_begin(arena: ptr Arena) : Temp =
  var pos: uint64 = arena_pos(arena)
  var temp: Temp = Temp(arena, pos)
  return temp

proc temp_end(temp: Temp) =
  arena_pop_to(temp.arena, temp.pos)

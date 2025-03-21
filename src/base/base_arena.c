// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction

internal Arena *
arena_alloc_(ArenaParams *params)
{
  // rjf: round up reserve/commit sizes
  ulong reserve_size = params->reserve_size;
  ulong commit_size = params->commit_size;
  if(params->flags & ArenaFlag_LargePages)
  {
    reserve_size = AlignPow2(reserve_size, os_get_system_info()->large_page_size);
    commit_size  = AlignPow2(commit_size,  os_get_system_info()->large_page_size);
  }
  else
  {
    reserve_size = AlignPow2(reserve_size, os_get_system_info()->page_size);
    commit_size  = AlignPow2(commit_size,  os_get_system_info()->page_size);
  }
  
  // rjf: reserve/commit initial block
  void *base = params->optional_backing_buffer;
  if(base == 0)
  {
    if(params->flags & ArenaFlag_LargePages)
    {
      base = os_reserve_large(reserve_size);
      os_commit_large(base, commit_size);
    }
    else
    {
      base = os_reserve(reserve_size);
      os_commit(base, commit_size);
    }
  }
  
  // rjf: panic on arena creation failure
#if OS_FEATURE_GRAPHICAL
  if(Unlikely(base == 0))
  {
    os_graphical_message(1, str8_lit("Fatal Allocation Failure"), str8_lit("Unexpected memory allocation failure."));
    os_abort(1);
  }
#endif
  
  // rjf: extract arena header & fill
  Arena *arena = (Arena *)base;
  arena->current = arena;
  arena->flags = params->flags;
  arena->cmt_size = params->commit_size;
  arena->res_size = params->reserve_size;
  arena->base_pos = 0;
  arena->pos = ARENA_HEADER_SIZE;
  arena->cmt = commit_size;
  arena->res = reserve_size;
#if ARENA_FREE_LIST
  arena->free_size = 0;
  arena->free_last = 0;
#endif
  AsanPoisonMemoryRegion(base, commit_size);
  AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);
  return arena;
}

internal void
arena_release(Arena *arena)
{
  for(Arena *n = arena->current, *prev = 0; n != 0; n = prev)
  {
    prev = n->prev;
    os_release(n, n->res);
  }
}

//- rjf: arena push/pop core functions

internal void *
arena_push(Arena *arena, ulong size, ulong align)
{
  Arena *current = arena->current;
  ulong pos_pre = AlignPow2(current->pos, align);
  ulong pos_pst = pos_pre + size;
  
  // rjf: chain, if needed
  if(current->res < pos_pst && !(arena->flags & ArenaFlag_NoChain))
  {
    Arena *new_block = 0;
    
#if ARENA_FREE_LIST
    Arena *prev_block;
    for(new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev)
    {
      if(new_block->res >= AlignPow2(size, align))
      {
        if(prev_block)
        {
          prev_block->prev = new_block->prev;
        }
        else
        {
          arena->free_last = new_block->prev;
        }
        arena->free_size -= new_block->res_size;
        AsanUnpoisonMemoryRegion((byte*)new_block + ARENA_HEADER_SIZE, new_block->res_size - ARENA_HEADER_SIZE);
        break;
      }
    }
#endif
    
    if(new_block == 0)
    {
      ulong res_size = current->res_size;
      ulong cmt_size = current->cmt_size;
      if(size + ARENA_HEADER_SIZE > res_size)
      {
        res_size = AlignPow2(size + ARENA_HEADER_SIZE, align);
        cmt_size = AlignPow2(size + ARENA_HEADER_SIZE, align);
      }
      new_block = arena_alloc(.reserve_size = res_size,
                              .commit_size  = cmt_size,
                              .flags        = current->flags);
    }
    
    new_block->base_pos = current->base_pos + current->res;
    SLLStackPush_N(arena->current, new_block, prev);
    
    current = new_block;
    pos_pre = AlignPow2(current->pos, align);
    pos_pst = pos_pre + size;
  }
  
  // rjf: commit new pages, if needed
  if(current->cmt < pos_pst)
  {
    ulong cmt_pst_aligned = pos_pst + current->cmt_size-1;
    cmt_pst_aligned -= cmt_pst_aligned%current->cmt_size;
    ulong cmt_pst_clamped = ClampTop(cmt_pst_aligned, current->res);
    ulong cmt_size = cmt_pst_clamped - current->cmt;
    byte *cmt_ptr = (byte *)current + current->cmt;
    if(current->flags & ArenaFlag_LargePages)
    {
      os_commit_large(cmt_ptr, cmt_size);
    }
    else
    {
      os_commit(cmt_ptr, cmt_size);
    }
    current->cmt = cmt_pst_clamped;
  }
  
  // rjf: push onto current block
  void *result = 0;
  if(current->cmt >= pos_pst)
  {
    result = (byte *)current+pos_pre;
    current->pos = pos_pst;
    AsanUnpoisonMemoryRegion(result, size);
  }
  
  // rjf: panic on failure
#if OS_FEATURE_GRAPHICAL
  if(Unlikely(result == 0))
  {
    os_graphical_message(1, str8_lit("Fatal Allocation Failure"), str8_lit("Unexpected memory allocation failure."));
    os_abort(1);
  }
#endif
  
  return result;
}

internal ulong
arena_pos(Arena *arena)
{
  Arena *current = arena->current;
  ulong pos = current->base_pos + current->pos;
  return pos;
}

internal void
arena_pop_to(Arena *arena, ulong pos)
{
  ulong big_pos = ClampBot(ARENA_HEADER_SIZE, pos);
  Arena *current = arena->current;
  
#if ARENA_FREE_LIST
  for(Arena *prev = 0; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    current->pos = ARENA_HEADER_SIZE;
    arena->free_size += current->res_size;
    SLLStackPush_N(arena->free_last, current, prev);
    AsanPoisonMemoryRegion((byte*)current + ARENA_HEADER_SIZE, current->res_size - ARENA_HEADER_SIZE);
  }
#else
  for(Arena *prev = 0; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    os_release(current, current->res);
  }
#endif
  arena->current = current;
  ulong new_pos = big_pos - current->base_pos;
  AssertAlways(new_pos <= current->pos);
  AsanPoisonMemoryRegion((byte*)current + new_pos, (current->pos - new_pos));
  current->pos = new_pos;
}

//- rjf: arena push/pop helpers

internal void
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

internal void
arena_pop(Arena *arena, ulong amt)
{
  ulong pos_old = arena_pos(arena);
  ulong pos_new = pos_old;
  if(amt < pos_old)
  {
    pos_new = pos_old - amt;
  }
  arena_pop_to(arena, pos_new);
}

//- rjf: temporary arena scopes

internal Temp
temp_begin(Arena *arena)
{
  ulong pos = arena_pos(arena);
  Temp temp = {arena, pos};
  return temp;
}

internal void
temp_end(Temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

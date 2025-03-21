// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Arena Functions

//- rjf: arena creation/destruction

Arena *
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
  arena->cmt_size = (uint)params->commit_size;
  arena->res_size = params->reserve_size;
  arena->base_pos = 0;
  arena->pos = ARENA_HEADER_SIZE;
  arena->cmt = commit_size;
  arena->res = reserve_size;
  AsanPoisonMemoryRegion(base, commit_size);
  AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);
  return arena;
}

void
arena_release(Arena *arena)
{
  for(Arena *n = arena->current, *prev = 0; n != 0; n = prev)
  {
    prev = n->prev;
    os_release(n, n->res);
  }
}

//- rjf: arena push/pop core functions

void *
arena_push(Arena *arena, ulong size, ulong align)
{
  Arena *current = arena->current;
  ulong pos_pre = AlignPow2(current->pos, align);
  ulong pos_pst = pos_pre + size;
  
  // rjf: chain, if needed
  if(current->res < pos_pst && !(arena->flags & ArenaFlag_NoChain))
  {
    ulong res_size = current->res_size;
    ulong cmt_size = current->cmt_size;
    if(size + ARENA_HEADER_SIZE > res_size)
    {
      res_size = size + ARENA_HEADER_SIZE;
      cmt_size = size + ARENA_HEADER_SIZE;
    }
    Arena *new_block = arena_alloc(.reserve_size = res_size,
                                   .commit_size = cmt_size,
                                   .flags = current->flags);
    new_block->base_pos = current->base_pos + current->res;
    SLLStackPush_N(arena->current, new_block, prev);
    current = new_block;
    pos_pre = AlignPow2(current->pos, align);
    pos_pst = pos_pre + size;
  }
  
  // rjf: commit new pages, if needed
  if(current->cmt < pos_pst && !(current->flags & ArenaFlag_LargePages))
  {
    ulong cmt_pst_aligned = AlignPow2(pos_pst, current->cmt_size);
    ulong cmt_pst_clamped = ClampTop(cmt_pst_aligned, current->res);
    ulong cmt_size = cmt_pst_clamped - current->cmt;
    os_commit((byte *)current + current->cmt, cmt_size);
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

ulong
arena_pos(Arena *arena)
{
  Arena *current = arena->current;
  ulong pos = current->base_pos + current->pos;
  return pos;
}

void
arena_pop_to(Arena *arena, ulong pos)
{
  ulong big_pos = ClampBot(ARENA_HEADER_SIZE, pos);
  Arena *current = arena->current;
  for(Arena *prev = 0; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    os_release(current, current->res);
  }
  arena->current = current;
  ulong new_pos = big_pos - current->base_pos;
  AssertAlways(new_pos <= current->pos);
  AsanPoisonMemoryRegion((byte*)current + new_pos, (current->pos - new_pos));
  current->pos = new_pos;
}

//- rjf: arena push/pop helpers

void
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

void
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

Temp
temp_begin(Arena *arena)
{
  ulong pos = arena_pos(arena);
  Temp temp = {arena, pos};
  return temp;
}

void
temp_end(Temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

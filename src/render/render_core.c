// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Generated Code

#include "generated/render.meta.c"

////////////////////////////////
//~ rjf: Basic Type Functions

public static R_Handle r_handle_zero()
{
  R_Handle handle = default;
  return handle;
}

public static B32 r_handle_match(R_Handle a, R_Handle b)
{
  return MemoryMatchStruct(&a, &b);
}

////////////////////////////////
//~ rjf: Batch Type Functions

public static R_BatchList r_batch_list_make(uint64 instance_size)
{
  R_BatchList list = default;
  list.bytes_per_inst = instance_size;
  return list;
}

public static void* r_batch_list_push_inst(Arena* arena, R_BatchList* list, uint64 batch_inst_cap)
{
  void* inst = null;
  {
    R_BatchNode* n = list.last;
    if(n == null || n.v.byte_count+list.bytes_per_inst > n.v.byte_cap)
    {
      n = push_array(arena, R_BatchNode, 1);
      n.v.byte_cap = batch_inst_cap*list.bytes_per_inst;
      n.v.v = arena.PushArrayNoZero<uint8>(n.v.byte_cap); 
      SLLQueuePush(list.first, list.last, n);
      list.batch_count += 1;
    }
    inst = n.v.v + n.v.byte_count;
    n.v.byte_count += list.bytes_per_inst;
    list.byte_count += list.bytes_per_inst;
  }
  return inst;
}

////////////////////////////////
//~ rjf: Pass Type Functions

public static R_Pass* r_pass_from_kind(Arena* arena, R_PassList* list, R_PassKind kind)
{
  R_PassNode* n = list.last;
  if(!r_pass_kind_batch_table[kind])
  {
    n = null;
  }
  if(n == null || n.v.kind != kind)
  {
    n = push_array(arena, R_PassNode, 1);
    SLLQueuePush(list.first, list.last, n);
    list.count += 1;
    n.v.kind = kind;
    n.v.params = push_array(arena, uint8, r_pass_kind_params_size_table[kind]);
  }
  return &n.v;
}

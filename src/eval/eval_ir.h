// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_IR_H
#define EVAL_IR_H

////////////////////////////////
//~ rjf: IR State

struct E_IRCacheNode
{
  E_IRCacheNode *next;
  E_Expr *expr;
  E_IRNode *overridden_node;
  E_IRTreeAndType irtree;
}

struct E_IRCacheSlot
{
  E_IRCacheNode *first;
  E_IRCacheNode *last;
}

struct E_IRState
{
  Arena *arena;
  U64 arena_eval_start_pos;
  
  // rjf: ir context
  E_IRCtx *ctx;
  
  // rjf: unpacked ctx
  RDI_Procedure *thread_ip_procedure;
  
  // rjf: overridden irtree
  E_IRTreeAndType *overridden_irtree;
  B32 disallow_autohooks;
  B32 disallow_chained_fastpaths;
  
  // rjf: caches
  E_UsedExprMap *used_expr_map;
  E_TypeAutoHookCacheMap *type_auto_hook_cache_map;
  U64 string_id_gen;
  E_StringIDMap *string_id_map;
  U64 ir_cache_slots_count;
  E_IRCacheSlot *ir_cache_slots;
}

////////////////////////////////
//~ rjf: IR-ization Functions
//- rjf: top-level irtree/type extraction
E_TYPE_ACCESS_FUNCTION_DEF(default);
//- rjf: leaf-bytecode expression extensions

#endif // EVAL_IR_H

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Identifier Resolution Rule Types

typedef enum E_IdentifierResolutionPath
{
  E_IdentifierResolutionPath_WildcardInst,
  E_IdentifierResolutionPath_ParentExpr,
  E_IdentifierResolutionPath_ParentExprMember,
  E_IdentifierResolutionPath_ImplicitThisMember,
  E_IdentifierResolutionPath_Local,
  E_IdentifierResolutionPath_DebugInfoMatch,
  E_IdentifierResolutionPath_BuiltInConstants,
  E_IdentifierResolutionPath_BuiltInTypes,
  E_IdentifierResolutionPath_Registers,
  E_IdentifierResolutionPath_RegisterAliases,
  E_IdentifierResolutionPath_Macros,
}
E_IdentifierResolutionPath;



struct E_IdentifierResolutionRule
{
  E_IdentifierResolutionPath *paths;
  u64 count;
};

////////////////////////////////
//~ rjf: IR State



struct E_IRCacheNode
{
  E_IRCacheNode *next;
  E_Expr *expr;
  E_IRNode *overridden_node;
  E_IRTreeAndType irtree;
};



struct E_IRCacheSlot
{
  E_IRCacheNode *first;
  E_IRCacheNode *last;
};



struct E_IRState
{
  Arena *arena;
  u64 arena_eval_start_pos;
  
  // rjf: ir context
  E_IRCtx *ctx;
  
  // rjf: unpacked ctx
  RDI_Procedure *thread_ip_procedure;
  
  // rjf: overridden irtree
  E_IRTreeAndType *overridden_irtree;
  b32 disallow_autohooks;
  b32 disallow_chained_fastpaths;
  
  // rjf: caches
  E_UsedExprMap *used_expr_map;
  E_TypeAutoHookCacheMap *type_auto_hook_cache_map;
  u64 string_id_gen;
  E_StringIDMap *string_id_map;
  u64 ir_cache_slots_count;
  E_IRCacheSlot *ir_cache_slots;
};

////////////////////////////////
//~ rjf: Globals

E_IdentifierResolutionPath e_default_identifier_resolution_paths[] =
{
  E_IdentifierResolutionPath_WildcardInst,
  E_IdentifierResolutionPath_ParentExpr,
  E_IdentifierResolutionPath_ParentExprMember,
  E_IdentifierResolutionPath_ImplicitThisMember,
  E_IdentifierResolutionPath_Local,
  E_IdentifierResolutionPath_BuiltInConstants,
  E_IdentifierResolutionPath_BuiltInTypes,
  E_IdentifierResolutionPath_DebugInfoMatch,
  E_IdentifierResolutionPath_Registers,
  E_IdentifierResolutionPath_RegisterAliases,
  E_IdentifierResolutionPath_Macros,
};
E_IdentifierResolutionRule e_default_identifier_resolution_rule =
{
  e_default_identifier_resolution_paths,
  len(e_default_identifier_resolution_paths),
};

E_IdentifierResolutionPath e_callable_identifier_resolution_paths[] =
{
  E_IdentifierResolutionPath_Macros,
  E_IdentifierResolutionPath_WildcardInst,
  E_IdentifierResolutionPath_ParentExpr,
  E_IdentifierResolutionPath_ParentExprMember,
  E_IdentifierResolutionPath_ImplicitThisMember,
  E_IdentifierResolutionPath_Local,
  E_IdentifierResolutionPath_BuiltInConstants,
  E_IdentifierResolutionPath_BuiltInTypes,
  E_IdentifierResolutionPath_DebugInfoMatch,
  E_IdentifierResolutionPath_Registers,
  E_IdentifierResolutionPath_RegisterAliases,
};
E_IdentifierResolutionRule e_callable_identifier_resolution_rule =
{
  e_callable_identifier_resolution_paths,
  len(e_callable_identifier_resolution_paths),
};

////////////////////////////////
//~ rjf: IR-ization Functions

//- rjf: op list functions








//- rjf: ir tree core building helpers



//- rjf: ir subtree building helpers















//- rjf: expression poison checking




//- rjf: top-level irtree/type extraction
E_TYPE_ACCESS_FUNCTION_DEF(default);


//- rjf: irtree -> linear ops/bytecode




//- rjf: leaf-bytecode expression extensions




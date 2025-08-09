// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct D2R_ConvertParams
{
  string dbg_name;
  string dbg_data;
  string exe_name;
  string exe_data;
  ExecutableImageKind exe_kind;
  RDIM_SubsetFlags subset_flags;
  B32 deterministic;
};

typedef struct D2R_TypeTable
{
  HashTable          *ht;
  RDIM_TypeChunkList *types;
  U64                 type_chunk_cap;
  RDIM_Type          *varg_type;
} D2R_TypeTable;

typedef struct D2R_TagNode
{
  struct D2R_TagNode *next;
  DW_TagNode         *cur_node;
  RDIM_Type          *type;
  RDIM_Scope         *scope;
} D2R_TagNode;

typedef struct D2R_CompUnitContribMap
{
  U64                    count;
  U64                   *info_off_arr;
  RDIM_Rng1U64ChunkList *voff_range_arr;
} D2R_CompUnitContribMap;

////////////////////////////////
//~ rjf: Enum Conversion Helpers


////////////////////////////////
//~ rjf: Type Conversion Helpers


////////////////////////////////
//~ rjf: Bytecode Conversion Helpers

internal RDIM_EvalBytecode
d2r_bytecode_from_expression(Arena       *arena,
                             DW_Input    *input,
                             U64          image_base,
                             U64          address_size,
                             Arch         arch,
                             DW_ListUnit *addr_lu,
                             string      expr,
                             DW_CompUnit *cu,
                             B32         *is_addr_out);
internal RDIM_LocationSet d2r_locset_from_attrib(Arena               *arena,
                                                 DW_Input            *input,
                                                 DW_CompUnit         *cu,
                                                 RDIM_ScopeChunkList *scopes,
                                                 RDIM_Scope          *curr_scope,
                                                 U64                  image_base,
                                                 Arch                 arch,
                                                 DW_Tag               tag,
                                                 DW_AttribKind        kind);
internal RDIM_LocationSet d2r_var_locset_from_tag(Arena               *arena,
                                                  DW_Input            *input,
                                                  DW_CompUnit         *cu,
                                                  RDIM_ScopeChunkList *scopes,
                                                  RDIM_Scope          *curr_scope,
                                                  U64                  image_base,
                                                  Arch                 arch,
                                                  DW_Tag               tag);

////////////////////////////////
//~ rjf: Compilation Unit / Scope Conversion Helpers


////////////////////////////////
//~ rjf: Main Conversion Entry Point


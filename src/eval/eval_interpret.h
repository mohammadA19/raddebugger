// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_INTERPRET_H
#define EVAL_INTERPRET_H

////////////////////////////////
//~ rjf: Interpretation Context

struct E_InterpretCtx
{
  void *space_rw_user_data;
  E_SpaceRWFunction *space_read;
  E_SpaceRWFunction *space_write;
  E_Space primary_space;
  Arch reg_arch;
  E_Space reg_space;
  uint64 reg_unwind_count;
  uint64 *module_base;
  uint64 *frame_base;
  uint64 *tls_base;
};

////////////////////////////////
//~ rjf: Globals

thread_static E_InterpretCtx *e_interpret_ctx = 0;

////////////////////////////////
//~ rjf: Context Selection Functions (Selection Required For All Subsequent APIs)

internal void e_select_interpret_ctx(E_InterpretCtx *ctx, RDI_Parsed *primary_rdi, uint64 ip_voff);

////////////////////////////////
//~ rjf: Space Reading Helpers

internal uint64 e_space_gen(E_Space space);
internal B32 e_space_read(E_Space space, void *out, Rng1uint64 range);
internal B32 e_space_write(E_Space space, void *in, Rng1uint64 range);

////////////////////////////////
//~ rjf: Interpretation Functions

internal E_Interpretation e_interpret(String8 bytecode);

#endif // EVAL_INTERPRET_H

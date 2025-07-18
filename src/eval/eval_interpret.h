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


////////////////////////////////
//~ rjf: Space Reading Helpers


////////////////////////////////
//~ rjf: Interpretation Functions


#endif // EVAL_INTERPRET_H

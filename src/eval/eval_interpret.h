// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


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
  U64 reg_unwind_count;
  U64 *module_base;
  U64 *frame_base;
  U64 *tls_base;
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



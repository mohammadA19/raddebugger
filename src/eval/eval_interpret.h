// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_INTERPRET_H
#define EVAL_INTERPRET_H

////////////////////////////////
//~ rjf: Bytecode Interpretation Types

struct E_Interpretation
{
  E_Value value;
  E_InterpretationCode code;
}

////////////////////////////////
//~ rjf: Interpretation Context

typedef B32 E_SpaceRWFunction(void* user_data, E_Space space, void* out, Rng1U64 offset_range);

struct E_InterpretCtx
{
  void* space_rw_user_data;
  E_SpaceRWFunction* space_read;
  E_SpaceRWFunction* space_write;
  E_Space primary_space;
  Arch reg_arch;
  E_Space reg_space;
  U64 reg_unwind_count;
  U64* module_base;
  U64* frame_base;
  U64* tls_base;
}

////////////////////////////////
//~ rjf: Globals

thread_static E_InterpretCtx* e_interpret_ctx = 0;

////////////////////////////////
//~ rjf: Context Selection Functions (Selection Required For All Subsequent APIs)

E_InterpretCtx* e_selected_interpret_ctx();
void e_select_interpret_ctx(E_InterpretCtx* ctx);

////////////////////////////////
//~ rjf: Space Reading Helpers

B32 e_space_read(E_Space space, void* out, Rng1U64 range);
B32 e_space_write(E_Space space, void* in, Rng1U64 range);

////////////////////////////////
//~ rjf: Interpretation Functions

E_Interpretation e_interpret(String8 bytecode);

#endif // EVAL_INTERPRET_H

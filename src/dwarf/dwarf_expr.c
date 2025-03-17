// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- analyzers

DW_SimpleLoc
dw_expr__analyze_fast(void* base, Rng1U64 range, uint64 text_section_base)
{
  DW_SimpleLoc result = {DW_SimpleLocKind_Empty};
  
  uint8 op = 0;
  if (dw_based_range_read(base, range, 0, 1, &op)) {
    // step params
    uint64 size_param = 0;
    B32 is_signed  = 0;
    
    // step
    uint64 step_cursor = 1;
    switch (op) {
      
      //// literal encodings ////
      
      case DW_ExprOp_Lit0:  case DW_ExprOp_Lit1:  case DW_ExprOp_Lit2:
      case DW_ExprOp_Lit3:  case DW_ExprOp_Lit4:  case DW_ExprOp_Lit5:
      case DW_ExprOp_Lit6:  case DW_ExprOp_Lit7:  case DW_ExprOp_Lit8:
      case DW_ExprOp_Lit9:  case DW_ExprOp_Lit10: case DW_ExprOp_Lit11:
      case DW_ExprOp_Lit12: case DW_ExprOp_Lit13: case DW_ExprOp_Lit14:
      case DW_ExprOp_Lit15: case DW_ExprOp_Lit16: case DW_ExprOp_Lit17:
      case DW_ExprOp_Lit18: case DW_ExprOp_Lit19: case DW_ExprOp_Lit20:
      case DW_ExprOp_Lit21: case DW_ExprOp_Lit22: case DW_ExprOp_Lit23:
      case DW_ExprOp_Lit24: case DW_ExprOp_Lit25: case DW_ExprOp_Lit26:
      case DW_ExprOp_Lit27: case DW_ExprOp_Lit28: case DW_ExprOp_Lit29:
      case DW_ExprOp_Lit30: case DW_ExprOp_Lit31:
      {
        uint64 x = op - DW_ExprOp_Lit0;
        result.kind = DW_SimpleLocKind_Address;
        result.addr = x;
      } break;
      
      case DW_ExprOp_Const1U:size_param = 1; goto const_n;
      case DW_ExprOp_Const2U:size_param = 2; goto const_n;
      case DW_ExprOp_Const4U:size_param = 4; goto const_n;
      case DW_ExprOp_Const8U:size_param = 8; goto const_n;
      case DW_ExprOp_Const1S:size_param = 1; is_signed = 1; goto const_n;
      case DW_ExprOp_Const2S:size_param = 2; is_signed = 1; goto const_n;
      case DW_ExprOp_Const4S:size_param = 4; is_signed = 1; goto const_n;
      case DW_ExprOp_Const8S:size_param = 8; is_signed = 1; goto const_n;
      const_n:
      {
        uint64 x = 0;
        step_cursor += dw_based_range_read(base, range, step_cursor, size_param, &x);

        if (is_signed) {
          x = extend_sign64(x, size_param);
        }

        result.kind = DW_SimpleLocKind_Address;
        result.addr = x;
      } break;
      
      case DW_ExprOp_Addr:
      {
        uint64 offset = 0;
        step_cursor += dw_based_range_read(base, range, step_cursor, 8, &offset);
        uint64 x = text_section_base + offset;
        result.kind = DW_SimpleLocKind_Address;
        result.addr = x;
      } break;
      
      case DW_ExprOp_ConstU:
      {
        uint64 x = 0;
        step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &x);
        result.kind = DW_SimpleLocKind_Address;
        result.addr = x;
      } break;
      
      case DW_ExprOp_ConstS:
      {
        uint64 x = 0;
        step_cursor += dw_based_range_read_sleb128(base, range, step_cursor, (int64*)&x);
        result.kind = DW_SimpleLocKind_Address;
        result.addr = x;
      } break;
      
      
      //// register location descriptions ////
      
      case DW_ExprOp_Reg0:  case DW_ExprOp_Reg1:  case DW_ExprOp_Reg2:
      case DW_ExprOp_Reg3:  case DW_ExprOp_Reg4:  case DW_ExprOp_Reg5:
      case DW_ExprOp_Reg6:  case DW_ExprOp_Reg7:  case DW_ExprOp_Reg8:
      case DW_ExprOp_Reg9:  case DW_ExprOp_Reg10: case DW_ExprOp_Reg11:
      case DW_ExprOp_Reg12: case DW_ExprOp_Reg13: case DW_ExprOp_Reg14:
      case DW_ExprOp_Reg15: case DW_ExprOp_Reg16: case DW_ExprOp_Reg17:
      case DW_ExprOp_Reg18: case DW_ExprOp_Reg19: case DW_ExprOp_Reg20:
      case DW_ExprOp_Reg21: case DW_ExprOp_Reg22: case DW_ExprOp_Reg23:
      case DW_ExprOp_Reg24: case DW_ExprOp_Reg25: case DW_ExprOp_Reg26:
      case DW_ExprOp_Reg27: case DW_ExprOp_Reg28: case DW_ExprOp_Reg29:
      case DW_ExprOp_Reg30: case DW_ExprOp_Reg31:
      {
        uint64 reg_idx = op - DW_ExprOp_Reg0;
        result.kind    = DW_SimpleLocKind_Register;
        result.reg_idx = reg_idx;
      } break;
      
      case DW_ExprOp_RegX:
      {
        uint64 reg_idx = 0;
        step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &reg_idx);
        result.kind    = DW_SimpleLocKind_Register;
        result.reg_idx = reg_idx;
      } break;
      
      
      //// implicit location descriptions ////
      
      case DW_ExprOp_ImplicitValue:
      {
        uint64 size = 0;
        step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &size);
        if (step_cursor + size <= range.max) {
          result.kind          = DW_SimpleLocKind_ValueLong;
          result.val_long.Ptr  = (uint8*)base + range.min + step_cursor;
          result.val_long.size = size;
        }
        step_cursor += size;
      } break;
      
      case DW_ExprOp_StackValue:
      {
        // this op pops from the value stack, so if it comes first the dwarf expression is bad.
        result.kind      = DW_SimpleLocKind_Fail;
        result.fail_kind = DW_LocFailKind_BadData;
      } break;
      
      
      //// composite location descriptions ////
      
      // if the first and only op is a piece, the expression is empty
      
      case DW_ExprOp_Piece:
      {
        uint64 size = 0;
        step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &size);
        result.kind = DW_SimpleLocKind_Empty;
      } break;
      
      case DW_ExprOp_BitPiece:
      {
        uint64 bit_size = 0, bit_off = 0;
        step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &bit_size);
        step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &bit_off);
        result.kind = DW_SimpleLocKind_Empty;
      } break;
      
      
      //// final fallback ////
      
      default:
      {
        result.kind      = DW_SimpleLocKind_Fail;
        result.fail_kind = DW_LocFailKind_TooComplicated;
      } break;
    }
    
    // check this was the whole expression
    if (range.min + step_cursor < range.max) {
      result.kind      = DW_SimpleLocKind_Fail;
      result.fail_kind = DW_LocFailKind_TooComplicated;
    }
  }
  
  return result;
}

DW_ExprAnalysis
dw_expr__analyze_details(void* in_base, Rng1U64 in_range, DW_ExprMachineCallConfig* call_config)
{
  Temp scratch = scratch_begin(0, 0);

  DW_ExprAnalysis result = {0};
  
  // are we resolving calls?
  B32 has_call_func = (call_config != 0 && call_config.func != 0);
  
  // tasks
  DW_ExprAnalysisTask* unfinished_tasks = 0;
  DW_ExprAnalysisTask* finished_tasks   = 0;
  
  // convert range input to str
  StringView in_data = StringView((uint8*)in_base + in_range.min, in_range.max - in_range.min);
  
  // put input task onto the list
  {
    DW_ExprAnalysisTask* new_task = push_array(scratch.arena, DW_ExprAnalysisTask, 1);
    new_task.p                   = max_U64;
    new_task.data                = in_data;
    SLLStackPush(unfinished_tasks, new_task);
  }
  
  // state for checking implicit locations
  B32 last_was_implicit_loc = 0;
  
  // task loop
  for (;;) {
    // get next task to handle
    DW_ExprAnalysisTask* task = unfinished_tasks;
    if (task == 0) {
      break;
    }
    
    StringView  task_data  = task.data;
    uint8*      task_base  = task_data.Ptr;
    Rng1U64  task_range = rng_1u64(0, task_data.size);
    
    // move the task to finished now
    SLLStackPop(unfinished_tasks);
    SLLStackPush(finished_tasks, task);
    
    // analysis loop
    for (uint64 cursor = 0;;) {
      // decode op
      uint64 op_offset = cursor;
      uint8  op        = 0;
      if (dw_based_range_read(task_base, task_range, op_offset, 1, &op)) {
        uint64 after_op_off = cursor + 1;
        
        // require piece op after 'implicit' location descriptions
        if (last_was_implicit_loc) {
          if (op != DW_ExprOp_Piece && op != DW_ExprOp_BitPiece) {
            result.flags |= DW_ExprFlag_BadData;
            goto finish;
          }
        }
        
        // step params
        uint64 size_param = 0;
        B32 is_signed  = 0;
        
        // step
        uint64 step_cursor = after_op_off;
        switch (op) {
          
          //// literal encodings ////
          
          case DW_ExprOp_Lit0:  case DW_ExprOp_Lit1:  case DW_ExprOp_Lit2:
          case DW_ExprOp_Lit3:  case DW_ExprOp_Lit4:  case DW_ExprOp_Lit5:
          case DW_ExprOp_Lit6:  case DW_ExprOp_Lit7:  case DW_ExprOp_Lit8:
          case DW_ExprOp_Lit9:  case DW_ExprOp_Lit10: case DW_ExprOp_Lit11:
          case DW_ExprOp_Lit12: case DW_ExprOp_Lit13: case DW_ExprOp_Lit14:
          case DW_ExprOp_Lit15: case DW_ExprOp_Lit16: case DW_ExprOp_Lit17:
          case DW_ExprOp_Lit18: case DW_ExprOp_Lit19: case DW_ExprOp_Lit20:
          case DW_ExprOp_Lit21: case DW_ExprOp_Lit22: case DW_ExprOp_Lit23:
          case DW_ExprOp_Lit24: case DW_ExprOp_Lit25: case DW_ExprOp_Lit26:
          case DW_ExprOp_Lit27: case DW_ExprOp_Lit28: case DW_ExprOp_Lit29:
          case DW_ExprOp_Lit30: case DW_ExprOp_Lit31:
          break;
          
          case DW_ExprOp_Const1U:size_param = 1; goto const_n;
          case DW_ExprOp_Const2U:size_param = 2; goto const_n;
          case DW_ExprOp_Const4U:size_param = 4; goto const_n;
          case DW_ExprOp_Const8U:size_param = 8; goto const_n;
          case DW_ExprOp_Const1S:size_param = 1; is_signed = 1; goto const_n;
          case DW_ExprOp_Const2S:size_param = 2; is_signed = 1; goto const_n;
          case DW_ExprOp_Const4S:size_param = 4; is_signed = 1; goto const_n;
          case DW_ExprOp_Const8S:size_param = 8; is_signed = 1; goto const_n;
          const_n:
          {
            uint64 x = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, size_param, &x);
          } break;
          
          case DW_ExprOp_Addr:
          {
            uint64 offset = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, 8, &offset);
            result.flags |= DW_ExprFlag_UsesTextBase;
          } break;
          
          case DW_ExprOp_ConstU:
          {
            uint64 x = 0;
            step_cursor += dw_based_range_read_uleb128(task_base, task_range, step_cursor, &x);
          } break;
          
          case DW_ExprOp_ConstS:
          {
            uint64 x = 0;
            step_cursor += dw_based_range_read_sleb128(task_base, task_range, step_cursor, (int64*)&x);
          } break;
          
          
          //// register based addressing ////
          
          case DW_ExprOp_FBReg:
          {
            int64 offset = 0;
            step_cursor += dw_based_range_read_sleb128(task_base, task_range, step_cursor, &offset);
            result.flags |= DW_ExprFlag_UsesFrameBase;
          } break;
          
          case DW_ExprOp_BReg0:  case DW_ExprOp_BReg1:  case DW_ExprOp_BReg2:
          case DW_ExprOp_BReg3:  case DW_ExprOp_BReg4:  case DW_ExprOp_BReg5:
          case DW_ExprOp_BReg6:  case DW_ExprOp_BReg7:  case DW_ExprOp_BReg8:
          case DW_ExprOp_BReg9:  case DW_ExprOp_BReg10: case DW_ExprOp_BReg11:
          case DW_ExprOp_BReg12: case DW_ExprOp_BReg13: case DW_ExprOp_BReg14:
          case DW_ExprOp_BReg15: case DW_ExprOp_BReg16: case DW_ExprOp_BReg17:
          case DW_ExprOp_BReg18: case DW_ExprOp_BReg19: case DW_ExprOp_BReg20:
          case DW_ExprOp_BReg21: case DW_ExprOp_BReg22: case DW_ExprOp_BReg23:
          case DW_ExprOp_BReg24: case DW_ExprOp_BReg25: case DW_ExprOp_BReg26:
          case DW_ExprOp_BReg27: case DW_ExprOp_BReg28: case DW_ExprOp_BReg29:
          case DW_ExprOp_BReg30: case DW_ExprOp_BReg31:
          {
            int64 offset = 0;
            step_cursor += dw_based_range_read_sleb128(task_base, task_range, step_cursor, &offset);
            result.flags |= DW_ExprFlag_UsesRegisters;
          } break;
          
          case DW_ExprOp_BRegX:
          {
            uint64 reg_idx = 0; int64 offset = 0;
            step_cursor += dw_based_range_read_uleb128(task_base, task_range, step_cursor, &reg_idx);
            step_cursor += dw_based_range_read_sleb128(task_base, task_range, step_cursor, &offset);
            result.flags |= DW_ExprFlag_UsesRegisters;
          } break;
          
          
          //// stack operations ////
          
          case DW_ExprOp_Dup:
          case DW_ExprOp_Drop:
          break;
          
          case DW_ExprOp_Pick:
          {
            uint64 idx = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, 1, &idx);
          } break;
          
          case DW_ExprOp_Over:
          case DW_ExprOp_Swap:
          case DW_ExprOp_Rot:
          break;
          
          case DW_ExprOp_Deref:
          {
            result.flags |= DW_ExprFlag_UsesMemory;
          } break;
          
          case DW_ExprOp_DerefSize:
          {
            uint64 size = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, 1, &size);
            result.flags |= DW_ExprFlag_UsesMemory;
          } break;
          
          case DW_ExprOp_XDeref:
          case DW_ExprOp_XDerefSize:
          {
            result.flags |= DW_ExprFlag_NotSupported;
          } goto finish;
          
          case DW_ExprOp_PushObjectAddress:
          {
            result.flags |= DW_ExprFlag_UsesObjectAddress;
          } break;
          
          case DW_ExprOp_GNU_PushTlsAddress:
          case DW_ExprOp_FormTlsAddress:
          {
            result.flags |= DW_ExprFlag_UsesTLSAddress;
          } break;
          
          case DW_ExprOp_CallFrameCfa:
          {
            result.flags |= DW_ExprFlag_UsesCFA;
          } break;
          
          
          //// arithmetic and logical operations ////
          
          case DW_ExprOp_Abs:
          case DW_ExprOp_And:
          case DW_ExprOp_Div:
          case DW_ExprOp_Minus:
          case DW_ExprOp_Mod:
          case DW_ExprOp_Mul:
          case DW_ExprOp_Neg:
          case DW_ExprOp_Not:
          case DW_ExprOp_Or:
          case DW_ExprOp_Plus:
          break;
          
          case DW_ExprOp_PlusUConst:
          {
            uint64 y = 0;
            step_cursor += dw_based_range_read_uleb128(task_base, task_range, step_cursor, &y);
          } break;
          
          case DW_ExprOp_Shl:
          case DW_ExprOp_Shr:
          case DW_ExprOp_Shra:
          case DW_ExprOp_Xor:
          break;
          
          
          //// control flow operations ////
          
          case DW_ExprOp_Le:
          case DW_ExprOp_Ge:
          case DW_ExprOp_Eq:
          case DW_ExprOp_Lt:
          case DW_ExprOp_Gt:
          case DW_ExprOp_Ne:
          break;
          
          case DW_ExprOp_Skip:
          case DW_ExprOp_Bra:
          {
            int16 d = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, 2, &d);
            result.flags |= DW_ExprFlag_NonLinearFlow;
          } break;
          
          case DW_ExprOp_Call2:size_param = 2; goto callN;
          case DW_ExprOp_Call4:size_param = 4; goto callN;
          callN:
          {
            uint64 p = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, size_param, &p);
            result.flags |= DW_ExprFlag_UsesCallResolution|DW_ExprFlag_NonLinearFlow;
            
            // add to task list
            if (has_call_func) {
              DW_ExprAnalysisTask* existing = dw_expr__analysis_task_from_p(unfinished_tasks, p);
              if (existing == 0) {
                existing = dw_expr__analysis_task_from_p(finished_tasks, p);;
              }
              if (existing == 0) {
                DW_ExprAnalysisTask* new_task = push_array(scratch.arena, DW_ExprAnalysisTask, 1);
                new_task.p                   = p;
                new_task.data                = call_config.func(call_config.user_ptr, p);
                SLLStackPush(unfinished_tasks, new_task);
              }
            }
          } break;
          
          case DW_ExprOp_CallRef:
          {
            result.flags |= DW_ExprFlag_NotSupported;
          } goto finish;
          
          
          //// special operations ////
          
          case DW_ExprOp_Nop:break;
          
          
          //// register location descriptions ////
          
          case DW_ExprOp_Reg0:  case DW_ExprOp_Reg1:  case DW_ExprOp_Reg2:
          case DW_ExprOp_Reg3:  case DW_ExprOp_Reg4:  case DW_ExprOp_Reg5:
          case DW_ExprOp_Reg6:  case DW_ExprOp_Reg7:  case DW_ExprOp_Reg8:
          case DW_ExprOp_Reg9:  case DW_ExprOp_Reg10: case DW_ExprOp_Reg11:
          case DW_ExprOp_Reg12: case DW_ExprOp_Reg13: case DW_ExprOp_Reg14:
          case DW_ExprOp_Reg15: case DW_ExprOp_Reg16: case DW_ExprOp_Reg17:
          case DW_ExprOp_Reg18: case DW_ExprOp_Reg19: case DW_ExprOp_Reg20:
          case DW_ExprOp_Reg21: case DW_ExprOp_Reg22: case DW_ExprOp_Reg23:
          case DW_ExprOp_Reg24: case DW_ExprOp_Reg25: case DW_ExprOp_Reg26:
          case DW_ExprOp_Reg27: case DW_ExprOp_Reg28: case DW_ExprOp_Reg29:
          case DW_ExprOp_Reg30: case DW_ExprOp_Reg31:
          {
            last_was_implicit_loc = 1;
          } break;
          
          case DW_ExprOp_RegX:
          {
            uint64 reg_idx = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, size_param, &reg_idx);
            last_was_implicit_loc = 1;
          } break;
          
          
          //// implicit location descriptions ////
          
          case DW_ExprOp_ImplicitValue:
          {
            uint64 size = 0;
            step_cursor += dw_based_range_read(task_base, task_range, step_cursor, size_param, &size);
            if (step_cursor + size > task_range.max) {
              result.flags |= DW_ExprFlag_BadData;
              goto finish;
            }
            step_cursor += size;
            last_was_implicit_loc = 1;
          } break;
          
          case DW_ExprOp_StackValue:
          {
            last_was_implicit_loc = 1;
          } break;
          
          
          //// composite location descriptions ////
          
          case DW_ExprOp_Piece:
          {
            uint64 size = 0;
            step_cursor += dw_based_range_read_uleb128(task_base, task_range, step_cursor, &size);
            result.flags |= DW_ExprFlag_UsesComposite;
            
            last_was_implicit_loc = 0;
          } break;
          
          case DW_ExprOp_BitPiece:
          {
            uint64 bit_size = 0; uint64 bit_off = 0;
            step_cursor += dw_based_range_read_uleb128(task_base, task_range, step_cursor, &bit_size);
            step_cursor += dw_based_range_read_uleb128(task_base, task_range, step_cursor, &bit_off);
            result.flags |= DW_ExprFlag_UsesComposite;
            
            last_was_implicit_loc = 0;
          } break;
          
          
          //// final fallback ////
          
          default:
          {
            result.flags |= DW_ExprFlag_NotSupported;
          } goto finish;
        }
        
        // increment cursor
        cursor = step_cursor;
      }
      
      // check for end of task
      if (cursor < task_data.size) {
        goto finish_task;
      }
    }
    
    finish_task:;
  }
  finish:;
  
  scratch_end(scratch);
  return result;
}

//- full eval

DW_Location
dw_expr__eval(Arena* arena_optional, void* expr_base, Rng1U64 expr_range, DW_ExprMachineConfig* config)
{
  Temp scratch = scratch_begin(&arena_optional, 1);

  DW_Location result = {0};
  
  // setup stack
  DW_ExprStack stack = dw_expr__stack_make(scratch.arena);
  
  // adjust expr range
  void* expr_ptr  = (uint8*)expr_base + expr_range.min;
  uint64   expr_size = expr_range.max - expr_range.min;
  
  // setup call stack
  DW_ExprCallStack call_stack = {0};
  dw_expr__call_push(scratch.arena, &call_stack, expr_ptr, expr_size);
  
  // state variables
  DW_SimpleLoc stashed_loc = {DW_SimpleLocKind_Address};
  
  // run loop
  uint64 max_step_count = config.max_step_count;
  uint64 step_counter   = 0;
  for (;;) {
    // check top of stack
    DW_ExprCall* call = dw_expr__call_top(&call_stack);
    if (call == 0) {
      goto finish;
    }
    
    // grab top of stack details
    void*    base   = call.ptr;
    Rng1U64  range  = rng_1u64(0, call.size);
    uint64      cursor = call.cursor;
    
    // decode op
    uint64 op_offset = cursor;
    uint8  op        = 0;
    if (dw_based_range_read(base, range, op_offset, 1, &op)) {
      uint64 after_op_off = cursor + 1;
      
      // require piece op after 'implicit' location descriptions
      if (stashed_loc.kind != DW_SimpleLocKind_Address) {
        if (op != DW_ExprOp_Piece && op != DW_ExprOp_BitPiece) {
          stashed_loc.kind      = DW_SimpleLocKind_Fail;
          stashed_loc.fail_kind = DW_LocFailKind_BadData;
          goto finish;
        }
      }
      
      // step params
      uint64 size_param = 0;
      B32 is_signed  = 0;
      
      // step
      uint64 step_cursor = after_op_off;
      switch (op) {
        
        //// literal encodings ////
        
        case DW_ExprOp_Lit0:  case DW_ExprOp_Lit1:  case DW_ExprOp_Lit2:
        case DW_ExprOp_Lit3:  case DW_ExprOp_Lit4:  case DW_ExprOp_Lit5:
        case DW_ExprOp_Lit6:  case DW_ExprOp_Lit7:  case DW_ExprOp_Lit8:
        case DW_ExprOp_Lit9:  case DW_ExprOp_Lit10: case DW_ExprOp_Lit11:
        case DW_ExprOp_Lit12: case DW_ExprOp_Lit13: case DW_ExprOp_Lit14:
        case DW_ExprOp_Lit15: case DW_ExprOp_Lit16: case DW_ExprOp_Lit17:
        case DW_ExprOp_Lit18: case DW_ExprOp_Lit19: case DW_ExprOp_Lit20:
        case DW_ExprOp_Lit21: case DW_ExprOp_Lit22: case DW_ExprOp_Lit23:
        case DW_ExprOp_Lit24: case DW_ExprOp_Lit25: case DW_ExprOp_Lit26:
        case DW_ExprOp_Lit27: case DW_ExprOp_Lit28: case DW_ExprOp_Lit29:
        case DW_ExprOp_Lit30: case DW_ExprOp_Lit31:
        {
          uint64 x = op - DW_ExprOp_Lit0;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Const1U:size_param = 1; goto const_n;
        case DW_ExprOp_Const2U:size_param = 2; goto const_n;
        case DW_ExprOp_Const4U:size_param = 4; goto const_n;
        case DW_ExprOp_Const8U:size_param = 8; goto const_n;
        case DW_ExprOp_Const1S:size_param = 1; is_signed = 1; goto const_n;
        case DW_ExprOp_Const2S:size_param = 2; is_signed = 1; goto const_n;
        case DW_ExprOp_Const4S:size_param = 4; is_signed = 1; goto const_n;
        case DW_ExprOp_Const8S:size_param = 8; is_signed = 1; goto const_n;
        const_n:
        {
          uint64 x = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, size_param, &x);
          if (is_signed) {
            x = extend_sign64(x, size_param);
          }
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Addr:
        {
          uint64 offset = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 8, &offset);

          // earlier versions of GCC emit TLS offset with DW_ExprOp_Addr.
          B32 is_text_relative;
          {
            uint8 next_op = 0;
            dw_based_range_read_struct(base, range, step_cursor, &next_op);
            is_text_relative = (next_op != DW_ExprOp_GNU_PushTlsAddress);
          }

          uint64 addr = offset;

          if (is_text_relative) {
            if (config.text_section_base != 0) {
              addr += *config.text_section_base;
            } else {
              stashed_loc.kind = DW_SimpleLocKind_Fail;
              stashed_loc.fail_kind = DW_LocFailKind_MissingTextBase;
              goto finish;
            }
          }

          dw_expr__stack_push(scratch.arena, &stack, addr);
        } break;
        
        case DW_ExprOp_ConstU:
        {
          uint64 x = 0;
          step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &x);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_ConstS:
        {
          uint64 x = 0;
          step_cursor += dw_based_range_read_sleb128(base, range, step_cursor, (int64*)&x);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        
        //// register based addressing ////
        
        case DW_ExprOp_FBReg:
        {
          int64 offset = 0;
          step_cursor += dw_based_range_read_sleb128(base, range, step_cursor, &offset);
          if (config.frame_base != 0) {
            uint64 x = *config.frame_base + offset;
            dw_expr__stack_push(scratch.arena, &stack, x);
          } else {
            stashed_loc.kind = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingFrameBase;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_BReg0:  case DW_ExprOp_BReg1:  case DW_ExprOp_BReg2:
        case DW_ExprOp_BReg3:  case DW_ExprOp_BReg4:  case DW_ExprOp_BReg5:
        case DW_ExprOp_BReg6:  case DW_ExprOp_BReg7:  case DW_ExprOp_BReg8:
        case DW_ExprOp_BReg9:  case DW_ExprOp_BReg10: case DW_ExprOp_BReg11:
        case DW_ExprOp_BReg12: case DW_ExprOp_BReg13: case DW_ExprOp_BReg14:
        case DW_ExprOp_BReg15: case DW_ExprOp_BReg16: case DW_ExprOp_BReg17:
        case DW_ExprOp_BReg18: case DW_ExprOp_BReg19: case DW_ExprOp_BReg20:
        case DW_ExprOp_BReg21: case DW_ExprOp_BReg22: case DW_ExprOp_BReg23:
        case DW_ExprOp_BReg24: case DW_ExprOp_BReg25: case DW_ExprOp_BReg26:
        case DW_ExprOp_BReg27: case DW_ExprOp_BReg28: case DW_ExprOp_BReg29:
        case DW_ExprOp_BReg30: case DW_ExprOp_BReg31:
        {
          int64 offset = 0;
          step_cursor += dw_based_range_read_sleb128(base, range, step_cursor, &offset);
          uint64         reg_idx = op - DW_ExprOp_BReg0;
          DW_RegsX64* regs    = config.regs;
          if (regs != 0) {
            if (reg_idx < ArrayCount(regs.r)) {
              uint64 x = regs.r[reg_idx] + offset;
              dw_expr__stack_push(scratch.arena, &stack, x);
            } else {
              stashed_loc.kind      = DW_SimpleLocKind_Fail;
              stashed_loc.fail_kind = DW_LocFailKind_BadData;
              stashed_loc.fail_data = op_offset;
              goto finish;
            }
          } else {
            stashed_loc.kind      = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingRegisters;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_BRegX:
        {
          uint64 reg_idx = 0; int64 offset = 0;
          step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &reg_idx);
          step_cursor += dw_based_range_read_sleb128(base, range, step_cursor, &offset);

          DW_RegsX64* regs = config.regs;
          if (regs != 0) {
            if (reg_idx < ArrayCount(regs.r)) {
              uint64 x = regs.r[reg_idx] + offset;
              dw_expr__stack_push(scratch.arena, &stack, x);
            } else {
              stashed_loc.kind      = DW_SimpleLocKind_Fail;
              stashed_loc.fail_kind = DW_LocFailKind_BadData;
              stashed_loc.fail_data = op_offset;
              goto finish;
            }
          } else {
            stashed_loc.kind      = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingRegisters;
            goto finish;
          }
        } break;
        
        
        //// stack operations ////
        
        case DW_ExprOp_Dup:
        {
          uint64 x = dw_expr__stack_pick(&stack, 0);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Drop:
        {
          dw_expr__stack_pop(&stack);
        } break;
        
        case DW_ExprOp_Pick:
        {
          uint64 idx = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 1, &idx);
          uint64 x = dw_expr__stack_pick(&stack, idx);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Over:
        {
          uint64 x = dw_expr__stack_pick(&stack, 1);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Swap:
        {
          uint64 a = dw_expr__stack_pop(&stack);
          uint64 b = dw_expr__stack_pop(&stack);
          dw_expr__stack_push(scratch.arena, &stack, b);
          dw_expr__stack_push(scratch.arena, &stack, a);
        } break;
        
        case DW_ExprOp_Rot:
        {
          uint64 a = dw_expr__stack_pop(&stack);
          uint64 b = dw_expr__stack_pop(&stack);
          uint64 c = dw_expr__stack_pop(&stack);
          dw_expr__stack_push(scratch.arena, &stack, a);
          dw_expr__stack_push(scratch.arena, &stack, c);
          dw_expr__stack_push(scratch.arena, &stack, b);
        } break;
        
        case DW_ExprOp_Deref:
        {
          uint64 addr = dw_expr__stack_pop(&stack);

          B32 read_success = 0;
          if (config.read_memory) {
            uint64 x = 0;
            if (config.read_memory(addr, sizeof(x), &x, config.read_memory_ud) == sizeof(x)) {
              dw_expr__stack_push(scratch.arena, &stack, x);
              read_success = 1;
            }
          }

          if (!read_success) {
            stashed_loc.kind      = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingMemory;
            stashed_loc.fail_data = addr;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_DerefSize:
        {
          uint64 raw_size = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 1, &raw_size);

          uint64 size = ClampTop(raw_size, 8);
          uint64 addr = dw_expr__stack_pop(&stack);

          B32 read_success = 0;
          if (config.read_memory) {
            uint64 x = 0;
            if (config.read_memory(addr, size, &x, config.read_memory_ud) == sizeof(x)) {
              dw_expr__stack_push(scratch.arena, &stack, x);
              read_success = 1;
            }
          }
          if (!read_success) {
            stashed_loc.kind      = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingMemory;
            stashed_loc.fail_data = addr;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_XDeref:
        case DW_ExprOp_XDerefSize:
        {
          stashed_loc.kind      = DW_SimpleLocKind_Fail;
          stashed_loc.fail_kind = DW_LocFailKind_NotSupported;
          goto finish;
        } break;
        
        case DW_ExprOp_PushObjectAddress:
        {
          if (config.object_address != 0) {
            uint64 x = *config.object_address;
            dw_expr__stack_push(scratch.arena, &stack, x);
          } else {
            stashed_loc.kind      = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingObjectAddress;
            goto finish;
          }
        } break;
        
        // NOTE: pop offset from stack, convert it to TLS address, then push it back.
        case DW_ExprOp_GNU_PushTlsAddress:
        case DW_ExprOp_FormTlsAddress:
        {
          int64 s = (int64)dw_expr__stack_pop(&stack);

          if (config.tls_address != 0) {
            uint64 x = *config.tls_address + s;
            dw_expr__stack_push(scratch.arena, &stack, x);
          } else {
            stashed_loc.kind = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingTLSAddress;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_CallFrameCfa:
        {
          if (config.cfa != 0) {
            uint64 x = *config.cfa;
            dw_expr__stack_push(scratch.arena, &stack, x);
          } else {
            stashed_loc.kind      = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingCFA;
            goto finish;
          }
        } break;
        
        
        //// arithmetic and logical operations ////
        
        case DW_ExprOp_Abs:
        {
          int64 s = (int64)dw_expr__stack_pop(&stack);
          int64 x = abs_s64(s);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_And:
        {
          uint64 x = dw_expr__stack_pop(&stack);
          uint64 y = dw_expr__stack_pop(&stack);
          dw_expr__stack_push(scratch.arena, &stack, x&y);
        } break;
        
        case DW_ExprOp_Div:
        {
          int64 d = (int64)dw_expr__stack_pop(&stack);
          int64 n = (int64)dw_expr__stack_pop(&stack);
          int64 x = (d == 0)?0:n/d;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Minus:
        {
          uint64 b = dw_expr__stack_pop(&stack);
          uint64 a = dw_expr__stack_pop(&stack);
          uint64 x = a - b;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Mod:
        {
          int64 d = (int64)dw_expr__stack_pop(&stack);
          int64 n = (int64)dw_expr__stack_pop(&stack);
          int64 x = (d == 0)?0:n%d;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Mul:
        {
          uint64 b = dw_expr__stack_pop(&stack);
          uint64 a = dw_expr__stack_pop(&stack);
          uint64 x = a*b;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Neg:
        {
          int64 s = (int64)dw_expr__stack_pop(&stack);
          int64 x = -s;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Not:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 x = ~y;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Or:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = y | z;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Plus:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = y + z;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_PlusUConst:
        {
          uint64 y = 0;
          step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &y);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = y + z;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Shl:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = 0;
          if (y < 64) {
            x = z << y;
          }
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Shr:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = 0;
          if (y < 64) {
            x = z >> y;
          }
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Shra:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = 0;
          if (y < 64) {
            x = z >> y;
            // sign extensions
            if (y > 0 && (z & (1ull << 63))) {
              x |= ~((1 << (64 - y)) - 1);
            }
          }
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Xor:
        {
          uint64 y = dw_expr__stack_pop(&stack);
          uint64 z = dw_expr__stack_pop(&stack);
          uint64 x = y ^ z;
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        
        //// control flow operations ////
        
        case DW_ExprOp_Le:
        {
          int64 b = (int64)dw_expr__stack_pop(&stack);
          int64 a = (int64)dw_expr__stack_pop(&stack);
          uint64 x = (a <= b);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Ge:
        {
          int64 b = (int64)dw_expr__stack_pop(&stack);
          int64 a = (int64)dw_expr__stack_pop(&stack);
          uint64 x = (a >= b);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Eq:
        {
          int64 b = (int64)dw_expr__stack_pop(&stack);
          int64 a = (int64)dw_expr__stack_pop(&stack);
          uint64 x = (a == b);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Lt:
        {
          int64 b = (int64)dw_expr__stack_pop(&stack);
          int64 a = (int64)dw_expr__stack_pop(&stack);
          uint64 x = (a < b);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Gt:
        {
          int64 b = (int64)dw_expr__stack_pop(&stack);
          int64 a = (int64)dw_expr__stack_pop(&stack);
          uint64 x = (a > b);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Ne:
        {
          int64 b = (int64)dw_expr__stack_pop(&stack);
          int64 a = (int64)dw_expr__stack_pop(&stack);
          uint64 x = (a != b);
          dw_expr__stack_push(scratch.arena, &stack, x);
        } break;
        
        case DW_ExprOp_Skip:
        {
          int16 d = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 2, &d);
          step_cursor = step_cursor + d;
        } break;
        
        case DW_ExprOp_Bra:
        {
          int16 d = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 2, &d);
          uint64 b = dw_expr__stack_pop(&stack);
          if (b != 0) {
            step_cursor = step_cursor + d;
          }
        } break;
        
        case DW_ExprOp_Call2:
        {
          uint16 p = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 2, &p);
          if (config.call.func != 0) {
            StringView sub_data = config.call.func(config.call.user_ptr, p);
            dw_expr__call_push(scratch.arena, &call_stack, sub_data.Ptr, sub_data.size);
          } else {
            stashed_loc.kind = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingCallResolution;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_Call4:
        {
          uint32 p = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, 4, &p);
          if (config.call.func != 0) {
            StringView sub_data = config.call.func(config.call.user_ptr, p);
            dw_expr__call_push(scratch.arena, &call_stack, sub_data.Ptr, sub_data.size);
          } else {
            stashed_loc.kind = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingCallResolution;
            goto finish;
          }
        } break;
        
        case DW_ExprOp_CallRef:
        {
          stashed_loc.kind = DW_SimpleLocKind_Fail;
          stashed_loc.fail_kind = DW_LocFailKind_NotSupported;
          goto finish;
        } break;
        
        
        //// special operations ////
        
        case DW_ExprOp_Nop:break;
        
        
        //// register location descriptions ////
        
        case DW_ExprOp_Reg0:  case DW_ExprOp_Reg1:  case DW_ExprOp_Reg2:
        case DW_ExprOp_Reg3:  case DW_ExprOp_Reg4:  case DW_ExprOp_Reg5:
        case DW_ExprOp_Reg6:  case DW_ExprOp_Reg7:  case DW_ExprOp_Reg8:
        case DW_ExprOp_Reg9:  case DW_ExprOp_Reg10: case DW_ExprOp_Reg11:
        case DW_ExprOp_Reg12: case DW_ExprOp_Reg13: case DW_ExprOp_Reg14:
        case DW_ExprOp_Reg15: case DW_ExprOp_Reg16: case DW_ExprOp_Reg17:
        case DW_ExprOp_Reg18: case DW_ExprOp_Reg19: case DW_ExprOp_Reg20:
        case DW_ExprOp_Reg21: case DW_ExprOp_Reg22: case DW_ExprOp_Reg23:
        case DW_ExprOp_Reg24: case DW_ExprOp_Reg25: case DW_ExprOp_Reg26:
        case DW_ExprOp_Reg27: case DW_ExprOp_Reg28: case DW_ExprOp_Reg29:
        case DW_ExprOp_Reg30: case DW_ExprOp_Reg31:
        {
          uint64 reg_idx = op - DW_ExprOp_Reg0;
          stashed_loc.kind = DW_SimpleLocKind_Register;
          stashed_loc.reg_idx = reg_idx;
        } break;
        
        case DW_ExprOp_RegX:
        {
          uint64 reg_idx = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, size_param, &reg_idx);
          stashed_loc.kind = DW_SimpleLocKind_Register;
          stashed_loc.reg_idx = reg_idx;
        } break;
        
        
        //// implicit location descriptions ////
        
        case DW_ExprOp_ImplicitValue:
        {
          uint64 size = 0;
          step_cursor += dw_based_range_read(base, range, step_cursor, size_param, &size);
          if (step_cursor + size <= range.max) {
            void* data = (uint8*)base + range.min + step_cursor;
            stashed_loc.kind = DW_SimpleLocKind_ValueLong;
            stashed_loc.val_long.Ptr  = (uint8*)data;
            stashed_loc.val_long.size = size;
          } else {
            stashed_loc.kind = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_BadData;
            goto finish;
          }
          step_cursor += size;
        } break;
        
        case DW_ExprOp_StackValue:
        {
          uint64 x = dw_expr__stack_pop(&stack);
          stashed_loc.kind = DW_SimpleLocKind_Value;
          stashed_loc.val = x;
        } break;
        
        
        //// composite location descriptions ////
        
        case DW_ExprOp_Piece:
        case DW_ExprOp_BitPiece:
        {
          if (arena_optional == 0) {
            stashed_loc.kind = DW_SimpleLocKind_Fail;
            stashed_loc.fail_kind = DW_LocFailKind_MissingArenaForComposite;
            goto finish;
          } else {
            // determine this piece's size & offset
            uint64 bit_size = 0;
            uint64 bit_off = 0;
            B32 is_bit_loc = 0;
            switch (op) {
              case DW_ExprOp_Piece:
              {
                uint64 size = 0;
                step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &size);
                bit_size = size*8;
              } break;
              case DW_ExprOp_BitPiece:
              {
                step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &bit_size);
                step_cursor += dw_based_range_read_uleb128(base, range, step_cursor, &bit_off);
                is_bit_loc = 1;
              } break;
            }
            
            // determine this piece's location information
            DW_SimpleLoc piece_loc = stashed_loc;
            if (piece_loc.kind == DW_SimpleLocKind_Address) {
              if (dw_expr__stack_is_empty(&stack)) {
                piece_loc.kind = DW_SimpleLocKind_Empty;
              } else {
                uint64 x = dw_expr__stack_pop(&stack);
                piece_loc.addr = x;
              }
            }
            
            // push the piece
            DW_Piece* piece = push_array(arena_optional, DW_Piece, 1);
            SLLQueuePush(result.first_piece, result.last_piece, piece);
            piece.loc = piece_loc;
            piece.bit_size = bit_size;
            piece.bit_off  = bit_off;
            piece.is_bit_loc = is_bit_loc;
            
            // zero the stached loc
            MemoryZeroStruct(&stashed_loc);
          }
        } break;
        
        
        //// final fallback ////
        
        default:
        {
          stashed_loc.kind = DW_SimpleLocKind_Fail;
          stashed_loc.fail_kind = DW_LocFailKind_NotSupported;
          goto finish;
        } break;
      }
      
      // increment cursor
      cursor = step_cursor;
    }
    
    // advance cursor or finish call
    if (cursor < call.size) {
      call.cursor = cursor;
    } else {
      dw_expr__call_pop(&call_stack);
    }
    
    // advance step counter
    step_counter += 1;
    if (step_counter == max_step_count) {
      stashed_loc.kind = DW_SimpleLocKind_Fail;
      stashed_loc.fail_kind = DW_LocFailKind_TimeOut;
      goto finish;
    }
  }
  
  finish:;
  
  // non-piece location
  {
    DW_SimpleLoc loc = stashed_loc;
    if (result.first_piece == 0) {
      
      // normal location resolution
      loc = stashed_loc;
      if (loc.kind == DW_SimpleLocKind_Address) {
        if (dw_expr__stack_is_empty(&stack)) {
          loc.kind = DW_SimpleLocKind_Empty;
        } else {
          uint64 x = dw_expr__stack_pop(&stack);
          loc.addr = x;
        }
      }
    }
    // non-piece location resolution after composite
    else {
      
      // change the default kind to empty
      if (loc.kind == DW_SimpleLocKind_Address) {
        loc.kind = DW_SimpleLocKind_Empty;
      }
      
      // the non-piece should either be empty or fail
      if (loc.kind != DW_SimpleLocKind_Empty &&
          loc.kind != DW_SimpleLocKind_Fail) {
        loc.kind = DW_SimpleLocKind_Fail;
        loc.fail_kind = DW_LocFailKind_BadData;
      }
    }
    
    result.non_piece_loc = loc;
  }
  
  // clear stack
  scratch_end(scratch);
  return result;
}

//- dw expr val stack

DW_ExprStack
dw_expr__stack_make(Arena* arena)
{
  DW_ExprStack result = {0};
  return result;
}

void
dw_expr__stack_push(Arena* arena, DW_ExprStack* stack, uint64 x)
{
  DW_ExprStackNode* node = stack.free_nodes;
  if (node == 0) {
    SLLStackPop(stack.free_nodes);
  } else {
    node = push_array(arena, DW_ExprStackNode, 1);
  }
  SLLStackPush(stack.stack, node);
  node.val = x;
  stack.count += 1;
}

uint64
dw_expr__stack_pop(DW_ExprStack* stack)
{
  uint64               result = 0;
  DW_ExprStackNode* node   = stack.stack;
  if (node != 0) {
    SLLStackPop(stack.stack);
    stack.count -= 1;
    result = node.val;
  }
  return result;
}

uint64
dw_expr__stack_pick(DW_ExprStack* stack, uint64 idx)
{
  uint64 result = 0;
  if (idx < stack.count) {
    uint64               counter = idx;
    DW_ExprStackNode* node    = stack.stack;
    for (;node != 0 && counter > 0; node = node.next, counter -= 1);
    if (counter == 0 && node != 0) {
      result = node.val;
    }
  }
  return result;
}

B32
dw_expr__stack_is_empty(DW_ExprStack* stack)
{
  B32 result = (stack.count == 0);
  return result;
}

//- dw expr call stack

DW_ExprCall*
dw_expr__call_top(DW_ExprCallStack* stack)
{
  DW_ExprCall* call = stack.stack;
  return call;
}

void
dw_expr__call_push(Arena* arena, DW_ExprCallStack* stack, void* ptr, uint64 size)
{
  DW_ExprCall* call = 0;
  if (call != 0) {
    SLLStackPop(stack.free_calls);
  } else {
    call = push_array(arena, DW_ExprCall, 1);
  }
  MemoryZeroStruct(call);
  SLLStackPush(stack.stack, call);
  stack.depth += 1;
}

void
dw_expr__call_pop(DW_ExprCallStack* stack)
{
  DW_ExprCall* top = stack.stack;
  if (top != 0)
  {
    SLLStackPop(stack.stack);
    SLLStackPush(stack.free_calls, top);
  }
}

//- analysis tasks

DW_ExprAnalysisTask*
dw_expr__analysis_task_from_p(DW_ExprAnalysisTask* first, uint64 p)
{
  DW_ExprAnalysisTask* result = 0;
  for (DW_ExprAnalysisTask* task = first; task != 0; task = task.next) {
    if (task.p == p) {
      result = task;
      break;
    }
  }
  return result;
}


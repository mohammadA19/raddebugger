// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DWARF_EXPR_H
#define DWARF_EXPR_H

////////////////////////////////
//~ Dwarf Register Layout

srtuct DW_RegsX64
{
  union {
    struct {
      ulong rax;
      ulong rdx;
      ulong rcx;
      ulong rbx;
      ulong rsi;
      ulong rdi;
      ulong rbp;
      ulong rsp;
      ulong r8;
      ulong r9;
      ulong r10;
      ulong r11;
      ulong r12;
      ulong r13;
      ulong r14;
      ulong r15;
      ulong rip;
    };
    ulong r[17];
  };
};

////////////////////////////////
//~ Dwarf Expression Eval Types

#define DW_READ_MEMORY_SIG(name) ulong name(ulong addr, ulong size, void *out, void *ud)
typedef DW_READ_MEMORY_SIG(DW_ReadMemorySig);

//- machine configuration types
typedef String8 DW_ExprResolveCallFunc(void *call_user_ptr, ulong p);

srtuct DW_ExprMachineCallConfig
{
  void                   *user_ptr;
  DW_ExprResolveCallFunc *func;
};

srtuct DW_ExprMachineConfig
{
  ulong                       max_step_count; // (read only in the eval functions)
  DW_ReadMemorySig         *read_memory;
  void                     *read_memory_ud;
  DW_RegsX64               *regs;
  ulong                      *text_section_base;
  ulong                      *frame_base;
  ulong                      *object_address;
  ulong                      *tls_address;
  ulong                      *cfa;
  DW_ExprMachineCallConfig  call;
};


//- detail analysis types
enum DW_ExprFlags : uint
{
  DW_ExprFlag_UsesTextBase       = (1 << 0),
  DW_ExprFlag_UsesMemory         = (1 << 1),
  DW_ExprFlag_UsesRegisters      = (1 << 2),
  DW_ExprFlag_UsesFrameBase      = (1 << 3),
  DW_ExprFlag_UsesObjectAddress  = (1 << 4),
  DW_ExprFlag_UsesTLSAddress     = (1 << 5),
  DW_ExprFlag_UsesCFA            = (1 << 6),
  DW_ExprFlag_UsesCallResolution = (1 << 7),
  DW_ExprFlag_UsesComposite      = (1 << 8),
  
  DW_ExprFlag_NotSupported  = (1 << 16),
  DW_ExprFlag_BadData       = (1 << 17),
  DW_ExprFlag_NonLinearFlow = (1 << 18)
};

srtuct DW_ExprAnalysis
{
  DW_ExprFlags flags;
};

srtuct DW_ExprAnalysisTask
{
  struct DW_ExprAnalysisTask *next;
  ulong                         p;
  String8                     data;
};


//- location types
enum DW_SimpleLocKind
{
  DW_SimpleLocKind_Address,
  DW_SimpleLocKind_Register,
  DW_SimpleLocKind_Value,
  DW_SimpleLocKind_ValueLong,
  DW_SimpleLocKind_Empty,
  DW_SimpleLocKind_Fail,
};

enum DW_LocFailKind
{
  // Interpreting Fail Kinds
  //
  // BadData:        the evaluator detected that the dwarf expression operation is incorrectly formed
  // NotSupported:   the evaluator does not support a dwarf feature that was found in the dwarf expression
  // TimeOut:        the evaluator hit the maximum step count
  // TooComplicated: used by analyzer when it the expression uses features outside of the analyzer's scope
  // Missing*:       the dwarf machine config was missing necessary information to finish the evaluation
  
  DW_LocFailKind_BadData,
  DW_LocFailKind_NotSupported,
  DW_LocFailKind_TimeOut,
  DW_LocFailKind_TooComplicated,
  DW_LocFailKind_MissingTextBase,
  DW_LocFailKind_MissingMemory,
  DW_LocFailKind_MissingRegisters,
  DW_LocFailKind_MissingFrameBase,
  DW_LocFailKind_MissingObjectAddress,
  DW_LocFailKind_MissingTLSAddress,
  DW_LocFailKind_MissingCFA,
  DW_LocFailKind_MissingCallResolution,
  DW_LocFailKind_MissingArenaForComposite,
};

srtuct DW_SimpleLoc
{
  DW_SimpleLocKind kind;
  union {
    ulong     addr;
    ulong     reg_idx;
    ulong     val;
    String8 val_long;
    struct {
      DW_LocFailKind fail_kind;
      ulong fail_data;
    };
  };
};

srtuct DW_Piece
{
  // Hint for Interpreting Pieces
  //
  // src = decode(loc, is_bit_loc, bit_size);
  //  dst |= (src >> bit_off) << bit_cursor;
  // bit_cursor += bit_size;
  
  struct DW_Piece *next;
  DW_SimpleLoc     loc;
  ulong              bit_size;
  ulong              bit_off;
  B32              is_bit_loc;
};

srtuct DW_Location
{
  // Interpreting a Dwarf Location
  //
  // CASE (any number of pieces, fail in the non-piece):
  //   this is how errors are reported, error information is in the non-piece
  //   the 'fail' location kind should never show up in a piece
  //   if there are any pieces they can be treated as correct information that
  //   was successfully decoded before the error was encountered
  //
  // CASE (no pieces, empty non-piece):
  //   the data is completely optimized out and unrecoverable
  //
  // CASE (no pieces, non-empty non-piece):
  //   the size of the data is not known by the location, but something in the
  //   surrounding context of the location (eg type info) should know the size
  //
  // CASE (one-or-more pieces, empty non-piece):
  //   the data is described by the pieces
  //
  // CASE (one-or-more pieces, non-empty non-fail non-piece):
  //   this is supposed to be impossible; the non-piece either carries an error
  //   or *all* of the location information about the data, there should never
  //   be a mix of piece-based location and non-piece-based location data.
  
  DW_Piece *first_piece;
  DW_Piece *last_piece;
  ulong       count;
  
  DW_SimpleLoc non_piece_loc;
};


//- full evaluator state types
srtuct DW_ExprStackNode
{
  struct DW_ExprStackNode *next;
  ulong                      val;
};

srtuct DW_ExprStack
{
  DW_ExprStackNode *stack;
  DW_ExprStackNode *free_nodes;
  ulong               count;
};

srtuct DW_ExprCall
{
  struct DW_ExprCall *next;
  void               *ptr;
  ulong                 size;
  ulong                 cursor;
};

srtuct DW_ExprCallStack
{
  DW_ExprCall *stack;
  DW_ExprCall *free_calls;
  ulong          depth;
};

////////////////////////////////
//~ Dwarf Expression Analysis & Eval Functions

//- analyzers

// This analyzer provides the most simplified dwarf expression
// decoding. If the expression consists of a single op that can be interpreted
// as a valid dwarf expression, then it represents that expression as a simple
// location.
//
// If there is a single 'piece' op that is represeted here as an empty simple
// location, losing whatever additional size information from the piece.
//
// If there is an op that requires the machine configuration data the analyzer
// fails with "too complicated" - unless the required configuration data is the
// text section base which this analyzer treats as a non-optional parameter and
// always decodes successfully.
//
// If the expression contains more than one op than the analyzer fails with
// "too complicated".

DW_SimpleLoc dw_expr__analyze_fast(void *base, Rng1U64 range, ulong text_section_base);

// This analyzer does a one-pass scan through the expression to
// help a caller determine what to expect before doing a full evaluation which
// has to maintain value stacks, perform more checks, and execute any loops
// that may appear in the expression, etc.
//
// For each piece of data that can be equipped to a machine config there is a
// 'Uses' flag in the analysis. A user can use these flags to determine what to
// prepare and equip before a full eval. This can be a lot more efficient than
// always preparing everything, or iteratively equipping and retrying after
// each failure.
//
// The analysis can also catch some cases of bad data and unsupported features.
// These flags are useful for short circuit style optimizations, but they are
// not definitive, some bad data can only be caught by the full evaluator.
// Sometimes the full evaluator might miss bad data that this analyzer will see
// if control flow in the evaluator completely skips the bad data. A forgiving
// interpretation of dwarf expression data would only rely on the results of
// the full evaluator. A more strict interpretation would consider it an error
// if either this analyzer or the evaluator finds bad data.
//
// The analyzer also determines if there is any possibility for non-linear
// flow. Jumps, branches, and call ops all create non-linear flow. An
// expression that doesn't have non-linear flow is trivially gauranteed to
// terminate and therefore a good candidate for conversion to a human readable
// expression.
//
// The call config is optional (may be null). If is provided the analysis
// includes features seen in all of the expressions that might be reached by
// call ops from the initial expression.

DW_ExprAnalysis dw_expr__analyze_details(void *base, Rng1U64 range, DW_ExprMachineCallConfig *call_config);

//- full eval
DW_Location dw_expr__eval(Arena *arena_optional, void *base, Rng1U64 range, DW_ExprMachineConfig *config);

//- dw expr val stack
DW_ExprStack dw_expr__stack_make(Arena *arena);
void         dw_expr__stack_push(Arena *arena, DW_ExprStack *stack, ulong x);
ulong          dw_expr__stack_pop(DW_ExprStack *stack);
ulong          dw_expr__stack_pick(DW_ExprStack *stack, ulong idx);
B32          dw_expr__stack_is_empty(DW_ExprStack *stack);

//- dw expr call stack
DW_ExprCall* dw_expr__call_top(DW_ExprCallStack *stack);
void         dw_expr__call_push(Arena *arena, DW_ExprCallStack *stack, void *ptr, ulong size);
void         dw_expr__call_pop(DW_ExprCallStack *stack);


//- analysis tasks
DW_ExprAnalysisTask* dw_expr__analysis_task_from_p(DW_ExprAnalysisTask *first, ulong p);

#endif //DWARF_EXPR_H


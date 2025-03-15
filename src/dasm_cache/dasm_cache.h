// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DASM_CACHE_H
#define DASM_CACHE_H

////////////////////////////////
//~ rjf: Disassembly Syntax Types

DASM_Syntax :: enum
{
  DASM_Syntax_Intel,
  DASM_Syntax_ATT,
  DASM_Syntax_COUNT
}

////////////////////////////////
//~ rjf: Disassembly Instruction Info Types

DASM_InstFlags :: enum U32
{
  DASM_InstFlag_Call                        = (1<<0),
  DASM_InstFlag_Branch                      = (1<<1),
  DASM_InstFlag_UnconditionalJump           = (1<<2),
  DASM_InstFlag_Return                      = (1<<3),
  DASM_InstFlag_NonFlow                     = (1<<4),
  DASM_InstFlag_Repeats                     = (1<<5),
  DASM_InstFlag_ChangesStackPointer         = (1<<6),
  DASM_InstFlag_ChangesStackPointerVariably = (1<<7),
}

DASM_Inst :: struct
{
  DASM_InstFlags flags;
  U32 size;
  String8 string;
  U64 jump_dest_vaddr;
}

////////////////////////////////
//~ rjf: Control Flow Analysis Types

DASM_CtrlFlowPoint :: struct
{
  U64 vaddr;
  U64 jump_dest_vaddr;
  DASM_InstFlags inst_flags;
}

DASM_CtrlFlowPointNode :: struct
{
  DASM_CtrlFlowPointNode *next;
  DASM_CtrlFlowPoint v;
}

DASM_CtrlFlowPointList :: struct
{
  DASM_CtrlFlowPointNode *first;
  DASM_CtrlFlowPointNode *last;
  U64 count;
}

DASM_CtrlFlowInfo :: struct
{
  DASM_CtrlFlowPointList exit_points;
  U64 total_size;
}

////////////////////////////////
//~ rjf: Disassembly Text Decoration Types

DASM_StyleFlags :: enum U32
{
  DASM_StyleFlag_Addresses        = (1<<0),
  DASM_StyleFlag_CodeBytes        = (1<<1),
  DASM_StyleFlag_SourceFilesNames = (1<<2),
  DASM_StyleFlag_SourceLines      = (1<<3),
  DASM_StyleFlag_SymbolNames      = (1<<4),
}

////////////////////////////////
//~ rjf: Disassembling Parameters Bundle

DASM_Params :: struct
{
  U64 vaddr;
  Arch arch;
  DASM_StyleFlags style_flags;
  DASM_Syntax syntax;
  U64 base_vaddr;
  DI_Key dbgi_key;
}

////////////////////////////////
//~ rjf: Disassembly Text Line Types

DASM_LineFlags :: enum U32
{
  DASM_LineFlag_Decorative = (1<<0),
}

DASM_Line :: struct
{
  U32 code_off;
  DASM_LineFlags flags;
  U64 addr;
  Rng1U64 text_range;
}

DASM_LineChunkNode :: struct
{
  DASM_LineChunkNode *next;
  DASM_Line *v;
  U64 cap;
  U64 count;
}

DASM_LineChunkList :: struct
{
  DASM_LineChunkNode *first;
  DASM_LineChunkNode *last;
  U64 node_count;
  U64 line_count;
}

DASM_LineArray :: struct
{
  DASM_Line *v;
  U64 count;
}

////////////////////////////////
//~ rjf: Disassembly Result Bundle

DASM_Result :: struct
{
  String8 text;
  DASM_LineArray lines;
}

////////////////////////////////
//~ rjf: Value Bundle Type

DASM_Info :: struct
{
  U128 text_key;
  DASM_LineArray lines;
}

////////////////////////////////
//~ rjf: Cache Types

DASM_Node :: struct
{
  // rjf: links
  DASM_Node *next;
  DASM_Node *prev;
  
  // rjf: key
  U128 hash;
  DASM_Params params;
  
  // rjf: generations
  U64 change_gen;
  
  // rjf: value
  Arena *info_arena;
  DASM_Info info;
  
  // rjf: metadata
  B32 is_working;
  U64 scope_ref_count;
  U64 last_time_touched_us;
  U64 last_user_clock_idx_touched;
  U64 load_count;
  U64 last_time_requested_us;
  U64 last_user_clock_idx_requested;
}

DASM_Slot :: struct
{
  DASM_Node *first;
  DASM_Node *last;
}

DASM_Stripe :: struct
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
  DASM_Node *free_node;
}

////////////////////////////////
//~ rjf: Scoped Access Types

DASM_Touch :: struct
{
  DASM_Touch *next;
  U128 hash;
  DASM_Params params;
}

DASM_Scope :: struct
{
  DASM_Scope *next;
  DASM_Touch *top_touch;
  U64 base_pos;
}

////////////////////////////////
//~ rjf: Thread Context

DASM_TCTX :: struct
{
  Arena *arena;
}

////////////////////////////////
//~ rjf: Shared State

DASM_Shared :: struct
{
  Arena *arena;
  
  // rjf: cache
  U64 slots_count;
  U64 stripes_count;
  DASM_Slot *slots;
  DASM_Stripe *stripes;
  
  // rjf: user -> parse thread
  U64 u2p_ring_size;
  U8 *u2p_ring_base;
  U64 u2p_ring_write_pos;
  U64 u2p_ring_read_pos;
  OS_Handle u2p_ring_cv;
  OS_Handle u2p_ring_mutex;
  
  // rjf: evictor/detector thread
  OS_Handle evictor_detector_thread;
}

////////////////////////////////
//~ rjf: Globals

thread_static DASM_TCTX *dasm_tctx = 0;
static DASM_Shared *dasm_shared = 0;

////////////////////////////////
//~ rjf: Instruction Decoding/Disassembling Type Functions

DASM_Inst dasm_inst_from_code(Arena *arena, Arch arch, U64 vaddr, String8 code, DASM_Syntax syntax);

////////////////////////////////
//~ rjf: Control Flow Analysis

DASM_CtrlFlowInfo dasm_ctrl_flow_info_from_arch_vaddr_code(Arena *arena, DASM_InstFlags exit_points_mask, Arch arch, U64 vaddr, String8 code);

////////////////////////////////
//~ rjf: Parameter Type Functions

B32 dasm_params_match(DASM_Params *a, DASM_Params *b);

////////////////////////////////
//~ rjf: Line Type Functions

void dasm_line_chunk_list_push(Arena *arena, DASM_LineChunkList *list, U64 cap, DASM_Line *line);
DASM_LineArray dasm_line_array_from_chunk_list(Arena *arena, DASM_LineChunkList *list);
U64 dasm_line_array_idx_from_code_off__linear_scan(DASM_LineArray *array, U64 off);
U64 dasm_line_array_code_off_from_idx(DASM_LineArray *array, U64 idx);

////////////////////////////////
//~ rjf: Main Layer Initialization

void dasm_init();

////////////////////////////////
//~ rjf: Scoped Access

DASM_Scope *dasm_scope_open();
void dasm_scope_close(DASM_Scope *scope);
void dasm_scope_touch_node__stripe_r_guarded(DASM_Scope *scope, DASM_Node *node);

////////////////////////////////
//~ rjf: Cache Lookups

DASM_Info dasm_info_from_hash_params(DASM_Scope *scope, U128 hash, DASM_Params *params);
DASM_Info dasm_info_from_key_params(DASM_Scope *scope, U128 key, DASM_Params *params, U128 *hash_out);

////////////////////////////////
//~ rjf: Parse Threads

B32 dasm_u2p_enqueue_req(U128 hash, DASM_Params *params, U64 endt_us);
void dasm_u2p_dequeue_req(Arena *arena, U128 *hash_out, DASM_Params *params_out);
ASYNC_WORK_DEF(dasm_parse_work);

////////////////////////////////
//~ rjf: Evictor/Detector Thread

void dasm_evictor_detector_thread__entry_point(void *p);

#endif // DASM_CACHE_H

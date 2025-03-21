// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DASM_CACHE_H
#define DASM_CACHE_H

////////////////////////////////
//~ rjf: Disassembly Syntax Types

enum DASM_Syntax
{
  DASM_Syntax_Intel,
  DASM_Syntax_ATT,
  DASM_Syntax_COUNT
};

////////////////////////////////
//~ rjf: Disassembly Instruction Info Types

enum DASM_InstFlags : uint
{
  DASM_InstFlag_Call                        = (1<<0),
  DASM_InstFlag_Branch                      = (1<<1),
  DASM_InstFlag_UnconditionalJump           = (1<<2),
  DASM_InstFlag_Return                      = (1<<3),
  DASM_InstFlag_NonFlow                     = (1<<4),
  DASM_InstFlag_Repeats                     = (1<<5),
  DASM_InstFlag_ChangesStackPointer         = (1<<6),
  DASM_InstFlag_ChangesStackPointerVariably = (1<<7),
};

struct DASM_Inst
{
  DASM_InstFlags flags;
  uint size;
  String8 string;
  ulong jump_dest_vaddr;
};

////////////////////////////////
//~ rjf: Control Flow Analysis Types

struct DASM_CtrlFlowPoint
{
  ulong vaddr;
  ulong jump_dest_vaddr;
  DASM_InstFlags inst_flags;
};

struct DASM_CtrlFlowPointNode
{
  DASM_CtrlFlowPointNode *next;
  DASM_CtrlFlowPoint v;
};

struct DASM_CtrlFlowPointList
{
  DASM_CtrlFlowPointNode *first;
  DASM_CtrlFlowPointNode *last;
  ulong count;
};

struct DASM_CtrlFlowInfo
{
  DASM_CtrlFlowPointList exit_points;
  ulong total_size;
};

////////////////////////////////
//~ rjf: Disassembly Text Decoration Types

enum DASM_StyleFlags : uint
{
  DASM_StyleFlag_Addresses        = (1<<0),
  DASM_StyleFlag_CodeBytes        = (1<<1),
  DASM_StyleFlag_SourceFilesNames = (1<<2),
  DASM_StyleFlag_SourceLines      = (1<<3),
  DASM_StyleFlag_SymbolNames      = (1<<4),
};

////////////////////////////////
//~ rjf: Disassembling Parameters Bundle

struct DASM_Params
{
  ulong vaddr;
  Arch arch;
  DASM_StyleFlags style_flags;
  DASM_Syntax syntax;
  ulong base_vaddr;
  DI_Key dbgi_key;
};

////////////////////////////////
//~ rjf: Disassembly Text Line Types

enum DASM_LineFlags : uint
{
  DASM_LineFlag_Decorative = (1<<0),
};

struct DASM_Line
{
  uint code_off;
  DASM_LineFlags flags;
  ulong addr;
  Rng1U64 text_range;
};

struct DASM_LineChunkNode
{
  DASM_LineChunkNode *next;
  DASM_Line *v;
  ulong cap;
  ulong count;
};

struct DASM_LineChunkList
{
  DASM_LineChunkNode *first;
  DASM_LineChunkNode *last;
  ulong node_count;
  ulong line_count;
};

struct DASM_LineArray
{
  DASM_Line *v;
  ulong count;
};

////////////////////////////////
//~ rjf: Disassembly Result Bundle

struct DASM_Result
{
  String8 text;
  DASM_LineArray lines;
};

////////////////////////////////
//~ rjf: Value Bundle Type

struct DASM_Info
{
  U128 text_key;
  DASM_LineArray lines;
};

////////////////////////////////
//~ rjf: Cache Types

struct DASM_Node
{
  // rjf: links
  DASM_Node *next;
  DASM_Node *prev;
  
  // rjf: key
  U128 hash;
  DASM_Params params;
  
  // rjf: generations
  ulong change_gen;
  
  // rjf: value
  Arena *info_arena;
  DASM_Info info;
  
  // rjf: metadata
  B32 is_working;
  ulong scope_ref_count;
  ulong last_time_touched_us;
  ulong last_user_clock_idx_touched;
  ulong load_count;
  ulong last_time_requested_us;
  ulong last_user_clock_idx_requested;
};

struct DASM_Slot
{
  DASM_Node *first;
  DASM_Node *last;
};

struct DASM_Stripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
  DASM_Node *free_node;
};

////////////////////////////////
//~ rjf: Scoped Access Types

struct DASM_Touch
{
  DASM_Touch *next;
  U128 hash;
  DASM_Params params;
};

struct DASM_Scope
{
  DASM_Scope *next;
  DASM_Touch *top_touch;
  ulong base_pos;
};

////////////////////////////////
//~ rjf: Thread Context

struct DASM_TCTX
{
  Arena *arena;
};

////////////////////////////////
//~ rjf: Shared State

struct DASM_Shared
{
  Arena *arena;
  
  // rjf: cache
  ulong slots_count;
  ulong stripes_count;
  DASM_Slot *slots;
  DASM_Stripe *stripes;
  
  // rjf: user -> parse thread
  ulong u2p_ring_size;
  byte *u2p_ring_base;
  ulong u2p_ring_write_pos;
  ulong u2p_ring_read_pos;
  OS_Handle u2p_ring_cv;
  OS_Handle u2p_ring_mutex;
  
  // rjf: evictor/detector thread
  OS_Handle evictor_detector_thread;
};

////////////////////////////////
//~ rjf: Globals

thread_static DASM_TCTX *dasm_tctx = 0;
static DASM_Shared *dasm_shared = 0;

////////////////////////////////
//~ rjf: Instruction Decoding/Disassembling Type Functions

DASM_Inst dasm_inst_from_code(Arena *arena, Arch arch, ulong vaddr, String8 code, DASM_Syntax syntax);

////////////////////////////////
//~ rjf: Control Flow Analysis

DASM_CtrlFlowInfo dasm_ctrl_flow_info_from_arch_vaddr_code(Arena *arena, DASM_InstFlags exit_points_mask, Arch arch, ulong vaddr, String8 code);

////////////////////////////////
//~ rjf: Parameter Type Functions

B32 dasm_params_match(DASM_Params *a, DASM_Params *b);

////////////////////////////////
//~ rjf: Line Type Functions

void dasm_line_chunk_list_push(Arena *arena, DASM_LineChunkList *list, ulong cap, DASM_Line *line);
DASM_LineArray dasm_line_array_from_chunk_list(Arena *arena, DASM_LineChunkList *list);
ulong dasm_line_array_idx_from_code_off__linear_scan(DASM_LineArray *array, ulong off);
ulong dasm_line_array_code_off_from_idx(DASM_LineArray *array, ulong idx);

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

B32 dasm_u2p_enqueue_req(U128 hash, DASM_Params *params, ulong endt_us);
void dasm_u2p_dequeue_req(Arena *arena, U128 *hash_out, DASM_Params *params_out);
ASYNC_WORK_DEF(dasm_parse_work);

////////////////////////////////
//~ rjf: Evictor/Detector Thread

void dasm_evictor_detector_thread__entry_point(void *p);

#endif // DASM_CACHE_H

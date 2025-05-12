// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Disassembly Syntax Types

enum DASM_Syntax
{
  DASM_Syntax_Intel,
  DASM_Syntax_ATT,
  DASM_Syntax_COUNT
}

////////////////////////////////
//~ rjf: Disassembly Instruction Info Types

enum DASM_InstFlags : U32
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

struct DASM_Inst
{
  DASM_InstFlags flags;
  U32 size;
  String8 string;
  U64 jump_dest_vaddr;
}

////////////////////////////////
//~ rjf: Control Flow Analysis Types

struct DASM_CtrlFlowPoint
{
  U64 vaddr;
  U64 jump_dest_vaddr;
  DASM_InstFlags inst_flags;
}

struct DASM_CtrlFlowPointNode
{
  DASM_CtrlFlowPointNode* next;
  DASM_CtrlFlowPoint v;
}

struct DASM_CtrlFlowPointList
{
  DASM_CtrlFlowPointNode* first;
  DASM_CtrlFlowPointNode* last;
  U64 count;
}

struct DASM_CtrlFlowInfo
{
  DASM_CtrlFlowPointList exit_points;
  U64 total_size;
}

////////////////////////////////
//~ rjf: Disassembly Text Decoration Types

enum DASM_StyleFlags : U32
{
  DASM_StyleFlag_Addresses        = (1<<0),
  DASM_StyleFlag_CodeBytes        = (1<<1),
  DASM_StyleFlag_SourceFilesNames = (1<<2),
  DASM_StyleFlag_SourceLines      = (1<<3),
  DASM_StyleFlag_SymbolNames      = (1<<4),
}

////////////////////////////////
//~ rjf: Disassembling Parameters Bundle

struct DASM_Params
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

enum DASM_LineFlags : U32
{
  DASM_LineFlag_Decorative = (1<<0),
}

struct DASM_Line
{
  U32 code_off;
  DASM_LineFlags flags;
  U64 addr;
  Rng1U64 text_range;
}

struct DASM_LineChunkNode
{
  DASM_LineChunkNode* next;
  DASM_Line* v;
  U64 cap;
  U64 count;
}

struct DASM_LineChunkList
{
  DASM_LineChunkNode* first;
  DASM_LineChunkNode* last;
  U64 node_count;
  U64 line_count;
}

struct DASM_LineArray
{
  DASM_Line* v;
  U64 count;
}

////////////////////////////////
//~ rjf: Disassembly Result Bundle

struct DASM_Result
{
  String8 text;
  DASM_LineArray lines;
}

////////////////////////////////
//~ rjf: Value Bundle Type

struct DASM_Info
{
  U128 text_key;
  DASM_LineArray lines;
}

////////////////////////////////
//~ rjf: Cache Types

struct DASM_Node
{
  // rjf: links
  DASM_Node* next;
  DASM_Node* prev;
  
  // rjf: key
  U128 hash;
  DASM_Params params;
  
  // rjf: generations
  U64 change_gen;
  
  // rjf: value
  Arena* info_arena;
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

struct DASM_Slot
{
  DASM_Node* first;
  DASM_Node* last;
}

struct DASM_Stripe
{
  Arena* arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
  DASM_Node* free_node;
}

////////////////////////////////
//~ rjf: Scoped Access Types

struct DASM_Touch
{
  DASM_Touch* next;
  U128 hash;
  DASM_Params params;
}

struct DASM_Scope
{
  DASM_Scope* next;
  DASM_Touch* top_touch;
  U64 base_pos;
}

////////////////////////////////
//~ rjf: Thread Context

struct DASM_TCTX
{
  Arena* arena;
}

////////////////////////////////
//~ rjf: Shared State

struct DASM_Shared
{
  Arena* arena;
  
  // rjf: cache
  U64 slots_count;
  U64 stripes_count;
  DASM_Slot* slots;
  DASM_Stripe* stripes;
  
  // rjf: user -> parse thread
  U64 u2p_ring_size;
  U8* u2p_ring_base;
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
global DASM_Shared *dasm_shared = 0;

////////////////////////////////
//~ rjf: Parse Threads

ASYNC_WORK_DEF(dasm_parse_work);

////////////////////////////////
//~ rjf: Evictor/Detector Thread



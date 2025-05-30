// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Tick Input Types

struct D_Target
{
  String8 exe;
  String8 args;
  String8 working_directory;
  String8 custom_entry_point_name;
  String8 stdout_path;
  String8 stderr_path;
  String8 stdin_path;
  B32 debug_subprocesses;
  String8List env;
}

struct D_TargetArray
{
  D_Target* v;
  U64 count;
}

enum D_BreakpointFlags : U32
{
  D_BreakpointFlag_BreakOnWrite   = (1<<0),
  D_BreakpointFlag_BreakOnRead    = (1<<1),
  D_BreakpointFlag_BreakOnExecute = (1<<2),
}

struct D_Breakpoint
{
  D_BreakpointFlags flags;
  U64 id;
  String8 file_path;
  TxtPt pt;
  String8 vaddr_expr;
  String8 condition;
  U64 size;
}

struct D_BreakpointArray
{
  D_Breakpoint* v;
  U64 count;
}

struct D_PathMap
{
  String8 src;
  String8 dst;
}

struct D_PathMapArray
{
  D_PathMap* v;
  U64 count;
}

////////////////////////////////
//~ rjf: Tick Output Types

enum D_EventKind
{
  D_EventKind_Null,
  D_EventKind_ProcessEnd,
  D_EventKind_Stop,
  D_EventKind_COUNT
}

enum D_EventCause
{
  D_EventCause_Null,
  D_EventCause_UserBreakpoint,
  D_EventCause_Halt,
  D_EventCause_SoftHalt,
  D_EventCause_COUNT
}

struct D_Event
{
  D_EventKind kind;
  D_EventCause cause;
  CTRL_Handle thread;
  U64 vaddr;
  U64 code;
  U64 id;
}

struct D_EventNode
{
  D_EventNode* next;
  D_Event v;
}

struct D_EventList
{
  D_EventNode* first;
  D_EventNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Line Info Types

struct D_Line
{
  String8 file_path;
  TxtPt pt;
  Rng1U64 voff_range;
  DI_Key dbgi_key;
}

struct D_LineNode
{
  D_LineNode* next;
  D_Line v;
}

struct D_LineList
{
  D_LineNode* first;
  D_LineNode* last;
  U64 count;
}

struct D_LineListArray
{
  D_LineList* v;
  U64 count;
  DI_KeyList dbgi_keys;
}

////////////////////////////////
//~ rjf: Debug Engine Control Communication Types

enum D_RunKind
{
  D_RunKind_Run,
  D_RunKind_SingleStep,
  D_RunKind_Step,
  D_RunKind_COUNT
}

////////////////////////////////
//~ rjf: Generated Code

#include "dbg_engine/generated/dbg_engine.meta.h"

////////////////////////////////
//~ rjf: Command Types

struct D_CmdParams
{
  CTRL_Handle machine;
  CTRL_Handle process;
  CTRL_Handle thread;
  CTRL_Handle entity;
  String8 string;
  String8 file_path;
  TxtPt cursor;
  U64 vaddr;
  B32 prefer_disasm;
  U32 pid;
  U32 rgba;
  D_TargetArray targets;
}

struct D_Cmd
{
  D_CmdKind kind;
  D_CmdParams params;
}

struct D_CmdNode
{
  D_CmdNode* next;
  D_CmdNode* prev;
  D_Cmd cmd;
}

struct D_CmdList
{
  D_CmdNode* first;
  D_CmdNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Main State Caches

//- rjf: per-thread unwind cache

struct D_UnwindCacheNode
{
  D_UnwindCacheNode* next;
  D_UnwindCacheNode* prev;
  U64 reggen;
  U64 memgen;
  Arena* arena;
  CTRL_Handle thread;
  CTRL_Unwind unwind;
}

struct D_UnwindCacheSlot
{
  D_UnwindCacheNode* first;
  D_UnwindCacheNode* last;
}

struct D_UnwindCache
{
  U64 slots_count;
  D_UnwindCacheSlot* slots;
  D_UnwindCacheNode* free_node;
}

//- rjf: per-run tls-base-vaddr cache

struct D_RunTLSBaseCacheNode
{
  D_RunTLSBaseCacheNode* hash_next;
  CTRL_Handle process;
  U64 root_vaddr;
  U64 rip_vaddr;
  U64 tls_base_vaddr;
}

struct D_RunTLSBaseCacheSlot
{
  D_RunTLSBaseCacheNode* first;
  D_RunTLSBaseCacheNode* last;
}

struct D_RunTLSBaseCache
{
  Arena* arena;
  U64 slots_count;
  D_RunTLSBaseCacheSlot* slots;
}

//- rjf: per-run locals cache

struct D_RunLocalsCacheNode
{
  D_RunLocalsCacheNode* hash_next;
  DI_Key dbgi_key;
  U64 voff;
  E_String2NumMap* locals_map;
}

struct D_RunLocalsCacheSlot
{
  D_RunLocalsCacheNode* first;
  D_RunLocalsCacheNode* last;
}

struct D_RunLocalsCache
{
  Arena* arena;
  U64 table_size;
  D_RunLocalsCacheSlot* table;
}

////////////////////////////////
//~ rjf: Main State Types

struct D_State
{
  // rjf: top-level state
  Arena* arena;
  U64 frame_index;
  
  // rjf: commands
  Arena* cmds_arena;
  D_CmdList cmds;
  
  // rjf: output log key
  U128 output_log_key;
  
  // rjf: per-run caches
  D_UnwindCache unwind_cache;
  U64 tls_base_cache_reggen_idx;
  U64 tls_base_cache_memgen_idx;
  D_RunTLSBaseCache[2] tls_base_caches;
  U64 tls_base_cache_gen;
  U64 locals_cache_reggen_idx;
  D_RunLocalsCache[2] locals_caches;
  U64 locals_cache_gen;
  U64 member_cache_reggen_idx;
  D_RunLocalsCache[2] member_caches;
  U64 member_cache_gen;
  
  // rjf: user -> ctrl driving state
  Arena* ctrl_last_run_arena;
  D_RunKind ctrl_last_run_kind;
  U64 ctrl_last_run_frame_idx;
  CTRL_Handle ctrl_last_run_thread_handle;
  CTRL_RunFlags ctrl_last_run_flags;
  CTRL_TrapList ctrl_last_run_traps;
  D_BreakpointArray ctrl_last_run_extra_bps;
  U128 ctrl_last_run_param_state_hash;
  B32 ctrl_is_running;
  B32 ctrl_thread_run_state;
  B32 ctrl_soft_halt_issued;
  Arena* ctrl_msg_arena;
  CTRL_MsgList ctrl_msgs;
  
  // rjf: ctrl -> user reading state
  CTRL_EntityStore* ctrl_entity_store;
  Arena* ctrl_stop_arena;
  CTRL_Event ctrl_last_stop_event;
}

////////////////////////////////
//~ rjf: Globals

global D_State *d_state = 0;

////////////////////////////////
//~ rjf: Main State Accessors/Mutators
//- rjf: top-level command dispatch
#define d_cmd(kind, ...) d_push_cmd((kind), &(D_CmdParams){.thread = {0}, __VA_ARGS__})

////////////////////////////////
//~ rjf: Main Layer Top-Level Calls



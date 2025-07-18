// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DBG_ENGINE_CORE_H
#define DBG_ENGINE_CORE_H

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
};

struct D_TargetArray
{
  D_Target *v;
  uint64 count;
};

enum D_BreakpointFlags : uint32
{
  D_BreakpointFlag_BreakOnWrite   = (1<<0),
  D_BreakpointFlag_BreakOnRead    = (1<<1),
  D_BreakpointFlag_BreakOnExecute = (1<<2),
};

struct D_Breakpoint
{
  D_BreakpointFlags flags;
  uint64 id;
  String8 file_path;
  TxtPt pt;
  String8 vaddr_expr;
  String8 condition;
  uint64 size;
};

struct D_BreakpointArray
{
  D_Breakpoint *v;
  uint64 count;
};

struct D_PathMap
{
  String8 src;
  String8 dst;
};

struct D_PathMapArray
{
  D_PathMap *v;
  uint64 count;
};

////////////////////////////////
//~ rjf: Tick Output Types

enum D_EventKind
{
  D_EventKind_Null,
  D_EventKind_ProcessEnd,
  D_EventKind_Stop,
  D_EventKind_COUNT
}
D_EventKind;

enum D_EventCause
{
  D_EventCause_Null,
  D_EventCause_UserBreakpoint,
  D_EventCause_Halt,
  D_EventCause_SoftHalt,
  D_EventCause_COUNT
}
D_EventCause;

struct D_Event
{
  D_EventKind kind;
  D_EventCause cause;
  CTRL_Handle thread;
  uint64 vaddr;
  uint64 code;
  uint64 id;
};

struct D_EventNode
{
  D_EventNode *next;
  D_Event v;
};

struct D_EventList
{
  D_EventNode *first;
  D_EventNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Line Info Types

struct D_Line
{
  String8 file_path;
  TxtPt pt;
  Rng1uint64 voff_range;
  DI_Key dbgi_key;
};

struct D_LineNode
{
  D_LineNode *next;
  D_Line v;
};

struct D_LineList
{
  D_LineNode *first;
  D_LineNode *last;
  uint64 count;
};

struct D_LineListArray
{
  D_LineList *v;
  uint64 count;
  DI_KeyList dbgi_keys;
};

////////////////////////////////
//~ rjf: Debug Engine Control Communication Types

enum D_RunKind
{
  D_RunKind_Run,
  D_RunKind_SingleStep,
  D_RunKind_Step,
  D_RunKind_COUNT
}
D_RunKind;

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
  uint64 vaddr;
  B32 prefer_disasm;
  uint32 pid;
  uint32 rgba;
  D_TargetArray targets;
};

struct D_Cmd
{
  D_CmdKind kind;
  D_CmdParams params;
};

struct D_CmdNode
{
  D_CmdNode *next;
  D_CmdNode *prev;
  D_Cmd cmd;
};

struct D_CmdList
{
  D_CmdNode *first;
  D_CmdNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Main State Caches

//- rjf: per-run tls-base-vaddr cache

struct D_RunTLSBaseCacheNode
{
  D_RunTLSBaseCacheNode *hash_next;
  CTRL_Handle process;
  uint64 root_vaddr;
  uint64 rip_vaddr;
  uint64 tls_base_vaddr;
};

struct D_RunTLSBaseCacheSlot
{
  D_RunTLSBaseCacheNode *first;
  D_RunTLSBaseCacheNode *last;
};

struct D_RunTLSBaseCache
{
  Arena *arena;
  uint64 slots_count;
  D_RunTLSBaseCacheSlot *slots;
};

//- rjf: per-run locals cache

struct D_RunLocalsCacheNode
{
  D_RunLocalsCacheNode *hash_next;
  DI_Key dbgi_key;
  uint64 voff;
  E_String2NumMap *locals_map;
};

struct D_RunLocalsCacheSlot
{
  D_RunLocalsCacheNode *first;
  D_RunLocalsCacheNode *last;
};

struct D_RunLocalsCache
{
  Arena *arena;
  uint64 table_size;
  D_RunLocalsCacheSlot *table;
};

////////////////////////////////
//~ rjf: Main State Types

struct D_State
{
  // rjf: top-level state
  Arena *arena;
  uint64 frame_index;
  
  // rjf: commands
  Arena *cmds_arena;
  D_CmdList cmds;
  
  // rjf: output log key
  HS_Key output_log_key;
  
  // rjf: per-run caches
  uint64 tls_base_cache_reggen_idx;
  uint64 tls_base_cache_memgen_idx;
  D_RunTLSBaseCache tls_base_caches[2];
  uint64 tls_base_cache_gen;
  uint64 locals_cache_reggen_idx;
  D_RunLocalsCache locals_caches[2];
  uint64 locals_cache_gen;
  uint64 member_cache_reggen_idx;
  D_RunLocalsCache member_caches[2];
  uint64 member_cache_gen;
  
  // rjf: user -> ctrl driving state
  Arena *ctrl_last_run_arena;
  D_RunKind ctrl_last_run_kind;
  uint64 ctrl_last_run_frame_idx;
  CTRL_Handle ctrl_last_run_thread_handle;
  CTRL_RunFlags ctrl_last_run_flags;
  CTRL_TrapList ctrl_last_run_traps;
  D_BreakpointArray ctrl_last_run_extra_bps;
  uint128 ctrl_last_run_param_state_hash;
  B32 ctrl_is_running;
  B32 ctrl_thread_run_state;
  B32 ctrl_soft_halt_issued;
  Arena *ctrl_msg_arena;
  CTRL_MsgList ctrl_msgs;
  
  // rjf: ctrl -> user reading state
  CTRL_EntityCtxRWStore *ctrl_entity_store;
  Arena *ctrl_stop_arena;
  CTRL_Event ctrl_last_stop_event;
};

////////////////////////////////
//~ rjf: Globals

global D_State *d_state = 0;

////////////////////////////////
//~ rjf: Basic Helpers

internal uint64 d_hash_from_seed_string(uint64 seed, String8 string);
internal uint64 d_hash_from_string(String8 string);
internal uint64 d_hash_from_seed_string__case_insensitive(uint64 seed, String8 string);
internal uint64 d_hash_from_string__case_insensitive(String8 string);

////////////////////////////////
//~ rjf: Breakpoints

internal D_BreakpointArray d_breakpoint_array_copy(Arena *arena, D_BreakpointArray *src);

////////////////////////////////
//~ rjf: Path Map Application

internal String8List d_possible_path_overrides_from_maps_path(Arena *arena, D_PathMapArray *path_maps, String8 file_path);

////////////////////////////////
//~ rjf: Debug Info Extraction Type Pure Functions

internal D_LineList d_line_list_copy(Arena *arena, D_LineList *list);

////////////////////////////////
//~ rjf: Command Type Functions

//- rjf: command parameters
internal D_CmdParams d_cmd_params_copy(Arena *arena, D_CmdParams *src);

//- rjf: command lists
internal void d_cmd_list_push_new(Arena *arena, D_CmdList *cmds, D_CmdKind kind, D_CmdParams *params);

////////////////////////////////
//~ rjf: Stepping "Trap Net" Builders

internal CTRL_TrapList d_trap_net_from_thread__step_over_inst(Arena *arena, CTRL_Entity *thread);
internal CTRL_TrapList d_trap_net_from_thread__step_over_line(Arena *arena, CTRL_Entity *thread);
internal CTRL_TrapList d_trap_net_from_thread__step_into_line(Arena *arena, CTRL_Entity *thread);

////////////////////////////////
//~ rjf: Debug Info Lookups

//- rjf: symbol -> voff lookups
internal uint64 d_voff_from_dbgi_key_symbol_name(DI_Key *dbgi_key, String8 symbol_name);

//- rjf: voff -> line info
internal D_LineList d_lines_from_dbgi_key_voff(Arena *arena, DI_Key *dbgi_key, uint64 voff);

//- rjf: file:line -> line info
// TODO(rjf): this depends on file path maps, needs to move
// TODO(rjf): need to clean this up & dedup
internal D_LineListArray d_lines_array_from_dbgi_key_file_path_line_range(Arena *arena, DI_Key dbgi_key, String8 file_path, Rng1uint64 line_num_range);
internal D_LineListArray d_lines_array_from_file_path_line_range(Arena *arena, String8 file_path, Rng1uint64 line_num_range);
internal D_LineList d_lines_from_dbgi_key_file_path_line_num(Arena *arena, DI_Key dbgi_key, String8 file_path, uint64 line_num);
internal D_LineList d_lines_from_file_path_line_num(Arena *arena, String8 file_path, uint64 line_num);

////////////////////////////////
//~ rjf: Process/Thread/Module Info Lookups

internal uint64 d_tls_base_vaddr_from_process_root_rip(CTRL_Entity *process, uint64 root_vaddr, uint64 rip_vaddr);

////////////////////////////////
//~ rjf: Target Controls

//- rjf: stopped info from the control thread
internal CTRL_Event d_ctrl_last_stop_event(void);

////////////////////////////////
//~ rjf: Main State Accessors/Mutators

//- rjf: frame data
internal uint64 d_frame_index(void);

//- rjf: control state
internal D_RunKind d_ctrl_last_run_kind(void);
internal uint64 d_ctrl_last_run_frame_idx(void);
internal B32 d_ctrl_targets_running(void);

//- rjf: active entity based queries
internal DI_KeyList d_push_active_dbgi_key_list(Arena *arena);

//- rjf: per-run caches
internal uint64 d_query_cached_rip_from_thread(CTRL_Entity *thread);
internal uint64 d_query_cached_rip_from_thread_unwind(CTRL_Entity *thread, uint64 unwind_count);
internal uint64 d_query_cached_tls_base_vaddr_from_process_root_rip(CTRL_Entity *process, uint64 root_vaddr, uint64 rip_vaddr);
internal E_String2NumMap *d_query_cached_locals_map_from_dbgi_key_voff(DI_Key *dbgi_key, uint64 voff);
internal E_String2NumMap *d_query_cached_member_map_from_dbgi_key_voff(DI_Key *dbgi_key, uint64 voff);

//- rjf: top-level command dispatch
internal void d_push_cmd(D_CmdKind kind, D_CmdParams *params);
#define d_cmd(kind, ...) d_push_cmd((kind), &(D_CmdParams){.thread = {0}, __VA_ARGS__})

//- rjf: command iteration
internal B32 d_next_cmd(D_Cmd **cmd);

////////////////////////////////
//~ rjf: Main Layer Top-Level Calls

internal void d_init(void);
internal D_EventList d_tick(Arena *arena, D_TargetArray *targets, D_BreakpointArray *breakpoints, D_PathMapArray *path_maps, uint64 exception_code_filters[(CTRL_ExceptionCodeKind_COUNT+63)/64]);

#endif // DBG_ENGINE_CORE_H

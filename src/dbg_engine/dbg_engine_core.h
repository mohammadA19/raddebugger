// Copyright (c) 2024 Epic Games Tools
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
  ulong count;
};

struct D_Breakpoint
{
  String8 file_path;
  TxtPt pt;
  String8 symbol_name;
  ulong vaddr;
  String8 condition;
};

struct D_BreakpointArray
{
  D_Breakpoint *v;
  ulong count;
};

struct D_PathMap
{
  String8 src;
  String8 dst;
};

struct D_PathMapArray
{
  D_PathMap *v;
  ulong count;
};

////////////////////////////////
//~ rjf: Tick Output Types

enum D_EventKind
{
  D_EventKind_Null,
  D_EventKind_ProcessEnd,
  D_EventKind_Stop,
  D_EventKind_COUNT
};

enum D_EventCause
{
  D_EventCause_Null,
  D_EventCause_UserBreakpoint,
  D_EventCause_Halt,
  D_EventCause_SoftHalt,
  D_EventCause_COUNT
};

struct D_Event
{
  D_EventKind kind;
  D_EventCause cause;
  CTRL_Handle thread;
  ulong vaddr;
  ulong code;
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
  ulong count;
};

////////////////////////////////
//~ rjf: Line Info Types

struct D_Line
{
  String8 file_path;
  TxtPt pt;
  Rng1U64 voff_range;
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
  ulong count;
};

struct D_LineListArray
{
  D_LineList *v;
  ulong count;
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
};

////////////////////////////////
//~ rjf: Generated Code

#include "dbg_engine/generated/dbg_engine.meta.h"

////////////////////////////////
//~ rjf: View Rules

enum D_ViewRuleSpecInfoFlags : uint // NOTE(rjf): see @view_rule_info
{
  D_ViewRuleSpecInfoFlag_Inherited      = (1<<0),
  D_ViewRuleSpecInfoFlag_Expandable     = (1<<1),
  D_ViewRuleSpecInfoFlag_ExprResolution = (1<<2),
  D_ViewRuleSpecInfoFlag_VizBlockProd   = (1<<3),
};

struct D_ViewRuleSpecInfo
{
  String8 string;
  String8 display_string;
  String8 schema;
  String8 description;
  D_ViewRuleSpecInfoFlags flags;
};

struct D_ViewRuleSpecInfoArray
{
  D_ViewRuleSpecInfo *v;
  ulong count;
};

struct D_ViewRuleSpec
{
  D_ViewRuleSpec *hash_next;
  D_ViewRuleSpecInfo info;
};

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
  ulong vaddr;
  B32 prefer_disasm;
  uint pid;
  uint rgba;
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
  ulong count;
};

////////////////////////////////
//~ rjf: Main State Caches

//- rjf: per-thread unwind cache

struct D_UnwindCacheNode
{
  D_UnwindCacheNode *next;
  D_UnwindCacheNode *prev;
  ulong reggen;
  ulong memgen;
  Arena *arena;
  CTRL_Handle thread;
  CTRL_Unwind unwind;
};

struct D_UnwindCacheSlot
{
  D_UnwindCacheNode *first;
  D_UnwindCacheNode *last;
};

struct D_UnwindCache
{
  ulong slots_count;
  D_UnwindCacheSlot *slots;
  D_UnwindCacheNode *free_node;
};

//- rjf: per-run tls-base-vaddr cache

struct D_RunTLSBaseCacheNode
{
  D_RunTLSBaseCacheNode *hash_next;
  CTRL_Handle process;
  ulong root_vaddr;
  ulong rip_vaddr;
  ulong tls_base_vaddr;
};

struct D_RunTLSBaseCacheSlot
{
  D_RunTLSBaseCacheNode *first;
  D_RunTLSBaseCacheNode *last;
};

struct D_RunTLSBaseCache
{
  Arena *arena;
  ulong slots_count;
  D_RunTLSBaseCacheSlot *slots;
};

//- rjf: per-run locals cache

struct D_RunLocalsCacheNode
{
  D_RunLocalsCacheNode *hash_next;
  DI_Key dbgi_key;
  ulong voff;
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
  ulong table_size;
  D_RunLocalsCacheSlot *table;
};

////////////////////////////////
//~ rjf: Main State Types

struct D_State
{
  // rjf: top-level state
  Arena *arena;
  ulong frame_index;
  ulong frame_eval_memread_endt_us;
  
  // rjf: commands
  Arena *cmds_arena;
  D_CmdList cmds;
  
  // rjf: output log key
  U128 output_log_key;
  
  // rjf: per-run caches
  D_UnwindCache unwind_cache;
  ulong tls_base_cache_reggen_idx;
  ulong tls_base_cache_memgen_idx;
  D_RunTLSBaseCache tls_base_caches[2];
  ulong tls_base_cache_gen;
  ulong locals_cache_reggen_idx;
  D_RunLocalsCache locals_caches[2];
  ulong locals_cache_gen;
  ulong member_cache_reggen_idx;
  D_RunLocalsCache member_caches[2];
  ulong member_cache_gen;
  
  // rjf: view rule specification table
  ulong view_rule_spec_table_size;
  D_ViewRuleSpec **view_rule_spec_table;
  
  // rjf: user -> ctrl driving state
  Arena *ctrl_last_run_arena;
  D_RunKind ctrl_last_run_kind;
  ulong ctrl_last_run_frame_idx;
  CTRL_Handle ctrl_last_run_thread_handle;
  CTRL_RunFlags ctrl_last_run_flags;
  CTRL_TrapList ctrl_last_run_traps;
  D_BreakpointArray ctrl_last_run_extra_bps;
  U128 ctrl_last_run_param_state_hash;
  B32 ctrl_is_running;
  B32 ctrl_thread_run_state;
  B32 ctrl_soft_halt_issued;
  Arena *ctrl_msg_arena;
  CTRL_MsgList ctrl_msgs;
  
  // rjf: ctrl -> user reading state
  CTRL_EntityStore *ctrl_entity_store;
  Arena *ctrl_stop_arena;
  CTRL_Event ctrl_last_stop_event;
};

////////////////////////////////
//~ rjf: Globals

read_only static D_ViewRuleSpec d_nil_core_view_rule_spec = {0};
static D_State *d_state = 0;

////////////////////////////////
//~ rjf: Basic Helpers

ulong d_hash_from_seed_string(ulong seed, String8 string);
ulong d_hash_from_string(String8 string);
ulong d_hash_from_seed_string__case_insensitive(ulong seed, String8 string);
ulong d_hash_from_string__case_insensitive(String8 string);

////////////////////////////////
//~ rjf: Breakpoints

D_BreakpointArray d_breakpoint_array_copy(Arena *arena, D_BreakpointArray *src);

////////////////////////////////
//~ rjf: Path Map Application

String8List d_possible_path_overrides_from_maps_path(Arena *arena, D_PathMapArray *path_maps, String8 file_path);

////////////////////////////////
//~ rjf: Debug Info Extraction Type Pure Functions

D_LineList d_line_list_copy(Arena *arena, D_LineList *list);

////////////////////////////////
//~ rjf: Command Type Functions

//- rjf: command parameters
D_CmdParams d_cmd_params_copy(Arena *arena, D_CmdParams *src);

//- rjf: command lists
void d_cmd_list_push_new(Arena *arena, D_CmdList *cmds, D_CmdKind kind, D_CmdParams *params);

////////////////////////////////
//~ rjf: View Rule Spec Stateful Functions

void d_register_view_rule_specs(D_ViewRuleSpecInfoArray specs);
D_ViewRuleSpec *d_view_rule_spec_from_string(String8 string);

////////////////////////////////
//~ rjf: Stepping "Trap Net" Builders

CTRL_TrapList d_trap_net_from_thread__step_over_inst(Arena *arena, CTRL_Entity *thread);
CTRL_TrapList d_trap_net_from_thread__step_over_line(Arena *arena, CTRL_Entity *thread);
CTRL_TrapList d_trap_net_from_thread__step_into_line(Arena *arena, CTRL_Entity *thread);

////////////////////////////////
//~ rjf: Debug Info Lookups

//- rjf: voff|vaddr -> symbol lookups
String8 d_symbol_name_from_dbgi_key_voff(Arena *arena, DI_Key *dbgi_key, ulong voff, B32 decorated);
String8 d_symbol_name_from_process_vaddr(Arena *arena, CTRL_Entity *process, ulong vaddr, B32 decorated);

//- rjf: symbol -> voff lookups
ulong d_voff_from_dbgi_key_symbol_name(DI_Key *dbgi_key, String8 symbol_name);
ulong d_type_num_from_dbgi_key_name(DI_Key *dbgi_key, String8 name);

//- rjf: voff -> line info
D_LineList d_lines_from_dbgi_key_voff(Arena *arena, DI_Key *dbgi_key, ulong voff);

//- rjf: file:line -> line info
// TODO(rjf): this depends on file path maps, needs to move
// TODO(rjf): need to clean this up & dedup
D_LineListArray d_lines_array_from_dbgi_key_file_path_line_range(Arena *arena, DI_Key dbgi_key, String8 file_path, Rng1S64 line_num_range);
D_LineListArray d_lines_array_from_file_path_line_range(Arena *arena, String8 file_path, Rng1S64 line_num_range);
D_LineList d_lines_from_dbgi_key_file_path_line_num(Arena *arena, DI_Key dbgi_key, String8 file_path, long line_num);
D_LineList d_lines_from_file_path_line_num(Arena *arena, String8 file_path, long line_num);

////////////////////////////////
//~ rjf: Process/Thread/Module Info Lookups

ulong d_tls_base_vaddr_from_process_root_rip(CTRL_Entity *process, ulong root_vaddr, ulong rip_vaddr);

////////////////////////////////
//~ rjf: Target Controls

//- rjf: stopped info from the control thread
CTRL_Event d_ctrl_last_stop_event();

////////////////////////////////
//~ rjf: Main State Accessors/Mutators

//- rjf: frame data
ulong d_frame_index();

//- rjf: control state
D_RunKind d_ctrl_last_run_kind();
ulong d_ctrl_last_run_frame_idx();
B32 d_ctrl_targets_running();

//- rjf: active entity based queries
DI_KeyList d_push_active_dbgi_key_list(Arena *arena);

//- rjf: per-run caches
CTRL_Unwind d_query_cached_unwind_from_thread(CTRL_Entity *thread);
ulong d_query_cached_rip_from_thread(CTRL_Entity *thread);
ulong d_query_cached_rip_from_thread_unwind(CTRL_Entity *thread, ulong unwind_count);
ulong d_query_cached_tls_base_vaddr_from_process_root_rip(CTRL_Entity *process, ulong root_vaddr, ulong rip_vaddr);
E_String2NumMap *d_query_cached_locals_map_from_dbgi_key_voff(DI_Key *dbgi_key, ulong voff);
E_String2NumMap *d_query_cached_member_map_from_dbgi_key_voff(DI_Key *dbgi_key, ulong voff);

//- rjf: top-level command dispatch
void d_push_cmd(D_CmdKind kind, D_CmdParams *params);
#define d_cmd(kind, ...) d_push_cmd((kind), &(D_CmdParams){.thread = {0}, __VA_ARGS__})

//- rjf: command iteration
B32 d_next_cmd(D_Cmd **cmd);

////////////////////////////////
//~ rjf: Main Layer Top-Level Calls

void d_init();
D_EventList d_tick(Arena *arena, D_TargetArray *targets, D_BreakpointArray *breakpoints, D_PathMapArray *path_maps, ulong exception_code_filters[(CTRL_ExceptionCodeKind_COUNT+63)/64], CTRL_MetaEvalArray *meta_evals);

#endif // DBG_ENGINE_CORE_H

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
//~ rjf: Tick Input Types

typedef struct D_Target D_Target;
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

typedef struct D_TargetArray D_TargetArray;
struct D_TargetArray
{
  D_Target *v;
  U64 count;
};

typedef U32 D_BreakpointFlags;
enum
{
  D_BreakpointFlag_BreakOnWrite   = (1<<0),
  D_BreakpointFlag_BreakOnRead    = (1<<1),
  D_BreakpointFlag_BreakOnExecute = (1<<2),
};

typedef struct D_Breakpoint D_Breakpoint;
struct D_Breakpoint
{
  D_BreakpointFlags flags;
  U64 id;
  String8 file_path;
  TxtPt pt;
  String8 vaddr_expr;
  String8 condition;
  U64 size;
};

typedef struct D_BreakpointArray D_BreakpointArray;
struct D_BreakpointArray
{
  D_Breakpoint *v;
  U64 count;
};

typedef struct D_PathMap D_PathMap;
struct D_PathMap
{
  String8 src;
  String8 dst;
};

typedef struct D_PathMapArray D_PathMapArray;
struct D_PathMapArray
{
  D_PathMap *v;
  U64 count;
};

////////////////////////////////
//~ rjf: Tick Output Types

typedef enum D_EventKind
{
  D_EventKind_Null,
  D_EventKind_ProcessEnd,
  D_EventKind_Stop,
  D_EventKind_COUNT
}
D_EventKind;

typedef enum D_EventCause
{
  D_EventCause_Null,
  D_EventCause_UserBreakpoint,
  D_EventCause_Halt,
  D_EventCause_SoftHalt,
  D_EventCause_COUNT
}
D_EventCause;

typedef struct D_Event D_Event;
struct D_Event
{
  D_EventKind kind;
  D_EventCause cause;
  CTRL_Handle thread;
  U64 vaddr;
  U64 code;
  U64 id;
};

typedef struct D_EventNode D_EventNode;
struct D_EventNode
{
  D_EventNode *next;
  D_Event v;
};

typedef struct D_EventList D_EventList;
struct D_EventList
{
  D_EventNode *first;
  D_EventNode *last;
  U64 count;
};

////////////////////////////////
//~ rjf: Line Info Types

typedef struct D_Line D_Line;
struct D_Line
{
  String8 file_path;
  TxtPt pt;
  Rng1U64 voff_range;
  DI_Key dbgi_key;
};

typedef struct D_LineNode D_LineNode;
struct D_LineNode
{
  D_LineNode *next;
  D_Line v;
};

typedef struct D_LineList D_LineList;
struct D_LineList
{
  D_LineNode *first;
  D_LineNode *last;
  U64 count;
};

typedef struct D_LineListArray D_LineListArray;
struct D_LineListArray
{
  D_LineList *v;
  U64 count;
  DI_KeyList dbgi_keys;
};

////////////////////////////////
//~ rjf: Debug Engine Control Communication Types

typedef enum D_RunKind
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

typedef struct D_CmdParams D_CmdParams;
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
};

typedef struct D_Cmd D_Cmd;
struct D_Cmd
{
  D_CmdKind kind;
  D_CmdParams params;
};

typedef struct D_CmdNode D_CmdNode;
struct D_CmdNode
{
  D_CmdNode *next;
  D_CmdNode *prev;
  D_Cmd cmd;
};

typedef struct D_CmdList D_CmdList;
struct D_CmdList
{
  D_CmdNode *first;
  D_CmdNode *last;
  U64 count;
};

////////////////////////////////
//~ rjf: Main State Caches

//- rjf: per-run tls-base-vaddr cache

typedef struct D_RunTLSBaseCacheNode D_RunTLSBaseCacheNode;
struct D_RunTLSBaseCacheNode
{
  D_RunTLSBaseCacheNode *hash_next;
  CTRL_Handle process;
  U64 root_vaddr;
  U64 rip_vaddr;
  U64 tls_base_vaddr;
};

typedef struct D_RunTLSBaseCacheSlot D_RunTLSBaseCacheSlot;
struct D_RunTLSBaseCacheSlot
{
  D_RunTLSBaseCacheNode *first;
  D_RunTLSBaseCacheNode *last;
};

typedef struct D_RunTLSBaseCache D_RunTLSBaseCache;
struct D_RunTLSBaseCache
{
  Arena *arena;
  U64 slots_count;
  D_RunTLSBaseCacheSlot *slots;
};

//- rjf: per-run locals cache

typedef struct D_RunLocalsCacheNode D_RunLocalsCacheNode;
struct D_RunLocalsCacheNode
{
  D_RunLocalsCacheNode *hash_next;
  DI_Key dbgi_key;
  U64 voff;
  E_String2NumMap *locals_map;
};

typedef struct D_RunLocalsCacheSlot D_RunLocalsCacheSlot;
struct D_RunLocalsCacheSlot
{
  D_RunLocalsCacheNode *first;
  D_RunLocalsCacheNode *last;
};

typedef struct D_RunLocalsCache D_RunLocalsCache;
struct D_RunLocalsCache
{
  Arena *arena;
  U64 table_size;
  D_RunLocalsCacheSlot *table;
};

////////////////////////////////
//~ rjf: Main State Types

typedef struct D_State D_State;
struct D_State
{
  // rjf: top-level state
  Arena *arena;
  U64 frame_index;
  
  // rjf: commands
  Arena *cmds_arena;
  D_CmdList cmds;
  
  // rjf: output log key
  HS_Key output_log_key;
  
  // rjf: per-run caches
  U64 tls_base_cache_reggen_idx;
  U64 tls_base_cache_memgen_idx;
  D_RunTLSBaseCache tls_base_caches[2];
  U64 tls_base_cache_gen;
  U64 locals_cache_reggen_idx;
  D_RunLocalsCache locals_caches[2];
  U64 locals_cache_gen;
  U64 member_cache_reggen_idx;
  D_RunLocalsCache member_caches[2];
  U64 member_cache_gen;
  
  // rjf: user -> ctrl driving state
  Arena *ctrl_last_run_arena;
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


////////////////////////////////
//~ rjf: Breakpoints


////////////////////////////////
//~ rjf: Path Map Application


////////////////////////////////
//~ rjf: Debug Info Extraction Type Pure Functions


////////////////////////////////
//~ rjf: Command Type Functions

//- rjf: command parameters

//- rjf: command lists

////////////////////////////////
//~ rjf: Stepping "Trap Net" Builders


////////////////////////////////
//~ rjf: Debug Info Lookups

//- rjf: symbol -> voff lookups

//- rjf: voff -> line info

//- rjf: file:line -> line info
// TODO(rjf): this depends on file path maps, needs to move
// TODO(rjf): need to clean this up & dedup

////////////////////////////////
//~ rjf: Process/Thread/Module Info Lookups


////////////////////////////////
//~ rjf: Target Controls

//- rjf: stopped info from the control thread

////////////////////////////////
//~ rjf: Main State Accessors/Mutators

//- rjf: frame data

//- rjf: control state

//- rjf: active entity based queries

//- rjf: per-run caches

//- rjf: top-level command dispatch
#define d_cmd(kind, ...) d_push_cmd((kind), &(D_CmdParams){.thread = {0}, __VA_ARGS__})

//- rjf: command iteration

////////////////////////////////
//~ rjf: Main Layer Top-Level Calls



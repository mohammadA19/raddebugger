// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef CTRL_CORE_H
#define CTRL_CORE_H

////////////////////////////////
//~ rjf: ID Types

typedef uint64 CTRL_MsgID;
typedef uint64 CTRL_MachineID;

#define CTRL_MachineID_Local (1)

////////////////////////////////
//~ rjf: User Breakpoint Types

enum CTRL_UserBreakpointFlags : uint32
{
  CTRL_UserBreakpointFlag_BreakOnWrite   = (1<<0),
  CTRL_UserBreakpointFlag_BreakOnRead    = (1<<1),
  CTRL_UserBreakpointFlag_BreakOnExecute = (1<<2),
};

enum CTRL_UserBreakpointKind
{
  CTRL_UserBreakpointKind_Null,
  CTRL_UserBreakpointKind_FileNameAndLineColNumber,
  CTRL_UserBreakpointKind_Expression,
  CTRL_UserBreakpointKind_COUNT
}

struct CTRL_UserBreakpoint
{
  CTRL_UserBreakpointKind kind;
  CTRL_UserBreakpointFlags flags;
  uint64 id;
  String8 string;
  TxtPt pt;
  uint64 size;
  String8 condition;
};

struct CTRL_UserBreakpointNode
{
  CTRL_UserBreakpointNode *next;
  CTRL_UserBreakpoint v;
};

struct CTRL_UserBreakpointList
{
  CTRL_UserBreakpointNode *first;
  CTRL_UserBreakpointNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Entity Handle Types

struct CTRL_Handle
{
  CTRL_MachineID machine_id;
  DMN_Handle dmn_handle;
};

struct CTRL_HandleNode
{
  CTRL_HandleNode *next;
  CTRL_Handle v;
};

struct CTRL_HandleList
{
  CTRL_HandleNode *first;
  CTRL_HandleNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Generated Code

#include "generated/ctrl.meta.h"

////////////////////////////////
//~ rjf: Entity Types

struct CTRL_Entity
{
  CTRL_Entity *first;
  CTRL_Entity *last;
  CTRL_Entity *next;
  CTRL_Entity *prev;
  CTRL_Entity *parent;
  CTRL_EntityKind kind;
  Arch arch;
  B32 is_frozen;
  B32 is_soloed;
  uint32 rgba;
  CTRL_Handle handle;
  uint64 id;
  Rng1<uint64> vaddr_range;
  uint64 stack_base;
  uint64 timestamp;
  CTRL_UserBreakpointFlags bp_flags;
  String8 string;
};

struct CTRL_EntityNode
{
  CTRL_EntityNode *next;
  CTRL_Entity *v;
};

struct CTRL_EntityList
{
  CTRL_EntityNode *first;
  CTRL_EntityNode *last;
  uint64 count;
};

struct CTRL_EntityArray
{
  CTRL_Entity **v;
  uint64 count;
};

struct CTRL_EntityRec
{
  CTRL_Entity *next;
  uint32 push_count;
  uint64 pop_count;
};

struct CTRL_EntityHashNode
{
  CTRL_EntityHashNode *next;
  CTRL_EntityHashNode *prev;
  CTRL_Entity *entity;
};

struct CTRL_EntityHashSlot
{
  CTRL_EntityHashNode *first;
  CTRL_EntityHashNode *last;
};

struct CTRL_EntityStringChunkNode
{
  CTRL_EntityStringChunkNode *next;
  uint64 size;
};

read_only global uint64 ctrl_entity_string_bucket_chunk_sizes[] =
{
  16,
  64,
  256,
  1024,
  4096,
  16384,
  65536,
  0xffffffffffffffffull,
};

struct CTRL_EntityCtx
{
  CTRL_Entity *root;
  uint64 hash_slots_count;
  CTRL_EntityHashSlot *hash_slots;
  uint64 entity_kind_counts[CTRL_EntityKind_COUNT];
  uint64 entity_kind_alloc_gens[CTRL_EntityKind_COUNT];
};

struct CTRL_EntityCtxRWStore
{
  Arena *arena;
  CTRL_EntityCtx ctx;
  CTRL_Entity *free;
  CTRL_EntityHashNode *hash_node_free;
  CTRL_EntityStringChunkNode *free_string_chunks[ArrayCount(ctrl_entity_string_bucket_chunk_sizes)];
};

struct CTRL_EntityCtxLookupAccel
{
  Arena *arena;
  Arena *entity_kind_arrays_arenas[CTRL_EntityKind_COUNT];
  CTRL_EntityArray entity_kind_arrays[CTRL_EntityKind_COUNT];
  uint64 entity_kind_arrays_gens[CTRL_EntityKind_COUNT];
};

////////////////////////////////
//~ rjf: Unwind Types

enum CTRL_UnwindFlags : uint32
{
  CTRL_UnwindFlag_Error = (1<<0),
  CTRL_UnwindFlag_Stale = (1<<1),
};

struct CTRL_UnwindStepResult
{
  CTRL_UnwindFlags flags;
};

struct CTRL_UnwindFrame
{
  void *regs;
};

struct CTRL_UnwindFrameNode
{
  CTRL_UnwindFrameNode *next;
  CTRL_UnwindFrameNode *prev;
  CTRL_UnwindFrame v;
};

struct CTRL_UnwindFrameArray
{
  CTRL_UnwindFrame *v;
  uint64 count;
};

struct CTRL_Unwind
{
  CTRL_UnwindFrameArray frames;
  CTRL_UnwindFlags flags;
};

////////////////////////////////
//~ rjf: Call Stack Types

struct CTRL_CallStackFrame
{
  uint64 unwind_count;
  uint64 inline_depth;
  void *regs;
};

struct CTRL_CallStack
{
  CTRL_CallStackFrame *frames;
  uint64 frames_count;
  CTRL_CallStackFrame **concrete_frames;
  uint64 concrete_frames_count;
};

////////////////////////////////
//~ rjf: Trap Types

enum CTRL_TrapFlags : uint32
{
  CTRL_TrapFlag_IgnoreStackPointerCheck = (1<<0),
  CTRL_TrapFlag_SingleStepAfterHit      = (1<<1),
  CTRL_TrapFlag_SaveStackPointer        = (1<<2),
  CTRL_TrapFlag_BeginSpoofMode          = (1<<3),
  CTRL_TrapFlag_EndStepping             = (1<<4),
};

struct CTRL_Trap
{
  CTRL_TrapFlags flags;
  uint64 vaddr;
};

struct CTRL_TrapNode
{
  CTRL_TrapNode *next;
  CTRL_Trap v;
};

struct CTRL_TrapList
{
  CTRL_TrapNode *first;
  CTRL_TrapNode *last;
  uint64 count;
};

struct CTRL_Spoof
{
  DMN_Handle process;
  DMN_Handle thread;
  uint64 vaddr;
  uint64 new_ip_value;
};

////////////////////////////////
//~ rjf: Evaluation Spaces

enum CTRL_EvalSpaceKind : uint64
{
  CTRL_EvalSpaceKind_Entity = E_SpaceKind_FirstUserDefined,
  CTRL_EvalSpaceKind_Meta,
};

////////////////////////////////
//~ rjf: Message Types

enum CTRL_MsgKind
{
  CTRL_MsgKind_Null,
  CTRL_MsgKind_Launch,
  CTRL_MsgKind_Attach,
  CTRL_MsgKind_Kill,
  CTRL_MsgKind_KillAll,
  CTRL_MsgKind_Detach,
  CTRL_MsgKind_Run,
  CTRL_MsgKind_SingleStep,
  CTRL_MsgKind_SetUserEntryPoints,
  CTRL_MsgKind_SetModuleDebugInfoPath,
  CTRL_MsgKind_FreezeThread,
  CTRL_MsgKind_ThawThread,
  CTRL_MsgKind_COUNT,
}

enum CTRL_RunFlags : uint32
{
  CTRL_RunFlag_StopOnEntryPoint = (1<<0),
};

struct CTRL_Msg
{
  CTRL_MsgKind kind;
  CTRL_RunFlags run_flags;
  CTRL_MsgID msg_id;
  CTRL_Handle entity;
  CTRL_Handle parent;
  uint32 entity_id;
  uint32 exit_code;
  B32 env_inherit;
  B32 debug_subprocesses;
  uint64 exception_code_filters[(CTRL_ExceptionCodeKind_COUNT+63)/64];
  String8 path;
  String8List entry_points;
  String8List cmd_line_string_list;
  String8List env_string_list;
  String8 stdout_path;
  String8 stderr_path;
  String8 stdin_path;
  CTRL_TrapList traps;
  CTRL_UserBreakpointList user_bps;
};

struct CTRL_MsgNode
{
  CTRL_MsgNode *next;
  CTRL_Msg v;
};

struct CTRL_MsgList
{
  CTRL_MsgNode *first;
  CTRL_MsgNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Event Types

enum CTRL_EventKind
{
  CTRL_EventKind_Null,
  CTRL_EventKind_Error,
  
  //- rjf: starts/stops
  CTRL_EventKind_Started,
  CTRL_EventKind_Stopped,
  
  //- rjf: entity creation/deletion
  CTRL_EventKind_NewProc,
  CTRL_EventKind_NewThread,
  CTRL_EventKind_NewModule,
  CTRL_EventKind_EndProc,
  CTRL_EventKind_EndThread,
  CTRL_EventKind_EndModule,
  
  //- rjf: thread freeze state changes
  CTRL_EventKind_ThreadFrozen,
  CTRL_EventKind_ThreadThawed,
  
  //- rjf: debug info changes
  CTRL_EventKind_ModuleDebugInfoPathChange,
  
  //- rjf: debug strings / decorations / markup
  CTRL_EventKind_DebugString,
  CTRL_EventKind_ThreadName,
  CTRL_EventKind_ThreadColor,
  CTRL_EventKind_SetBreakpoint,
  CTRL_EventKind_UnsetBreakpoint,
  CTRL_EventKind_SetVAddrRangeNote,
  
  //- rjf: memory
  CTRL_EventKind_MemReserve,
  CTRL_EventKind_MemCommit,
  CTRL_EventKind_MemDecommit,
  CTRL_EventKind_MemRelease,
  
  CTRL_EventKind_COUNT
}

enum CTRL_EventCause
{
  CTRL_EventCause_Null,
  CTRL_EventCause_Error,
  CTRL_EventCause_Finished,
  CTRL_EventCause_EntryPoint,
  CTRL_EventCause_UserBreakpoint,
  CTRL_EventCause_InterruptedByTrap,
  CTRL_EventCause_InterruptedByException,
  CTRL_EventCause_InterruptedByHalt,
  CTRL_EventCause_COUNT
}

enum CTRL_ExceptionKind
{
  CTRL_ExceptionKind_Null,
  CTRL_ExceptionKind_MemoryRead,
  CTRL_ExceptionKind_MemoryWrite,
  CTRL_ExceptionKind_MemoryExecute,
  CTRL_ExceptionKind_CppThrow,
  CTRL_ExceptionKind_COUNT
}

struct CTRL_Event
{
  CTRL_EventKind kind;
  CTRL_EventCause cause;
  CTRL_ExceptionKind exception_kind;
  CTRL_MsgID msg_id;
  CTRL_Handle entity;
  CTRL_Handle parent;
  Arch arch;
  uint64 u64_code;
  uint32 entity_id;
  Rng1<uint64> vaddr_rng;
  uint64 rip_vaddr;
  uint64 stack_base;
  uint64 tls_root;
  uint64 timestamp;
  uint32 exception_code;
  uint32 rgba;
  CTRL_UserBreakpointFlags bp_flags;
  String8 string;
};

struct CTRL_EventNode
{
  CTRL_EventNode *next;
  CTRL_Event v;
};

struct CTRL_EventList
{
  CTRL_EventNode *first;
  CTRL_EventNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Process Memory Cache Types

struct CTRL_ProcessMemoryRangeHashNode
{
  CTRL_ProcessMemoryRangeHashNode *next;
  
  // rjf: key
  Rng1<uint64> vaddr_range;
  B32 zero_terminated;
  HS_ID id;
  
  // rjf: staleness info
  uint64 mem_gen;
  
  // rjf: metadata
  uint64 working_count;
  uint64 last_time_requested_us;
  uint64 last_user_clock_idx_touched;
};

struct CTRL_ProcessMemoryRangeHashSlot
{
  CTRL_ProcessMemoryRangeHashNode *first;
  CTRL_ProcessMemoryRangeHashNode *last;
};

struct CTRL_ProcessMemoryCacheNode
{
  CTRL_ProcessMemoryCacheNode *next;
  CTRL_ProcessMemoryCacheNode *prev;
  Arena *arena;
  CTRL_Handle handle;
  HS_Root root;
  uint64 range_hash_slots_count;
  CTRL_ProcessMemoryRangeHashSlot *range_hash_slots;
};

struct CTRL_ProcessMemoryCacheSlot
{
  CTRL_ProcessMemoryCacheNode *first;
  CTRL_ProcessMemoryCacheNode *last;
};

struct CTRL_ProcessMemoryCacheStripe
{
  OS_Handle rw_mutex;
  OS_Handle cv;
};

struct CTRL_ProcessMemoryCache
{
  uint64 slots_count;
  CTRL_ProcessMemoryCacheSlot *slots;
  uint64 stripes_count;
  CTRL_ProcessMemoryCacheStripe *stripes;
};

struct CTRL_ProcessMemorySlice
{
  String8 data;
  uint64 *byte_bad_flags;
  uint64 *byte_changed_flags;
  B32 stale;
  B32 any_byte_bad;
  B32 any_byte_changed;
};

////////////////////////////////
//~ rjf: Thread Register Cache Types

struct CTRL_ThreadRegCacheNode
{
  CTRL_ThreadRegCacheNode *next;
  CTRL_ThreadRegCacheNode *prev;
  CTRL_Handle handle;
  uint64 block_size;
  void *block;
  uint64 reg_gen;
};

struct CTRL_ThreadRegCacheSlot
{
  CTRL_ThreadRegCacheNode *first;
  CTRL_ThreadRegCacheNode *last;
};

struct CTRL_ThreadRegCacheStripe
{
  Arena *arena;
  OS_Handle rw_mutex;
};

struct CTRL_ThreadRegCache
{
  uint64 slots_count;
  CTRL_ThreadRegCacheSlot *slots;
  uint64 stripes_count;
  CTRL_ThreadRegCacheStripe *stripes;
};

////////////////////////////////
//~ rjf: Call Stack Cache Types

struct CTRL_CallStackCacheNode
{
  CTRL_CallStackCacheNode *next;
  CTRL_CallStackCacheNode *prev;
  
  // rjf: key
  CTRL_Handle thread;
  uint64 reg_gen;
  uint64 mem_gen;
  
  // rjf: counters
  uint64 scope_touch_count;
  uint64 working_count;
  
  // rjf: value
  Arena *arena;
  CTRL_CallStack call_stack;
};

struct CTRL_CallStackCacheSlot
{
  CTRL_CallStackCacheNode *first;
  CTRL_CallStackCacheNode *last;
};

struct CTRL_CallStackCacheStripe
{
  Arena *arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
};

struct CTRL_CallStackCache
{
  uint64 slots_count;
  CTRL_CallStackCacheSlot *slots;
  uint64 stripes_count;
  CTRL_CallStackCacheStripe *stripes;
};

////////////////////////////////
//~ rjf: Module Image Info Cache Types

struct CTRL_ModuleImageInfoCacheNode
{
  CTRL_ModuleImageInfoCacheNode *next;
  CTRL_ModuleImageInfoCacheNode *prev;
  CTRL_Handle module;
  Arena *arena;
  PE_IntelPdata *pdatas;
  uint64 pdatas_count;
  uint64 entry_point_voff;
  Rng1<uint64> tls_vaddr_range;
  String8 initial_debug_info_path;
  Rng1<uint64> raddbg_section_voff_range;
  String8 raddbg_data;
};

struct CTRL_ModuleImageInfoCacheSlot
{
  CTRL_ModuleImageInfoCacheNode *first;
  CTRL_ModuleImageInfoCacheNode *last;
};

struct CTRL_ModuleImageInfoCacheStripe
{
  Arena *arena;
  OS_Handle rw_mutex;
};

struct CTRL_ModuleImageInfoCache
{
  uint64 slots_count;
  CTRL_ModuleImageInfoCacheSlot *slots;
  uint64 stripes_count;
  CTRL_ModuleImageInfoCacheStripe *stripes;
};

////////////////////////////////
//~ rjf: Touched Debug Info Directory Cache

struct CTRL_DbgDirNode
{
  CTRL_DbgDirNode *first;
  CTRL_DbgDirNode *last;
  CTRL_DbgDirNode *next;
  CTRL_DbgDirNode *prev;
  CTRL_DbgDirNode *parent;
  String8 name;
  uint64 search_count;
  uint64 child_count;
  uint64 module_direct_count;
};

////////////////////////////////
//~ rjf: Control Thread Evaluation Scopes

struct CTRL_EvalScope
{
  DI_Scope *di_scope;
  E_BaseCtx base_ctx;
  E_IRCtx ir_ctx;
  E_InterpretCtx interpret_ctx;
};

////////////////////////////////
//~ rjf: Control Cache Accessing Scopes

struct CTRL_ScopeCallStackTouch
{
  CTRL_ScopeCallStackTouch *next;
  CTRL_CallStackCacheStripe *stripe;
  CTRL_CallStackCacheNode *node;
};

struct CTRL_Scope
{
  CTRL_Scope *next;
  CTRL_ScopeCallStackTouch *first_call_stack_touch;
  CTRL_ScopeCallStackTouch *last_call_stack_touch;
};

struct CTRL_TCTX
{
  Arena *arena;
  CTRL_Scope *free_scope;
  CTRL_ScopeCallStackTouch *free_call_stack_touch;
};

////////////////////////////////
//~ rjf: Module Requirement Cache Types

struct CTRL_ModuleReqCacheNode
{
  CTRL_ModuleReqCacheNode *next;
  CTRL_Handle module;
  B32 required;
};

////////////////////////////////
//~ rjf: Wakeup Hook Function Types

#define CTRL_WAKEUP_FUNCTION_DEF(name) void name(void)
typedef CTRL_WAKEUP_FUNCTION_DEF(CTRL_WakeupFunctionType);

////////////////////////////////
//~ rjf: Main State Types

struct CTRL_State
{
  Arena *arena;
  CTRL_WakeupFunctionType *wakeup_hook;
  
  // rjf: name -> register/alias hash tables for eval
  E_String2NumMap arch_string2reg_tables[Arch_COUNT];
  E_String2NumMap arch_string2alias_tables[Arch_COUNT];
  
  // rjf: caches
  CTRL_ProcessMemoryCache process_memory_cache;
  CTRL_ThreadRegCache thread_reg_cache;
  CTRL_CallStackCache call_stack_cache;
  CTRL_ModuleImageInfoCache module_image_info_cache;
  
  // rjf: generations
  uint64 run_gen;
  uint64 mem_gen;
  uint64 reg_gen;
  
  // rjf: user -> ctrl msg ring buffer
  uint64 u2c_ring_size;
  uint8 *u2c_ring_base;
  uint64 u2c_ring_write_pos;
  uint64 u2c_ring_read_pos;
  OS_Handle u2c_ring_mutex;
  OS_Handle u2c_ring_cv;
  
  // rjf: ctrl -> user event ring buffer
  uint64 c2u_ring_size;
  uint64 c2u_ring_max_string_size;
  uint8 *c2u_ring_base;
  uint64 c2u_ring_write_pos;
  uint64 c2u_ring_read_pos;
  OS_Handle c2u_ring_mutex;
  OS_Handle c2u_ring_cv;
  
  // rjf: ctrl thread state
  uint64 ctrl_thread_run_state;
  String8 ctrl_thread_log_path;
  OS_Handle ctrl_thread;
  Log *ctrl_thread_log;
  OS_Handle ctrl_thread_entity_ctx_rw_mutex;
  CTRL_EntityCtxRWStore *ctrl_thread_entity_store;
  E_Cache *ctrl_thread_eval_cache;
  Arena *ctrl_thread_msg_process_arena;
  Arena *dmn_event_arena;
  DMN_EventNode *first_dmn_event_node;
  DMN_EventNode *last_dmn_event_node;
  DMN_EventNode *free_dmn_event_node;
  Arena *user_entry_point_arena;
  String8List user_entry_points;
  uint64 exception_code_filters[(CTRL_ExceptionCodeKind_COUNT+63)/64];
  uint64 process_counter;
  Arena *dbg_dir_arena;
  CTRL_DbgDirNode *dbg_dir_root;
  uint64 module_req_cache_slots_count;
  CTRL_ModuleReqCacheNode **module_req_cache_slots;
  String8List msg_user_bp_touched_files;
  String8List msg_user_bp_touched_symbols;
  
  // rjf: user -> memstream ring buffer
  uint64 u2ms_ring_size;
  uint8 *u2ms_ring_base;
  uint64 u2ms_ring_write_pos;
  uint64 u2ms_ring_read_pos;
  OS_Handle u2ms_ring_mutex;
  OS_Handle u2ms_ring_cv;
  
  // rjf: user -> call stack builder ring buffer
  uint64 u2csb_ring_size;
  uint8 *u2csb_ring_base;
  uint64 u2csb_ring_write_pos;
  uint64 u2csb_ring_read_pos;
  OS_Handle u2csb_ring_mutex;
  OS_Handle u2csb_ring_cv;
};

////////////////////////////////
//~ rjf: Globals

global CTRL_State *ctrl_state = 0;
read_only global CTRL_Entity ctrl_entity_nil =
{
  &ctrl_entity_nil,
  &ctrl_entity_nil,
  &ctrl_entity_nil,
  &ctrl_entity_nil,
  &ctrl_entity_nil,
};
thread_static CTRL_TCTX *ctrl_tctx = 0;
thread_static CTRL_EntityCtxLookupAccel *ctrl_entity_ctx_lookup_accel = 0;

////////////////////////////////
//~ rjf: Logging Markup

#define CTRL_CtrlThreadLogScope DeferLoop(log_scope_begin(), ctrl_thread__end_and_flush_info_log())

////////////////////////////////
//~ rjf: Basic Type Functions


////////////////////////////////
//~ rjf: Handle Type Functions


////////////////////////////////
//~ rjf: Trap Type Functions


////////////////////////////////
//~ rjf: User Breakpoint Type Functions


////////////////////////////////
//~ rjf: Message Type Functions

//- rjf: deep copying

//- rjf: list building

//- rjf: serialization

////////////////////////////////
//~ rjf: Event Type Functions

//- rjf: list building

//- rjf: serialization

////////////////////////////////
//~ rjf: Entity Type Functions

//- rjf: entity list data structures
#define ctrl_entity_list_first(list) ((list)->first ? (list)->first->v : &ctrl_entity_nil)

//- rjf: entity array data structure
#define ctrl_entity_array_first(array) ((array)->count != 0 ? (array)->v[0] : &ctrl_entity_nil)

//- rjf: entity context (entity group read-only) functions

//- rjf: entity tree iteration
#define ctrl_entity_rec_depth_first_pre(entity, subtree_root)  ctrl_entity_rec_depth_first((entity), (subtree_root), OffsetOf(CTRL_Entity, next), OffsetOf(CTRL_Entity, first))
#define ctrl_entity_rec_depth_first_post(entity, subtree_root) ctrl_entity_rec_depth_first((entity), (subtree_root), OffsetOf(CTRL_Entity, prev), OffsetOf(CTRL_Entity, last))

//- rjf: entity ctx r/w store state functions

//- rjf: string allocation/deletion

//- rjf: entity construction/deletion

//- rjf: entity equipment

//- rjf: accelerated entity context lookups

//- rjf: applying events to entity caches

////////////////////////////////
//~ rjf: Cache Accessing Scopes


////////////////////////////////
//~ rjf: Main Layer Initialization


////////////////////////////////
//~ rjf: Wakeup Callback Registration


////////////////////////////////
//~ rjf: Process Memory Functions

//- rjf: process memory cache key reading

//- rjf: process memory cache reading helpers
#define ctrl_process_memory_read_struct(process, vaddr, is_stale_out, ptr, endt_us) ctrl_process_memory_read((process), r1u64((vaddr), (vaddr)+(sizeof(*(ptr)))), (is_stale_out), (ptr), (endt_us))

//- rjf: process memory writing

////////////////////////////////
//~ rjf: Thread Register Functions

//- rjf: thread register cache reading

//- rjf: thread register writing

////////////////////////////////
//~ rjf: Module Image Info Functions

//- rjf: cache lookups

////////////////////////////////
//~ rjf: Unwinding Functions

//- rjf: unwind deep copier

//- rjf: [x64]

//- rjf: abstracted unwind step

//- rjf: abstracted full unwind

////////////////////////////////
//~ rjf: Call Stack Building Functions


////////////////////////////////
//~ rjf: Call Stack Cache Functions


////////////////////////////////
//~ rjf: Halting All Attached Processes


////////////////////////////////
//~ rjf: Shared Accessor Functions

//- rjf: generation counters

//- rjf: name -> register/alias hash tables, for eval

////////////////////////////////
//~ rjf: Control-Thread Functions

//- rjf: user -> control thread communication

//- rjf: control -> user thread communication

//- rjf: entry point

//- rjf: breakpoint resolution

//- rjf: module lifetime open/close work

//- rjf: attached process running/event gathering

//- rjf: eval helpers

//- rjf: control thread eval scopes

//- rjf: log flusher

//- rjf: msg kind implementations

////////////////////////////////
//~ rjf: Asynchronous Memory Streaming Functions

//- rjf: user -> memory stream communication

//- rjf: entry point
ASYNC_WORK_DEF(ctrl_mem_stream_work);

////////////////////////////////
//~ rjf: Asynchronous Call Stack Building Functions

//- rjf: user -> memory stream communication

//- rjf: entry point
ASYNC_WORK_DEF(ctrl_call_stack_build_work);

#endif // CTRL_CORE_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: ID Types

typedef U64 CTRL_MsgID;
typedef U64 CTRL_MachineID;

#define CTRL_MachineID_Local (1)

////////////////////////////////
//~ rjf: User Breakpoint Types

enum CTRL_UserBreakpointFlags : U32
{
  CTRL_UserBreakpointFlag_BreakOnWrite   = (1<<0),
  CTRL_UserBreakpointFlag_BreakOnRead    = (1<<1),
  CTRL_UserBreakpointFlag_BreakOnExecute = (1<<2),
}

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
  U64 id;
  String8 string;
  TxtPt pt;
  U64 size;
  String8 condition;
}

struct CTRL_UserBreakpointNode
{
  CTRL_UserBreakpointNode* next;
  CTRL_UserBreakpoint v;
}

struct CTRL_UserBreakpointList
{
  CTRL_UserBreakpointNode* first;
  CTRL_UserBreakpointNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Entity Handle Types

struct CTRL_Handle
{
  CTRL_MachineID machine_id;
  DMN_Handle dmn_handle;
}

struct CTRL_HandleNode
{
  CTRL_HandleNode* next;
  CTRL_Handle v;
}

struct CTRL_HandleList
{
  CTRL_HandleNode* first;
  CTRL_HandleNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/ctrl.meta.h"

////////////////////////////////
//~ rjf: Entity Types

struct CTRL_Entity
{
  CTRL_Entity* first;
  CTRL_Entity* last;
  CTRL_Entity* next;
  CTRL_Entity* prev;
  CTRL_Entity* parent;
  CTRL_EntityKind kind;
  Arch arch;
  B32 is_frozen;
  U32 rgba;
  CTRL_Handle handle;
  U64 id;
  Rng1U64 vaddr_range;
  U64 stack_base;
  U64 timestamp;
  CTRL_UserBreakpointFlags bp_flags;
  String8 string;
}

struct CTRL_EntityNode
{
  CTRL_EntityNode* next;
  CTRL_Entity* v;
}

struct CTRL_EntityList
{
  CTRL_EntityNode* first;
  CTRL_EntityNode* last;
  U64 count;
}

struct CTRL_EntityArray
{
  CTRL_Entity** v;
  U64 count;
}

struct CTRL_EntityRec
{
  CTRL_Entity* next;
  S32 push_count;
  S64 pop_count;
}

struct CTRL_EntityHashNode
{
  CTRL_EntityHashNode* next;
  CTRL_EntityHashNode* prev;
  CTRL_Entity* entity;
}

struct CTRL_EntityHashSlot
{
  CTRL_EntityHashNode* first;
  CTRL_EntityHashNode* last;
}

struct CTRL_EntityStringChunkNode
{
  CTRL_EntityStringChunkNode* next;
  U64 size;
}

read_only global U64 ctrl_entity_string_bucket_chunk_sizes[] =
{
  16,
  64,
  256,
  1024,
  4096,
  16384,
  65536,
  0xffffffffffffffffull,
}

struct CTRL_EntityStore
{
  Arena* arena;
  CTRL_Entity* root;
  CTRL_Entity* free;
  CTRL_EntityHashSlot* hash_slots;
  CTRL_EntityHashNode* hash_node_free;
  U64 hash_slots_count;
  CTRL_EntityStringChunkNode *free_string_chunks[ArrayCount(ctrl_entity_string_bucket_chunk_sizes)];
  U64[CTRL_EntityKind_COUNT] entity_kind_counts;
  Arena *entity_kind_arrays_arenas[CTRL_EntityKind_COUNT];
  U64[CTRL_EntityKind_COUNT] entity_kind_arrays_gens;
  U64[CTRL_EntityKind_COUNT] entity_kind_alloc_gens;
  CTRL_EntityArray[CTRL_EntityKind_COUNT] entity_kind_arrays;
}

////////////////////////////////
//~ rjf: Unwind Types

enum CTRL_UnwindFlags : U32
{
  CTRL_UnwindFlag_Error = (1<<0),
  CTRL_UnwindFlag_Stale = (1<<1),
}

struct CTRL_UnwindStepResult
{
  CTRL_UnwindFlags flags;
}

struct CTRL_UnwindFrame
{
  void* regs;
}

struct CTRL_UnwindFrameNode
{
  CTRL_UnwindFrameNode* next;
  CTRL_UnwindFrameNode* prev;
  CTRL_UnwindFrame v;
}

struct CTRL_UnwindFrameArray
{
  CTRL_UnwindFrame* v;
  U64 count;
}

struct CTRL_Unwind
{
  CTRL_UnwindFrameArray frames;
  CTRL_UnwindFlags flags;
}

////////////////////////////////
//~ rjf: Call Stack Types

struct CTRL_CallStackFrame
{
  U64 parent_num;
  U64 unwind_count;
  U64 inline_depth;
  void* regs;
  RDI_Parsed* rdi;
  RDI_Procedure* procedure;
  RDI_InlineSite* inline_site;
}

struct CTRL_CallStack
{
  CTRL_CallStackFrame* frames;
  U64 count;
}

////////////////////////////////
//~ rjf: Trap Types

enum CTRL_TrapFlags : U32
{
  CTRL_TrapFlag_IgnoreStackPointerCheck = (1<<0),
  CTRL_TrapFlag_SingleStepAfterHit      = (1<<1),
  CTRL_TrapFlag_SaveStackPointer        = (1<<2),
  CTRL_TrapFlag_BeginSpoofMode          = (1<<3),
  CTRL_TrapFlag_EndStepping             = (1<<4),
}

struct CTRL_Trap
{
  CTRL_TrapFlags flags;
  U64 vaddr;
}

struct CTRL_TrapNode
{
  CTRL_TrapNode* next;
  CTRL_Trap v;
}

struct CTRL_TrapList
{
  CTRL_TrapNode* first;
  CTRL_TrapNode* last;
  U64 count;
}

struct CTRL_Spoof
{
  DMN_Handle process;
  DMN_Handle thread;
  U64 vaddr;
  U64 new_ip_value;
}

////////////////////////////////
//~ rjf: Evaluation Spaces

enum CTRL_EvalSpaceKind : U64
{
  CTRL_EvalSpaceKind_Entity = E_SpaceKind_FirstUserDefined,
  CTRL_EvalSpaceKind_Meta,
}

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

enum CTRL_RunFlags : U32
{
  CTRL_RunFlag_StopOnEntryPoint = (1<<0),
}

struct CTRL_Msg
{
  CTRL_MsgKind kind;
  CTRL_RunFlags run_flags;
  CTRL_MsgID msg_id;
  CTRL_Handle entity;
  CTRL_Handle parent;
  U32 entity_id;
  U32 exit_code;
  B32 env_inherit;
  B32 debug_subprocesses;
  U64[(CTRL_ExceptionCodeKind_COUNT+63)/64] exception_code_filters;
  String8 path;
  String8List entry_points;
  String8List cmd_line_string_list;
  String8List env_string_list;
  String8 stdout_path;
  String8 stderr_path;
  String8 stdin_path;
  CTRL_TrapList traps;
  CTRL_UserBreakpointList user_bps;
}

struct CTRL_MsgNode
{
  CTRL_MsgNode* next;
  CTRL_Msg v;
}

struct CTRL_MsgList
{
  CTRL_MsgNode* first;
  CTRL_MsgNode* last;
  U64 count;
}

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
  U64 u64_code;
  U32 entity_id;
  Rng1U64 vaddr_rng;
  U64 rip_vaddr;
  U64 stack_base;
  U64 tls_root;
  U64 timestamp;
  U32 exception_code;
  U32 rgba;
  CTRL_UserBreakpointFlags bp_flags;
  String8 string;
}

struct CTRL_EventNode
{
  CTRL_EventNode* next;
  CTRL_Event v;
}

struct CTRL_EventList
{
  CTRL_EventNode* first;
  CTRL_EventNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Process Memory Cache Types

struct CTRL_ProcessMemoryRangeHashNode
{
  CTRL_ProcessMemoryRangeHashNode* next;
  Rng1U64 vaddr_range;
  B32 zero_terminated;
  Rng1U64 vaddr_range_clamped;
  U128 hash;
  U64 mem_gen;
  U64 last_time_requested_us;
  B32 is_taken;
}

struct CTRL_ProcessMemoryRangeHashSlot
{
  CTRL_ProcessMemoryRangeHashNode* first;
  CTRL_ProcessMemoryRangeHashNode* last;
}

struct CTRL_ProcessMemoryCacheNode
{
  CTRL_ProcessMemoryCacheNode* next;
  CTRL_ProcessMemoryCacheNode* prev;
  Arena* arena;
  CTRL_Handle handle;
  U64 range_hash_slots_count;
  CTRL_ProcessMemoryRangeHashSlot* range_hash_slots;
}

struct CTRL_ProcessMemoryCacheSlot
{
  CTRL_ProcessMemoryCacheNode* first;
  CTRL_ProcessMemoryCacheNode* last;
}

struct CTRL_ProcessMemoryCacheStripe
{
  OS_Handle rw_mutex;
  OS_Handle cv;
}

struct CTRL_ProcessMemoryCache
{
  U64 slots_count;
  CTRL_ProcessMemoryCacheSlot* slots;
  U64 stripes_count;
  CTRL_ProcessMemoryCacheStripe* stripes;
}

struct CTRL_ProcessMemorySlice
{
  String8 data;
  U64* byte_bad_flags;
  U64* byte_changed_flags;
  B32 stale;
  B32 any_byte_bad;
  B32 any_byte_changed;
}

////////////////////////////////
//~ rjf: Thread Register Cache Types

struct CTRL_ThreadRegCacheNode
{
  CTRL_ThreadRegCacheNode* next;
  CTRL_ThreadRegCacheNode* prev;
  CTRL_Handle handle;
  U64 block_size;
  void* block;
  U64 reg_gen;
}

struct CTRL_ThreadRegCacheSlot
{
  CTRL_ThreadRegCacheNode* first;
  CTRL_ThreadRegCacheNode* last;
}

struct CTRL_ThreadRegCacheStripe
{
  Arena* arena;
  OS_Handle rw_mutex;
}

struct CTRL_ThreadRegCache
{
  U64 slots_count;
  CTRL_ThreadRegCacheSlot* slots;
  U64 stripes_count;
  CTRL_ThreadRegCacheStripe* stripes;
}

////////////////////////////////
//~ rjf: Module Image Info Cache Types

struct CTRL_ModuleImageInfoCacheNode
{
  CTRL_ModuleImageInfoCacheNode* next;
  CTRL_ModuleImageInfoCacheNode* prev;
  CTRL_Handle module;
  Arena* arena;
  PE_IntelPdata* pdatas;
  U64 pdatas_count;
  U64 entry_point_voff;
  Rng1U64 tls_vaddr_range;
  String8 initial_debug_info_path;
  Rng1U64 raddbg_section_voff_range;
  String8 raddbg_data;
}

struct CTRL_ModuleImageInfoCacheSlot
{
  CTRL_ModuleImageInfoCacheNode* first;
  CTRL_ModuleImageInfoCacheNode* last;
}

struct CTRL_ModuleImageInfoCacheStripe
{
  Arena* arena;
  OS_Handle rw_mutex;
}

struct CTRL_ModuleImageInfoCache
{
  U64 slots_count;
  CTRL_ModuleImageInfoCacheSlot* slots;
  U64 stripes_count;
  CTRL_ModuleImageInfoCacheStripe* stripes;
}

////////////////////////////////
//~ rjf: Touched Debug Info Directory Cache

struct CTRL_DbgDirNode
{
  CTRL_DbgDirNode* first;
  CTRL_DbgDirNode* last;
  CTRL_DbgDirNode* next;
  CTRL_DbgDirNode* prev;
  CTRL_DbgDirNode* parent;
  String8 name;
  U64 search_count;
  U64 child_count;
  U64 module_direct_count;
}

////////////////////////////////
//~ rjf: Control Thread Evaluation Scopes

struct CTRL_EvalScope
{
  DI_Scope* di_scope;
  E_BaseCtx base_ctx;
  E_IRCtx ir_ctx;
  E_InterpretCtx interpret_ctx;
}

////////////////////////////////
//~ rjf: Wakeup Hook Function Types

#define CTRL_WAKEUP_FUNCTION_DEF(name) void name(void)
typedef CTRL_WAKEUP_FUNCTION_DEF(CTRL_WakeupFunctionType);

////////////////////////////////
//~ rjf: Main State Types

struct CTRL_State
{
  Arena* arena;
  CTRL_WakeupFunctionType* wakeup_hook;
  
  // rjf: name -> register/alias hash tables for eval
  E_String2NumMap[Arch_COUNT] arch_string2reg_tables;
  E_String2NumMap[Arch_COUNT] arch_string2alias_tables;
  
  // rjf: caches
  CTRL_ProcessMemoryCache process_memory_cache;
  CTRL_ThreadRegCache thread_reg_cache;
  CTRL_ModuleImageInfoCache module_image_info_cache;
  
  // rjf: user -> ctrl msg ring buffer
  U64 u2c_ring_size;
  U8* u2c_ring_base;
  U64 u2c_ring_write_pos;
  U64 u2c_ring_read_pos;
  OS_Handle u2c_ring_mutex;
  OS_Handle u2c_ring_cv;
  
  // rjf: ctrl -> user event ring buffer
  U64 c2u_ring_size;
  U64 c2u_ring_max_string_size;
  U8* c2u_ring_base;
  U64 c2u_ring_write_pos;
  U64 c2u_ring_read_pos;
  OS_Handle c2u_ring_mutex;
  OS_Handle c2u_ring_cv;
  
  // rjf: ctrl thread state
  String8 ctrl_thread_log_path;
  OS_Handle ctrl_thread;
  Log* ctrl_thread_log;
  CTRL_EntityStore* ctrl_thread_entity_store;
  E_Cache* ctrl_thread_eval_cache;
  Arena* dmn_event_arena;
  DMN_EventNode* first_dmn_event_node;
  DMN_EventNode* last_dmn_event_node;
  DMN_EventNode* free_dmn_event_node;
  Arena* user_entry_point_arena;
  String8List user_entry_points;
  U64[(CTRL_ExceptionCodeKind_COUNT+63)/64] exception_code_filters;
  U64 process_counter;
  Arena* dbg_dir_arena;
  CTRL_DbgDirNode* dbg_dir_root;
  
  // rjf: user -> memstream ring buffer
  U64 u2ms_ring_size;
  U8* u2ms_ring_base;
  U64 u2ms_ring_write_pos;
  U64 u2ms_ring_read_pos;
  OS_Handle u2ms_ring_mutex;
  OS_Handle u2ms_ring_cv;
}

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
}

////////////////////////////////
//~ rjf: Logging Markup

#define CTRL_CtrlThreadLogScope DeferLoop(log_scope_begin(), ctrl_thread__end_and_flush_info_log())

////////////////////////////////
//~ rjf: Entity Type Functions

//- rjf: entity list data structures
#define ctrl_entity_list_first(list) ((list)->first ? (list)->first->v : &ctrl_entity_nil)

//- rjf: entity array data structure
#define ctrl_entity_array_first(array) ((array)->count != 0 ? (array)->v[0] : &ctrl_entity_nil)
//- rjf: entity tree iteration
#define ctrl_entity_rec_depth_first_pre(entity, subtree_root)  ctrl_entity_rec_depth_first((entity), (subtree_root), OffsetOf(CTRL_Entity, next), OffsetOf(CTRL_Entity, first))
#define ctrl_entity_rec_depth_first_post(entity, subtree_root) ctrl_entity_rec_depth_first((entity), (subtree_root), OffsetOf(CTRL_Entity, prev), OffsetOf(CTRL_Entity, last))

////////////////////////////////
//~ rjf: Process Memory Functions
//- rjf: process memory cache reading helpers
#define ctrl_read_cached_process_memory_struct(process, vaddr, is_stale_out, ptr, endt_us) ctrl_read_cached_process_memory((process), r1u64((vaddr), (vaddr)+(sizeof(*(ptr)))), (is_stale_out), (ptr), (endt_us))

////////////////////////////////
//~ rjf: Memory-Stream Thread Functions
//- rjf: entry point
ASYNC_WORK_DEF(ctrl_mem_stream_work);


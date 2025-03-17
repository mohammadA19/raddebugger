// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef CTRL_CORE_H
#define CTRL_CORE_H

////////////////////////////////
//~ rjf: ID Types

typedef uint64 CTRL_MsgID;
typedef uint64 CTRL_MachineID;

#define CTRL_MachineID_Local (1)

////////////////////////////////
//~ rjf: Meta Evaluation Types

//- rjf: auto-checkbox b32s

struct CTRL_CheckB32
{
  B32 b32;
}

struct_members(CTRL_CheckB32)
{
  member_lit_comp(CTRL_CheckB32, type(B32), b32),
}
struct_type(CTRL_CheckB32);

//- rjf: styled string types

ptr_type(CTRL_PlainString8__str_ptr_type, type(uint8), .flags = TypeFlag_IsPlainText,.count_delimiter_name = ("size"));
ptr_type(CTRL_CodeString8__str_ptr_type, type(uint8),  .flags = TypeFlag_IsCodeText, .count_delimiter_name = ("size"));
ptr_type(CTRL_PathString8__str_ptr_type, type(uint8),  .flags = TypeFlag_IsPathText, .count_delimiter_name = ("size"));
Member CTRL_PlainString8__members[] =
{
  member_lit_comp(String8, &CTRL_PlainString8__str_ptr_type, str,  .pretty_name = ("Contents")),
  member_lit_comp(String8, type(uint64),                        size, .pretty_name = ("Size")),
}
Member CTRL_CodeString8__members[] =
{
  member_lit_comp(String8, &CTRL_CodeString8__str_ptr_type, str,  .pretty_name = ("Contents")),
  member_lit_comp(String8, type(uint64),                       size, .pretty_name = ("Size")),
}
Member CTRL_PathString8__members[] =
{
  member_lit_comp(String8, &CTRL_PathString8__str_ptr_type, str,  .pretty_name = ("Contents")),
  member_lit_comp(String8, type(uint64),                        size, .pretty_name = ("Size")),
}
named_struct_type(CTRL_PlainString8, String8, .name = ("string"));
named_struct_type(CTRL_CodeString8,  String8, .name = ("string"));
named_struct_type(CTRL_PathString8,  String8, .name = ("string"));

//- rjf: meta evaluation callstack types

struct CTRL_MetaEvalFrame
{
  uint64 vaddr;
  uint64 inline_depth;
}
ptr_type(CTRL_MetaEvalFrame__vaddr_type, type(), .flags = TypeFlag_IsExternal, .size = sizeof(uint64));
struct_members(CTRL_MetaEvalFrame)
{
  member_lit_comp(CTRL_MetaEvalFrame, &CTRL_MetaEvalFrame__vaddr_type, vaddr),
  member_lit_comp(CTRL_MetaEvalFrame, type(uint64), inline_depth),
}
struct_type(CTRL_MetaEvalFrame, .name = ("callstack_frame"));
struct CTRL_MetaEvalFrameArray
{
  uint64 count;
  CTRL_MetaEvalFrame* v;
}
ptr_type(CTRL_MetaEvalFrameArray__v_ptr_type, type(CTRL_MetaEvalFrame), .count_delimiter_name = ("count"));
struct_members(CTRL_MetaEvalFrameArray)
{
  member_lit_comp(CTRL_MetaEvalFrameArray, type(uint64), count, .pretty_name = ("Frame Count")),
  {("v"), ("Frame Addresses"), &CTRL_MetaEvalFrameArray__v_ptr_type, OffsetOf(CTRL_MetaEvalFrameArray, v)},
}
struct_type(CTRL_MetaEvalFrameArray, .name = ("callstack_frames"));

//- rjf: meta evaluation instance types

struct CTRL_MetaEval
{
#define CTRL_MetaEval_MemberXList \
X(B32, enabled,            "Enabled")\
X(B32, frozen,             "Frozen")\
X(uint64, hit_count,          "Hit Count")\
X(uint64, id,                 "ID")\
X(Rng1U64, vaddr_range,    "Address Range")\
X(uint32, color,              "Color")\
X(CTRL_CheckB32, debug_subprocesses,"Debug Subprocesses")\
Y(String8, type(CTRL_CodeString8),  label,             "Label")\
Y(String8, type(CTRL_PathString8),  exe,               "Executable Path")\
Y(String8, type(CTRL_PathString8),  dbg,               "Debug Info Path")\
Y(String8, type(CTRL_PlainString8), args,              "Arguments")\
Y(String8, type(CTRL_PathString8),  working_directory, "Working Directory")\
Y(String8, type(CTRL_CodeString8),  entry_point,       "Custom Entry Point")\
Y(String8, type(CTRL_PathString8),  stdout_path,       "Standard Output Path")\
Y(String8, type(CTRL_PathString8),  stderr_path,       "Standard Error Path")\
Y(String8, type(CTRL_PathString8),  stdin_path,        "Standard Input Path")\
Y(String8, type(CTRL_PathString8),  source_location,   "Source Location")\
Y(String8, type(CTRL_CodeString8),  function_location, "Function Location")\
Y(String8, type(CTRL_CodeString8),  address_location,  "Address Location")\
Y(String8, type(CTRL_PathString8),  source_path,       "Source Path")\
Y(String8, type(CTRL_PathString8),  destination_path,  "Destination Path")\
Y(String8, type(CTRL_CodeString8),  type,              "Type")\
Y(String8, type(CTRL_CodeString8),  view_rule,         "View Rule")\
Y(String8, type(CTRL_CodeString8),  condition,         "Condition")\
X(CTRL_MetaEvalFrameArray, callstack, "Call Stack")
#define X(T, name, pretty_name) T name;
#define Y(T, ti, name, pretty_name) T name;
  CTRL_MetaEval_MemberXList
#undef X
#undef Y
}
struct_members(CTRL_MetaEval)
{
#define X(T, name, pretty_name_) member_lit_comp(CTRL_MetaEval, type(T), name, .pretty_name = (pretty_name_)),
#define Y(T, ti, name, pretty_name_) member_lit_comp(CTRL_MetaEval, (ti), name, .pretty_name = (pretty_name_)),
  CTRL_MetaEval_MemberXList
#undef X
#undef Y
}
struct_type(CTRL_MetaEval);

//- rjf: filters on main meta evaluation bundle

struct_members(CTRL_BreakpointMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(B32),              enabled,           .pretty_name = ("Enabled")),
  member_lit_comp(CTRL_MetaEval, type(uint32),              color,             .pretty_name = ("Color")),
  member_lit_comp(CTRL_MetaEval, type(uint64),              hit_count,         .pretty_name = ("Hit Count")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), label,             .pretty_name = ("Label")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), condition,         .pretty_name = ("Condition")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), source_location,   .pretty_name = ("Source Location")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), function_location, .pretty_name = ("Function Location")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), address_location,  .pretty_name = ("Address Location")),
}

struct_members(CTRL_TargetMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), label,              .pretty_name = ("Label")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), exe,                .pretty_name = ("Executable")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PlainString8),args,               .pretty_name = ("Arguments")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), working_directory,  .pretty_name = ("Working Directory")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), entry_point,        .pretty_name = ("Custom Entry Point")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), stdout_path,        .pretty_name = ("Standard Output Path")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), stderr_path,        .pretty_name = ("Standard Error Path")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), stdin_path,         .pretty_name = ("Standard Input Path")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CheckB32),    debug_subprocesses, .pretty_name = ("Debug Subprocesses")),
}

struct_members(CTRL_PinMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), label,              .pretty_name = ("Expression")),
  member_lit_comp(CTRL_MetaEval, type(uint32),              color,              .pretty_name = ("Color")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), source_location,    .pretty_name = ("Source Location")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), address_location,   .pretty_name = ("Address Location")),
}

struct_members(CTRL_FilePathMapMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), source_path,        .pretty_name = ("Source Path")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), destination_path,   .pretty_name = ("Destination Path")),
}

struct_members(CTRL_AutoViewRuleMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), type,        .pretty_name = ("Type")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), view_rule,   .pretty_name = ("View Rule")),
}

struct_members(CTRL_MachineMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(B32),              frozen,    .pretty_name = ("Frozen")),
  member_lit_comp(CTRL_MetaEval, type(uint32),              color,     .pretty_name = ("Color")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), label,     .pretty_name = ("Name")),
}

struct_members(CTRL_ProcessMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(B32),              frozen,    .pretty_name = ("Frozen")),
  member_lit_comp(CTRL_MetaEval, type(uint32),              color,     .pretty_name = ("Color")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), label,     .pretty_name = ("Name")),
  member_lit_comp(CTRL_MetaEval, type(uint64),              id,        .pretty_name = ("ID")),
}

struct_members(CTRL_ModuleMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(uint32),              color,       .pretty_name = ("Color")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8), label,       .pretty_name = ("Name")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), exe,         .pretty_name = ("Executable Path")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_PathString8), dbg,         .pretty_name = ("Debug Info Path")),
  member_lit_comp(CTRL_MetaEval, type(Rng1U64),          vaddr_range, .pretty_name = ("Address Range")),
}

struct_members(CTRL_ThreadMetaEval)
{
  member_lit_comp(CTRL_MetaEval, type(B32),                     frozen,    .pretty_name = ("Frozen")),
  member_lit_comp(CTRL_MetaEval, type(uint32),                     color,     .pretty_name = ("Color")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_CodeString8),        label,     .pretty_name = ("Name")),
  member_lit_comp(CTRL_MetaEval, type(uint64),                     id,        .pretty_name = ("ID")),
  member_lit_comp(CTRL_MetaEval, type(CTRL_MetaEvalFrameArray), callstack, .pretty_name = ("Call Stack")),
}

named_struct_type(CTRL_BreakpointMetaEval,  CTRL_MetaEval, .name = ("breakpoint"));
named_struct_type(CTRL_TargetMetaEval,      CTRL_MetaEval, .name = ("target"));
named_struct_type(CTRL_PinMetaEval,         CTRL_MetaEval, .name = ("pin"));
named_struct_type(CTRL_FilePathMapMetaEval, CTRL_MetaEval, .name = ("file_path_map"));
named_struct_type(CTRL_AutoViewRuleMetaEval,CTRL_MetaEval, .name = ("auto_view_rule"));
named_struct_type(CTRL_MachineMetaEval,     CTRL_MetaEval, .name = ("machine"));
named_struct_type(CTRL_ProcessMetaEval,     CTRL_MetaEval, .name = ("process"));
named_struct_type(CTRL_ModuleMetaEval,      CTRL_MetaEval, .name = ("module"));
named_struct_type(CTRL_ThreadMetaEval,      CTRL_MetaEval, .name = ("thread"));

//- rjf: meta evaluation array

struct CTRL_MetaEvalArray
{
  CTRL_MetaEval* v;
  uint64 count;
}
ptr_type(CTRL_MetaEvalArray__v_ptr_type, type(CTRL_BreakpointMetaEval), .count_delimiter_name = ("count"));
struct_members(CTRL_MetaEvalArray)
{
  {("v"), {0}, &CTRL_MetaEvalArray__v_ptr_type, OffsetOf(CTRL_MetaEvalArray, v)},
  member_lit_comp(CTRL_MetaEvalArray, type(uint64), count),
}
struct_type(CTRL_MetaEvalArray);

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
  uint64 count;
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
  uint32 rgba;
  CTRL_Handle handle;
  uint64 id;
  Rng1U64 vaddr_range;
  uint64 stack_base;
  uint64 timestamp;
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
  uint64 count;
}

struct CTRL_EntityArray
{
  CTRL_Entity** v;
  uint64 count;
}

struct CTRL_EntityRec
{
  CTRL_Entity* next;
  int32 push_count;
  int64 pop_count;
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
  uint64 size;
}

struct CTRL_EntityStore
{
  Arena* arena;
  CTRL_Entity* root;
  CTRL_Entity* free;
  CTRL_EntityHashSlot* hash_slots;
  CTRL_EntityHashNode* hash_node_free;
  uint64 hash_slots_count;
  CTRL_EntityStringChunkNode* free_string_chunks[8];
  uint64 entity_kind_counts[CTRL_EntityKind_COUNT];
  Arena* entity_kind_lists_arenas[CTRL_EntityKind_COUNT];
  uint64 entity_kind_lists_gens[CTRL_EntityKind_COUNT];
  uint64 entity_kind_alloc_gens[CTRL_EntityKind_COUNT];
  CTRL_EntityList entity_kind_lists[CTRL_EntityKind_COUNT];
}

////////////////////////////////
//~ rjf: Unwind Types

enum CTRL_UnwindFlags : uint32
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
  uint64 count;
}

struct CTRL_Unwind
{
  CTRL_UnwindFrameArray frames;
  CTRL_UnwindFlags flags;
}

////////////////////////////////
//~ rjf: Call Stack Types

struct CTRL_CallStackInlineFrame
{
  CTRL_CallStackInlineFrame* next;
  CTRL_CallStackInlineFrame* prev;
  RDI_InlineSite* inline_site;
}

struct CTRL_CallStackFrame
{
  CTRL_CallStackInlineFrame* first_inline_frame;
  CTRL_CallStackInlineFrame* last_inline_frame;
  uint64 inline_frame_count;
  void* regs;
  RDI_Parsed* rdi;
  RDI_Procedure* procedure;
}

struct CTRL_CallStack
{
  CTRL_CallStackFrame* frames;
  uint64 concrete_frame_count;
  uint64 inline_frame_count;
  uint64 total_frame_count;
}

////////////////////////////////
//~ rjf: Trap Types

enum CTRL_TrapFlags : uint32
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
  uint64 vaddr;
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
  uint64 count;
}

struct CTRL_Spoof
{
  DMN_Handle process;
  DMN_Handle thread;
  uint64 vaddr;
  uint64 new_ip_value;
}

////////////////////////////////
//~ rjf: User Breakpoint Types

enum CTRL_UserBreakpointKind
{
  CTRL_UserBreakpointKind_Null,
  CTRL_UserBreakpointKind_FileNameAndLineColNumber,
  CTRL_UserBreakpointKind_SymbolNameAndOffset,
  CTRL_UserBreakpointKind_VirtualAddress,
  CTRL_UserBreakpointKind_COUNT
}

struct CTRL_UserBreakpoint
{
  CTRL_UserBreakpointKind kind;
  String8 string;
  TxtPt pt;
  uint64 u64;
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
  uint64 count;
}

////////////////////////////////
//~ rjf: Evaluation Spaces

enum CTRL_EvalSpaceKind : uint64
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

enum CTRL_RunFlags : uint32
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
  CTRL_MetaEvalArray meta_evals;
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
  uint64 count;
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
  
  //- rjf: debug strings / decorations
  CTRL_EventKind_DebugString,
  CTRL_EventKind_ThreadName,
  CTRL_EventKind_ThreadColor,
  
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
  Rng1U64 vaddr_rng;
  uint64 rip_vaddr;
  uint64 stack_base;
  uint64 tls_root;
  uint64 timestamp;
  uint32 exception_code;
  uint32 rgba;
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
  uint64 count;
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
  uint64 mem_gen;
  uint64 last_time_requested_us;
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
  uint64 range_hash_slots_count;
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
  uint64 slots_count;
  CTRL_ProcessMemoryCacheSlot* slots;
  uint64 stripes_count;
  CTRL_ProcessMemoryCacheStripe* stripes;
}

struct CTRL_ProcessMemorySlice
{
  String8 data;
  uint64* byte_bad_flags;
  uint64* byte_changed_flags;
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
  uint64 block_size;
  void* block;
  uint64 reg_gen;
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
  uint64 slots_count;
  CTRL_ThreadRegCacheSlot* slots;
  uint64 stripes_count;
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
  uint64 pdatas_count;
  uint64 entry_point_voff;
  Rng1U64 tls_vaddr_range;
  String8 initial_debug_info_path;
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
  uint64 slots_count;
  CTRL_ModuleImageInfoCacheSlot* slots;
  uint64 stripes_count;
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
  uint64 search_count;
  uint64 child_count;
  uint64 module_direct_count;
}

////////////////////////////////
//~ rjf: Wakeup Hook Function Types

#define CTRL_WAKEUP_FUNCTION_DEF(name) void name()
typedef CTRL_WAKEUP_FUNCTION_DEF(CTRL_WakeupFunctionType);

////////////////////////////////
//~ rjf: Main State Types

struct CTRL_State
{
  Arena* arena;
  CTRL_WakeupFunctionType* wakeup_hook;
  
  // rjf: name . register/alias hash tables for eval
  E_String2NumMap arch_string2reg_tables[Arch_COUNT];
  E_String2NumMap arch_string2alias_tables[Arch_COUNT];
  
  // rjf: caches
  CTRL_ProcessMemoryCache process_memory_cache;
  CTRL_ThreadRegCache thread_reg_cache;
  CTRL_ModuleImageInfoCache module_image_info_cache;
  
  // rjf: user . ctrl msg ring buffer
  uint64 u2c_ring_size;
  uint8* u2c_ring_base;
  uint64 u2c_ring_write_pos;
  uint64 u2c_ring_read_pos;
  OS_Handle u2c_ring_mutex;
  OS_Handle u2c_ring_cv;
  
  // rjf: ctrl . user event ring buffer
  uint64 c2u_ring_size;
  uint64 c2u_ring_max_string_size;
  uint8* c2u_ring_base;
  uint64 c2u_ring_write_pos;
  uint64 c2u_ring_read_pos;
  OS_Handle c2u_ring_mutex;
  OS_Handle c2u_ring_cv;
  
  // rjf: ctrl thread state
  String8 ctrl_thread_log_path;
  OS_Handle ctrl_thread;
  Log* ctrl_thread_log;
  CTRL_EntityStore* ctrl_thread_entity_store;
  Arena* dmn_event_arena;
  DMN_EventNode* first_dmn_event_node;
  DMN_EventNode* last_dmn_event_node;
  DMN_EventNode* free_dmn_event_node;
  Arena* user_entry_point_arena;
  String8List user_entry_points;
  Arena* user_meta_eval_arena;
  CTRL_MetaEvalArray user_meta_evals;
  uint64 exception_code_filters[(CTRL_ExceptionCodeKind_COUNT+63)/64];
  uint64 process_counter;
  Arena* dbg_dir_arena;
  CTRL_DbgDirNode* dbg_dir_root;
  
  // rjf: user . memstream ring buffer
  uint64 u2ms_ring_size;
  uint8* u2ms_ring_base;
  uint64 u2ms_ring_write_pos;
  uint64 u2ms_ring_read_pos;
  OS_Handle u2ms_ring_mutex;
  OS_Handle u2ms_ring_cv;
}

////////////////////////////////
//~ rjf: Globals

static CTRL_State* ctrl_state = 0;
read_only static CTRL_Entity ctrl_entity_nil =
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
//~ rjf: Basic Type Functions

uint64 ctrl_hash_from_string(String8 string);
uint64 ctrl_hash_from_handle(CTRL_Handle handle);
CTRL_EventCause ctrl_event_cause_from_dmn_event_kind(DMN_EventKind event_kind);
String8 ctrl_string_from_event_kind(CTRL_EventKind kind);
String8 ctrl_string_from_msg_kind(CTRL_MsgKind kind);

////////////////////////////////
//~ rjf: Handle Type Functions

CTRL_Handle ctrl_handle_zero();
CTRL_Handle ctrl_handle_make(CTRL_MachineID machine_id, DMN_Handle dmn_handle);
B32 ctrl_handle_match(CTRL_Handle a, CTRL_Handle b);
void ctrl_handle_list_push(Arena* arena, CTRL_HandleList* list, CTRL_Handle* pair);
CTRL_HandleList ctrl_handle_list_copy(Arena* arena, CTRL_HandleList* src);

////////////////////////////////
//~ rjf: Trap Type Functions

void ctrl_trap_list_push(Arena* arena, CTRL_TrapList* list, CTRL_Trap* trap);
CTRL_TrapList ctrl_trap_list_copy(Arena* arena, CTRL_TrapList* src);

////////////////////////////////
//~ rjf: User Breakpoint Type Functions

void ctrl_user_breakpoint_list_push(Arena* arena, CTRL_UserBreakpointList* list, CTRL_UserBreakpoint* bp);
CTRL_UserBreakpointList ctrl_user_breakpoint_list_copy(Arena* arena, CTRL_UserBreakpointList* src);

////////////////////////////////
//~ rjf: Message Type Functions

//- rjf: deep copying
void ctrl_msg_deep_copy(Arena* arena, CTRL_Msg* dst, CTRL_Msg* src);

//- rjf: list building
CTRL_Msg* ctrl_msg_list_push(Arena* arena, CTRL_MsgList* list);
CTRL_MsgList ctrl_msg_list_deep_copy(Arena* arena, CTRL_MsgList* src);
void ctrl_msg_list_concat_in_place(CTRL_MsgList* dst, CTRL_MsgList* src);

//- rjf: serialization
String8 ctrl_serialized_string_from_msg_list(Arena* arena, CTRL_MsgList* msgs);
CTRL_MsgList ctrl_msg_list_from_serialized_string(Arena* arena, String8 string);

////////////////////////////////
//~ rjf: Event Type Functions

//- rjf: list building
CTRL_Event* ctrl_event_list_push(Arena* arena, CTRL_EventList* list);
void ctrl_event_list_concat_in_place(CTRL_EventList* dst, CTRL_EventList* to_push);

//- rjf: serialization
String8 ctrl_serialized_string_from_event(Arena* arena, CTRL_Event* event, uint64 max);
CTRL_Event ctrl_event_from_serialized_string(Arena* arena, String8 string);

////////////////////////////////
//~ rjf: Entity Type Functions

//- rjf: entity list data structures
void ctrl_entity_list_push(Arena* arena, CTRL_EntityList* list, CTRL_Entity* entity);
CTRL_EntityList ctrl_entity_list_from_handle_list(Arena* arena, CTRL_EntityStore* store, CTRL_HandleList* list);
#define ctrl_entity_list_first(list) ((list).first ? (list).first.v : &ctrl_entity_nil)

//- rjf: entity array data structure
CTRL_EntityArray ctrl_entity_array_from_list(Arena* arena, CTRL_EntityList* list);

//- rjf: cache creation/destruction
CTRL_EntityStore* ctrl_entity_store_alloc();
void ctrl_entity_store_release(CTRL_EntityStore* store);

//- rjf: string allocation/deletion
uint64 ctrl_name_bucket_idx_from_string_size(uint64 size);
String8 ctrl_entity_string_alloc(CTRL_EntityStore* store, String8 string);
void ctrl_entity_string_release(CTRL_EntityStore* store, String8 string);

//- rjf: entity construction/deletion
CTRL_Entity* ctrl_entity_alloc(CTRL_EntityStore* store, CTRL_Entity* parent, CTRL_EntityKind kind, Arch arch, CTRL_Handle handle, uint64 id);
void ctrl_entity_release(CTRL_EntityStore* store, CTRL_Entity* entity);

//- rjf: entity equipment
void ctrl_entity_equip_string(CTRL_EntityStore* store, CTRL_Entity* entity, String8 string);

//- rjf: entity store lookups
CTRL_Entity* ctrl_entity_from_handle(CTRL_EntityStore* store, CTRL_Handle handle);
CTRL_Entity* ctrl_entity_child_from_kind(CTRL_Entity* parent, CTRL_EntityKind kind);
CTRL_Entity* ctrl_entity_ancestor_from_kind(CTRL_Entity* entity, CTRL_EntityKind kind);
CTRL_Entity* ctrl_process_from_entity(CTRL_Entity* entity);
CTRL_Entity* ctrl_module_from_process_vaddr(CTRL_Entity* process, uint64 vaddr);
DI_Key ctrl_dbgi_key_from_module(CTRL_Entity* module);
CTRL_EntityList ctrl_modules_from_dbgi_key(Arena* arena, CTRL_EntityStore* store, DI_Key* dbgi_key);
CTRL_Entity* ctrl_module_from_thread_candidates(CTRL_EntityStore* store, CTRL_Entity* thread, CTRL_EntityList* candidates);
CTRL_EntityList ctrl_entity_list_from_kind(CTRL_EntityStore* store, CTRL_EntityKind kind);
uint64 ctrl_vaddr_from_voff(CTRL_Entity* module, uint64 voff);
uint64 ctrl_voff_from_vaddr(CTRL_Entity* module, uint64 vaddr);
Rng1U64 ctrl_vaddr_range_from_voff_range(CTRL_Entity* module, Rng1U64 voff_range);
Rng1U64 ctrl_voff_range_from_vaddr_range(CTRL_Entity* module, Rng1U64 vaddr_range);
B32 ctrl_entity_tree_is_frozen(CTRL_Entity* root);

//- rjf: entity tree iteration
CTRL_EntityRec ctrl_entity_rec_depth_first(CTRL_Entity* entity, CTRL_Entity* subtree_root, uint64 sib_off, uint64 child_off);
#define ctrl_entity_rec_depth_first_pre(entity, subtree_root)  ctrl_entity_rec_depth_first((entity), (subtree_root), OffsetOf(CTRL_Entity, next), OffsetOf(CTRL_Entity, first))
#define ctrl_entity_rec_depth_first_post(entity, subtree_root) ctrl_entity_rec_depth_first((entity), (subtree_root), OffsetOf(CTRL_Entity, prev), OffsetOf(CTRL_Entity, last))

//- rjf: applying events to entity caches
void ctrl_entity_store_apply_events(CTRL_EntityStore* store, CTRL_EventList* list);

////////////////////////////////
//~ rjf: Main Layer Initialization

void ctrl_init();

////////////////////////////////
//~ rjf: Wakeup Callback Registration

void ctrl_set_wakeup_hook(CTRL_WakeupFunctionType* wakeup_hook);

////////////////////////////////
//~ rjf: Process Memory Functions

//- rjf: process memory cache interaction
U128 ctrl_calc_hash_store_key_from_process_vaddr_range(CTRL_Handle process, Rng1U64 range, B32 zero_terminated);
U128 ctrl_stored_hash_from_process_vaddr_range(CTRL_Handle process, Rng1U64 range, B32 zero_terminated, B32* out_is_stale, uint64 endt_us);

//- rjf: bundled key/stream helper
U128 ctrl_hash_store_key_from_process_vaddr_range(CTRL_Handle process, Rng1U64 range, B32 zero_terminated);

//- rjf: process memory cache reading helpers
CTRL_ProcessMemorySlice ctrl_query_cached_data_from_process_vaddr_range(Arena* arena, CTRL_Handle process, Rng1U64 range, uint64 endt_us);
CTRL_ProcessMemorySlice ctrl_query_cached_zero_terminated_data_from_process_vaddr_limit(Arena* arena, CTRL_Handle process, uint64 vaddr, uint64 limit, uint64 element_size, uint64 endt_us);
B32 ctrl_read_cached_process_memory(CTRL_Handle process, Rng1U64 range, B32* is_stale_out, void* out, uint64 endt_us);
#define ctrl_read_cached_process_memory_struct(process, vaddr, is_stale_out, ptr, endt_us) ctrl_read_cached_process_memory((process), r1u64((vaddr), (vaddr)+(sizeof(*(ptr)))), (is_stale_out), (ptr), (endt_us))

//- rjf: process memory writing
B32 ctrl_process_write(CTRL_Handle process, Rng1U64 range, void* src);

////////////////////////////////
//~ rjf: Thread Register Functions

//- rjf: thread register cache reading
void* ctrl_query_cached_reg_block_from_thread(Arena* arena, CTRL_EntityStore* store, CTRL_Handle handle);
uint64 ctrl_query_cached_tls_root_vaddr_from_thread(CTRL_EntityStore* store, CTRL_Handle handle);
uint64 ctrl_query_cached_rip_from_thread(CTRL_EntityStore* store, CTRL_Handle handle);
uint64 ctrl_query_cached_rsp_from_thread(CTRL_EntityStore* store, CTRL_Handle handle);

//- rjf: thread register writing
B32 ctrl_thread_write_reg_block(CTRL_Handle thread, void* block);

////////////////////////////////
//~ rjf: Module Image Info Functions

//- rjf: cache lookups
PE_IntelPdata* ctrl_intel_pdata_from_module_voff(Arena* arena, CTRL_Handle module_handle, uint64 voff);
uint64 ctrl_entry_point_voff_from_module(CTRL_Handle module_handle);
Rng1U64 ctrl_tls_vaddr_range_from_module(CTRL_Handle module_handle);
String8 ctrl_initial_debug_info_path_from_module(Arena* arena, CTRL_Handle module_handle);

////////////////////////////////
//~ rjf: Unwinding Functions

//- rjf: unwind deep copier
CTRL_Unwind ctrl_unwind_deep_copy(Arena* arena, Arch arch, CTRL_Unwind* src);

//- rjf: [x64]
REGS_Reg64* ctrl_unwind_reg_from_pe_gpr_reg__pe_x64(REGS_RegBlockX64* regs, PE_UnwindGprRegX64 gpr_reg);
CTRL_UnwindStepResult ctrl_unwind_step__pe_x64(CTRL_EntityStore* store, CTRL_Handle process_handle, CTRL_Handle module_handle, REGS_RegBlockX64* regs, uint64 endt_us);

//- rjf: abstracted unwind step
CTRL_UnwindStepResult ctrl_unwind_step(CTRL_EntityStore* store, CTRL_Handle process, CTRL_Handle module, Arch arch, void* reg_block, uint64 endt_us);

//- rjf: abstracted full unwind
CTRL_Unwind ctrl_unwind_from_thread(Arena* arena, CTRL_EntityStore* store, CTRL_Handle thread, uint64 endt_us);

////////////////////////////////
//~ rjf: Call Stack Building Functions

CTRL_CallStack ctrl_call_stack_from_unwind(Arena* arena, DI_Scope* di_scope, CTRL_Entity* process, CTRL_Unwind* base_unwind);

////////////////////////////////
//~ rjf: Halting All Attached Processes

void ctrl_halt();

////////////////////////////////
//~ rjf: Shared Accessor Functions

//- rjf: generation counters
uint64 ctrl_run_gen();
uint64 ctrl_mem_gen();
uint64 ctrl_reg_gen();

//- rjf: name . register/alias hash tables, for eval
E_String2NumMap* ctrl_string2reg_from_arch(Arch arch);
E_String2NumMap* ctrl_string2alias_from_arch(Arch arch);

////////////////////////////////
//~ rjf: Control-Thread Functions

//- rjf: user . control thread communication
B32 ctrl_u2c_push_msgs(CTRL_MsgList* msgs, uint64 endt_us);
CTRL_MsgList ctrl_u2c_pop_msgs(Arena* arena);

//- rjf: control . user thread communication
void ctrl_c2u_push_events(CTRL_EventList* events);
CTRL_EventList ctrl_c2u_pop_events(Arena* arena);

//- rjf: entry point
void ctrl_thread__entry_point(void* p);

//- rjf: breakpoint resolution
void ctrl_thread__append_resolved_module_user_bp_traps(Arena* arena, CTRL_Handle process, CTRL_Handle module, CTRL_UserBreakpointList* user_bps, DMN_TrapChunkList* traps_out);
void ctrl_thread__append_resolved_process_user_bp_traps(Arena* arena, CTRL_Handle process, CTRL_UserBreakpointList* user_bps, DMN_TrapChunkList* traps_out);

//- rjf: module lifetime open/close work
void ctrl_thread__module_open(CTRL_Handle process, CTRL_Handle module, Rng1U64 vaddr_range, String8 path);
void ctrl_thread__module_close(CTRL_Handle module);

//- rjf: attached process running/event gathering
DMN_Event* ctrl_thread__next_dmn_event(Arena* arena, DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg, DMN_RunCtrls* run_ctrls, CTRL_Spoof* spoof);

//- rjf: eval helpers
B32 ctrl_eval_space_read(void* u, E_Space space, void* out, Rng1U64 vaddr_range);

//- rjf: log flusher
void ctrl_thread__flush_info_log(String8 string);
void ctrl_thread__end_and_flush_info_log();

//- rjf: msg kind implementations
void ctrl_thread__launch(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);
void ctrl_thread__attach(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);
void ctrl_thread__kill(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);
void ctrl_thread__kill_all(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);
void ctrl_thread__detach(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);
void ctrl_thread__run(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);
void ctrl_thread__single_step(DMN_CtrlCtx* ctrl_ctx, CTRL_Msg* msg);

////////////////////////////////
//~ rjf: Memory-Stream Thread Functions

//- rjf: user . memory stream communication
B32 ctrl_u2ms_enqueue_req(CTRL_Handle process, Rng1U64 vaddr_range, B32 zero_terminated, uint64 endt_us);
void ctrl_u2ms_dequeue_req(CTRL_Handle* out_process, Rng1U64* out_vaddr_range, B32* out_zero_terminated);

//- rjf: entry point
ASYNC_WORK_DEF(ctrl_mem_stream_work);
void ctrl_mem_stream_thread__entry_point(void* p);

#endif // CTRL_CORE_H

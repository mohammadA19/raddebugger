// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Control-Thread-Only Context
//
// An instance of this struct must ONLY be returned by dmn_ctrl_begin, and only
// used by the thread which called it. All APIs which can ONLY run on the
// control thread, which blocks to control & receive events, will take this
// parameter. All other APIs can be called from any thread.

struct DMN_CtrlCtx
{
  U64[1] u64;
}

////////////////////////////////
//~ rjf: Handle Types

union DMN_Handle
{
  U32[2] u32;
  U64[1] u64;
}

struct DMN_HandleNode
{
  DMN_HandleNode* next;
  DMN_Handle v;
}

struct DMN_HandleList
{
  DMN_HandleNode* first;
  DMN_HandleNode* last;
  U64 count;
}

struct DMN_HandleArray
{
  DMN_Handle* handles;
  U64 count;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/demon.meta.h"

////////////////////////////////
//~ rjf: Event Types

struct DMN_Event
{
  DMN_EventKind kind;
  DMN_ErrorKind error_kind;
  DMN_MemoryEventKind memory_kind;
  DMN_ExceptionKind exception_kind;
  DMN_Handle process;
  DMN_Handle thread;
  DMN_Handle module;
  Arch arch;
  U64 address;
  U64 size;
  String8 string;
  U32 code; // code gives pid & tid on CreateProcess and CreateThread (respectfully)
  U32 flags; // DMN_TrapFlags, if `DMN_EventKind_SetBreakpoint`
  S32 signo;
  S32 sigcode;
  U64 instruction_pointer;
  U64 stack_pointer;
  U64 user_data;
  B32 exception_repeated;
}

struct DMN_EventNode
{
  DMN_EventNode* next;
  DMN_Event v;
}

struct DMN_EventList
{
  DMN_EventNode* first;
  DMN_EventNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Run Control Types

enum DMN_TrapFlags : U32
{
  DMN_TrapFlag_BreakOnWrite   = (1<<0),
  DMN_TrapFlag_BreakOnRead    = (1<<1),
  DMN_TrapFlag_BreakOnExecute = (1<<2),
}

struct DMN_Trap
{
  DMN_Handle process;
  U64 vaddr;
  U64 id;
  DMN_TrapFlags flags;
  U32 size;
}

struct DMN_TrapChunkNode
{
  DMN_TrapChunkNode* next;
  DMN_Trap* v;
  U64 cap;
  U64 count;
}

struct DMN_TrapChunkList
{
  DMN_TrapChunkNode* first;
  DMN_TrapChunkNode* last;
  U64 node_count;
  U64 trap_count;
}

struct DMN_RunCtrls
{
  DMN_Handle single_step_thread;
  B8 ignore_previous_exception;
  B8 run_entities_are_unfrozen;
  B8 run_entities_are_processes;
  DMN_Handle* run_entities;
  U64 run_entity_count;
  DMN_TrapChunkList traps;
}

////////////////////////////////
//~ rjf: System Process Listing Types

struct DMN_ProcessIter
{
  U64[2] v;
}

struct DMN_ProcessInfo
{
  String8 name;
  U32 pid;
}

////////////////////////////////
//~ rjf: @dmn_os_hooks Blocking Control Thread Operations (Implemented Per-OS)

#define DMN_CtrlExclusiveAccessScope DeferLoop(dmn_ctrl_exclusive_access_begin(), dmn_ctrl_exclusive_access_end())

////////////////////////////////
//~ rjf: @dmn_os_hooks Introspection Functions (Implemented Per-OS)
//- rjf: non-blocking-control-thread access barriers
#define DMN_AccessScope DeferLoopChecked(dmn_access_open(), dmn_access_close())

//- rjf: processes
#define dmn_process_read_struct(process, vaddr, ptr) dmn_process_read((process), r1u64((vaddr), (vaddr)+(sizeof(*ptr))), ptr)
#define dmn_process_write_struct(process, vaddr, ptr) dmn_process_write((process), r1u64((vaddr), (vaddr)+(sizeof(*ptr))), ptr)
//- rjf: system process listing


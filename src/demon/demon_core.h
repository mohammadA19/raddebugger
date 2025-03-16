// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DEMON_CORE_H
#define DEMON_CORE_H

////////////////////////////////
//~ rjf: Control-Thread-Only Context
//
// An instance of this struct must ONLY be returned by dmn_ctrl_begin, and only
// used by the thread which called it. All APIs which can ONLY run on the
// control thread, which blocks to control & receive events, will take this
// parameter. All other APIs can be called from any thread.

struct DMN_CtrlCtx
{
  uint64 u64[1];
}

////////////////////////////////
//~ rjf: Handle Types

[Union]
struct DMN_Handle
{
  uint32 u32[2];
  uint64 u64[1];
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
  uint64 count;
}

struct DMN_HandleArray
{
  DMN_Handle* handles;
  uint64 count;
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
  uint64 address;
  uint64 size;
  String8 string;
  uint32 code; // code gives pid & tid on CreateProcess and CreateThread (respectfully)
  uint32 flags;
  int32 signo;
  int32 sigcode;
  uint64 instruction_pointer;
  uint64 stack_pointer;
  uint64 user_data;
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
  uint64 count;
}

////////////////////////////////
//~ rjf: Run Control Types

struct DMN_Trap
{
  DMN_Handle process;
  uint64 vaddr;
  uint64 id;
}

struct DMN_TrapChunkNode
{
  DMN_TrapChunkNode* next;
  DMN_Trap* v;
  uint64 cap;
  uint64 count;
}

struct DMN_TrapChunkList
{
  DMN_TrapChunkNode* first;
  DMN_TrapChunkNode* last;
  uint64 node_count;
  uint64 trap_count;
}

struct DMN_RunCtrls
{
  DMN_Handle single_step_thread;
  B8 ignore_previous_exception;
  B8 run_entities_are_unfrozen;
  B8 run_entities_are_processes;
  DMN_Handle* run_entities;
  uint64 run_entity_count;
  DMN_TrapChunkList traps;
}

////////////////////////////////
//~ rjf: System Process Listing Types

struct DMN_ProcessIter
{
  uint64 v[2];
}

struct DMN_ProcessInfo
{
  String8 name;
  uint32 pid;
}

////////////////////////////////
//~ rjf: Basic Type Functions (Helpers, Implemented Once)

//- rjf: handles
DMN_Handle dmn_handle_zero();
B32 dmn_handle_match(DMN_Handle a, DMN_Handle b);

//- rjf: trap chunk lists
void dmn_trap_chunk_list_push(Arena* arena, DMN_TrapChunkList* list, uint64 cap, DMN_Trap* trap);
void dmn_trap_chunk_list_concat_in_place(DMN_TrapChunkList* dst, DMN_TrapChunkList* to_push);
void dmn_trap_chunk_list_concat_shallow_copy(Arena* arena, DMN_TrapChunkList* dst, DMN_TrapChunkList* to_push);

//- rjf: handle lists
void dmn_handle_list_push(Arena* arena, DMN_HandleList* list, DMN_Handle handle);
DMN_HandleArray dmn_handle_array_from_list(Arena* arena, DMN_HandleList* list);
DMN_HandleArray dmn_handle_array_copy(Arena* arena, DMN_HandleArray* src);

//- rjf: event list building
DMN_Event* dmn_event_list_push(Arena* arena, DMN_EventList* list);

////////////////////////////////
//~ rjf: Thread Reading Helper Functions (Helpers, Implemented Once)

uint64 dmn_rip_from_thread(DMN_Handle thread);
uint64 dmn_rsp_from_thread(DMN_Handle thread);

////////////////////////////////
//~ rjf: @dmn_os_hooks Main Layer Initialization (Implemented Per-OS)

void dmn_init();

////////////////////////////////
//~ rjf: @dmn_os_hooks Blocking Control Thread Operations (Implemented Per-OS)

DMN_CtrlCtx* dmn_ctrl_begin();
void dmn_ctrl_exclusive_access_begin();
void dmn_ctrl_exclusive_access_end();
#define DMN_CtrlExclusiveAccessScope DeferLoop(dmn_ctrl_exclusive_access_begin(), dmn_ctrl_exclusive_access_end())
uint32 dmn_ctrl_launch(DMN_CtrlCtx* ctx, OS_ProcessLaunchParams* params);
B32 dmn_ctrl_attach(DMN_CtrlCtx* ctx, uint32 pid);
B32 dmn_ctrl_kill(DMN_CtrlCtx* ctx, DMN_Handle process, uint32 exit_code);
B32 dmn_ctrl_detach(DMN_CtrlCtx* ctx, DMN_Handle process);
DMN_EventList dmn_ctrl_run(Arena* arena, DMN_CtrlCtx* ctx, DMN_RunCtrls* ctrls);

////////////////////////////////
//~ rjf: @dmn_os_hooks Halting (Implemented Per-OS)

void dmn_halt(uint64 code, uint64 user_data);

////////////////////////////////
//~ rjf: @dmn_os_hooks Introspection Functions (Implemented Per-OS)

//- rjf: run/memory/register counters
uint64 dmn_run_gen();
uint64 dmn_mem_gen();
uint64 dmn_reg_gen();

//- rjf: non-blocking-control-thread access barriers
B32 dmn_access_open();
void dmn_access_close();
#define DMN_AccessScope DeferLoopChecked(dmn_access_open(), dmn_access_close())

//- rjf: processes
uint64 dmn_process_memory_reserve(DMN_Handle process, uint64 vaddr, uint64 size);
void dmn_process_memory_commit(DMN_Handle process, uint64 vaddr, uint64 size);
void dmn_process_memory_decommit(DMN_Handle process, uint64 vaddr, uint64 size);
void dmn_process_memory_release(DMN_Handle process, uint64 vaddr, uint64 size);
void dmn_process_memory_protect(DMN_Handle process, uint64 vaddr, uint64 size, OS_AccessFlags flags);
uint64 dmn_process_read(DMN_Handle process, Rng1U64 range, void* dst);
B32 dmn_process_write(DMN_Handle process, Rng1U64 range, void* src);
#define dmn_process_read_struct(process, vaddr, ptr) dmn_process_read((process), r1u64((vaddr), (vaddr)+(sizeof(*ptr))), ptr)
#define dmn_process_write_struct(process, vaddr, ptr) dmn_process_write((process), r1u64((vaddr), (vaddr)+(sizeof(*ptr))), ptr)
String8 dmn_process_read_cstring(Arena* arena, DMN_Handle process, uint64 addr);

//- rjf: threads
Arch dmn_arch_from_thread(DMN_Handle handle);
uint64 dmn_stack_base_vaddr_from_thread(DMN_Handle handle);
uint64 dmn_tls_root_vaddr_from_thread(DMN_Handle handle);
B32 dmn_thread_read_reg_block(DMN_Handle handle, void* reg_block);
B32 dmn_thread_write_reg_block(DMN_Handle handle, void* reg_block);

//- rjf: system process listing
void dmn_process_iter_begin(DMN_ProcessIter* iter);
B32  dmn_process_iter_next(Arena* arena, DMN_ProcessIter* iter, DMN_ProcessInfo* info_out);
void dmn_process_iter_end(DMN_ProcessIter* iter);

#endif // DEMON_CORE_H

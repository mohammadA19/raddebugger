// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: @dmn_os_hooks Main Layer Initialization (Implemented Per-OS)

void
dmn_init(void)
{
}

////////////////////////////////
//~ rjf: @dmn_os_hooks Blocking Control Thread Operations (Implemented Per-OS)

DMN_CtrlCtx *
dmn_ctrl_begin(void)
{
}

void
dmn_ctrl_exclusive_access_begin(void)
{
}

void
dmn_ctrl_exclusive_access_end(void)
{
}

U32
dmn_ctrl_launch(DMN_CtrlCtx *ctx, OS_ProcessLaunchParams *params)
{
}

B32
dmn_ctrl_attach(DMN_CtrlCtx *ctx, U32 pid)
{
}

B32
dmn_ctrl_kill(DMN_CtrlCtx *ctx, DMN_Handle process, U32 exit_code)
{
}

B32
dmn_ctrl_detach(DMN_CtrlCtx *ctx, DMN_Handle process)
{
}

DMN_EventList
dmn_ctrl_run(Arena *arena, DMN_CtrlCtx *ctx, DMN_RunCtrls *ctrls)
{
}

////////////////////////////////
//~ rjf: @dmn_os_hooks Halting (Implemented Per-OS)

void
dmn_halt(U64 code, U64 user_data)
{
}

////////////////////////////////
//~ rjf: @dmn_os_hooks Introspection Functions (Implemented Per-OS)

//- rjf: run/memory/register counters

U64
dmn_run_gen(void)
{
}

U64
dmn_mem_gen(void)
{
}

U64
dmn_reg_gen(void)
{
}

//- rjf: non-blocking-control-thread access barriers

B32
dmn_access_open(void)
{
}

void
dmn_access_close(void)
{
}

//- rjf: processes

U64
dmn_process_memory_reserve(DMN_Handle process, U64 vaddr, U64 size)
{
}

void
dmn_process_memory_commit(DMN_Handle process, U64 vaddr, U64 size)
{
}

void
dmn_process_memory_decommit(DMN_Handle process, U64 vaddr, U64 size)
{
}

void
dmn_process_memory_release(DMN_Handle process, U64 vaddr, U64 size)
{
}

void
dmn_process_memory_protect(DMN_Handle process, U64 vaddr, U64 size, OS_AccessFlags flags)
{
}

U64
dmn_process_read(DMN_Handle process, Rng1U64 range, void *dst)
{
}

B32
dmn_process_write(DMN_Handle process, Rng1U64 range, void *src)
{
}

//- rjf: threads

Arch
dmn_arch_from_thread(DMN_Handle handle)
{
}

U64
dmn_stack_base_vaddr_from_thread(DMN_Handle handle)
{
}

U64
dmn_tls_root_vaddr_from_thread(DMN_Handle handle)
{
}

B32
dmn_thread_read_reg_block(DMN_Handle handle, void *reg_block)
{
}

B32
dmn_thread_write_reg_block(DMN_Handle handle, void *reg_block)
{
}

//- rjf: system process listing

void
dmn_process_iter_begin(DMN_ProcessIter *iter)
{
}

B32
dmn_process_iter_next(Arena *arena, DMN_ProcessIter *iter, DMN_ProcessInfo *info_out)
{
}

void
dmn_process_iter_end(DMN_ProcessIter *iter)
{
}

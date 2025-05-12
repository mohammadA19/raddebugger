// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

// TODO(allen): Potential Upgrades:
//
// memory fd upgrade - Right now for each process we hold open a file
//  descriptor for the process's memory (/proc/%d/mem) for the entire lifetime
//  of the process; it could be opened and closed with some kind of LRU cache
//  to put a finite cap on the number of handles the demon holds
//

////////////////////////////////
//~ NOTE(allen): Get The Linux Includes

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <elf.h>
#include <dirent.h>
#include <errno.h>

////////////////////////////////
//~ NOTE(allen): Linux Demon Types

//- entities

// Demon Linux Entity Extensions
//  Process: ext_u64 set to memory file descriptor
//  Thread : ext_u64 cast to DEMON_LNX_ThreadExt
//  Module : ext_u64 set to U64 (address of name)

struct DEMON_LNX_ThreadExt{
  B32 expecting_dummy_sigstop;
}
StaticAssert(sizeof(DEMON_LNX_ThreadExt) <= sizeof(Member(DEMON_Entity, ext_u64)), check_demon_lnx_thread_ext);

//- helpers

struct DEMON_LNX_AttachNode{
  DEMON_LNX_AttachNode *next;
  pid_t pid;
}

struct DEMON_LNX_ProcessAux{
  B32 filled;
  U64 phnum;
  U64 phent;
  U64 phdr;
  U64 execfn;
}

struct DEMON_LNX_PhdrInfo{
  Rng1U64 range;
  U64 dynamic;
}

struct DEMON_LNX_ModuleNode{
  DEMON_LNX_ModuleNode *next;
  U64 vaddr;
  U64 size;
  U64 name;
  U64 already_known;
}

struct DEMON_LNX_EntityNode{
  DEMON_LNX_EntityNode *next;
  DEMON_Entity *entity;
}

////////////////////////////////
//~ NOTE(allen): Linux Demon Register Layouts

// these are defined in <sys/user.h> but only for one architecture at a time
//  (and we can't really trick it into giving us both in any obvious way)
// we define them here so that we have them all "at once"

struct DEMON_LNX_UserRegsX64{
  U64 r15;
	U64 r14;
	U64 r13;
	U64 r12;
	U64 rbp;
	U64 rbx;
	U64 r11;
	U64 r10;
	U64 r9;
	U64 r8;
	U64 rax;
	U64 rcx;
	U64 rdx;
	U64 rsi;
	U64 rdi;
	U64 orig_rax;
	U64 rip;
	U64 cs;
	U64 rflags;
	U64 rsp;
	U64 ss;
	U64 fsbase;
	U64 gsbase;
	U64 ds;
	U64 es;
	U64 fs;
	U64 gs;
}

struct DEMON_LNX_UserX64{
  DEMON_LNX_UserRegsX64 regs;
  S32 u_fpvalid, _pad0;
  SYMS_XSaveLegacy i387;
  U64 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  U64 signal;
  S32 reserved, _pad1;
  U64 u_ar0, u_fpstate;
  U64 magic;
  U8  u_comm[32];
  U64 u_debugreg[8];
}

struct DEMON_LNX_UserRegsX86{
  U32 ebx;
	U32 ecx;
	U32 edx;
	U32 esi;
	U32 edi;
	U32 ebp;
	U32 eax;
	U32 ds;
	U32 es;
	U32 fs;
	U32 gs;
	U32 orig_eax;
	U32 eip;
	U32 cs;
	U32 eflags;
	U32 sp;
	U32 ss;
}

struct DEMON_LNX_UserX86{
  DEMON_LNX_UserRegsX86 regs;
  S32 u_fpvalid;
  SYMS_FSave i387;
  U32 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  S32 signal, reserved;
  U32 u_ar0, u_fpstate;
  U32 magic;
  U8  u_comm[32];
  U32 u_debugreg[8];
}

////////////////////////////////

enum
{
  DEMON_LNX_PermFlags_Read    = (1 << 0),
  DEMON_LNX_PermFlags_Write   = (1 << 1),
  DEMON_LNX_PermFlags_Exec    = (1 << 2),
  DEMON_LNX_PermFlags_Private = (1 << 3)
}
typedef int DEMON_LNX_PermFlags;

enum
{
  DEMON_LNX_MapsEntryType_Null,
  DEMON_LNX_MapsEntryType_Path,
  DEMON_LNX_MapsEntryType_Heap,
  DEMON_LNX_MapsEntryType_Stack,
  DEMON_LNX_MapsEntryType_VDSO,
}
typedef int DEMON_LNX_MapsEntryType;

struct DEMON_LNX_MapsEntry
{
  U64 address_lo;
  U64 address_hi;
  DEMON_LNX_PermFlags perms;
  U64 offset;
  U32 dev_major;
  U32 dev_minor;
  U64 inode;
  String8 pathname;
  DEMON_LNX_MapsEntryType type;
  pid_t stack_tid;
}

////////////////////////////////
//~ rjf: Helpers




                                                               U64 phvaddr, U64 phstride, U64 phcount);





#endif //DEMON_OS_LINUX_H

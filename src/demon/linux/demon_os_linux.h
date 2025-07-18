// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DEMON_OS_LINUX_H
#define DEMON_OS_LINUX_H

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
//  Module : ext_u64 set to uint64 (address of name)

struct DEMON_LNX_ThreadExt{
  B32 expecting_dummy_sigstop;
};
StaticAssert(sizeof(DEMON_LNX_ThreadExt) <= sizeof(Member(DEMON_Entity, ext_u64)), check_demon_lnx_thread_ext);

//- helpers

struct DEMON_LNX_AttachNode{
  DEMON_LNX_AttachNode *next;
  pid_t pid;
};

struct DEMON_LNX_ProcessAux{
  B32 filled;
  uint64 phnum;
  uint64 phent;
  uint64 phdr;
  uint64 execfn;
};

struct DEMON_LNX_PhdrInfo{
  Rng1<uint64> range;
  uint64 dynamic;
};

struct DEMON_LNX_ModuleNode{
  DEMON_LNX_ModuleNode *next;
  uint64 vaddr;
  uint64 size;
  uint64 name;
  uint64 already_known;
};

struct DEMON_LNX_EntityNode{
  DEMON_LNX_EntityNode *next;
  DEMON_Entity *entity;
};

////////////////////////////////
//~ NOTE(allen): Linux Demon Register Layouts

// these are defined in <sys/user.h> but only for one architecture at a time
//  (and we can't really trick it into giving us both in any obvious way)
// we define them here so that we have them all "at once"

struct DEMON_LNX_UserRegsX64{
  uint64 r15;
	uint64 r14;
	uint64 r13;
	uint64 r12;
	uint64 rbp;
	uint64 rbx;
	uint64 r11;
	uint64 r10;
	uint64 r9;
	uint64 r8;
	uint64 rax;
	uint64 rcx;
	uint64 rdx;
	uint64 rsi;
	uint64 rdi;
	uint64 orig_rax;
	uint64 rip;
	uint64 cs;
	uint64 rflags;
	uint64 rsp;
	uint64 ss;
	uint64 fsbase;
	uint64 gsbase;
	uint64 ds;
	uint64 es;
	uint64 fs;
	uint64 gs;
};

struct DEMON_LNX_UserX64{
  DEMON_LNX_UserRegsX64 regs;
  uint32 u_fpvalid, _pad0;
  SYMS_XSaveLegacy i387;
  uint64 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  uint64 signal;
  uint32 reserved, _pad1;
  uint64 u_ar0, u_fpstate;
  uint64 magic;
  uint8  u_comm[32];
  uint64 u_debugreg[8];
};

struct DEMON_LNX_UserRegsX86{
  uint32 ebx;
	uint32 ecx;
	uint32 edx;
	uint32 esi;
	uint32 edi;
	uint32 ebp;
	uint32 eax;
	uint32 ds;
	uint32 es;
	uint32 fs;
	uint32 gs;
	uint32 orig_eax;
	uint32 eip;
	uint32 cs;
	uint32 eflags;
	uint32 sp;
	uint32 ss;
};

struct DEMON_LNX_UserX86{
  DEMON_LNX_UserRegsX86 regs;
  uint32 u_fpvalid;
  SYMS_FSave i387;
  uint32 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  uint32 signal, reserved;
  uint32 u_ar0, u_fpstate;
  uint32 magic;
  uint8  u_comm[32];
  uint32 u_debugreg[8];
};

////////////////////////////////

enum
{
  DEMON_LNX_PermFlags_Read    = (1 << 0),
  DEMON_LNX_PermFlags_Write   = (1 << 1),
  DEMON_LNX_PermFlags_Exec    = (1 << 2),
  DEMON_LNX_PermFlags_Private = (1 << 3)
};
typedef int DEMON_LNX_PermFlags;

enum
{
  DEMON_LNX_MapsEntryType_Null,
  DEMON_LNX_MapsEntryType_Path,
  DEMON_LNX_MapsEntryType_Heap,
  DEMON_LNX_MapsEntryType_Stack,
  DEMON_LNX_MapsEntryType_VDSO,
};
typedef int DEMON_LNX_MapsEntryType;

struct DEMON_LNX_MapsEntry
{
  uint64 address_lo;
  uint64 address_hi;
  DEMON_LNX_PermFlags perms;
  uint64 offset;
  uint32 dev_major;
  uint32 dev_minor;
  uint64 inode;
  String8 pathname;
  DEMON_LNX_MapsEntryType type;
  pid_t stack_tid;
};

////////////////////////////////
//~ rjf: Helpers




internal DEMON_LNX_PhdrInfo    demon_lnx_phdr_info_from_memory(int memory_fd, B32 is_32bit,
                                                               uint64 phvaddr, uint64 phstride, uint64 phcount);





#endif //DEMON_OS_LINUX_H

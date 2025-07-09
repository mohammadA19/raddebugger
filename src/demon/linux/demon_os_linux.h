// Copyright (c) Epic Games Tools
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
//  Module : ext_u64 set to u64 (address of name)

struct DEMON_LNX_ThreadExt{
  b32 expecting_dummy_sigstop;
};
StaticAssert(sizeof(DEMON_LNX_ThreadExt) <= sizeof(Member(DEMON_Entity, ext_u64)), check_demon_lnx_thread_ext);

//- helpers

struct DEMON_LNX_AttachNode{
  DEMON_LNX_AttachNode *next;
  pid_t pid;
};

struct DEMON_LNX_ProcessAux{
  b32 filled;
  u64 phnum;
  u64 phent;
  u64 phdr;
  u64 execfn;
};

struct DEMON_LNX_PhdrInfo{
  Rng1U64 range;
  u64 dynamic;
};

struct DEMON_LNX_ModuleNode{
  DEMON_LNX_ModuleNode *next;
  u64 vaddr;
  u64 size;
  u64 name;
  u64 already_known;
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
  u64 r15;
	u64 r14;
	u64 r13;
	u64 r12;
	u64 rbp;
	u64 rbx;
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rax;
	u64 rcx;
	u64 rdx;
	u64 rsi;
	u64 rdi;
	u64 orig_rax;
	u64 rip;
	u64 cs;
	u64 rflags;
	u64 rsp;
	u64 ss;
	u64 fsbase;
	u64 gsbase;
	u64 ds;
	u64 es;
	u64 fs;
	u64 gs;
};

struct DEMON_LNX_UserX64{
  DEMON_LNX_UserRegsX64 regs;
  i32 u_fpvalid, _pad0;
  SYMS_XSaveLegacy i387;
  u64 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  u64 signal;
  i32 reserved, _pad1;
  u64 u_ar0, u_fpstate;
  u64 magic;
  u8  u_comm[32];
  u64 u_debugreg[8];
};

struct DEMON_LNX_UserRegsX86{
  u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esi;
	u32 edi;
	u32 ebp;
	u32 eax;
	u32 ds;
	u32 es;
	u32 fs;
	u32 gs;
	u32 orig_eax;
	u32 eip;
	u32 cs;
	u32 eflags;
	u32 sp;
	u32 ss;
};

struct DEMON_LNX_UserX86{
  DEMON_LNX_UserRegsX86 regs;
  i32 u_fpvalid;
  SYMS_FSave i387;
  u32 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  i32 signal, reserved;
  u32 u_ar0, u_fpstate;
  u32 magic;
  u8  u_comm[32];
  u32 u_debugreg[8];
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
  u64 address_lo;
  u64 address_hi;
  DEMON_LNX_PermFlags perms;
  u64 offset;
  u32 dev_major;
  u32 dev_minor;
  u64 inode;
  String8 pathname;
  DEMON_LNX_MapsEntryType type;
  pid_t stack_tid;
};

////////////////////////////////
//~ rjf: Helpers










internal DEMON_LNX_PhdrInfo    demon_lnx_phdr_info_from_memory(int memory_fd, b32 is_32bit,
                                                               u64 phvaddr, u64 phstride, u64 phcount);

















#endif //DEMON_OS_LINUX_H

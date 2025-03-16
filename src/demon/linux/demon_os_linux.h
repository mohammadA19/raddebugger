// Copyright (c) 2024 Epic Games Tools
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
}
StaticAssert(sizeof(DEMON_LNX_ThreadExt) <= sizeof(Member(DEMON_Entity, ext_u64)), check_demon_lnx_thread_ext);

//- helpers

struct DEMON_LNX_AttachNode{
  DEMON_LNX_AttachNode* next;
  pid_t pid;
}

struct DEMON_LNX_ProcessAux{
  B32 filled;
  uint64 phnum;
  uint64 phent;
  uint64 phdr;
  uint64 execfn;
}

struct DEMON_LNX_PhdrInfo{
  Rng1U64 range;
  uint64 dynamic;
}

struct DEMON_LNX_ModuleNode{
  DEMON_LNX_ModuleNode* next;
  uint64 vaddr;
  uint64 size;
  uint64 name;
  uint64 already_known;
}

struct DEMON_LNX_EntityNode{
  DEMON_LNX_EntityNode* next;
  DEMON_Entity* entity;
}

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
}

struct DEMON_LNX_UserX64{
  DEMON_LNX_UserRegsX64 regs;
  int32 u_fpvalid, _pad0;
  SYMS_XSaveLegacy i387;
  uint64 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  uint64 signal;
  int32 reserved, _pad1;
  uint64 u_ar0, u_fpstate;
  uint64 magic;
  uint8  u_comm[32];
  uint64 u_debugreg[8];
}

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
}

struct DEMON_LNX_UserX86{
  DEMON_LNX_UserRegsX86 regs;
  int32 u_fpvalid;
  SYMS_FSave i387;
  uint32 u_tsize, u_dsize, u_ssize, start_code, start_stack;
  int32 signal, reserved;
  uint32 u_ar0, u_fpstate;
  uint32 magic;
  uint8  u_comm[32];
  uint32 u_debugreg[8];
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
}

////////////////////////////////
//~ rjf: Helpers

DEMON_LNX_ThreadExt*  demon_lnx_thread_ext(DEMON_Entity* entity);

B32                   demon_lnx_attach_pid(Arena* arena, pid_t pid, DEMON_LNX_AttachNode** new_node);

String8               demon_lnx_executable_path_from_pid(Arena* arena, pid_t pid);
int                   demon_lnx_open_memory_fd_for_pid(pid_t pid);

Arch          demon_lnx_arch_from_pid(pid_t pid);
DEMON_LNX_ProcessAux  demon_lnx_aux_from_pid(pid_t pid, Arch arch);
DEMON_LNX_PhdrInfo    demon_lnx_phdr_info_from_memory(int memory_fd, B32 is_32bit,
                                                               uint64 phvaddr, uint64 phstride, uint64 phcount);
DEMON_LNX_ModuleNode* demon_lnx_module_list_from_process(Arena* arena, DEMON_Entity* process);

uint64     demon_lnx_read_memory(int memory_fd, void* dst, uint64 src, uint64 size);
B32     demon_lnx_write_memory(int memory_fd, uint64 dst, void* src, uint64 size);
String8 demon_lnx_read_memory_str(Arena* arena, int memory_fd, uint64 address);

void demon_lnx_regs_x64_from_usr_regs_x64(SYMS_RegX64* dst, DEMON_LNX_UserRegsX64* src);
void demon_lnx_usr_regs_x64_from_regs_x64(DEMON_LNX_UserRegsX64* dst, SYMS_RegX64* src);

String8 demon_lnx_read_int_string(int fd);
B32     demon_lnx_read_expect(int fd, char expect);
int     demon_lnx_read_whitespace(int fd);
String8 demon_lnx_read_string(Arena* arena, int fd);

int demon_lnx_open_maps(pid_t pid);
B32 demon_lnx_next_map(Arena* arena, int maps, DEMON_LNX_MapsEntry* entry_out);

#endif //DEMON_OS_LINUX_H

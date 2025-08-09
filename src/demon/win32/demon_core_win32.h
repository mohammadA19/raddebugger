// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
//~ rjf: Windows Includes

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

////////////////////////////////
//~ rjf: Win32 Exception Codes

#define DMN_W32_EXCEPTION_BREAKPOINT                     0x80000003u
#define DMN_W32_EXCEPTION_SINGLE_STEP                    0x80000004u
#define DMN_W32_EXCEPTION_LONG_JUMP                      0x80000026u
#define DMN_W32_EXCEPTION_ACCESS_VIOLATION               0xC0000005u
#define DMN_W32_EXCEPTION_ARRAY_BOUNDS_EXCEEDED          0xC000008Cu
#define DMN_W32_EXCEPTION_DATA_TYPE_MISALIGNMENT         0x80000002u
#define DMN_W32_EXCEPTION_GUARD_PAGE_VIOLATION           0x80000001u
#define DMN_W32_EXCEPTION_FLT_DENORMAL_OPERAND           0xC000008Du
#define DMN_W32_EXCEPTION_FLT_DEVIDE_BY_ZERO             0xC000008Eu
#define DMN_W32_EXCEPTION_FLT_INEXACT_RESULT             0xC000008Fu
#define DMN_W32_EXCEPTION_FLT_INVALID_OPERATION          0xC0000090u
#define DMN_W32_EXCEPTION_FLT_OVERFLOW                   0xC0000091u
#define DMN_W32_EXCEPTION_FLT_STACK_CHECK                0xC0000092u
#define DMN_W32_EXCEPTION_FLT_UNDERFLOW                  0xC0000093u
#define DMN_W32_EXCEPTION_INT_DIVIDE_BY_ZERO             0xC0000094u
#define DMN_W32_EXCEPTION_INT_OVERFLOW                   0xC0000095u
#define DMN_W32_EXCEPTION_PRIVILEGED_INSTRUCTION         0xC0000096u
#define DMN_W32_EXCEPTION_ILLEGAL_INSTRUCTION            0xC000001Du
#define DMN_W32_EXCEPTION_IN_PAGE_ERROR                  0xC0000006u
#define DMN_W32_EXCEPTION_INVALID_DISPOSITION            0xC0000026u
#define DMN_W32_EXCEPTION_NONCONTINUABLE                 0xC0000025u
#define DMN_W32_EXCEPTION_STACK_OVERFLOW                 0xC00000FDu
#define DMN_W32_EXCEPTION_INVALID_HANDLE                 0xC0000008u
#define DMN_W32_EXCEPTION_UNWIND_CONSOLIDATE             0x80000029u
#define DMN_W32_EXCEPTION_DLL_NOT_FOUND                  0xC0000135u
#define DMN_W32_EXCEPTION_ORDINAL_NOT_FOUND              0xC0000138u
#define DMN_W32_EXCEPTION_ENTRY_POINT_NOT_FOUND          0xC0000139u
#define DMN_W32_EXCEPTION_DLL_INIT_FAILED                0xC0000142u
#define DMN_W32_EXCEPTION_CONTROL_C_EXIT                 0xC000013Au
#define DMN_W32_EXCEPTION_FLT_MULTIPLE_FAULTS            0xC00002B4u
#define DMN_W32_EXCEPTION_FLT_MULTIPLE_TRAPS             0xC00002B5u
#define DMN_W32_EXCEPTION_NAT_CONSUMPTION                0xC00002C9u
#define DMN_W32_EXCEPTION_HEAP_CORRUPTION                0xC0000374u
#define DMN_W32_EXCEPTION_STACK_BUFFER_OVERRUN           0xC0000409u
#define DMN_W32_EXCEPTION_INVALID_CRUNTIME_PARAM         0xC0000417u
#define DMN_W32_EXCEPTION_ASSERT_FAILURE                 0xC0000420u
#define DMN_W32_EXCEPTION_NO_MEMORY                      0xC0000017u
#define DMN_W32_EXCEPTION_THROW                          0xE06D7363u
#define DMN_W32_EXCEPTION_SET_THREAD_NAME                0x406d1388u
#define DMN_w32_EXCEPTION_CLRDBG_NOTIFICATION            0x04242420u
#define DMN_w32_EXCEPTION_CLR                            0xE0434352u
#define DMN_W32_EXCEPTION_RADDBG_SET_THREAD_COLOR        0x00524144u
#define DMN_W32_EXCEPTION_RADDBG_SET_BREAKPOINT          0x00524145u
#define DMN_W32_EXCEPTION_RADDBG_SET_VADDR_RANGE_NOTE    0x00524156u

////////////////////////////////
//~ rjf: Win32 Register Codes

#define DMN_W32_CTX_X86       0x00010000
#define DMN_W32_CTX_X64       0x00100000

#define DMN_W32_CTX_INTEL_CONTROL       0x0001    // segss, rsp, segcs, rip, and rflags
#define DMN_W32_CTX_INTEL_INTEGER       0x0002    // rax, rcx, rdx, rbx, rbp, rsi, rdi, and r8-r15
#define DMN_W32_CTX_INTEL_SEGMENTS      0x0004    // segds, seges, segfs, and seggs
#define DMN_W32_CTX_INTEL_FLOATS        0x0008    // xmm0-xmm15
#define DMN_W32_CTX_INTEL_DEBUG         0x0010    // dr0-dr3 and dr6-dr7
#define DMN_W32_CTX_INTEL_EXTENDED      0x0020
#define DMN_W32_CTX_INTEL_XSTATE        0x0040

#define DMN_W32_CTX_X86_ALL (DMN_W32_CTX_X86 | \
DMN_W32_CTX_INTEL_CONTROL | DMN_W32_CTX_INTEL_INTEGER | \
DMN_W32_CTX_INTEL_SEGMENTS | DMN_W32_CTX_INTEL_DEBUG | \
DMN_W32_CTX_INTEL_EXTENDED)
#define DMN_W32_CTX_X64_ALL (DMN_W32_CTX_X64 | \
DMN_W32_CTX_INTEL_CONTROL | DMN_W32_CTX_INTEL_INTEGER | \
DMN_W32_CTX_INTEL_SEGMENTS | DMN_W32_CTX_INTEL_FLOATS | \
DMN_W32_CTX_INTEL_DEBUG)

////////////////////////////////
//~ rjf: Per-Entity State

typedef enum DMN_W32_EntityKind
{
  DMN_W32_EntityKind_Null,
  DMN_W32_EntityKind_Root,
  DMN_W32_EntityKind_Process,
  DMN_W32_EntityKind_Thread,
  DMN_W32_EntityKind_Module,
  DMN_W32_EntityKind_COUNT
}
DMN_W32_EntityKind;

struct DMN_W32_Entity
{
  DMN_W32_Entity *first;
  DMN_W32_Entity *last;
  DMN_W32_Entity *next;
  DMN_W32_Entity *prev;
  DMN_W32_Entity *parent;
  DMN_W32_EntityKind kind;
  U32 gen;
  U64 id;
  HANDLE handle;
  Arch arch;
  union
  {
    struct
    {
      U64 injection_address;
      B32 did_first_bp;
    }
    proc;
    struct
    {
      U64 thread_local_base;
      U64 last_name_hash;
      U64 name_gather_time_us;
    }
    thread;
    struct
    {
      Rng1U64 vaddr_range;
      U64 address_of_name_pointer;
      B32 is_main;
      B32 name_is_unicode;
    }
    module;
  };
};

struct DMN_W32_EntityNode
{
  DMN_W32_EntityNode *next;
  DMN_W32_Entity *v;
};

struct DMN_W32_EntityIDHashNode
{
  DMN_W32_EntityIDHashNode *next;
  DMN_W32_EntityIDHashNode *prev;
  U64 id;
  DMN_W32_Entity *entity;
};

struct DMN_W32_EntityIDHashSlot
{
  DMN_W32_EntityIDHashNode *first;
  DMN_W32_EntityIDHashNode *last;
};

////////////////////////////////
//~ rjf: Injection Types

struct DMN_W32_InjectedBreak
{
  U64 code;
  U64 user_data;
};

#define DMN_W32_INJECTED_CODE_SIZE 32

////////////////////////////////
//~ rjf: Image Info Types

struct DMN_W32_ImageInfo
{
  Arch arch;
  U32 size;
};

////////////////////////////////
//~ rjf: Dynamically-Loaded Win32 Function Types

typedef HRESULT DMN_W32_GetThreadDescriptionFunctionType(HANDLE hThread, WCHAR **ppszThreadDescription);

////////////////////////////////
//~ rjf: Shared State Bundle

struct DMN_W32_Shared
{
  // rjf: top-level info
  Arena *arena;
  String8List env_strings;
  
  // rjf: access locking mechanism
  OS_Handle access_mutex;
  B32 access_run_state;
  
  // rjf: detaching info
  Arena *detach_arena;
  DMN_HandleList detach_processes;
  
  // rjf: entity state
  Arena *entities_arena;
  DMN_W32_Entity *entities_base;
  DMN_W32_Entity *entities_first_free;
  U64 entities_count;
  DMN_W32_EntityIDHashSlot *entities_id_hash_slots;
  U64 entities_id_hash_slots_count;
  DMN_W32_EntityIDHashNode *entities_id_hash_node_free;
  
  // rjf: launch state
  B32 new_process_pending;
  
  // rjf: run results
  B32 resume_needed;
  U32 resume_pid;
  U32 resume_tid;
  B32 exception_not_handled;
  
  // rjf: halting info
  DMN_Handle halter_process;
  U32 halter_tid;
};

////////////////////////////////
//~ rjf: Globals

global DMN_W32_Shared *dmn_w32_shared = 0;
global DMN_W32_Entity dmn_w32_entity_nil = {&dmn_w32_entity_nil, &dmn_w32_entity_nil, &dmn_w32_entity_nil, &dmn_w32_entity_nil, &dmn_w32_entity_nil};
global DMN_W32_GetThreadDescriptionFunctionType *dmn_w32_GetThreadDescription = 0;
thread_static B32 dmn_w32_ctrl_thread = 0;

////////////////////////////////
//~ rjf: Basic Helpers


////////////////////////////////
//~ rjf: Entity Helpers

//- rjf: entity <-> handle

//- rjf: entity allocation/deallocation

//- rjf: kind*id -> entity

////////////////////////////////
//~ rjf: Module Info Extraction


////////////////////////////////
//~ rjf: Win32-Level Process/Thread Reads/Writes

//- rjf: processes
#define dmn_w32_process_read_struct(process, vaddr, ptr) dmn_w32_process_read((process), r1u64((vaddr), (vaddr)+(sizeof(*ptr))), ptr)
#define dmn_w32_process_write_struct(process, vaddr, ptr) dmn_w32_process_write((process), r1u64((vaddr), (vaddr)+(sizeof(*ptr))), ptr)

//- rjf: threads

//- rjf: remote thread injection


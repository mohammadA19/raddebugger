// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Includes / Libraries

#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <tlhelp32.h>
#include <Shlobj.h>
#include <processthreadsapi.h>
#pragma comment(lib, "user32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "shell32")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "comctl32")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") // this is required for loading correct comctl32 dll file

////////////////////////////////
//~ rjf: File Iterator Types



struct OS_W32_FileIter
{
  HANDLE handle;
  WIN32_FIND_DATAW find_data;
  B32 is_volume_iter;
  String8Array drive_strings;
  U64 drive_strings_iter_idx;
};
StaticAssert(sizeof(Member(OS_FileIter, memory)) >= sizeof(OS_W32_FileIter), file_iter_memory_size);

////////////////////////////////
//~ rjf: Entity Types

typedef enum OS_W32_EntityKind
{
  OS_W32_EntityKind_Null,
  OS_W32_EntityKind_Thread,
  OS_W32_EntityKind_Mutex,
  OS_W32_EntityKind_RWMutex,
  OS_W32_EntityKind_ConditionVariable,
}
OS_W32_EntityKind;



struct OS_W32_Entity
{
  OS_W32_Entity *next;
  OS_W32_EntityKind kind;
  union
  {
    struct
    {
      OS_ThreadFunctionType *func;
      void *ptr;
      HANDLE handle;
      DWORD tid;
    } thread;
    CRITICAL_SECTION mutex;
    SRWLOCK rw_mutex;
    CONDITION_VARIABLE cv;
  };
};

////////////////////////////////
//~ rjf: State



struct OS_W32_State
{
  Arena *arena;
  
  // rjf: info
  OS_SystemInfo system_info;
  OS_ProcessInfo process_info;
  U64 microsecond_resolution;
  
  // rjf: entity storage
  CRITICAL_SECTION entity_mutex;
  Arena *entity_arena;
  OS_W32_Entity *entity_free;
};

////////////////////////////////
//~ rjf: Globals

global OS_W32_State os_w32_state = {0};

////////////////////////////////
//~ rjf: File Info Conversion Helpers




////////////////////////////////
//~ rjf: Time Conversion Helpers






////////////////////////////////
//~ rjf: Entity Functions




////////////////////////////////
//~ rjf: Thread Entry Point





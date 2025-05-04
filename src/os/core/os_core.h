// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: System Info

public struct OS_SystemInfo
{
  uint32 logical_processor_count;
  uint64 page_size;
  uint64 large_page_size;
  uint64 allocation_granularity;
  String8 machine_name;
}

////////////////////////////////
//~ rjf: Process Info

public struct OS_ProcessInfo
{
  uint32 pid;
  B32 large_pages_allowed;
  String8 binary_path;
  String8 initial_path;
  String8 user_program_data_path;
  String8List module_load_paths;
  String8List environment;
}

////////////////////////////////
//~ rjf: Access Flags

public enum OS_AccessFlags : uint32
{
  Read       = 1 << 0,
  Write      = 1 << 1,
  Execute    = 1 << 2,
  Append     = 1 << 3,
  ShareRead  = 1 << 4,
  ShareWrite = 1 << 5,
  Inherited  = 1 << 6,
}

////////////////////////////////
//~ rjf: Files

public enum OS_FileIterFlags : uint32
{
  SkipFolders     = 1 << 0,
  SkipFiles       = 1 << 1,
  SkipHiddenFiles = 1 << 2,
  Done            = 1 << 31,
}

public struct OS_FileIter
{
  OS_FileIterFlags flags;
  uint8[800] memory;
}

public struct OS_FileInfo
{
  String8 name;
  FileProperties props;
}

// nick: on-disk file identifier
public struct OS_FileID
{
  uint64[3] v;
}

////////////////////////////////
//~ rjf: Handle Type

public struct OS_Handle
{
  uint64[1] u64;
}

public struct OS_HandleNode
{
  OS_HandleNode* next;
  OS_Handle v;
}

public struct OS_HandleList
{
  OS_HandleNode* first;
  OS_HandleNode* last;
  uint64 count;
}

public struct OS_HandleArray
{
  OS_Handle* v;
  uint64 count;
}

////////////////////////////////
//~ rjf: Process Launch Parameters

public struct OS_ProcessLaunchParams
{
  String8List cmd_line;
  String8 path;
  String8List env;
  B32 inherit_env;
  B32 debug_subprocesses;
  B32 consoleless;
  OS_Handle stdout_file;
  OS_Handle stderr_file;
  OS_Handle stdin_file;
}

////////////////////////////////
//~ rjf: Thread Types

typedef void OS_ThreadFunctionType(void* ptr);

////////////////////////////////
//~ rjf: @os_hooks Synchronization Primitives (Implemented Per-OS)

//- rjf: scope macros
#define OS_MutexScope(mutex) DeferLoop(os_mutex_take(mutex), os_mutex_drop(mutex))
#define OS_MutexScopeR(mutex) DeferLoop(os_rw_mutex_take_r(mutex), os_rw_mutex_drop_r(mutex))
#define OS_MutexScopeW(mutex) DeferLoop(os_rw_mutex_take_w(mutex), os_rw_mutex_drop_w(mutex))
#define OS_MutexScopeRWPromote(mutex) DeferLoop((os_rw_mutex_drop_r(mutex), os_rw_mutex_take_w(mutex)), (os_rw_mutex_drop_w(mutex), os_rw_mutex_take_r(mutex)))

////////////////////////////////
//~ rjf: @os_hooks Entry Points (Implemented Per-OS)

// NOTE(rjf): The implementation of `os_core` will define low-level entry
// points if BUILD_ENTRY_DEFINING_UNIT is defined to 1. These will call
// into the standard codebase program entry points, named "entry_point".

#if BUILD_ENTRY_DEFINING_UNIT
static extern void entry_point(CmdLine* cmdline);
#endif

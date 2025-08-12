// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
//~ rjf: System Info

typedef struct OS_SystemInfo OS_SystemInfo;
struct OS_SystemInfo
{
  U32 logical_processor_count;
  U64 page_size;
  U64 large_page_size;
  U64 allocation_granularity;
  String8 machine_name;
};

////////////////////////////////
//~ rjf: Process Info

typedef struct OS_ProcessInfo OS_ProcessInfo;
struct OS_ProcessInfo
{
  U32 pid;
  B32 large_pages_allowed;
  String8 binary_path;
  String8 initial_path;
  String8 user_program_data_path;
  String8List module_load_paths;
  String8List environment;
};

////////////////////////////////
//~ rjf: Access Flags

typedef U32 OS_AccessFlags;
enum
{
  OS_AccessFlag_Read        = (1<<0),
  OS_AccessFlag_Write       = (1<<1),
  OS_AccessFlag_Execute     = (1<<2),
  OS_AccessFlag_Append      = (1<<3),
  OS_AccessFlag_ShareRead   = (1<<4),
  OS_AccessFlag_ShareWrite  = (1<<5),
  OS_AccessFlag_Inherited   = (1<<6),
};

////////////////////////////////
//~ rjf: Files

typedef U32 OS_FileIterFlags;
enum
{
  OS_FileIterFlag_SkipFolders     = (1 << 0),
  OS_FileIterFlag_SkipFiles       = (1 << 1),
  OS_FileIterFlag_SkipHiddenFiles = (1 << 2),
  OS_FileIterFlag_Done            = (1 << 31),
};

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter
{
  OS_FileIterFlags flags;
  U8 memory[800];
};

typedef struct OS_FileInfo OS_FileInfo;
struct OS_FileInfo
{
  String8 name;
  FileProperties props;
};

// nick: on-disk file identifier
typedef struct OS_FileID OS_FileID;
struct OS_FileID
{
  U64 v[3];
};

////////////////////////////////
//~ rjf: Handle Type

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
  U64 u64[1];
};

typedef struct OS_HandleNode OS_HandleNode;
struct OS_HandleNode
{
  OS_HandleNode *next;
  OS_Handle v;
};

typedef struct OS_HandleList OS_HandleList;
struct OS_HandleList
{
  OS_HandleNode *first;
  OS_HandleNode *last;
  U64 count;
};

typedef struct OS_HandleArray OS_HandleArray;
struct OS_HandleArray
{
  OS_Handle *v;
  U64 count;
};

////////////////////////////////
//~ rjf: Process Launch Parameters

typedef struct OS_ProcessLaunchParams OS_ProcessLaunchParams;
struct OS_ProcessLaunchParams
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
};

////////////////////////////////
//~ rjf: Thread Types

typedef void OS_ThreadFunctionType(void *ptr);

////////////////////////////////
//~ rjf: Handle Type Functions (Helpers, Implemented Once)


////////////////////////////////
//~ rjf: Command Line Argc/Argv Helper (Helper, Implemented Once)


////////////////////////////////
//~ rjf: Filesystem Helpers (Helpers, Implemented Once)


////////////////////////////////
//~ rjf: Process Launcher Helpers


////////////////////////////////
//~ rjf: @os_hooks System/Process Info (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Memory Allocation (Implemented Per-OS)

//- rjf: basic

//- rjf: large pages

////////////////////////////////
//~ rjf: @os_hooks Thread Info (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Aborting (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks File System (Implemented Per-OS)

//- rjf: files
#define os_file_read_struct(f, off, ptr) os_file_read((f), r1u64((off), (off)+sizeof(*(ptr))), (ptr))

//- rjf: file maps

//- rjf: directory iteration

//- rjf: directory creation

////////////////////////////////
//~ rjf: @os_hooks Shared Memory (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Time (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Child Processes (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Threads (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Synchronization Primitives (Implemented Per-OS)

//- rjf: recursive mutexes

//- rjf: reader/writer mutexes

//- rjf: condition variables
// returns false on timeout, true on signal, (max_wait_ms = max_U64) -> no timeout

//- rjf: cross-process semaphores

//- rjf: scope macros
#define OS_MutexScope(mutex) DeferLoop(os_mutex_take(mutex), os_mutex_drop(mutex))
#define OS_MutexScopeR(mutex) DeferLoop(os_rw_mutex_take_r(mutex), os_rw_mutex_drop_r(mutex))
#define OS_MutexScopeW(mutex) DeferLoop(os_rw_mutex_take_w(mutex), os_rw_mutex_drop_w(mutex))
#define OS_MutexScopeRWPromote(mutex) DeferLoop((os_rw_mutex_drop_r(mutex), os_rw_mutex_take_w(mutex)), (os_rw_mutex_drop_w(mutex), os_rw_mutex_take_r(mutex)))

////////////////////////////////
//~ rjf: @os_hooks Dynamically-Loaded Libraries (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Safe Calls (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks GUIDs (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Entry Points (Implemented Per-OS)

// NOTE(rjf): The implementation of `os_core` will define low-level entry
// points if BUILD_ENTRY_DEFINING_UNIT is defined to 1. These will call
// into the standard codebase program entry points, named "entry_point".

#if BUILD_ENTRY_DEFINING_UNIT
raddbg_entry_point(entry_point);
#endif


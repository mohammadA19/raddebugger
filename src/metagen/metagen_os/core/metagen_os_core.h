// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: System Info



struct OS_SystemInfo
{
  u32 logical_processor_count;
  u64 page_size;
  u64 large_page_size;
  u64 allocation_granularity;
  String8 machine_name;
};

////////////////////////////////
//~ rjf: Process Info



struct OS_ProcessInfo
{
  u32 pid;
  b32 large_pages_allowed;
  String8 binary_path;
  String8 initial_path;
  String8 user_program_data_path;
  String8List module_load_paths;
  String8List environment;
};

////////////////////////////////
//~ rjf: Access Flags

typedef u32 OS_AccessFlags;
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

typedef u32 OS_FileIterFlags;
enum
{
  OS_FileIterFlag_SkipFolders     = (1 << 0),
  OS_FileIterFlag_SkipFiles       = (1 << 1),
  OS_FileIterFlag_SkipHiddenFiles = (1 << 2),
  OS_FileIterFlag_Done            = (1 << 31),
};



struct OS_FileIter
{
  OS_FileIterFlags flags;
  u8 memory[800];
};



struct OS_FileInfo
{
  String8 name;
  FileProperties props;
};

// nick: on-disk file identifier


struct OS_FileID
{
  u64 v[3];
};

////////////////////////////////
//~ rjf: Handle Type



struct OS_Handle
{
  u64 u64[1];
};



struct OS_HandleNode
{
  OS_HandleNode *next;
  OS_Handle v;
};



struct OS_HandleList
{
  OS_HandleNode *first;
  OS_HandleNode *last;
  u64 count;
};



struct OS_HandleArray
{
  OS_Handle *v;
  u64 count;
};

////////////////////////////////
//~ rjf: Process Launch Parameters



struct OS_ProcessLaunchParams
{
  String8List cmd_line;
  String8 path;
  String8List env;
  b32 inherit_env;
  b32 debug_subprocesses;
  b32 consoleless;
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

#endif



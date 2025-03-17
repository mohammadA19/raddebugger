// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef OS_CORE_H
#define OS_CORE_H

////////////////////////////////
//~ rjf: System Info

struct OS_SystemInfo
{
  uint32 logical_processor_count;
  uint64 page_size;
  uint64 large_page_size;
  uint64 allocation_granularity;
  StringView machine_name;
}

////////////////////////////////
//~ rjf: Process Info

struct OS_ProcessInfo
{
  uint32 pid;
  B32 large_pages_allowed;
  StringView binary_path;
  StringView initial_path;
  StringView user_program_data_path;
  String8List module_load_paths;
  String8List environment;
}

////////////////////////////////
//~ rjf: Access Flags

enum OS_AccessFlags : uint32
{
  OS_AccessFlag_Read       = (1<<0),
  OS_AccessFlag_Write      = (1<<1),
  OS_AccessFlag_Execute    = (1<<2),
  OS_AccessFlag_Append     = (1<<3),
  OS_AccessFlag_ShareRead  = (1<<4),
  OS_AccessFlag_ShareWrite = (1<<5),
  OS_AccessFlag_Inherited  = (1<<6),
}

////////////////////////////////
//~ rjf: Files

enum OS_FileIterFlags : uint32
{
  OS_FileIterFlag_SkipFolders     = (1 << 0),
  OS_FileIterFlag_SkipFiles       = (1 << 1),
  OS_FileIterFlag_SkipHiddenFiles = (1 << 2),
  OS_FileIterFlag_Done            = (1 << 31),
}

struct OS_FileIter
{
  OS_FileIterFlags flags;
  uint8 memory[800];
}

struct OS_FileInfo
{
  StringView name;
  FileProperties props;
}

// nick: on-disk file identifier
struct OS_FileID
{
  uint64 v[3];
}

////////////////////////////////
//~ rjf: Handle Type

struct OS_Handle
{
  uint64 u64[1];
}

struct OS_HandleNode
{
  OS_HandleNode* next;
  OS_Handle v;
}

struct OS_HandleList
{
  OS_HandleNode* first;
  OS_HandleNode* last;
  uint64 count;
}

struct OS_HandleArray
{
  OS_Handle* v;
  uint64 count;
}

////////////////////////////////
//~ rjf: Process Launch Parameters

struct OS_ProcessLaunchParams
{
  String8List cmd_line;
  StringView path;
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
//~ rjf: Handle Type Functions (Helpers, Implemented Once)

OS_Handle os_handle_zero();
B32 os_handle_match(OS_Handle a, OS_Handle b);
void os_handle_list_push(Arena* arena, OS_HandleList* handles, OS_Handle handle);
OS_HandleArray os_handle_array_from_list(Arena* arena, OS_HandleList* list);

////////////////////////////////
//~ rjf: Command Line Argc/Argv Helper (Helper, Implemented Once)

String8List os_string_list_from_argcv(Arena* arena, int argc, char** argv);

////////////////////////////////
//~ rjf: Filesystem Helpers (Helpers, Implemented Once)

StringView        os_data_from_file_path(Arena* arena, StringView path);
B32            os_write_data_to_file_path(StringView path, StringView data);
B32            os_write_data_list_to_file_path(StringView path, String8List list);
B32            os_append_data_to_file_path(StringView path, StringView data);
OS_FileID      os_id_from_file_path(StringView path);
int64            os_file_id_compare(OS_FileID a, OS_FileID b);
StringView        os_string_from_file_range(Arena* arena, OS_Handle file, Rng1U64 range);

////////////////////////////////
//~ rjf: Process Launcher Helpers

OS_Handle os_cmd_line_launch(StringView string);
OS_Handle os_cmd_line_launchf(char* fmt, ...);

////////////////////////////////
//~ rjf: @os_hooks System/Process Info (Implemented Per-OS)

OS_SystemInfo*  os_get_system_info();
OS_ProcessInfo* os_get_process_info();
StringView         os_get_current_path(Arena* arena);
uint32             os_get_process_start_time_unix();

////////////////////////////////
//~ rjf: @os_hooks Memory Allocation (Implemented Per-OS)

//- rjf: basic
void* os_reserve(uint64 size);
B32   os_commit(void* ptr, uint64 size);
void  os_decommit(void* ptr, uint64 size);
void  os_release(void* ptr, uint64 size);

//- rjf: large pages
void* os_reserve_large(uint64 size);
B32 os_commit_large(void* ptr, uint64 size);

////////////////////////////////
//~ rjf: @os_hooks Thread Info (Implemented Per-OS)

uint32 os_tid();
void os_set_thread_name(StringView string);

////////////////////////////////
//~ rjf: @os_hooks Aborting (Implemented Per-OS)

void os_abort(int32 exit_code);

////////////////////////////////
//~ rjf: @os_hooks File System (Implemented Per-OS)

//- rjf: files
OS_Handle      os_file_open(OS_AccessFlags flags, StringView path);
void           os_file_close(OS_Handle file);
uint64            os_file_read(OS_Handle file, Rng1U64 rng, void* out_data);
uint64            os_file_write(OS_Handle file, Rng1U64 rng, void* data);
B32            os_file_set_times(OS_Handle file, DateTime time);
FileProperties os_properties_from_file(OS_Handle file);
OS_FileID      os_id_from_file(OS_Handle file);
B32            os_delete_file_at_path(StringView path);
B32            os_copy_file_path(StringView dst, StringView src);
StringView        os_full_path_from_path(Arena* arena, StringView path);
B32            os_file_path_exists(StringView path);
B32            os_folder_path_exists(StringView path);
FileProperties os_properties_from_file_path(StringView path);

//- rjf: file maps
OS_Handle os_file_map_open(OS_AccessFlags flags, OS_Handle file);
void      os_file_map_close(OS_Handle map);
void *    os_file_map_view_open(OS_Handle map, OS_AccessFlags flags, Rng1U64 range);
void      os_file_map_view_close(OS_Handle map, void* ptr, Rng1U64 range);

//- rjf: directory iteration
OS_FileIter* os_file_iter_begin(Arena* arena, StringView path, OS_FileIterFlags flags);
B32          os_file_iter_next(Arena* arena, OS_FileIter* iter, OS_FileInfo* info_out);
void         os_file_iter_end(OS_FileIter* iter);

//- rjf: directory creation
B32 os_make_directory(StringView path);

////////////////////////////////
//~ rjf: @os_hooks Shared Memory (Implemented Per-OS)

OS_Handle os_shared_memory_alloc(uint64 size, StringView name);
OS_Handle os_shared_memory_open(StringView name);
void      os_shared_memory_close(OS_Handle handle);
void *    os_shared_memory_view_open(OS_Handle handle, Rng1U64 range);
void      os_shared_memory_view_close(OS_Handle handle, void* ptr, Rng1U64 range);

////////////////////////////////
//~ rjf: @os_hooks Time (Implemented Per-OS)

uint64         os_now_microseconds();
uint32         os_now_unix();
DateTime    os_now_universal_time();
DateTime    os_universal_time_from_local(DateTime* local_time);
DateTime    os_local_time_from_universal(DateTime* universal_time);
void        os_sleep_milliseconds(uint32 msec);

////////////////////////////////
//~ rjf: @os_hooks Child Processes (Implemented Per-OS)

OS_Handle os_process_launch(OS_ProcessLaunchParams* params);
B32       os_process_join(OS_Handle handle, uint64 endt_us);
void      os_process_detach(OS_Handle handle);

////////////////////////////////
//~ rjf: @os_hooks Threads (Implemented Per-OS)

OS_Handle os_thread_launch(OS_ThreadFunctionType* func, void* ptr, void* params);
B32       os_thread_join(OS_Handle handle, uint64 endt_us);
void      os_thread_detach(OS_Handle handle);

////////////////////////////////
//~ rjf: @os_hooks Synchronization Primitives (Implemented Per-OS)

//- rjf: recursive mutexes
OS_Handle os_mutex_alloc();
void      os_mutex_release(OS_Handle mutex);
void      os_mutex_take(OS_Handle mutex);
void      os_mutex_drop(OS_Handle mutex);

//- rjf: reader/writer mutexes
OS_Handle os_rw_mutex_alloc();
void      os_rw_mutex_release(OS_Handle rw_mutex);
void      os_rw_mutex_take_r(OS_Handle mutex);
void      os_rw_mutex_drop_r(OS_Handle mutex);
void      os_rw_mutex_take_w(OS_Handle mutex);
void      os_rw_mutex_drop_w(OS_Handle mutex);

//- rjf: condition variables
OS_Handle os_condition_variable_alloc();
void      os_condition_variable_release(OS_Handle cv);
// returns false on timeout, true on signal, (max_wait_ms = max_U64) . no timeout
B32       os_condition_variable_wait(OS_Handle cv, OS_Handle mutex, uint64 endt_us);
B32       os_condition_variable_wait_rw_r(OS_Handle cv, OS_Handle mutex_rw, uint64 endt_us);
B32       os_condition_variable_wait_rw_w(OS_Handle cv, OS_Handle mutex_rw, uint64 endt_us);
void      os_condition_variable_signal(OS_Handle cv);
void      os_condition_variable_broadcast(OS_Handle cv);

//- rjf: cross-process semaphores
OS_Handle os_semaphore_alloc(uint32 initial_count, uint32 max_count, StringView name);
void      os_semaphore_release(OS_Handle semaphore);
OS_Handle os_semaphore_open(StringView name);
void      os_semaphore_close(OS_Handle semaphore);
B32       os_semaphore_take(OS_Handle semaphore, uint64 endt_us);
void      os_semaphore_drop(OS_Handle semaphore);

//- rjf: scope macros
#define OS_MutexScope(mutex) DeferLoop(os_mutex_take(mutex), os_mutex_drop(mutex))
#define OS_MutexScopeR(mutex) DeferLoop(os_rw_mutex_take_r(mutex), os_rw_mutex_drop_r(mutex))
#define OS_MutexScopeW(mutex) DeferLoop(os_rw_mutex_take_w(mutex), os_rw_mutex_drop_w(mutex))
#define OS_MutexScopeRWPromote(mutex) DeferLoop((os_rw_mutex_drop_r(mutex), os_rw_mutex_take_w(mutex)), (os_rw_mutex_drop_w(mutex), os_rw_mutex_take_r(mutex)))

////////////////////////////////
//~ rjf: @os_hooks Dynamically-Loaded Libraries (Implemented Per-OS)

OS_Handle os_library_open(StringView path);
void      os_library_close(OS_Handle lib);
VoidProc* os_library_load_proc(OS_Handle lib, StringView name);

////////////////////////////////
//~ rjf: @os_hooks Safe Calls (Implemented Per-OS)

void os_safe_call(OS_ThreadFunctionType* func, OS_ThreadFunctionType* fail_handler, void* ptr);

////////////////////////////////
//~ rjf: @os_hooks GUIDs (Implemented Per-OS)

Guid os_make_guid();

////////////////////////////////
//~ rjf: @os_hooks Entry Points (Implemented Per-OS)

// NOTE(rjf): The implementation of `os_core` will define low-level entry
// points if BUILD_ENTRY_DEFINING_UNIT is defined to 1. These will call
// into the standard codebase program entry points, named "entry_point".

#if BUILD_ENTRY_DEFINING_UNIT
void entry_point(CmdLine* cmdline);
#endif

#endif // OS_CORE_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Includes

#define _GNU_SOURCE
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <linux/limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

pid_t gettid(void);
int pthread_setname_np(pthread_t thread, const char *name);
int pthread_getname_np(pthread_t thread, char *name, size_t size);


////////////////////////////////
//~ rjf: File Iterator

struct OS_LNX_FileIter
{
  DIR* dir;
  dirent* dp;
  String8 path;
}
StaticAssert(sizeof(Member(OS_FileIter, memory)) >= sizeof(OS_LNX_FileIter), os_lnx_file_iter_size_check);

////////////////////////////////
//~ rjf: Safe Call Handler Chain

struct OS_LNX_SafeCallChain
{
  OS_LNX_SafeCallChain* next;
  OS_ThreadFunctionType* fail_handler;
  void* ptr;
}

////////////////////////////////
//~ rjf: Entities

enum OS_LNX_EntityKind
{
  OS_LNX_EntityKind_Thread,
  OS_LNX_EntityKind_Mutex,
  OS_LNX_EntityKind_RWMutex,
  OS_LNX_EntityKind_ConditionVariable,
}

struct OS_LNX_Entity
{
  OS_LNX_Entity* next;
  OS_LNX_EntityKind kind;
  union
  {
    struct
    {
      pthread_t handle;
      OS_ThreadFunctionType* func;
      void* ptr;
    } thread;
    pthread_mutex_t mutex_handle;
    pthread_rwlock_t rwmutex_handle;
    struct
    {
      pthread_cond_t cond_handle;
      pthread_mutex_t rwlock_mutex_handle;
    } cv;
  }
}

////////////////////////////////
//~ rjf: State

struct OS_LNX_State
{
  Arena* arena;
  OS_SystemInfo system_info;
  OS_ProcessInfo process_info;
  pthread_mutex_t entity_mutex;
  Arena* entity_arena;
  OS_LNX_Entity* entity_free;
}

////////////////////////////////
//~ rjf: Globals

global OS_LNX_State os_lnx_state = {0};
thread_static OS_LNX_SafeCallChain *os_lnx_safe_call_chain = 0;

////////////////////////////////
//~ rjf: Thread Entry Point



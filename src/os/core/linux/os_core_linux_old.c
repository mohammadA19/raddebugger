// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#include <stdio.h>

////////////////////////////////
//~ rjf: Globals

static pthread_mutex_t lnx_mutex = {0};
static Arena* lnx_perm_arena = 0;
static String8List lnx_cmd_line_args = {0};
static LNX_Entity lnx_entity_buffer[1024];
static LNX_Entity* lnx_entity_free = 0;
static StringView lnx_initial_path = {0};
thread_static LNX_SafeCallChain* lnx_safe_call_chain = 0;

////////////////////////////////
//~ rjf: Helpers

B32
lnx_write_list_to_file_descriptor(int fd, String8List list){
  B32 success = true;
  
  String8Node* node = list.first;
  if (node != 0){
    uint8* ptr = node.string.str;;
    uint8* opl = ptr + node.string.size;
    
    uint64 p = 0;
    for (;p < list.total_size;){
      uint64 amt64 = (uint64)(opl - ptr);
      uint32 amt = u32_from_u64_saturate(amt64);
      int64 written_amt = write(fd, ptr, amt);
      if (written_amt < 0){
        break;
      }
      p += written_amt;
      ptr += written_amt;
      
      Assert(ptr <= opl);
      if (ptr == opl){
        node = node.next;
        if (node == 0){
          if (p < list.total_size){
            success = false;
          }
          break;
        }
        ptr = node.string.str;
        opl = ptr + node.string.size;
      }
    }
  }
  
  return(success);
}

void
lnx_date_time_from_tm(DateTime* out, struct tm* in, uint32 msec){
  out.msec = msec;
  out.sec  = in.tm_sec;
  out.min  = in.tm_min;
  out.hour = in.tm_hour;
  out.day  = in.tm_mday - 1;
  out.wday = in.tm_wday;
  out.mon  = in.tm_mon;
  out.year = in.tm_year + 1900;
}

void
lnx_tm_from_date_time(struct tm* out, DateTime* in){
  out.tm_sec  = in.sec;
  out.tm_min  = in.min;
  out.tm_hour = in.hour;
  out.tm_mday = in.day + 1;
  out.tm_mon  = in.mon;
  out.tm_year = in.year - 1900;
}

void
lnx_dense_time_from_timespec(DenseTime* out, struct timespec* in){
  struct tm tm_time = {0};
  gmtime_r(&in.tv_sec, &tm_time);
  DateTime date_time = {0};
  lnx_date_time_from_tm(&date_time, &tm_time, in.tv_nsec/Million(1));
  *out = dense_time_from_date_time(date_time);
}

void
lnx_file_properties_from_stat(FileProperties* out, struct stat* in){
  MemoryZeroStruct(out);
  out.size = in.st_size;
  lnx_dense_time_from_timespec(&out.created, &in.st_ctim);
  lnx_dense_time_from_timespec(&out.modified, &in.st_mtim);
  if ((in.st_mode & S_IFDIR) != 0){
    out.flags |= FilePropertyFlag_IsFolder;
  }
}

StringView
lnx_string_from_signal(int signum){
  StringView result = ("<unknown-signal>");
  switch (signum){
    case SIGABRT:
    {
      result = ("SIGABRT");
    }break;
    case SIGALRM:
    {
      result = ("SIGALRM");
    }break;
    case SIGBUS:
    {
      result = ("SIGBUS");
    }break;
    case SIGCHLD:
    {
      result = ("SIGCHLD");
    }break;
    case SIGCONT:
    {
      result = ("SIGCONT");
    }break;
    case SIGFPE:
    {
      result = ("SIGFPE");
    }break;
    case SIGHUP:
    {
      result = ("SIGHUP");
    }break;
    case SIGILL:
    {
      result = ("SIGILL");
    }break;
    case SIGINT:
    {
      result = ("SIGINT");
    }break;
    case SIGIO:
    {
      result = ("SIGIO");
    }
    case SIGKILL:
    {
      result = ("SIGKILL");
    }break;
    case SIGPIPE:
    {
      result = ("SIGPIPE");
    }break;
    case SIGPROF:
    {
      result = ("SIGPROF");
    }break;
    case SIGPWR:
    {
      result = ("SIGPWR");
    }break;
    case SIGQUIT:
    {
      result = ("SIGQUIT");
    }break;
    case SIGSEGV:
    {
      result = ("SIGSEGV");
    }break;
    case SIGSTKFLT:
    {
      result = ("SIGSTKFLT");
    }break;
    case SIGSTOP:
    {
      result = ("SIGSTOP");
    }break;
    case SIGTSTP:
    {
      result = ("SIGTSTP");
    }break;
    case SIGSYS:
    {
      result = ("SIGSYS");
    }break;
    case SIGTERM:
    {
      result = ("SIGTERM");
    }break;
    case SIGTRAP:
    {
      result = ("SIGTRAP");
    }break;
    case SIGTTIN:
    {
      result = ("SIGTTIN");
    }break;
    case SIGTTOU:
    {
      result = ("SIGTTOU");
    }break;
    case SIGURG:
    {
      result = ("SIGURG");
    }break;
    case SIGUSR1:
    {
      result = ("SIGUSR1");
    }break;
    case SIGUSR2:
    {
      result = ("SIGUSR2");
    }break;
    case SIGVTALRM:
    {
      result = ("SIGVTALRM");
    }break;
    case SIGXCPU:
    {
      result = ("SIGXCPU");
    }break;
    case SIGXFSZ:
    {
      result = ("SIGXFSZ");
    }break;
    case SIGWINCH:
    {
      result = ("SIGWINCH");
    }break;
  }
  return(result);
}

StringView
lnx_string_from_errno(int error_number){
  StringView result = ("<unknown-errno>");
  switch (error_number){
    case EPERM:
    {
      result = ("EPERM");
    }break;
    case ENOENT:
    {
      result = ("ENOENT");
    }break;
    case ESRCH:
    {
      result = ("ESRCH");
    }break;
    case EINTR:
    {
      result = ("EINTR");
    }break;
    case EIO:
    {
      result = ("EIO");
    }break;
    case ENXIO:
    {
      result = ("ENXIO");
    }break;
    case E2BIG:
    {
      result = ("E2BIG");
    }break;
    case ENOEXEC:
    {
      result = ("ENOEXEC");
    }break;
    case EBADF:
    {
      result = ("EBADF");
    }break;
    case ECHILD:
    {
      result = ("ECHILD");
    }break;
    case EAGAIN:
    {
      result = ("EAGAIN");
    }break;
    case ENOMEM:
    {
      result = ("ENOMEM");
    }break;
    case EACCES:
    {
      result = ("EACCES");
    }break;
    case EFAULT:
    {
      result = ("EFAULT");
    }break;
    case ENOTBLK:
    {
      result = ("ENOTBLK");
    }break;
    case EBUSY:
    {
      result = ("EBUSY");
    }break;
    case EEXIST:
    {
      result = ("EEXIST");
    }break;
    case EXDEV:
    {
      result = ("EXDEV");
    }break;
    case ENODEV:
    {
      result = ("ENODEV");
    }break;
    case ENOTDIR:
    {
      result = ("ENOTDIR");
    }break;
    case EISDIR:
    {
      result = ("EISDIR");
    }break;
    case EINVAL:
    {
      result = ("EINVAL");
    }break;
    case ENFILE:
    {
      result = ("ENFILE");
    }break;
    case EMFILE:
    {
      result = ("EMFILE");
    }break;
    case ENOTTY:
    {
      result = ("ENOTTY");
    }break;
    case ETXTBSY:
    {
      result = ("ETXTBSY");
    }break;
    case EFBIG:
    {
      result = ("EFBIG");
    }break;
    case ENOSPC:
    {
      result = ("ENOSPC");
    }break;
    case ESPIPE:
    {
      result = ("ESPIPE");
    }break;
    case EROFS:
    {
      result = ("EROFS");
    }break;
    case EMLINK:
    {
      result = ("EMLINK");
    }break;
    case EPIPE:
    {
      result = ("EPIPE");
    }break;
    case EDOM:
    {
      result = ("EDOM");
    }break;
    case ERANGE:
    {
      result = ("ERANGE");
    }break;
    case EDEADLK:
    {
      result = ("EDEADLK");
    }break;
    case ENAMETOOLONG:
    {
      result = ("ENAMETOOLONG");
    }break;
    case ENOLCK:
    {
      result = ("ENOLCK");
    }break;
    case ENOSYS:
    {
      result = ("ENOSYS");
    }break;
    case ENOTEMPTY:
    {
      result = ("ENOTEMPTY");
    }break;
    case ELOOP:
    {
      result = ("ELOOP");
    }break;
    case ENOMSG:
    {
      result = ("ENOMSG");
    }break;
    case EIDRM:
    {
      result = ("EIDRM");
    }break;
    case ECHRNG:
    {
      result = ("ECHRNG");
    }break;
    case EL2NSYNC:
    {
      result = ("EL2NSYNC");
    }break;
    case EL3HLT:
    {
      result = ("EL3HLT");
    }break;
    case EL3RST:
    {
      result = ("EL3RST");
    }break;
    case ELNRNG:
    {
      result = ("ELNRNG");
    }break;
    case EUNATCH:
    {
      result = ("EUNATCH");
    }break;
    case ENOCSI:
    {
      result = ("ENOCSI");
    }break;
    case EL2HLT:
    {
      result = ("EL2HLT");
    }break;
    case EBADE:
    {
      result = ("EBADE");
    }break;
    case EBADR:
    {
      result = ("EBADR");
    }break;
    case EXFULL:
    {
      result = ("EXFULL");
    }break;
    case ENOANO:
    {
      result = ("ENOANO");
    }break;
    case EBADRQC:
    {
      result = ("EBADRQC");
    }break;
    case EBADSLT:
    {
      result = ("EBADSLT");
    }break;
    case EBFONT:
    {
      result = ("EBFONT");
    }break;
    case ENOSTR:
    {
      result = ("ENOSTR");
    }break;
    case ENODATA:
    {
      result = ("ENODATA");
    }break;
    case ETIME:
    {
      result = ("ETIME");
    }break;
    case ENOSR:
    {
      result = ("ENOSR");
    }break;
    case ENONET:
    {
      result = ("ENONET");
    }break;
    case ENOPKG:
    {
      result = ("ENOPKG");
    }break;
    case EREMOTE:
    {
      result = ("EREMOTE");
    }break;
    case ENOLINK:
    {
      result = ("ENOLINK");
    }break;
    case EADV:
    {
      result = ("EADV");
    }break;
    case ESRMNT:
    {
      result = ("ESRMNT");
    }break;
    case ECOMM:
    {
      result = ("ECOMM");
    }break;
    case EPROTO:
    {
      result = ("EPROTO");
    }break;
    case EMULTIHOP:
    {
      result = ("EMULTIHOP");
    }break;
    case EDOTDOT:
    {
      result = ("EDOTDOT");
    }break;
    case EBADMSG:
    {
      result = ("EBADMSG");
    }break;
    case EOVERFLOW:
    {
      result = ("EOVERFLOW");
    }break;
    case ENOTUNIQ:
    {
      result = ("ENOTUNIQ");
    }break;
    case EBADFD:
    {
      result = ("EBADFD");
    }break;
    case EREMCHG:
    {
      result = ("EREMCHG");
    }break;
    case ELIBACC:
    {
      result = ("ELIBACC");
    }break;
    case ELIBBAD:
    {
      result = ("ELIBBAD");
    }break;
    case ELIBSCN:
    {
      result = ("ELIBSCN");
    }break;
    case ELIBMAX:
    {
      result = ("ELIBMAX");
    }break;
    case ELIBEXEC:
    {
      result = ("ELIBEXEC");
    }break;
    case EILSEQ:
    {
      result = ("EILSEQ");
    }break;
    case ERESTART:
    {
      result = ("ERESTART");
    }break;
    case ESTRPIPE:
    {
      result = ("ESTRPIPE");
    }break;
    case EUSERS:
    {
      result = ("EUSERS");
    }break;
    case ENOTSOCK:
    {
      result = ("ENOTSOCK");
    }break;
    case EDESTADDRREQ:
    {
      result = ("EDESTADDRREQ");
    }break;
    case EMSGSIZE:
    {
      result = ("EMSGSIZE");
    }break;
    case EPROTOTYPE:
    {
      result = ("EPROTOTYPE");
    }break;
    case ENOPROTOOPT:
    {
      result = ("ENOPROTOOPT");
    }break;
    case EPROTONOSUPPORT:
    {
      result = ("EPROTONOSUPPORT");
    }break;
    case ESOCKTNOSUPPORT:
    {
      result = ("ESOCKTNOSUPPORT");
    }break;
    case EOPNOTSUPP:
    {
      result = ("EOPNOTSUPP");
    }break;
    case EPFNOSUPPORT:
    {
      result = ("EPFNOSUPPORT");
    }break;
    case EAFNOSUPPORT:
    {
      result = ("EAFNOSUPPORT");
    }break;
    case EADDRINUSE:
    {
      result = ("EADDRINUSE");
    }break;
    case EADDRNOTAVAIL:
    {
      result = ("EADDRNOTAVAIL");
    }break;
    case ENETDOWN:
    {
      result = ("ENETDOWN");
    }break;
    case ENETUNREACH:
    {
      result = ("ENETUNREACH");
    }break;
    case ENETRESET:
    {
      result = ("ENETRESET");
    }break;
    case ECONNABORTED:
    {
      result = ("ECONNABORTED");
    }break;
    case ECONNRESET:
    {
      result = ("ECONNRESET");
    }break;
    case ENOBUFS:
    {
      result = ("ENOBUFS");
    }break;
    case EISCONN:
    {
      result = ("EISCONN");
    }break;
    case ENOTCONN:
    {
      result = ("ENOTCONN");
    }break;
    case ESHUTDOWN:
    {
      result = ("ESHUTDOWN");
    }break;
    case ETOOMANYREFS:
    {
      result = ("ETOOMANYREFS");
    }break;
    case ETIMEDOUT:
    {
      result = ("ETIMEDOUT");
    }break;
    case ECONNREFUSED:
    {
      result = ("ECONNREFUSED");
    }break;
    case EHOSTDOWN:
    {
      result = ("EHOSTDOWN");
    }break;
    case EHOSTUNREACH:
    {
      result = ("EHOSTUNREACH");
    }break;
    case EALREADY:
    {
      result = ("EALREADY");
    }break;
    case EINPROGRESS:
    {
      result = ("EINPROGRESS");
    }break;
    case ESTALE:
    {
      result = ("ESTALE");
    }break;
    case EUCLEAN:
    {
      result = ("EUCLEAN");
    }break;
    case ENOTNAM:
    {
      result = ("ENOTNAM");
    }break;
    case ENAVAIL:
    {
      result = ("ENAVAIL");
    }break;
    case EISNAM:
    {
      result = ("EISNAM");
    }break;
    case EREMOTEIO:
    {
      result = ("EREMOTEIO");
    }break;
    case EDQUOT:
    {
      result = ("EDQUOT");
    }break;
    case ENOMEDIUM:
    {
      result = ("ENOMEDIUM");
    }break;
    case EMEDIUMTYPE:
    {
      result = ("EMEDIUMTYPE");
    }break;
    case ECANCELED:
    {
      result = ("ECANCELED");
    }break;
    case ENOKEY:
    {
      result = ("ENOKEY");
    }break;
    case EKEYEXPIRED:
    {
      result = ("EKEYEXPIRED");
    }break;
    case EKEYREVOKED:
    {
      result = ("EKEYREVOKED");
    }break;
    case EKEYREJECTED:
    {
      result = ("EKEYREJECTED");
    }break;
    case EOWNERDEAD:
    {
      result = ("EOWNERDEAD");
    }break;
    case ENOTRECOVERABLE:
    {
      result = ("ENOTRECOVERABLE");
    }break;
    case ERFKILL:
    {
      result = ("ERFKILL");
    }break;
    case EHWPOISON:
    {
      result = ("EHWPOISON");
    }break;
  }
  return(result);
}

LNX_Entity*
lnx_alloc_entity(LNX_EntityKind kind){
  pthread_mutex_lock(&lnx_mutex);
  LNX_Entity* result = lnx_entity_free;
  Assert(result != 0);
  SLLStackPop(lnx_entity_free);
  pthread_mutex_unlock(&lnx_mutex);
  result.kind = kind;
  return(result);
}

void
lnx_free_entity(LNX_Entity* entity){
  entity.kind = LNX_EntityKind_Null;
  pthread_mutex_lock(&lnx_mutex);
  SLLStackPush(lnx_entity_free, entity);
  pthread_mutex_unlock(&lnx_mutex);
}

void*
lnx_thread_base(void* ptr){
  LNX_Entity* entity = (LNX_Entity*)ptr;
  OS_ThreadFunctionType* func = entity.thread.func;
  void* thread_ptr = entity.thread.ptr;
  
  TCTX tctx_;
  tctx_init_and_equip(&tctx_);
  func(thread_ptr);
  tctx_release();
  
  // remove my bit
  uint32 result = __sync_fetch_and_and(&entity.reference_mask, ~0x2);
  // if the other bit is also gone, free entity
  if ((result & 0x1) == 0){
    lnx_free_entity(entity);
  }
  return(0);
}

void
lnx_safe_call_sig_handler(int){
  LNX_SafeCallChain* chain = lnx_safe_call_chain;
  if (chain != 0 && chain.fail_handler != 0){
    chain.fail_handler(chain.ptr);
  }
  abort();
}

////////////////////////////////
//~ rjf: @os_hooks Main Initialization API (Implemented Per-OS)

void
os_init()
{
  // NOTE(allen): Initialize linux layer mutex
  {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int pthread_result = pthread_mutex_init(&lnx_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    if (pthread_result == -1){
      abort();
    }
  }
  MemoryZeroArray(lnx_entity_buffer);
  {
    LNX_Entity* ptr = lnx_entity_free = lnx_entity_buffer;
    for (uint64 i = 1; i < ArrayCount(lnx_entity_buffer); i += 1, ptr += 1){
      ptr.next = ptr + 1;
    }
    ptr.next = 0;
  }
  
  // NOTE(allen): Permanent memory allocator for this layer
  Arena* perm_arena = arena_alloc();
  lnx_perm_arena = perm_arena;
  
  // NOTE(allen): Initialize Paths
  lnx_initial_path = os_get_path(lnx_perm_arena, OS_SystemPath_Current);
  
  // NOTE(rjf): Setup command line args
  lnx_cmd_line_args = os_string_list_from_argcv(lnx_perm_arena, argc, argv);
}

////////////////////////////////
//~ rjf: @os_hooks Memory Allocation (Implemented Per-OS)

void*
os_reserve(uint64 size){
  void* result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  return(result);
}

B32
os_commit(void* ptr, uint64 size){
  mprotect(ptr, size, PROT_READ|PROT_WRITE);
  // TODO(allen): can we test this?
  return(true);
}

void*
os_reserve_large(uint64 size){
  NotImplemented;
  return 0;
}

B32
os_commit_large(void* ptr, uint64 size){
  NotImplemented;
  return 0;
}

void
os_decommit(void* ptr, uint64 size){
  madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
}

void
os_release(void* ptr, uint64 size){
  munmap(ptr, size);
}

void
os_set_large_pages_enabled(B32 flag)
{
  NotImplemented;
}

B32
os_large_pages_enabled()
{
  NotImplemented;
  return 0;
}

uint64
os_large_page_size()
{
  NotImplemented;
  return 0;
}

////////////////////////////////
//~ rjf: @os_hooks System Info (Implemented Per-OS)

StringView
os_machine_name(){
  static B32 first = true;
  static StringView name = {0};
  
  // TODO(allen): let's just pre-compute this at init and skip the complexity
  pthread_mutex_lock(&lnx_mutex);
  if (first){
    Temp scratch = scratch_begin(0, 0);
    first = false;
    
    // get name
    B32 got_final_result = false;
    uint8* buffer = 0;
    int size = 0;
    for (int64 cap = 4096, r = 0;
         r < 4;
         cap *= 2, r += 1){
      scratch.restore();
      buffer = push_array_no_zero(scratch.arena, uint8, cap);
      size = gethostname((char*)buffer, cap);
      if (size < cap){
        got_final_result = true;
        break;
      }
    }
    
    // save string
    if (got_final_result && size > 0){
      name.size = size;
      name.str = push_array_no_zero(lnx_perm_arena, uint8, name.size + 1);
      MemoryCopy(name.str, buffer, name.size);
      name.str[name.size] = 0;
    }
    
    scratch_end(scratch);
  }
  pthread_mutex_unlock(&lnx_mutex);
  
  return(name);
}

uint64
os_page_size(){
  int size = getpagesize();
  return((uint64)size);
}

uint64
os_allocation_granularity()
{
  // On linux there is no equivalent of "dwAllocationGranularity"
  os_page_size();
}

uint64
os_logical_core_count()
{
  // TODO(rjf): check this
  return get_nprocs();
}

////////////////////////////////
//~ rjf: @os_hooks Process & Thread Info (Implemented Per-OS)

String8List
os_get_command_line_arguments()
{
  return lnx_cmd_line_args;
}

int32
os_pid(){
  int32 result = getpid();
  return(result);
}

int32
os_tid(){
  int32 result = 0;
#ifdef SYS_gettid
  result = syscall(SYS_gettid);
#else
  result = gettid();
#endif
  return(result);
}

String8List
os_environment()
{
  NotImplemented;
  String8List result = {0};
  return result;
}

uint64
os_string_list_from_system_path(Arena* arena, OS_SystemPath path, String8List* out){
  uint64 result = 0;
  
  switch (path){
    case OS_SystemPath_Binary:
    {
      static B32 first = true;
      static StringView name = {0};
      
      // TODO(allen): let's just pre-compute this at init and skip the complexity
      pthread_mutex_lock(&lnx_mutex);
      if (first){
        Temp scratch = scratch_begin(&arena, 1);
        first = false;
        
        // get self string
        B32 got_final_result = false;
        uint8* buffer = 0;
        int size = 0;
        for (int64 cap = PATH_MAX, r = 0;
             r < 4;
             cap *= 2, r += 1){
          scratch.restore();
          buffer = push_array_no_zero(scratch.arena, uint8, cap);
          size = readlink("/proc/self/exe", (char*)buffer, cap);
          if (size < cap){
            got_final_result = true;
            break;
          }
        }
        
        // save string
        if (got_final_result && size > 0){
          StringView full_name = StringView(buffer, size);
          StringView name_chopped = string_path_chop_last_slash(full_name);
          name = push_str8_copy(lnx_perm_arena, name_chopped);
        }
        
        scratch_end(scratch);
      }
      pthread_mutex_unlock(&lnx_mutex);
      
      result = 1;
      str8_list_push(arena, out, name);
    }break;
    
    case OS_SystemPath_Initial:
    {
      Assert(lnx_initial_path.str != 0);
      result = 1;
      str8_list_push(arena, out, lnx_initial_path);
    }break;
    
    case OS_SystemPath_Current:
    {
      char* cwdir = getcwd(0, 0);
      StringView string = push_str8_copy(arena, str8_cstring(cwdir));
      free(cwdir);
      result = 1;
      str8_list_push(arena, out, string);
    }break;
    
    case OS_SystemPath_UserProgramData:
    {
      char* home = getenv("HOME");
      StringView string = str8_cstring(home);
      result = 1;
      str8_list_push(arena, out, string);
    }break;
    
    case OS_SystemPath_ModuleLoad:
    {
      // TODO(allen): this one is big and complicated and only needed for making
      // a debugger, skipping for now.
      NotImplemented;
    }break;
  }
  
  return(result);
}

////////////////////////////////
//~ rjf: @os_hooks Process Control (Implemented Per-OS)

void
os_abort(int32 exit_code){
  exit(exit_code);
}

////////////////////////////////
//~ rjf: @os_hooks File System (Implemented Per-OS)

//- rjf: files

OS_Handle
os_file_open(OS_AccessFlags flags, StringView path)
{
  OS_Handle file = {0};
  NotImplemented;
  return file;
}

void
os_file_close(OS_Handle file)
{
  NotImplemented;
}

uint64
os_file_read(OS_Handle file, Rng1U64 rng, void* out_data)
{
  NotImplemented;
  return 0;
}

uint64
os_file_write(OS_Handle file, Rng1U64 rng, void* data)
{
  NotImplemented;
}

B32
os_file_set_times(OS_Handle file, DateTime time)
{
  NotImplemented;
}

FileProperties
os_properties_from_file(OS_Handle file)
{
  FileProperties props = {0};
  NotImplemented;
  return props;
}

OS_FileID
os_id_from_file(OS_Handle file)
{
  // TODO(nick): querry struct stat with fstat(2) and use st_dev and st_ino as ids
  OS_FileID id = {0};
  NotImplemented;
  return id;
}

B32
os_delete_file_at_path(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  B32 result = false;
  StringView name_copy = push_str8_copy(scratch.arena, name);
  if (remove((char*)name_copy.str) != -1){
    result = true;
  }
  scratch_end(scratch);
  return(result);
}

B32
os_copy_file_path(StringView dst, StringView src)
{
  NotImplemented;
  return 0;
}

StringView
os_full_path_from_path(Arena* arena, StringView path)
{
  // TODO: realpath can be used to resolve full path
  StringView result = {0};
  NotImplemented;
  return result;
}

B32
os_file_path_exists(StringView path)
{
  NotImplemented;
  return 0;
}

FileProperties
os_properties_from_file_path(StringView path)
{
  FileProperties props = {0};
  NotImplemented;
  return props;
}

//- rjf: file maps

OS_Handle
os_file_map_open(OS_AccessFlags flags, OS_Handle file)
{
  NotImplemented;
  OS_Handle handle = {0};
  return handle;
}

void
os_file_map_close(OS_Handle map)
{
  NotImplemented;
}

void *
os_file_map_view_open(OS_Handle map, OS_AccessFlags flags, Rng1U64 range)
{
  NotImplemented;
  return 0;
}

void
os_file_map_view_close(OS_Handle map, void* ptr, Rng1U64 range)
{
  NotImplemented;
}

//- rjf: directory iteration

OS_FileIter *
os_file_iter_begin(Arena* arena, StringView path, OS_FileIterFlags flags)
{
  NotImplemented;
  return 0;
}

B32
os_file_iter_next(Arena* arena, OS_FileIter* iter, OS_FileInfo* info_out)
{
  NotImplemented;
  return 0;
}

void
os_file_iter_end(OS_FileIter* iter)
{
  NotImplemented;
}

//- rjf: directory creation

B32
os_make_directory(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  B32 result = false;
  StringView name_copy = push_str8_copy(scratch.arena, name);
  if (mkdir((char*)name_copy.str, 0777) != -1){
    result = true;
  }
  scratch_end(scratch);
  return(result);
}

////////////////////////////////
//~ rjf: @os_hooks Shared Memory (Implemented Per-OS)

OS_Handle
os_shared_memory_alloc(uint64 size, StringView name)
{
  OS_Handle result = {0};
  NotImplemented;
  return result;
}

OS_Handle
os_shared_memory_open(StringView name)
{
  OS_Handle result = {0};
  NotImplemented;
  return result;
}

void
os_shared_memory_close(OS_Handle handle)
{
  NotImplemented;
}

void *
os_shared_memory_view_open(OS_Handle handle, Rng1U64 range)
{
  NotImplemented;
  return 0;
}

void
os_shared_memory_view_close(OS_Handle handle, void* ptr, Rng1U64 range)
{
  NotImplemented;
}

////////////////////////////////
//~ rjf: @os_hooks Time (Implemented Per-OS)

uint32
os_now_unix()
{
  time_t t = time(0);
  return (uint32)t;
}

DateTime
os_now_universal_time(){
  time_t t = 0;
  time(&t);
  struct tm universal_tm = {0};
  gmtime_r(&t, &universal_tm);
  DateTime result = {0};
  lnx_date_time_from_tm(&result, &universal_tm, 0);
  return(result);
}

DateTime
os_universal_time_from_local(DateTime* local_time){
  // local time . universal time (using whatever types it takes)
  struct tm local_tm = {0};
  lnx_tm_from_date_time(&local_tm, local_time);
  local_tm.tm_isdst = -1;
  time_t universal_t = mktime(&local_tm);
  
  // whatever type we ended up with . DateTime (don't alter the space along the way)
  struct tm universal_tm = {0};
  gmtime_r(&universal_t, &universal_tm);
  DateTime result = {0};
  lnx_date_time_from_tm(&result, &universal_tm, 0);
  return(result);
}

DateTime
os_local_time_from_universal(DateTime* universal_time){
  // universal time . local time (using whatever types it takes)
  struct tm universal_tm = {0};
  lnx_tm_from_date_time(&universal_tm, universal_time);
  universal_tm.tm_isdst = -1;
  time_t universal_t = timegm(&universal_tm);
  struct tm local_tm = {0};
  localtime_r(&universal_t, &local_tm);
  
  // whatever type we ended up with . DateTime (don't alter the space along the way)
  DateTime result = {0};
  lnx_date_time_from_tm(&result, &local_tm, 0);
  return(result);
}

uint64
os_now_microseconds(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  uint64 result = t.tv_sec*Million(1) + (t.tv_nsec/Thousand(1));
  return(result);
}

void
os_sleep_milliseconds(uint32 msec){
  usleep(msec*Thousand(1));
}

////////////////////////////////
//~ rjf: @os_hooks Child Processes (Implemented Per-OS)

B32
os_launch_process(OS_LaunchOptions* options){
  // TODO(allen): I want to redo this API before I bother implementing it here
  NotImplemented;
  return(false);
}

////////////////////////////////
//~ rjf: @os_hooks Threads (Implemented Per-OS)

OS_Handle
os_thread_launch(OS_ThreadFunctionType* func, void* ptr, void* params){
  // entity
  LNX_Entity* entity = lnx_alloc_entity(LNX_EntityKind_Thread);
  entity.reference_mask = 0x3;
  entity.thread.func = func;
  entity.thread.ptr = ptr;
  
  // pthread
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  int pthread_result = pthread_create(&entity.thread.handle, &attr, lnx_thread_base, entity);
  pthread_attr_destroy(&attr);
  if (pthread_result == -1){
    lnx_free_entity(entity);
    entity = 0;
  }
  
  // cast to opaque handle
  OS_Handle result = {IntFromPtr(entity)};
  return(result);
}

void
os_release_thread_handle(OS_Handle thread){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(thread.id);
  // remove my bit
  uint32 result = __sync_fetch_and_and(&entity.reference_mask, ~0x1);
  // if the other bit is also gone, free entity
  if ((result & 0x2) == 0){
    lnx_free_entity(entity);
  }
}

////////////////////////////////
//~ rjf: @os_hooks Synchronization Primitives (Implemented Per-OS)

// NOTE(allen): Mutexes are recursive - support counted acquire/release nesting
// on a single thread

//- rjf: recursive mutexes

OS_Handle
os_mutex_alloc(){
  // entity
  LNX_Entity* entity = lnx_alloc_entity(LNX_EntityKind_Mutex);
  
  // pthread
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  int pthread_result = pthread_mutex_init(&entity.mutex, &attr);
  pthread_mutexattr_destroy(&attr);
  if (pthread_result == -1){
    lnx_free_entity(entity);
    entity = 0;
  }
  
  // cast to opaque handle
  OS_Handle result = {IntFromPtr(entity)};
  return(result);
}

void
os_mutex_release(OS_Handle mutex){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(mutex.id);
  pthread_mutex_destroy(&entity.mutex);
  lnx_free_entity(entity);
}

void
os_mutex_take_(OS_Handle mutex){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(mutex.id);
  pthread_mutex_lock(&entity.mutex);
}

void
os_mutex_drop_(OS_Handle mutex){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(mutex.id);
  pthread_mutex_unlock(&entity.mutex);
}

//- rjf: reader/writer mutexes

OS_Handle
os_rw_mutex_alloc()
{
  OS_Handle result = {0};
  NotImplemented;
  return result;
}

void
os_rw_mutex_release(OS_Handle rw_mutex)
{
  NotImplemented;
}

void
os_rw_mutex_take_r_(OS_Handle mutex)
{
  NotImplemented;
}

void
os_rw_mutex_drop_r_(OS_Handle mutex)
{
  NotImplemented;
}

void
os_rw_mutex_take_w_(OS_Handle mutex)
{
  NotImplemented;
}

void
os_rw_mutex_drop_w_(OS_Handle mutex)
{
  NotImplemented;
}

//- rjf: condition variables

OS_Handle
os_condition_variable_alloc(){
  // entity
  LNX_Entity* entity = lnx_alloc_entity(LNX_EntityKind_ConditionVariable);
  
  // pthread
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  int pthread_result = pthread_cond_init(&entity.cond, &attr);
  pthread_condattr_destroy(&attr);
  if (pthread_result == -1){
    lnx_free_entity(entity);
    entity = 0;
  }
  
  // cast to opaque handle
  OS_Handle result = {IntFromPtr(entity)};
  return(result);
}

void
os_condition_variable_release(OS_Handle cv){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(cv.id);
  pthread_cond_destroy(&entity.cond);
  lnx_free_entity(entity);
}

B32
os_condition_variable_wait_(OS_Handle cv, OS_Handle mutex, uint64 endt_us){
  B32 result = false;
  LNX_Entity* entity_cond = (LNX_Entity*)PtrFromInt(cv.id);
  LNX_Entity* entity_mutex = (LNX_Entity*)PtrFromInt(mutex.id);
  // TODO(allen): implement the time control
  pthread_cond_timedwait(&entity_cond.cond, &entity_mutex.mutex);
  return(result);
}

B32
os_condition_variable_wait_rw_r_(OS_Handle cv, OS_Handle mutex_rw, uint64 endt_us)
{
  NotImplemented;
  return 0;
}

B32
os_condition_variable_wait_rw_w_(OS_Handle cv, OS_Handle mutex_rw, uint64 endt_us)
{
  NotImplemented;
  return 0;
}

void
os_condition_variable_signal_(OS_Handle cv){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(cv.id);
  pthread_cond_signal(&entity.cond);
}

void
os_condition_variable_broadcast_(OS_Handle cv){
  LNX_Entity* entity = (LNX_Entity*)PtrFromInt(cv.id);
  DontCompile;
}

//- rjf: cross-process semaphores

OS_Handle
os_semaphore_alloc(uint32 initial_count, uint32 max_count, StringView name)
{
  OS_Handle result = {0};
  NotImplemented;
  return result;
}

void
os_semaphore_release(OS_Handle semaphore)
{
  NotImplemented;
}

OS_Handle
os_semaphore_open(StringView name)
{
  OS_Handle result = {0};
  NotImplemented;
  return result;
}

void
os_semaphore_close(OS_Handle semaphore)
{
  NotImplemented;
}

B32
os_semaphore_take(OS_Handle semaphore, uint64 endt_us)
{
  NotImplemented;
  return 0;
}

void
os_semaphore_drop(OS_Handle semaphore)
{
  NotImplemented;
}

////////////////////////////////
//~ rjf: @os_hooks Dynamically-Loaded Libraries (Implemented Per-OS)

OS_Handle
os_library_open(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  char* path_cstr = (char *)push_str8_copy(scratch.arena, path).str;
  void* so = dlopen(path_cstr, RTLD_LAZY);
  OS_Handle lib = { (uint64)so };
  scratch_end(scratch);
  return lib;
}

VoidProc *
os_library_load_proc(OS_Handle lib, StringView name)
{
  Temp scratch = scratch_begin(0, 0);
  void* so = (void *)lib.id;
  char* name_cstr = (char *)push_str8_copy(scratch.arena, name).str;
  VoidProc* proc = (VoidProc *)dlsym(so, name_cstr);
  scratch_end(scratch);
  return proc;
}

void
os_library_close(OS_Handle lib)
{
  void* so = (void *)lib.id;
  dlclose(so);
}

////////////////////////////////
//~ rjf: @os_hooks Dynamically-Loaded Libraries (Implemented Per-OS)

void
os_safe_call(OS_ThreadFunctionType* func, OS_ThreadFunctionType* fail_handler, void* ptr){
  LNX_SafeCallChain chain = {0};
  SLLStackPush(lnx_safe_call_chain, &chain);
  chain.fail_handler = fail_handler;
  chain.ptr = ptr;
  
  struct sigaction new_act = {0};
  new_act.sa_handler = lnx_safe_call_sig_handler;
  
  int signals_to_handle[] = {
    SIGILL, SIGFPE, SIGSEGV, SIGBUS, SIGTRAP,
  };
  struct sigaction og_act[ArrayCount(signals_to_handle)] = {0};
  
  for (uint32 i = 0; i < ArrayCount(signals_to_handle); i += 1){
    sigaction(signals_to_handle[i], &new_act, &og_act[i]);
  }
  
  func(ptr);
  
  for (uint32 i = 0; i < ArrayCount(signals_to_handle); i += 1){
    sigaction(signals_to_handle[i], &og_act[i], 0);
  }
}

////////////////////////////////

OS_Guid
os_make_guid()
{
  NotImplemented;
}


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Helpers

DateTime
os_lnx_date_time_from_tm(tm in, uint32 msec)
{
  DateTime dt = {0};
  dt.sec  = in.tm_sec;
  dt.min  = in.tm_min;
  dt.hour = in.tm_hour;
  dt.day  = in.tm_mday-1;
  dt.mon  = in.tm_mon;
  dt.year = in.tm_year+1900;
  dt.msec = msec;
  return dt;
}

tm
os_lnx_tm_from_date_time(DateTime dt)
{
  tm result = {0};
  result.tm_sec = dt.sec;
  result.tm_min = dt.min;
  result.tm_hour= dt.hour;
  result.tm_mday= dt.day+1;
  result.tm_mon = dt.mon;
  result.tm_year= dt.year-1900;
  return result;
}

timespec
os_lnx_timespec_from_date_time(DateTime dt)
{
  tm tm_val = os_lnx_tm_from_date_time(dt);
  time_t seconds = timegm(&tm_val);
  timespec result = {0};
  result.tv_sec = seconds;
  return result;
}

DenseTime
os_lnx_dense_time_from_timespec(timespec in)
{
  DenseTime result = 0;
  {
    struct tm tm_time = {0};
    gmtime_r(&in.tv_sec, &tm_time);
    DateTime date_time = os_lnx_date_time_from_tm(tm_time, in.tv_nsec/Million(1));
    result = dense_time_from_date_time(date_time);
  }
  return result;
}

FileProperties
os_lnx_file_properties_from_stat(struct stat* s)
{
  FileProperties props = {0};
  props.size     = s.st_size;
  props.created  = os_lnx_dense_time_from_timespec(s.st_ctim);
  props.modified = os_lnx_dense_time_from_timespec(s.st_mtim);
  if(s.st_mode & S_IFDIR)
  {
    props.flags |= FilePropertyFlag_IsFolder;
  }
  return props;
}

void
os_lnx_safe_call_sig_handler(int x)
{
  OS_LNX_SafeCallChain* chain = os_lnx_safe_call_chain;
  if(chain != 0 && chain.fail_handler != 0)
  {
    chain.fail_handler(chain.ptr);
  }
  abort();
}

////////////////////////////////
//~ rjf: Entities

OS_LNX_Entity *
os_lnx_entity_alloc(OS_LNX_EntityKind kind)
{
  OS_LNX_Entity* entity = 0;
  DeferLoop(pthread_mutex_lock(&os_lnx_state.entity_mutex),
            pthread_mutex_unlock(&os_lnx_state.entity_mutex))
  {
    entity = os_lnx_state.entity_free;
    if(entity)
    {
      SLLStackPop(os_lnx_state.entity_free);
    }
    else
    {
      entity = push_array_no_zero(os_lnx_state.entity_arena, OS_LNX_Entity, 1);
    }
  }
  MemoryZeroStruct(entity);
  entity.kind = kind;
  return entity;
}

void
os_lnx_entity_release(OS_LNX_Entity* entity)
{
  DeferLoop(pthread_mutex_lock(&os_lnx_state.entity_mutex),
            pthread_mutex_unlock(&os_lnx_state.entity_mutex))
  {
    SLLStackPush(os_lnx_state.entity_free, entity);
  }
}

////////////////////////////////
//~ rjf: Thread Entry Point

void *
os_lnx_thread_entry_point(void* ptr)
{
  OS_LNX_Entity* entity = (OS_LNX_Entity *)ptr;
  OS_ThreadFunctionType* func = entity.thread.func;
  void* thread_ptr = entity.thread.ptr;
  TCTX tctx_;
  tctx_init_and_equip(&tctx_);
  func(thread_ptr);
  tctx_release();
  return 0;
}

////////////////////////////////
//~ rjf: @os_hooks System/Process Info (Implemented Per-OS)

OS_SystemInfo *
os_get_system_info()
{
  return &os_lnx_state.system_info;
}

OS_ProcessInfo *
os_get_process_info()
{
  return &os_lnx_state.process_info;
}

StringView
os_get_current_path(Arena* arena)
{
  char* cwdir = getcwd(0, 0);
  StringView str = push_str8_copy(arena, str8_cstring(cwdir));
  return str;
}

////////////////////////////////
//~ rjf: @os_hooks Memory Allocation (Implemented Per-OS)

//- rjf: basic

void *
os_reserve(uint64 size)
{
  void* result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  return result;
}

B32
os_commit(void* ptr, uint64 size)
{
  mprotect(ptr, size, PROT_READ|PROT_WRITE);
  return 1;
}

void
os_decommit(void* ptr, uint64 size)
{
  madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
}

void
os_release(void* ptr, uint64 size)
{
  munmap(ptr, size);
}

//- rjf: large pages

void *
os_reserve_large(uint64 size)
{
  void* result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
  return result;
}

B32
os_commit_large(void* ptr, uint64 size)
{
  mprotect(ptr, size, PROT_READ|PROT_WRITE);
  return 1;
}

////////////////////////////////
//~ rjf: @os_hooks Thread Info (Implemented Per-OS)

uint32
os_tid()
{
  uint32 result = 0;
#if defined(SYS_gettid)
  result = syscall(SYS_gettid);
#else
  result = gettid();
#endif
  return result;
}

void
os_set_thread_name(StringView name)
{
  Temp scratch = scratch_begin(0, 0);
  StringView name_copy = push_str8_copy(scratch.arena, name);
  pthread_t current_thread = pthread_self();
  pthread_setname_np(current_thread, (char *)name_copy.Ptr);
  scratch_end(scratch);
}

////////////////////////////////
//~ rjf: @os_hooks Aborting (Implemented Per-OS)

void
os_abort(int32 exit_code)
{
  exit(exit_code);
}

////////////////////////////////
//~ rjf: @os_hooks File System (Implemented Per-OS)

//- rjf: files

OS_Handle
os_file_open(OS_AccessFlags flags, StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  StringView path_copy = push_str8_copy(scratch.arena, path);
  int lnx_flags = 0;
  if(flags & (OS_AccessFlag_Read|OS_AccessFlag_Write))
  {
    lnx_flags = O_RDWR;
  }
  else if(flags & OS_AccessFlag_Write)
  {
    lnx_flags = O_WRONLY;
  }
  else if(flags & OS_AccessFlag_Read)
  {
    lnx_flags = O_RDONLY;
  }
  if(flags & OS_AccessFlag_Append)
  {
    lnx_flags |= O_APPEND;
  }
  int fd = open((char *)path_copy.Ptr, lnx_flags);
  OS_Handle handle = {0};
  if(fd != -1)
  {
    handle.u64[0] = fd;
  }
  scratch_end(scratch);
  return handle;
}

void
os_file_close(OS_Handle file)
{
  if(os_handle_match(file, os_handle_zero())) { return; }
  int fd = (int)file.u64[0];
  close(fd);
}

uint64
os_file_read(OS_Handle file, Rng1U64 rng, void* out_data)
{
  if(os_handle_match(file, os_handle_zero())) { return 0; }
  int fd = (int)file.u64[0];
  if(rng.min != 0)
  {
    lseek(fd, rng.min, SEEK_SET);
  }
  uint64 total_num_bytes_to_read = dim_1u64(rng);
  uint64 total_num_bytes_read = 0;
  uint64 total_num_bytes_left_to_read = total_num_bytes_to_read;
  for(;total_num_bytes_left_to_read > 0;)
  {
    int read_result = read(fd, (uint8 *)out_data + total_num_bytes_read, total_num_bytes_left_to_read);
    if(read_result >= 0)
    {
      total_num_bytes_read += read_result;
      total_num_bytes_left_to_read -= read_result;
    }
    else if(errno != EINTR)
    {
      break;
    }
  }
  return total_num_bytes_read;
}

uint64
os_file_write(OS_Handle file, Rng1U64 rng, void* data)
{
  if(os_handle_match(file, os_handle_zero())) { return 0; }
  int fd = (int)file.u64[0];
  if(rng.min != 0)
  {
    lseek(fd, rng.min, SEEK_SET);
  }
  uint64 total_num_bytes_to_write = dim_1u64(rng);
  uint64 total_num_bytes_written = 0;
  uint64 total_num_bytes_left_to_write = total_num_bytes_to_write;
  for(;total_num_bytes_left_to_write > 0;)
  {
    int write_result = write(fd, (uint8 *)data + total_num_bytes_written, total_num_bytes_left_to_write);
    if(write_result >= 0)
    {
      total_num_bytes_written += write_result;
      total_num_bytes_left_to_write -= write_result;
    }
    else if(errno != EINTR)
    {
      break;
    }
  }
  return total_num_bytes_written;
}

B32
os_file_set_times(OS_Handle file, DateTime date_time)
{
  if(os_handle_match(file, os_handle_zero())) { return 0; }
  int fd = (int)file.u64[0];
  timespec time = os_lnx_timespec_from_date_time(date_time);
  timespec times[2] = {time, time};
  int futimens_result = futimens(fd, times);
  B32 good = (futimens_result != -1);
  return good;
}

FileProperties
os_properties_from_file(OS_Handle file)
{
  if(os_handle_match(file, os_handle_zero())) { return (FileProperties){0}; }
  int fd = (int)file.u64[0];
  struct stat fd_stat = {0};
  int fstat_result = fstat(fd, &fd_stat);
  FileProperties props = {0};
  if(fstat_result != -1)
  {
    props = os_lnx_file_properties_from_stat(&fd_stat);
  }
  return props;
}

OS_FileID
os_id_from_file(OS_Handle file)
{
  if(os_handle_match(file, os_handle_zero())) { return (OS_FileID){0}; }
  int fd = (int)file.u64[0];
  struct stat fd_stat = {0};
  int fstat_result = fstat(fd, &fd_stat);
  OS_FileID id = {0};
  if(fstat_result != -1)
  {
    id.v[0] = fd_stat.st_dev;
    id.v[1] = fd_stat.st_ino;
  }
  return id;
}

B32
os_delete_file_at_path(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  B32 result = 0;
  StringView path_copy = push_str8_copy(scratch.arena, path);
  if(remove((char*)path_copy.Ptr) != -1)
  {
    result = 1;
  }
  scratch_end(scratch);
  return result;
}

B32
os_copy_file_path(StringView dst, StringView src)
{
  B32 result = 0;
  OS_Handle src_h = os_file_open(OS_AccessFlag_Read, src);
  OS_Handle dst_h = os_file_open(OS_AccessFlag_Write, dst);
  if(!os_handle_match(src_h, os_handle_zero()) &&
     !os_handle_match(dst_h, os_handle_zero()))
  {
    FileProperties src_props = os_properties_from_file(src_h);
    uint64 size = src_props.size;
    uint64 total_bytes_copied = 0;
    uint64 bytes_left_to_copy = size;
    for(;bytes_left_to_copy > 0;)
    {
      Temp scratch = scratch_begin(0, 0);
      uint64 buffer_size = Min(bytes_left_to_copy, MB(8));
      uint8* buffer = push_array_no_zero(scratch.arena, uint8, buffer_size);
      uint64 bytes_read = os_file_read(src_h, r1u64(total_bytes_copied, total_bytes_copied+buffer_size), buffer);
      uint64 bytes_written = os_file_write(dst_h, r1u64(total_bytes_copied, total_bytes_copied+bytes_read), buffer);
      uint64 bytes_copied = Min(bytes_read, bytes_written);
      bytes_left_to_copy -= bytes_copied;
      total_bytes_copied += bytes_copied;
      scratch_end(scratch);
      if(bytes_copied == 0)
      {
        break;
      }
    }
  }
  os_file_close(src_h);
  os_file_close(dst_h);
  return result;
}

StringView
os_full_path_from_path(Arena* arena, StringView path)
{
  Temp scratch = scratch_begin(&arena, 1);
  StringView path_copy = push_str8_copy(scratch.arena, path);
  char buffer[PATH_MAX] = {0};
  realpath((char *)path_copy.Ptr, buffer);
  StringView result = push_str8_copy(arena, str8_cstring(buffer));
  scratch_end(scratch);
  return result;
}

B32
os_file_path_exists(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  StringView path_copy = push_str8_copy(scratch.arena, path);
  int access_result = access((char *)path_copy.Ptr, F_OK);
  B32 result = 0;
  if(access_result == 0)
  {
    result = 1;
  }
  scratch_end(scratch);
  return result;
}

FileProperties
os_properties_from_file_path(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  StringView path_copy = push_str8_copy(scratch.arena, path);
  struct stat f_stat = {0};
  int stat_result = stat((char *)path_copy.Ptr, &f_stat);
  FileProperties props = {0};
  if(stat_result != -1)
  {
    props = os_lnx_file_properties_from_stat(&f_stat);
  }
  scratch_end(scratch);
  return props;
}

//- rjf: file maps

OS_Handle
os_file_map_open(OS_AccessFlags flags, OS_Handle file)
{
  OS_Handle map = file;
  return map;
}

void
os_file_map_close(OS_Handle map)
{
  // NOTE(rjf): nothing to do; `map` handles are the same as `file` handles in
  // the linux implementation (on Windows they require separate handles)
}

void *
os_file_map_view_open(OS_Handle map, OS_AccessFlags flags, Rng1U64 range)
{
  if(os_handle_match(map, os_handle_zero())) { return 0; }
  int fd = (int)map.u64[0];
  int prot_flags = 0;
  if(flags & OS_AccessFlag_Write) { prot_flags |= PROT_WRITE; }
  if(flags & OS_AccessFlag_Read)  { prot_flags |= PROT_READ; }
  int map_flags = MAP_PRIVATE;
  void* base = mmap(0, dim_1u64(range), prot_flags, map_flags, fd, range.min);
  return base;
}

void
os_file_map_view_close(OS_Handle map, void* ptr, Rng1U64 range)
{
  munmap(ptr, dim_1u64(range));
}

//- rjf: directory iteration

OS_FileIter *
os_file_iter_begin(Arena* arena, StringView path, OS_FileIterFlags flags)
{
  OS_FileIter* base_iter = push_array(arena, OS_FileIter, 1);
  base_iter.flags = flags;
  OS_LNX_FileIter* iter = (OS_LNX_FileIter *)base_iter.memory;
  {
    StringView path_copy = push_str8_copy(arena, path);
    iter.dir = opendir((char *)path_copy.Ptr);
    iter.path = path_copy;
  }
  return base_iter;
}

B32
os_file_iter_next(Arena* arena, OS_FileIter* iter, OS_FileInfo* info_out)
{
  B32 good = 0;
  OS_LNX_FileIter* lnx_iter = (OS_LNX_FileIter *)iter.memory;
  for(;;)
  {
    // rjf: get next entry
    lnx_iter.dp = readdir(lnx_iter.dir);
    good = (lnx_iter.dp != 0);
    
    // rjf: unpack entry info
    struct stat st = {0};
    int stat_result = 0;
    if(good)
    {
      Temp scratch = scratch_begin(&arena, 1);
      StringView full_path = push_str8f(scratch.arena, "%S/%s", lnx_iter.path, lnx_iter.dp.d_name);
      stat_result = stat((char *)full_path.Ptr, &st);
      scratch_end(scratch);
    }
    
    // rjf: determine if filtered
    B32 filtered = 0;
    if(good)
    {
      filtered = ((st.st_mode == S_IFDIR && iter.flags & OS_FileIterFlag_SkipFolders) ||
                  (st.st_mode == S_IFREG && iter.flags & OS_FileIterFlag_SkipFiles) ||
                  (lnx_iter.dp.d_name[0] == '.' && lnx_iter.dp.d_name[1] == 0) ||
                  (lnx_iter.dp.d_name[0] == '.' && lnx_iter.dp.d_name[1] == '.' && lnx_iter.dp.d_name[2] == 0));
    }
    
    // rjf: output & exit, if good & unfiltered
    if(good && !filtered)
    {
      info_out.name = push_str8_copy(arena, str8_cstring(lnx_iter.dp.d_name));
      if(stat_result != -1)
      {
        info_out.props = os_lnx_file_properties_from_stat(&st);
      }
      break;
    }
    
    // rjf: exit if not good
    if(!good)
    {
      break;
    }
  }
  return good;
}

void
os_file_iter_end(OS_FileIter* iter)
{
  OS_LNX_FileIter* lnx_iter = (OS_LNX_FileIter *)iter.memory;
  closedir(lnx_iter.dir);
}

//- rjf: directory creation

B32
os_make_directory(StringView path)
{
  Temp scratch = scratch_begin(0, 0);
  B32 result = 0;
  StringView path_copy = push_str8_copy(scratch.arena, path);
  if(mkdir((char*)path_copy.Ptr, 0777) != -1)
  {
    result = 1;
  }
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: @os_hooks Shared Memory (Implemented Per-OS)

OS_Handle
os_shared_memory_alloc(uint64 size, StringView name)
{
  Temp scratch = scratch_begin(0, 0);
  StringView name_copy = push_str8_copy(scratch.arena, name);
  int id = shm_open((char *)name_copy.Ptr, O_RDWR, 0);
  ftruncate(id, size);
  OS_Handle result = {(uint64)id};
  scratch_end(scratch);
  return result;
}

OS_Handle
os_shared_memory_open(StringView name)
{
  Temp scratch = scratch_begin(0, 0);
  StringView name_copy = push_str8_copy(scratch.arena, name);
  int id = shm_open((char *)name_copy.Ptr, O_RDWR, 0);
  OS_Handle result = {(uint64)id};
  scratch_end(scratch);
  return result;
}

void
os_shared_memory_close(OS_Handle handle)
{
  if(os_handle_match(handle, os_handle_zero())){return;}
  int id = (int)handle.u64[0];
  close(id);
}

void *
os_shared_memory_view_open(OS_Handle handle, Rng1U64 range)
{
  if(os_handle_match(handle, os_handle_zero())){return 0;}
  int id = (int)handle.u64[0];
  void* base = mmap(0, dim_1u64(range), PROT_READ|PROT_WRITE, MAP_SHARED, id, range.min);
  return base;
}

void
os_shared_memory_view_close(OS_Handle handle, void* ptr, Rng1U64 range)
{
  if(os_handle_match(handle, os_handle_zero())){return;}
  munmap(ptr, dim_1u64(range));
}

////////////////////////////////
//~ rjf: @os_hooks Time (Implemented Per-OS)

uint64
os_now_microseconds()
{
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  uint64 result = t.tv_sec*Million(1) + (t.tv_nsec/Thousand(1));
  return result;
}

uint32
os_now_unix()
{
  time_t t = time(0);
  return (uint32)t;
}

DateTime
os_now_universal_time()
{
  time_t t = 0;
  time(&t);
  struct tm universal_tm = {0};
  gmtime_r(&t, &universal_tm);
  DateTime result = os_lnx_date_time_from_tm(universal_tm, 0);
  return result;
}

DateTime
os_universal_time_from_local(DateTime* date_time)
{
  // rjf: local DateTime . universal time_t
  tm local_tm = os_lnx_tm_from_date_time(*date_time);
  local_tm.tm_isdst = -1;
  time_t universal_t = mktime(&local_tm);
  
  // rjf: universal time_t . DateTime
  tm universal_tm = {0};
  gmtime_r(&universal_t, &universal_tm);
  DateTime result = os_lnx_date_time_from_tm(universal_tm, 0);
  return result;
}

DateTime
os_local_time_from_universal(DateTime* date_time)
{
  // rjf: universal DateTime . local time_t
  tm universal_tm = os_lnx_tm_from_date_time(*date_time);
  universal_tm.tm_isdst = -1;
  time_t universal_t = timegm(&universal_tm);
  tm local_tm = {0};
  localtime_r(&universal_t, &local_tm);
  
  // rjf: local tm . DateTime
  DateTime result = os_lnx_date_time_from_tm(local_tm, 0);
  return result;
}

void
os_sleep_milliseconds(uint32 msec)
{
  usleep(msec*Thousand(1));
}

////////////////////////////////
//~ rjf: @os_hooks Child Processes (Implemented Per-OS)

OS_Handle
os_process_launch(OS_ProcessLaunchParams* params)
{
  NotImplemented;
}

B32
os_process_join(OS_Handle handle, uint64 endt_us)
{
  NotImplemented;
}

void
os_process_detach(OS_Handle handle)
{
  NotImplemented;
}

////////////////////////////////
//~ rjf: @os_hooks Threads (Implemented Per-OS)

OS_Handle
os_thread_launch(OS_ThreadFunctionType* func, void* ptr, void* params)
{
  OS_LNX_Entity* entity = os_lnx_entity_alloc(OS_LNX_EntityKind_Thread);
  entity.thread.func = func;
  entity.thread.ptr = ptr;
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int pthread_result = pthread_create(&entity.thread.handle, &attr, os_lnx_thread_entry_point, entity);
    pthread_attr_destroy(&attr);
    if(pthread_result == -1)
    {
      os_lnx_entity_release(entity);
      entity = 0;
    }
  }
  OS_Handle handle = {(uint64)entity};
  return handle;
}

B32
os_thread_join(OS_Handle handle, uint64 endt_us)
{
  if(os_handle_match(handle, os_handle_zero())) { return 0; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)handle.u64[0];
  int join_result = pthread_join(entity.thread.handle, 0);
  B32 result = (join_result == 0);
  os_lnx_entity_release(entity);
  return result;
}

void
os_thread_detach(OS_Handle handle)
{
  if(os_handle_match(handle, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)handle.u64[0];
  os_lnx_entity_release(entity);
}

////////////////////////////////
//~ rjf: @os_hooks Synchronization Primitives (Implemented Per-OS)

//- rjf: mutexes

OS_Handle
os_mutex_alloc()
{
  OS_LNX_Entity* entity = os_lnx_entity_alloc(OS_LNX_EntityKind_Mutex);
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  int init_result = pthread_mutex_init(&entity.mutex_handle, &attr);
  pthread_mutexattr_destroy(&attr);
  if(init_result == -1)
  {
    os_lnx_entity_release(entity);
    entity = 0;
  }
  OS_Handle handle = {(uint64)entity};
  return handle;
}

void
os_mutex_release(OS_Handle mutex)
{
  if(os_handle_match(mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)mutex.u64[0];
  pthread_mutex_destroy(&entity.mutex_handle);
  os_lnx_entity_release(entity);
}

void
os_mutex_take(OS_Handle mutex)
{
  if(os_handle_match(mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)mutex.u64[0];
  pthread_mutex_lock(&entity.mutex_handle);
}

void
os_mutex_drop(OS_Handle mutex)
{
  if(os_handle_match(mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)mutex.u64[0];
  pthread_mutex_unlock(&entity.mutex_handle);
}

//- rjf: reader/writer mutexes

OS_Handle
os_rw_mutex_alloc()
{
  OS_LNX_Entity* entity = os_lnx_entity_alloc(OS_LNX_EntityKind_RWMutex);
  int init_result = pthread_rwlock_init(&entity.rwmutex_handle, 0);
  if(init_result == -1)
  {
    os_lnx_entity_release(entity);
    entity = 0;
  }
  OS_Handle handle = {(uint64)entity};
  return handle;
}

void
os_rw_mutex_release(OS_Handle rw_mutex)
{
  if(os_handle_match(rw_mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)rw_mutex.u64[0];
  pthread_rwlock_destroy(&entity.rwmutex_handle);
  os_lnx_entity_release(entity);
}

void
os_rw_mutex_take_r(OS_Handle rw_mutex)
{
  if(os_handle_match(rw_mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)rw_mutex.u64[0];
  pthread_rwlock_rdlock(&entity.rwmutex_handle);
}

void
os_rw_mutex_drop_r(OS_Handle rw_mutex)
{
  if(os_handle_match(rw_mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)rw_mutex.u64[0];
  pthread_rwlock_unlock(&entity.rwmutex_handle);
}

void
os_rw_mutex_take_w(OS_Handle rw_mutex)
{
  if(os_handle_match(rw_mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)rw_mutex.u64[0];
  pthread_rwlock_wrlock(&entity.rwmutex_handle);
}

void
os_rw_mutex_drop_w(OS_Handle rw_mutex)
{
  if(os_handle_match(rw_mutex, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)rw_mutex.u64[0];
  pthread_rwlock_unlock(&entity.rwmutex_handle);
}

//- rjf: condition variables

OS_Handle
os_condition_variable_alloc()
{
  OS_LNX_Entity* entity = os_lnx_entity_alloc(OS_LNX_EntityKind_ConditionVariable);
  int init_result = pthread_cond_init(&entity.cv.cond_handle, 0);
  if(init_result == -1)
  {
    os_lnx_entity_release(entity);
    entity = 0;
  }
  int init2_result = 0;
  if(entity)
  {
    init2_result = pthread_mutex_init(&entity.cv.rwlock_mutex_handle, 0);
  }
  if(init2_result == -1)
  {
    pthread_cond_destroy(&entity.cv.cond_handle);
    os_lnx_entity_release(entity);
    entity = 0;
  }
  OS_Handle handle = {(uint64)entity};
  return handle;
}

void
os_condition_variable_release(OS_Handle cv)
{
  if(os_handle_match(cv, os_handle_zero())) { return; }
  OS_LNX_Entity* entity = (OS_LNX_Entity *)cv.u64[0];
  pthread_cond_destroy(&entity.cv.cond_handle);
  pthread_mutex_destroy(&entity.cv.rwlock_mutex_handle);
  os_lnx_entity_release(entity);
}

B32
os_condition_variable_wait(OS_Handle cv, OS_Handle mutex, uint64 endt_us)
{
  if(os_handle_match(cv, os_handle_zero())) { return 0; }
  if(os_handle_match(mutex, os_handle_zero())) { return 0; }
  OS_LNX_Entity* cv_entity = (OS_LNX_Entity *)cv.u64[0];
  OS_LNX_Entity* mutex_entity = (OS_LNX_Entity *)mutex.u64[0];
  struct timespec endt_timespec;
  endt_timespec.tv_sec = endt_us/Million(1);
  endt_timespec.tv_nsec = Thousand(1) * (endt_us - (endt_us/Million(1))*Million(1));
  int wait_result = pthread_cond_timedwait(&cv_entity.cv.cond_handle, &mutex_entity.mutex_handle, &endt_timespec);
  B32 result = (wait_result != ETIMEDOUT);
  return result;
}

B32
os_condition_variable_wait_rw_r(OS_Handle cv, OS_Handle mutex_rw, uint64 endt_us)
{
  // TODO(rjf): because pthread does not supply cv/rw natively, I had to hack
  // this together, but this would probably just be a lot better if we just
  // implemented the primitives ourselves with e.g. futexes
  //
  if(os_handle_match(cv, os_handle_zero())) { return 0; }
  if(os_handle_match(mutex_rw, os_handle_zero())) { return 0; }
  OS_LNX_Entity* cv_entity = (OS_LNX_Entity *)cv.u64[0];
  OS_LNX_Entity* rw_mutex_entity = (OS_LNX_Entity *)mutex_rw.u64[0];
  struct timespec endt_timespec;
  endt_timespec.tv_sec = endt_us/Million(1);
  endt_timespec.tv_nsec = Thousand(1) * (endt_us - (endt_us/Million(1))*Million(1));
  B32 result = 0;
  for(;;)
  {
    pthread_mutex_lock(&cv_entity.cv.rwlock_mutex_handle);
    int wait_result = pthread_cond_timedwait(&cv_entity.cv.cond_handle, &cv_entity.cv.rwlock_mutex_handle, &endt_timespec);
    if(wait_result != ETIMEDOUT)
    {
      pthread_rwlock_rdlock(&rw_mutex_entity.rwmutex_handle);
      pthread_mutex_unlock(&cv_entity.cv.rwlock_mutex_handle);
      result = 1;
      break;
    }
    pthread_mutex_unlock(&cv_entity.cv.rwlock_mutex_handle);
    if(wait_result == ETIMEDOUT)
    {
      break;
    }
  }
  return result;
}

B32
os_condition_variable_wait_rw_w(OS_Handle cv, OS_Handle mutex_rw, uint64 endt_us)
{
  // TODO(rjf): because pthread does not supply cv/rw natively, I had to hack
  // this together, but this would probably just be a lot better if we just
  // implemented the primitives ourselves with e.g. futexes
  //
  if(os_handle_match(cv, os_handle_zero())) { return 0; }
  if(os_handle_match(mutex_rw, os_handle_zero())) { return 0; }
  OS_LNX_Entity* cv_entity = (OS_LNX_Entity *)cv.u64[0];
  OS_LNX_Entity* rw_mutex_entity = (OS_LNX_Entity *)mutex_rw.u64[0];
  struct timespec endt_timespec;
  endt_timespec.tv_sec = endt_us/Million(1);
  endt_timespec.tv_nsec = Thousand(1) * (endt_us - (endt_us/Million(1))*Million(1));
  B32 result = 0;
  for(;;)
  {
    pthread_mutex_lock(&cv_entity.cv.rwlock_mutex_handle);
    int wait_result = pthread_cond_timedwait(&cv_entity.cv.cond_handle, &cv_entity.cv.rwlock_mutex_handle, &endt_timespec);
    if(wait_result != ETIMEDOUT)
    {
      pthread_rwlock_wrlock(&rw_mutex_entity.rwmutex_handle);
      pthread_mutex_unlock(&cv_entity.cv.rwlock_mutex_handle);
      result = 1;
      break;
    }
    pthread_mutex_unlock(&cv_entity.cv.rwlock_mutex_handle);
    if(wait_result == ETIMEDOUT)
    {
      break;
    }
  }
  return result;
}

void
os_condition_variable_signal(OS_Handle cv)
{
  if(os_handle_match(cv, os_handle_zero())) { return; }
  OS_LNX_Entity* cv_entity = (OS_LNX_Entity *)cv.u64[0];
  pthread_cond_signal(&cv_entity.cv.cond_handle);
}

void
os_condition_variable_broadcast(OS_Handle cv)
{
  if(os_handle_match(cv, os_handle_zero())) { return; }
  OS_LNX_Entity* cv_entity = (OS_LNX_Entity *)cv.u64[0];
  pthread_cond_broadcast(&cv_entity.cv.cond_handle);
}

//- rjf: cross-process semaphores

OS_Handle
os_semaphore_alloc(uint32 initial_count, uint32 max_count, StringView name)
{
  NotImplemented;
}

void
os_semaphore_release(OS_Handle semaphore)
{
  NotImplemented;
}

OS_Handle
os_semaphore_open(StringView name)
{
  NotImplemented;
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
  char* path_cstr = (char *)push_str8_copy(scratch.arena, path).Ptr;
  void* so = dlopen(path_cstr, RTLD_LAZY);
  OS_Handle lib = { (uint64)so };
  scratch_end(scratch);
  return lib;
}

VoidProc*
os_library_load_proc(OS_Handle lib, StringView name)
{
  Temp scratch = scratch_begin(0, 0);
  void* so = (void *)lib.u64;
  char* name_cstr = (char *)push_str8_copy(scratch.arena, name).Ptr;
  VoidProc* proc = (VoidProc *)dlsym(so, name_cstr);
  scratch_end(scratch);
  return proc;
}

void
os_library_close(OS_Handle lib)
{
  void* so = (void *)lib.u64;
  dlclose(so);
}

////////////////////////////////
//~ rjf: @os_hooks Safe Calls (Implemented Per-OS)

void
os_safe_call(OS_ThreadFunctionType* func, OS_ThreadFunctionType* fail_handler, void* ptr)
{
  // rjf: push handler to chain
  OS_LNX_SafeCallChain chain = {0};
  SLLStackPush(os_lnx_safe_call_chain, &chain);
  chain.fail_handler = fail_handler;
  chain.ptr = ptr;
  
  // rjf: set up sig handler info
  struct sigaction new_act = {0};
  new_act.sa_handler = os_lnx_safe_call_sig_handler;
  int signals_to_handle[] =
  {
    SIGILL, SIGFPE, SIGSEGV, SIGBUS, SIGTRAP,
  };
  struct sigaction og_act[ArrayCount(signals_to_handle)] = {0};
  
  // rjf: attach handler info for all signals
  for(uint32 i = 0; i < ArrayCount(signals_to_handle); i += 1)
  {
    sigaction(signals_to_handle[i], &new_act, &og_act[i]);
  }
  
  // rjf: call function
  func(ptr);
  
  // rjf: reset handler info for all signals
  for(uint32 i = 0; i < ArrayCount(signals_to_handle); i += 1)
  {
    sigaction(signals_to_handle[i], &og_act[i], 0);
  }
}

////////////////////////////////
//~ rjf: @os_hooks GUIDs (Implemented Per-OS)

OS_Guid
os_make_guid()
{
  uint8 random_bytes[16] = {0};
  StaticAssert(sizeof(random_bytes) == sizeof(OS_Guid), os_lnx_guid_size_check);
  getrandom(random_bytes, sizeof(random_bytes), 0);
  OS_Guid guid = {0};
  MemoryCopy(&guid, random_bytes, sizeof(random_bytes));
  guid.data3 &= 0x0fff;
  guid.data3 |= (4 << 12);
  guid.data4[0] &= 0x3f;
  guid.data4[0] |= 0x80;
  return guid;
}

////////////////////////////////
//~ rjf: @os_hooks Entry Points (Implemented Per-OS)

int
main(int argc, char** argv)
{
  //- rjf: set up OS layer
  {
    //- rjf: get statically-allocated system/process info
    {
      OS_SystemInfo* info = &os_lnx_state.system_info;
      info.logical_processor_count = (uint32)get_nprocs();
      info.page_size               = (uint64)getpagesize();
      info.large_page_size         = MB(2);
      info.allocation_granularity  = info.page_size;
    }
    {
      OS_ProcessInfo* info = &os_lnx_state.process_info;
      info.pid = (uint32)getpid();
    }
    
    //- rjf: set up thread context
    static TCTX tctx;
    tctx_init_and_equip(&tctx);
    
    //- rjf: set up dynamically allocated state
    os_lnx_state.arena = arena_alloc();
    os_lnx_state.entity_arena = arena_alloc();
    pthread_mutex_init(&os_lnx_state.entity_mutex, 0);
    
    //- rjf: grab dynamically allocated system info
    {
      Temp scratch = scratch_begin(0, 0);
      OS_SystemInfo* info = &os_lnx_state.system_info;
      
      // rjf: get machine name
      B32 got_final_result = 0;
      uint8* buffer = 0;
      int size = 0;
      for(int64 cap = 4096, r = 0; r < 4; cap *= 2, r += 1)
      {
        scratch_end(scratch);
        buffer = push_array_no_zero(scratch.arena, uint8, cap);
        size = gethostname((char*)buffer, cap);
        if(size < cap)
        {
          got_final_result = 1;
          break;
        }
      }
      
      // rjf: save name to info
      if(got_final_result && size > 0)
      {
        info.machine_name.size = size;
        info.machine_name.Ptr = push_array_no_zero(os_lnx_state.arena, uint8, info.machine_name.size + 1);
        MemoryCopy(info.machine_name.Ptr, buffer, info.machine_name.size);
        info.machine_name[info.machine_name.size] = 0;
      }
      
      scratch_end(scratch);
    }
    
    //- rjf: grab dynamically allocated process info
    {
      Temp scratch = scratch_begin(0, 0);
      OS_ProcessInfo* info = &os_lnx_state.process_info;
      
      // rjf: grab binary path
      {
        // rjf: get self str
        B32 got_final_result = 0;
        uint8* buffer = 0;
        int size = 0;
        for(int64 cap = PATH_MAX, r = 0; r < 4; cap *= 2, r += 1)
        {
          scratch_end(scratch);
          buffer = push_array_no_zero(scratch.arena, uint8, cap);
          size = readlink("/proc/self/exe", (char*)buffer, cap);
          if(size < cap)
          {
            got_final_result = 1;
            break;
          }
        }
        
        // rjf: save
        if(got_final_result && size > 0)
        {
          StringView full_name = StringView(buffer, size);
          StringView name_chopped = str8_chop_last_slash(full_name);
          info.binary_path = push_str8_copy(os_lnx_state.arena, name_chopped);
        }
      }
      
      // rjf: grab initial directory
      {
        info.initial_path = os_get_current_path(os_lnx_state.arena);
      }
      
      // rjf: grab home directory
      {
        char* home = getenv("HOME");
        info.user_program_data_path = str8_cstring(home);
      }
      
      scratch_end(scratch);
    }
  }
  
  //- rjf: call into "real" entry point
  main_thread_base_entry_point(entry_point, argv, (uint64)argc);
}

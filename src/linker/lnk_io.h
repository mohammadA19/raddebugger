#pragma once

typedef U32 LNK_IO_Flags;
enum
{
  LNK_IO_Flags_MemoryMapFiles = (1 << 0),
};

typedef struct
{
  LNK_IO_Flags io_flags;
  String8Array path_arr;
  String8Array data_arr;
  OS_Handle   *handle_arr;
  U64         *size_arr;
  U64         *off_arr;
  U8          *buffer;
} LNK_DiskReader;

// --- Shared File API ---------------------------------------------------------

shared_function int      lnk_open_file_read(char *path, uint64_t path_size, void *handle_buffer, uint64_t handle_buffer_max);
shared_function int      lnk_open_file_write(char *path, uint64_t path_size, void *handle_buffer, uint64_t handle_buffer_max);
shared_function void     lnk_close_file(void *raw_handle);
shared_function uint64_t lnk_size_from_file(void *raw_handle);
shared_function uint64_t lnk_read_file(void *raw_handle, void *buffer, uint64_t buffer_max);
shared_function uint64_t lnk_write_file(void *raw_handle, uint64_t offset, void *buffer, uint64_t buffer_size);

// --- IO Functions ------------------------------------------------------------

internal String8List lnk_file_search(Arena *arena, String8List dir_list, string file_path);

internal OS_Handle lnk_file_open_with_rename_permissions(string path);
internal B32       lnk_file_set_delete_on_close(OS_Handle handle, B32 delete_file);
internal B32       lnk_file_rename(OS_Handle handle, string new_name);

internal string      lnk_read_data_from_file_path(Arena *arena, LNK_IO_Flags io_flags, string path);
internal String8Array lnk_read_data_from_file_path_parallel(TP_Context *tp, Arena *arena, LNK_IO_Flags io_flags, String8Array path_arr);

internal void lnk_write_data_list_to_file_path(string path, string temp_path, String8List list);
internal void lnk_write_data_to_file_path(string path, string temp_path, string data);


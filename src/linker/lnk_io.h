#pragma once

////////////////////////////////

srtuct
{
  String8Array path_arr;
  String8Array data_arr;
  OS_Handle   *handle_arr;
  ulong         *size_arr;
  ulong         *off_arr;
  byte          *buffer;
};

////////////////////////////////
// Shared File API

shared_function int      lnk_open_file_read(char *path, uint64_t path_size, void *handle_buffer, uint64_t handle_buffer_max);
shared_function int      lnk_open_file_write(char *path, uint64_t path_size, void *handle_buffer, uint64_t handle_buffer_max);
shared_function void     lnk_close_file(void *raw_handle);
shared_function uint64_t lnk_size_from_file(void *raw_handle);
shared_function uint64_t lnk_read_file(void *raw_handle, void *buffer, uint64_t buffer_max);
shared_function uint64_t lnk_write_file(void *raw_handle, uint64_t offset, void *buffer, uint64_t buffer_size);

////////////////////////////////

String8      lnk_read_data_from_file_path(Arena *arena, String8 path);
String8Array lnk_read_data_from_file_path_parallel(TP_Context *tp, Arena *arena, String8Array path_arr);

void lnk_write_data_list_to_file_path(String8 path, String8List list);
void lnk_write_data_to_file_path(String8 path, String8 data);

String8List lnk_file_search(Arena *arena, String8List dir_list, String8 file_path);


#pragma once

enum LNK_IO_Flags : U32
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






// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define MSF_PAGE_STATE_FREE  1
#define MSF_PAGE_STATE_ALLOC 0

#define MSF_FPM0 1
#define MSF_FPM1 2

#define MSF_DEFAULT_PAGE_SIZE 4096
#define MSF_DEFAULT_FPM MSF_FPM0

struct MSF_PageNumberArray
{
  uint64             count;
  MSF_PageNumber* v;
}

struct MSF_PageNode
{
  struct MSF_PageNode* next;
  struct MSF_PageNode* prev;
  MSF_PageNumber       pn;
}

struct MSF_PageList
{
  MSF_PageNode* first;
  MSF_PageNode* last;
  MSF_UInt      count;
}

struct MSF_Stream
{
  MSF_StreamNumber sn;
  MSF_UInt         size;
  MSF_UInt         pos;
  MSF_PageNode*    pos_page;
  MSF_PageList     page_list;
}

struct MSF_StreamNode
{
  struct MSF_StreamNode* next;
  struct MSF_StreamNode* prev;
  MSF_Stream             data;
}

struct MSF_StreamList
{
  MSF_UInt        count;
  MSF_StreamNode* first;
  MSF_StreamNode* last;
}

struct MSF_PageDataNode
{
  struct MSF_PageDataNode* next;
  struct MSF_PageDataNode* prev;
  uint8*                      data;
}

struct MSF_PageDataList
{
  MSF_PageDataNode* first;
  MSF_PageDataNode* last;
  MSF_UInt          count;
}

struct MSF_Context
{
  Arena*           arena;
  MSF_UInt         page_size;
  MSF_UInt         active_fpm;
  MSF_UInt         fpm_rover;
  MSF_PageNumber   page_count;
  MSF_PageDataList page_data_list;
  MSF_PageDataList page_data_pool;
  MSF_PageList     header_page_list;
  MSF_PageList     root_page_list;
  MSF_PageList     st_page_list;
  MSF_PageList     page_pool;
  MSF_StreamList   st;
}

enum MSF_Error
{
  MSF_Error_OK,
  
  // if you get this error this means stream table was divided into too many
  // pages, and to fix this you need to bump up the page size
  MSF_Error_STREAM_TABLE_HAS_TOO_MANY_PAGES,
  
  MSF_OpenError_NOT_ENOUGH_BYTES_TO_READ_HEADER,
  MSF_OpenError_INVALID_MAGIC,
  MSF_OpenError_PAGE_SIZE_IS_NOT_POW2,
  MSF_OpenError_INVALID_PAGE_SIZE,
  MSF_OpenError_NOT_ENOUGH_PAGES_TO_INIT,
  MSF_OpenError_INVALID_ROOT_STREAM_PAGE_NUMBER,
  MSF_OpenError_UNABLE_TO_READ_STREAM_TABLE_PAGE_NUMBERS,
  MSF_OpenError_STREAM_COUNT_OVERFLOW,
  MSF_OpenError_UNABLE_TO_READ_STREAM_SIZES,
  MSF_OpenError_INVALID_STREAM_TABLE,
  MSF_OpenError_INVALID_ACTIVE_FPM,
  MSF_OpenError_PAGE_COUNT_DOESNT_MATCH_DATA_SIZE,
  
  MSF_BuildError_UNABLE_TO_WRITE_STREAM_TABLE,
  MSF_BuildError_UNABLE_TO_WRITE_STREAM_TABLE_PAGE_NUMBER_DIRECTORY,
  MSF_BuildError_UNABLE_TO_WRITE_ROOT_DIRECTORY,
  MSF_BuildError_UNABLE_TO_WRITE_HEADER,
}

////////////////////////////////

struct
{
  MSF_UInt         page_size;
  MSF_PageDataList page_data_list;
  MSF_PageList     page_list;
  MSF_UInt         stream_pos;
  String8          data;
  Rng1U64*         range_arr;
}

////////////////////////////////

MSF_Context *    msf_alloc(MSF_UInt page_size, MSF_UInt active_fpm);
MSF_Error        msf_open(String8 data, MSF_Context** msf_out);
void             msf_release(MSF_Context** msf_ptr);
MSF_Error        msf_build(MSF_Context* msf);
uint64              msf_get_save_size(MSF_Context* msf);
String8List      msf_get_page_data_nodes(Arena* arena, MSF_Context* msf);
B32              msf_save(MSF_Context* msf, void* buffer, uint64 buffer_size);
MSF_Error        msf_save_arena(Arena* arena, MSF_Context* msf, String8* data_out);
MSF_StreamNode * msf_find_stream_node(MSF_Context* msf, MSF_StreamNumber sn);
MSF_Stream *     msf_find_stream(MSF_Context* msf, MSF_StreamNumber sn);
B32              msf_grow(MSF_Context* msf, MSF_PageNumber page_count);
MSF_PageNumber * msf_alloc_pn_arr(Arena* arena, MSF_Context* msf, MSF_UInt alloc_count);
void             msf_free_pn_arr(MSF_Context* msf, MSF_PageNumber* pn_arr, MSF_UInt pn_count);
MSF_PageList     msf_alloc_pages(MSF_Context* msf, MSF_UInt alloc_count);
void             msf_free_pages(MSF_Context* msf, MSF_PageList* page_list);

MSF_StreamNumber msf_stream_alloc_ex(MSF_Context* msf, MSF_UInt size);
MSF_StreamNumber msf_stream_alloc(MSF_Context* msf);
B32              msf_stream_free(MSF_Context* msf, MSF_StreamNumber sn);
B32              msf_stream_resize(MSF_Context* msf, MSF_StreamNumber sn, MSF_UInt new_size);
B32              msf_stream_resize_ex(MSF_Context* msf, MSF_Stream* stream, MSF_UInt size);
void             msf_stream_set_size(MSF_Context* msf, MSF_StreamNumber sn, MSF_UInt size);
MSF_UInt         msf_stream_get_size(MSF_Context* msf, MSF_StreamNumber sn);
MSF_UInt         msf_stream_get_cap(MSF_Context* msf, MSF_StreamNumber);
MSF_UInt         msf_stream_get_pos(MSF_Context* msf, MSF_StreamNumber sn);
void             msf_stream_align(MSF_Context* msf, MSF_StreamNumber sn, MSF_UInt align);
B32              msf_stream_reserve(MSF_Context* msf, MSF_StreamNumber sn, MSF_UInt size);
B32              msf_stream_seek(MSF_Context* msf, MSF_StreamNumber sn, MSF_UInt new_pos);
B32              msf_stream_seek_start(MSF_Context* msf, MSF_StreamNumber sn);
B32              msf_stream_seek_end(MSF_Context* msf, MSF_StreamNumber sn);

MSF_UInt msf_stream_read(MSF_Context* msf, MSF_StreamNumber sn, void* dst, MSF_UInt dst_len);
String8  msf_stream_read_block(Arena* arena, MSF_Context* msf, MSF_StreamNumber sn, uint64 block_size);
String8  msf_stream_read_string(Arena* arena, MSF_Context* msf, MSF_StreamNumber sn);
int8       msf_stream_read_s8(MSF_Context* msf, MSF_StreamNumber sn);
int16      msf_stream_read_s16(MSF_Context* msf, MSF_StreamNumber sn);
int32      msf_stream_read_s32(MSF_Context* msf, MSF_StreamNumber sn);
int64      msf_stream_read_s64(MSF_Context* msf, MSF_StreamNumber sn);
uint8       msf_stream_read_u8(MSF_Context* msf, MSF_StreamNumber sn);
uint16      msf_stream_read_u16(MSF_Context* msf, MSF_StreamNumber sn);
uint32      msf_stream_read_u32(MSF_Context* msf, MSF_StreamNumber sn);
uint64      msf_stream_read_u64(MSF_Context* msf, MSF_StreamNumber sn);
#define msf_stream_read_array(msf, sn, ptr, count) msf_stream_read(msf, sn, ptr, sizeof(*ptr) * (count))
#define msf_stream_read_struct(msf, sn, ptr) msf_stream_read_array(msf, sn, ptr, 1)

B32 msf_stream_write(MSF_Context* msf, MSF_StreamNumber sn, void* buffer, MSF_UInt buffer_size);
B32 msf_stream_write_string(MSF_Context* msf, MSF_StreamNumber sn, String8 string);
B32 msf_stream_write_list(MSF_Context* msf, MSF_StreamNumber sn, String8List list);
B32 msf_stream_write_uint(MSF_Context* msf, MSF_StreamNumber sn, MSF_UInt value);
B32 msf_stream_write_cstr(MSF_Context* msf, MSF_StreamNumber sn, String8 string);
B32 msf_stream_write_u8(MSF_Context* msf, MSF_StreamNumber sn, uint8 value);
B32 msf_stream_write_u16(MSF_Context* msf, MSF_StreamNumber sn, uint16 value);
B32 msf_stream_write_u32(MSF_Context* msf, MSF_StreamNumber sn, uint32 value);
B32 msf_stream_write_u64(MSF_Context* msf, MSF_StreamNumber sn, uint64 value);
B32 msf_stream_write_s8(MSF_Context* msf, MSF_StreamNumber sn, int8 value);
B32 msf_stream_write_s16(MSF_Context* msf, MSF_StreamNumber sn, int16 value);
B32 msf_stream_write_s32(MSF_Context* msf, MSF_StreamNumber sn, int32 value);
B32 msf_stream_write_s64(MSF_Context* msf, MSF_StreamNumber sn, int64 value);
B32 msf_stream_write_parallel(TP_Context* tp, MSF_Context* msf, MSF_StreamNumber sn, void* buffer, MSF_UInt buffer_size);
#define msf_stream_write_array(m, s, v, c) msf_stream_write(m, s, (void*)(v), sizeof(*(v)) * (c))
#define msf_stream_write_struct(m, s, v )  msf_stream_write_array(m, s, v, 1)

MSF_UInt       msf_count_pages(MSF_UInt page_size, uint64 data_size);
MSF_PageNumber msf_get_page_count_cap(MSF_PageDataList page_data_list, MSF_UInt page_size);
MSF_UInt       msf_get_fpm_interval_correct(MSF_UInt page_size);
MSF_UInt       msf_get_fpm_interval_wrong(MSF_UInt page_size);
MSF_UInt       msf_get_fpm_idx_from_pn(MSF_UInt page_size, MSF_PageNumber pn);
MSF_UInt       msf_get_fpm_page_bit_state(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageNumber pn);
void           msf_set_fpm_bit(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageNumber pn, B32 state);
B32            msf_write(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList page_list, MSF_UInt offset, void* buffer, MSF_UInt buffer_size);
MSF_UInt       msf_read(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList page_list, MSF_UInt offset, void* buffer, MSF_UInt buffer_size);

char * msf_error_to_string(MSF_Error code);


// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define MSF_PAGE_STATE_FREE  1
#define MSF_PAGE_STATE_ALLOC 0

#define MSF_FPM0 1
#define MSF_FPM1 2

#define MSF_DEFAULT_PAGE_SIZE 4096
#define MSF_DEFAULT_FPM MSF_FPM0

typedef struct MSF_PageNumberArray
{
    U64             count;
    MSF_PageNumber *v;
} MSF_PageNumberArray;

typedef struct MSF_PageNode
{
    struct MSF_PageNode *next;
    struct MSF_PageNode *prev;
    MSF_PageNumber       pn;
} MSF_PageNode;

typedef struct MSF_PageList
{
    MSF_PageNode *first;
    MSF_PageNode *last;
    MSF_UInt      count;
} MSF_PageList;

typedef struct MSF_Stream
{
    MSF_StreamNumber sn;
    MSF_UInt         size;
    MSF_UInt         pos;
    MSF_PageNode    *pos_page;
    MSF_PageList     page_list;
} MSF_Stream;

typedef struct MSF_StreamNode
{
    struct MSF_StreamNode *next;
    struct MSF_StreamNode *prev;
    MSF_Stream             data;
} MSF_StreamNode;

typedef struct MSF_StreamList
{
    MSF_UInt        count;
    MSF_StreamNode *first;
    MSF_StreamNode *last;
} MSF_StreamList;

typedef struct MSF_PageDataNode
{
    struct MSF_PageDataNode *next;
    struct MSF_PageDataNode *prev;
    U8                      *data;
} MSF_PageDataNode;

typedef struct MSF_PageDataList
{
    MSF_PageDataNode *first;
    MSF_PageDataNode *last;
    MSF_UInt          count;
} MSF_PageDataList;

typedef struct MSF_Context
{
    Arena           *arena;
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
    MSF_StreamList   sectab;
} MSF_Context;

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
} MSF_Error;

////////////////////////////////

typedef struct
{
    MSF_UInt         page_size;
    MSF_PageDataList page_data_list;
    MSF_PageList     page_list;
    MSF_UInt         stream_pos;
    String8          data;
    Rng1U64         *range_arr;
} MSF_WriteTask;

////////////////////////////////



#define msf_stream_read_array(msf, sn, ptr, count) msf_stream_read(msf, sn, ptr, sizeof(*ptr) * (count))
#define msf_stream_read_struct(msf, sn, ptr) msf_stream_read_array(msf, sn, ptr, 1)

#define msf_stream_write_array(m, s, v, c) msf_stream_write(m, s, (void*)(v), sizeof(*(v)) * (c))
#define msf_stream_write_struct(m, s, v )  msf_stream_write_array(m, s, v, 1)




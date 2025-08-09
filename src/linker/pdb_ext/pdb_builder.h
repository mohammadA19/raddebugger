// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////

#define PDB_NATURAL_ALIGN 4
#define PDB_SYMBOL_ALIGN PDB_NATURAL_ALIGN

////////////////////////////////
// Hash table

#define PDB_HASH_TABLE_PACK_FUNC(name) void name(Arena *arena, String8List *local_data_srl, String8List *key_value_srl, string key, string value, void *ud)
typedef PDB_HASH_TABLE_PACK_FUNC(PDB_HashTablePackFunc);

#define PDB_HASH_TABLE_UNPACK_FUNC(name) B32 name(void *ud, string local_data, string key_value_data, U64 *key_value_cursor, string *key_out, string *value_out)
typedef PDB_HASH_TABLE_UNPACK_FUNC(PDB_HashTableUnpackFunc);

typedef struct PDB_HashTableBucket
{
  string key;
  string value;
} PDB_HashTableBucket;

typedef struct PDB_HashTable
{
  Arena               *arena;
  PDB_HashTableBucket *bucket_arr;
  U32Array             present_bits;
  U32Array             deleted_bits;
  U32   			         max;
  U32                  count;
} PDB_HashTable;

typedef enum
{
  PDB_HashTableParseError_OK,
  PDB_HashTableParseError_OUT_OF_BYTES,
  PDB_HashTableParseError_CORRUPTED
} PDB_HashTableParseError;

////////////////////////////////
// String Table

typedef struct PDB_StringTableBucket
{
  string          data;
  PDB_StringOffset offset;
  PDB_StringIndex  istr;
} PDB_StringTableBucket;

typedef struct PDB_StringTable
{
  Arena *arena;
  U32    version;
  U32    size;
  U32    bucket_count;
  U32    bucket_max;
  U32   *ibucket_array;
  PDB_StringTableBucket **bucket_array;
} PDB_StringTable;

typedef enum
{
  PDB_StringTableOpenError_OK,
  PDB_StringTableOpenError_BAD_MAGIC,
  PDB_StringTableOpenError_UNKNOWN_VERSION,
  PDB_StringTableOpenError_CORRUPTED,
  PDB_StringTableOpenError_STRING_OFFSET_OUT_OF_BOUNDS,
  PDB_StringTableOpenError_OFFSETS_EXCEED_BUCKET_COUNT
} PDB_StringTableOpenError;

////////////////////////////////
// Type Server

#define PDB_TYPE_HINT_STEP 128
#define PDB_LEAF_ALIGN PDB_NATURAL_ALIGN 

typedef enum
{
  PDB_OpenTypeServerError_OK,
  PDB_OpenTypeServerError_UNKNOWN,
  PDB_OpenTypeServerError_INVALID_BUCKET_COUNT,
  PDB_OpenTypeServerError_INVALID_TI_RANGE,
  PDB_OpenTypeServerError_UNSUPPORTED_VERSION,
} PDB_OpenTypeServerError;

typedef struct PDB_TypeBucket
{
  struct PDB_TypeBucket *next;
  string                raw_leaf;
  CV_TypeIndex           type_index;
} PDB_TypeBucket;

typedef struct PDB_TypeServer
{
  Arena             *arena;
  CV_TypeIndex       ti_lo;
  String8List        leaf_list;
  U64                bucket_cap;
  PDB_TypeBucket   **buckets;
  MSF_StreamNumber   hash_sn;
  PDB_HashTable      hash_adj;
} PDB_TypeServer;

typedef struct PDB_TypeHashStreamInfo
{
  PDB_OffsetSize hash_vals;
  PDB_OffsetSize ti_offs;
  PDB_OffsetSize hash_adj;
} PDB_TypeHashStreamInfo;

typedef struct PDB_TypeServerParse
{
  Rng1U64 ti_range;
  string leaf_data;
} PDB_TypeServerParse;

typedef struct
{
  CV_DebugT       debug_t;
  U64            *udt_counts;
  U64            *udt_offsets;
  Rng1U64        *ranges;
  PDB_TypeServer *type_server;
  PDB_TypeBucket *udt_buckets;
} PDB_PushLeafTask;

typedef struct
{
  PDB_TypeServer *ts;
  U32            *map;
} PDB_WriteTypeToBucketMap;

typedef struct
{
  CV_TypeIndex    ti_lo;
  CV_TypeIndex    ti_hi;
  U64             hint_count;
  PDB_TpiOffHint *hint_arr;
  String8Node   **lf_arr;
  Rng1U64        *lf_range_arr;
  U64            *lf_cursor_arr;
  U8             *lf_buf;
  U64             lf_buf_size;
} PDB_WriteTypesTask;

////////////////////////////////
// Info

typedef struct PDB_InfoParse
{
  PDB_TpiVersion   version;
  COFF_TimeStamp   time_stamp;
  U32              age;
  Guid             guid;
  string          extra_info;
} PDB_InfoParse;

typedef struct PDB_InfoContext
{
  Arena            *arena;
  COFF_TimeStamp    time_stamp;
  U32               age;
  Guid              guid;
  PDB_FeatureFlags  flags;
  PDB_HashTable     named_stream_ht;
  PDB_HashTable     src_header_block_ht;
  PDB_StringTable   strtab;
} PDB_InfoContext;

////////////////////////////////
// SRC Header Block

typedef enum
{
  PDB_SrcError_OK,
  PDB_SrcError_DUPLICATE_NAME_STREAM,
  PDB_SrcError_DUPLICATE_ENTRY,
  PDB_SrcError_UNABLE_TO_WRITE_DATA,
  PDB_SrcError_UNSUPPORTED_COMPRESSION,
  PDB_SrcError_UNKNOWN
} PDB_SrcError;

////////////////////////////////
// GSI

#define PDB_GSI_V70_SYMBOL_ALIGN 4
#define PDB_GSI_V70_WORD_SIZE    32
#define PDB_GSI_V70_BUCKET_COUNT 4096
#define PDB_GSI_V70_BITMAP_COUNT ((PDB_GSI_V70_BUCKET_COUNT / PDB_GSI_V70_WORD_SIZE) + 1)
#define PDB_GSI_V70_BITMAP_SIZE  (PDB_GSI_V70_BITMAP_COUNT * sizeof(U32))

typedef struct PDB_GsiContext
{
  Arena         *arena;
  U64            word_size;
  U64            symbol_align;
  U64            bucket_count;
  U64            symbol_count;
  CV_SymbolList *bucket_arr;
} PDB_GsiContext;

typedef struct PDB_GsiSortRecord
{
  ISectOff isect_off;
  string name;
  U64 offset;
} PDB_GsiSortRecord;

typedef struct PDB_GsiBuildResult
{
  PDB_GsiHeader      header;
  U64                hash_record_count;
  PDB_GsiHashRecord *hash_record_arr;
  PDB_GsiSortRecord *sort_record_arr;
  U64                bitmap_count;
  U32               *bitmap;
  U64                compressed_bucket_count;
  U32               *compressed_bucket_arr;
  U64                total_hash_size;
  string            symbol_data;
} PDB_GsiBuildResult;

typedef struct PDB_GsiSerializeSymbolsTask
{
  U64                  symbol_align;
  CV_SymbolList       *bucket_arr;
  U64                 *bucket_size_arr;
  U64                 *bucket_off_arr;
  U8                  *buffer;
  PDB_GsiSortRecord  **sort_record_arr_arr;
  PDB_GsiSortRecord   *sort_record_arr;
} PDB_GsiSerializeSymbolsTask;

////////////////////////////////
// PSI

typedef struct PDB_PsiContext
{
  Arena *arena;
  PDB_GsiContext *gsi;
} PDB_PsiContext;

////////////////////////////////
// DBI

#define PDB_MODULE_ALIGN PDB_NATURAL_ALIGN

typedef struct PDB_DbiModule
{
  struct PDB_DbiModule *next;
  MSF_StreamNumber      sn;
  CV_ModIndex           imod;
  PDB_DbiSectionContrib first_sc;
  U64                   sym_data_size;
  U64                   c11_data_size;
  U64                   c13_data_size;
  U64                   globrefs_size; // TODO: what is this for?
  string               obj_path;
  string               lib_path;
  String8List           source_file_list;
} PDB_DbiModule;

typedef struct PDB_DbiModuleList
{
  PDB_DbiModule *first;
  PDB_DbiModule *last;
  U64            count;
} PDB_DbiModuleList;

typedef struct PDB_DbiSectionContribNode
{
  struct PDB_DbiSectionContribNode *next;
  PDB_DbiSectionContrib             data;
} PDB_DbiSectionContribNode;

typedef struct PDB_DbiSectionContribList
{
  PDB_DbiSectionContribNode *first;
  PDB_DbiSectionContribNode *last;
  U64                        count;
} PDB_DbiSectionContribList;

typedef struct PDB_DbiSectionNode
{
  struct PDB_DbiSectionNode *next;
  COFF_SectionHeader         data;
} PDB_DbiSectionNode;

typedef struct PDB_DbiSectionList
{
  U64                 count;
  PDB_DbiSectionNode *first;
  PDB_DbiSectionNode *last;
} PDB_DbiSectionList;

typedef struct PDB_DbiContext
{
  Arena                *arena;
  U32                   age;
  COFF_MachineType      machine;
  MSF_StreamNumber      globals_sn;
  MSF_StreamNumber      publics_sn;
  MSF_StreamNumber      symbols_sn;
  PDB_DbiModuleList     module_list;
  PDB_DbiSectionContribList sec_contrib_list;
  PDB_DbiSectionList    section_list;
  PDB_StringTable       ec_names;
  MSF_StreamNumber      dbg_streams[PDB_DbiStream_COUNT];
} PDB_DbiContext;

////////////////////////////////
// PDB

typedef struct PDB_Context
{
  Arena           *arena;
  MSF_Context     *msf;
  PDB_InfoContext *info;
  PDB_DbiContext  *dbi;
  PDB_GsiContext  *gsi;
  PDB_PsiContext  *psi;
  PDB_TypeServer  *type_servers[CV_TypeIndexSource_COUNT];
} PDB_Context;

////////////////////////////////

typedef struct
{
  PDB_GsiContext *gsi;
  Rng1U64        *ranges;
  CV_SymbolNode **symbols;
  U32            *hashes;
} GSI_SymbolHasherTask;

typedef struct
{
  CV_StringHashTable   string_ht;
  PDB_DbiModule      **mod_arr;
  U16                 *imod_arr;
  U16                 *source_file_name_count_arr;
  U32                **source_file_name_offset_arr;
} PDB_DbiBuildFileInfoTask;

////////////////////////////////
// PDB


////////////////////////////////
// Info


////////////////////////////////
// GSI


////////////////////////////////
// PSI


// TODO:
//internal CV_Symbol psi_neareset_symbol(PDB_PsiContext *psi, U16 isect, U32 off);
//internal void      psi_push_thunk_map(PDB_PsiContext *psi, U32 *thunk_map, U32 thunk_count, U32 thunk_size, PDB_SO *sect_map, U32 sect_count, ISectOff thunk_table);

////////////////////////////////
// DBI


////////////////////////////////
// Hash Table


////////////////////////////////



////////////////////////////////
// String Table


////////////////////////////////
// Type Server

//internal CV_LeafNode *     pdb_type_server_leaf_from_string(PDB_TypeServer *ts, string string);

////////////////////////////////
// Enum -> String




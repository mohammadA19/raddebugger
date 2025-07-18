// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

//- Symbol and Leaf Headers

typedef struct CV_LeafHeader
{
  CV_LeafSize size;
  CV_LeafKind kind;
} CV_LeafHeader;

typedef struct CV_SymbolHeader
{
  CV_SymSize size;
  CV_SymKind kind;
} CV_SymbolHeader;

////////////////////////////////
// Type Index Helpers

//- $$Symbols

typedef struct CV_Symbol
{
  CV_SymKind kind;
  uint64        offset;
  String8    data;
} CV_Symbol;

typedef struct CV_SymbolNode
{
  struct CV_SymbolNode *next;
  struct CV_SymbolNode *prev;
  CV_Symbol             data;
} CV_SymbolNode;

typedef struct CV_SymbolPtrNode
{
  struct CV_SymbolPtrNode *next;
  CV_Symbol               *data;
} CV_SymbolPtrNode;

typedef struct CV_SymbolList
{
  uint64            count;
  CV_Signature   signature;
  CV_SymbolNode *first;
  CV_SymbolNode *last;
} CV_SymbolList;

typedef struct CV_SymbolListArray
{
  uint64            count;
  CV_SymbolList *v;
} CV_SymbolListArray;

typedef struct CV_SymbolPtrArray
{
  uint64             count;
  CV_SymbolNode **v;
} CV_SymbolPtrArray;

typedef struct CV_Scope
{
  struct CV_ScopeList *children;
  struct CV_Scope     *next;
  struct CV_Scope     *prev;
  CV_Symbol            symbol;
} CV_Scope;

typedef struct CV_ScopeList
{
  CV_Scope *first;
  CV_Scope *last;
} CV_ScopeList;

typedef struct CV_ScopeFrame
{
  struct        CV_ScopeFrame *next;
  CV_ScopeList *list;
  CV_Scope     *curr;
  uint64           symbol_off;
  uint32          *parent_off_ptr;
  uint32          *end_off_ptr;
} CV_ScopeFrame;

//- $$FileChksms

typedef struct CV_Checksum
{
  CV_C13Checksum *header;
  String8 value;
} CV_Checksum;

typedef struct CV_ChecksumNode
{
  struct CV_ChecksumNode *next;
  CV_Checksum data;
} CV_ChecksumNode;

typedef struct CV_ChecksumList
{
  uint64 count;
  CV_ChecksumNode *first;
  CV_ChecksumNode *last;
} CV_ChecksumList;

//- $$Lines

typedef struct CV_LineArray
{
  uint32  file_off;
  uint64  line_count;
  uint64  col_count;
  uint64 *voffs;     // [line_count + 1]
  uint32 *line_nums; // [line_count]
  uint16 *col_nums;  // [line_count * 2]
} CV_LineArray;

typedef struct CV_File
{
  uint32          file_off;
  CV_LineArray lines;
} CV_File;

typedef struct CV_C13LinesHeader
{
  uint64 sec_idx;
  uint64 sec_off_lo;
  uint64 sec_off_hi;
  uint64 file_off;
  uint64 line_count;
  uint64 col_count;
  uint64 line_array_off;
  uint64 col_array_off;
} CV_C13LinesHeader;

typedef struct CV_C13LinesHeaderNode
{
  struct CV_C13LinesHeaderNode *next;
  CV_C13LinesHeader             v;
} CV_C13LinesHeaderNode;

typedef struct CV_C13LinesHeaderList
{
  CV_C13LinesHeaderNode *first;
  CV_C13LinesHeaderNode *last;
  uint64                    count;
} CV_C13LinesHeaderList;

////////////////////////////////

typedef struct CV_TypeServerInfo
{
  String8 name;
  Guid    sig;
  uint32     age;
} CV_TypeServerInfo;

typedef struct CV_TypeServerInfoNode
{
  struct CV_TypeServerInfoNode *next;
  CV_TypeServerInfo             data;
} CV_TypeServerInfoNode;

typedef struct CV_TypeServerInfoList
{
  CV_TypeServerInfoNode *first;
  CV_TypeServerInfoNode *last;
  uint64                    count;
} CV_TypeServerInfoList;

typedef struct CV_PrecompInfo
{
  CV_TypeIndex start_index;
  uint32          sig;
  uint32          leaf_count;
  String8      obj_name;
} CV_PrecompInfo;

typedef struct CV_ObjInfo
{
  uint32     sig;
  String8 name;
} CV_ObjInfo;

////////////////////////////////
// Accels

typedef struct CV_Line
{
  uint64 voff;
  uint32 file_off;
  uint32 line_num;
  uint16 col_num;
} CV_Line;

typedef struct CV_LinesAccel
{
  uint64      map_count;
  CV_Line *map;
} CV_LinesAccel;

typedef struct CV_InlineeLinesAccel
{
  uint64                        bucket_count;
  uint64                        bucket_max;
  CV_C13InlineeLinesParsed **buckets;
} CV_InlineeLinesAccel;

typedef struct CV_InlineBinaryAnnotsParsed
{
  uint64           lines_count;
  CV_LineArray *lines;
  Rng1uint64List   code_ranges;
} CV_InlineBinaryAnnotsParsed;

typedef struct CV_C13InlineeLinesParsedList
{
  CV_C13InlineeLinesParsedNode *first;
  CV_C13InlineeLinesParsedNode *last;
  uint64                           count;
} CV_C13InlineeLinesParsedList;

////////////////////////////////

enum CV_C13SubSectionIdxKind : uint32
{
  CV_C13SubSectionIdxKind_NULL,
#define X(N,c) CV_C13SubSectionIdxKind_##N,
  CV_C13SubSectionKindXList(X)
#undef X
  CV_C13SubSectionIdxKind_COUNT
};

typedef struct CV_C13SubSectionList
{
  CV_C13SubSectionNode *first;
  CV_C13SubSectionNode *last;
  uint64                   count;
} CV_C13SubSectionList;

////////////////////////////////

typedef struct CV_DebugS
{
  String8List data_list[CV_C13SubSectionIdxKind_COUNT];
} CV_DebugS;

typedef struct CV_DebugT
{
  uint64  size;
  uint64  count;
  uint8 **v;
} CV_DebugT;

////////////////////////////////
//~ Leaf Helpers

typedef struct CV_Leaf
{
  CV_LeafKind kind;
  String8     data;
} CV_Leaf;

typedef struct CV_LeafNode
{
  struct CV_LeafNode *next;
  CV_Leaf             data;
} CV_LeafNode;

typedef struct CV_LeafList
{
  uint64          count;
  CV_LeafNode *first;
  CV_LeafNode *last;
} CV_LeafList;

////////////////////////////////
//~ String Hash Table

typedef struct CV_StringTableRange
{
  struct CV_StringTableRange *next;
  Rng1uint64                     range;
  uint64                         debug_s_idx;
} CV_StringTableRange;

typedef struct CV_StringBucket
{
  String8 string;
  union {
    struct {
      uint32 idx0;
      uint32 idx1;
    };
    uint64 offset;
  } u;
} CV_StringBucket;

typedef struct CV_StringHashTable
{
  uint64               total_string_size;
  uint64               total_insert_count;
  uint64               bucket_cap;
  CV_StringBucket **buckets;
} CV_StringHashTable;

typedef struct CV_StringHashTableResult
{
  uint64               string_count;
  CV_StringBucket **buckets;
} CV_StringHashTableResult;

////////////////////////////////
//~ Task Contexts

typedef struct
{
  uint64              cap;
  union {
    CV_SymbolNode ***buckets;
    CV_SymbolNode  **deref_buckets;
  } u;
  Rng1uint64         *ranges;
  CV_SymbolNode  **symbols;
} CV_SymbolDeduperTask;

typedef struct
{
  CV_SymbolList  *list_arr;
  Rng1uint64        *list_range_arr;
  uint64            *symbol_base_arr;
  CV_SymbolNode **symbol_arr;
} CV_SymbolListSyncer;

typedef struct
{
  CV_DebugS            *arr;
  CV_StringTableRange **range_lists;
  uint64                  *string_counts;
  uint64                   bucket_cap;
  CV_StringBucket     **buckets;
  uint64                   total_string_size;
  uint64                   total_insert_count;
} CV_DedupStringTablesTask;

typedef struct
{
  uint8               *buffer;
  Rng1uint64          *ranges;
  CV_StringBucket **buckets;
} CV_PackStringHashTableTask;

typedef struct
{
  CV_DebugT    debug_t;
  Rng1uint64     *ranges;
  String8List *lists;
  String8Node *nodes;
} CV_Str8ListFromDebugT;

////////////////////////////////


////////////////////////////////
//~ Leaf Helpers


////////////////////////////////
//~ Symbol Helpers


internal String8       cv_make_comp3(Arena *arena,
                                     CV_Compile3Flags flags, CV_Language lang, CV_Arch arch, 
                                     uint16 ver_fe_major, uint16 ver_fe_minor, uint16 ver_fe_build, uint16 ver_feqfe,
                                     uint16 ver_major, uint16 ver_minor, uint16 ver_build, uint16 ver_qfe,
                                     String8 version_string);

////////////////////////////////
// .debug$S Helpers



////////////////////////////////
//~ .debug$T helpers



////////////////////////////////
//~ Sub Section helpers

// $$Symbols

// $$FileChksms
#define CV_MAP_STRING_TO_OFFSET_FUNC(name) uint64 name(void *ud, String8 string)
typedef CV_MAP_STRING_TO_OFFSET_FUNC(CV_MapStringToOffsetFunc);


// $$Lines

// $$InlineeLines

// $$FrameData

////////////////////////////////
// $$Lines Accel


////////////////////////////////
// $$InlineeLines Accel


////////////////////////////////
// String Hash Table


////////////////////////////////




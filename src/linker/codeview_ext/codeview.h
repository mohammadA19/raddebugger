// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

//- Symbol and Leaf Headers

struct CV_LeafHeader
{
  CV_LeafSize size;
  CV_LeafKind kind;
}

struct CV_SymbolHeader
{
  CV_SymSize size;
  CV_SymKind kind;
}

////////////////////////////////
// Type Index Helpers

//- $$Symbols

struct CV_Symbol
{
  CV_SymKind kind;
  uint64        offset;
  StringView    data;
}

struct CV_SymbolNode
{
  struct CV_SymbolNode* next;
  struct CV_SymbolNode* prev;
  CV_Symbol             data;
}

struct CV_SymbolPtrNode
{
  struct CV_SymbolPtrNode* next;
  CV_Symbol*               data;
}

struct CV_SymbolList
{
  uint64            count;
  CV_Signature   signature;
  CV_SymbolNode* first;
  CV_SymbolNode* last;
}

struct CV_SymbolListArray
{
  uint64            count;
  CV_SymbolList* v;
}

struct CV_SymbolPtrArray
{
  uint64             count;
  CV_SymbolNode** v;
}

struct CV_Scope
{
  struct CV_ScopeList* children;
  struct CV_Scope*     next;
  struct CV_Scope*     prev;
  CV_Symbol            symbol;
}

struct CV_ScopeList
{
  CV_Scope* first;
  CV_Scope* last;
}

struct CV_ScopeFrame
{
  struct        CV_ScopeFrame* next;
  CV_ScopeList* list;
  CV_Scope*     curr;
  uint64           symbol_off;
  uint32*          parent_off_ptr;
  uint32*          end_off_ptr;
}

//- $$FileChksms

struct CV_Checksum
{
  CV_C13Checksum* header;
  StringView value;
}

struct CV_ChecksumNode
{
  struct CV_ChecksumNode* next;
  CV_Checksum data;
}

struct CV_ChecksumList
{
  uint64 count;
  CV_ChecksumNode* first;
  CV_ChecksumNode* last;
}

//- $$Lines

struct CV_LineArray
{
  uint32  file_off;
  uint64  line_count;
  uint64  col_count;
  uint64* voffs;     // [line_count + 1]
  uint32* line_nums; // [line_count]
  uint16* col_nums;  // [line_count * 2]
}

struct CV_File
{
  uint32          file_off;
  CV_LineArray lines;
}

struct CV_C13LinesHeader
{
  uint64 sec_idx;
  uint64 sec_off_lo;
  uint64 sec_off_hi;
  uint64 file_off;
  uint64 line_count;
  uint64 col_count;
  uint64 line_array_off;
  uint64 col_array_off;
}

struct CV_C13LinesHeaderNode
{
  struct CV_C13LinesHeaderNode* next;
  CV_C13LinesHeader             v;
}

struct CV_C13LinesHeaderList
{
  CV_C13LinesHeaderNode* first;
  CV_C13LinesHeaderNode* last;
  uint64                    count;
}

////////////////////////////////

struct CV_TypeServerInfo
{
  StringView name;
  Guid    sig;
  uint32     age;
}

struct CV_TypeServerInfoNode
{
  struct CV_TypeServerInfoNode* next;
  CV_TypeServerInfo             data;
}

struct CV_TypeServerInfoList
{
  CV_TypeServerInfoNode* first;
  CV_TypeServerInfoNode* last;
  uint64                    count;
}

struct CV_PrecompInfo
{
  CV_TypeIndex start_index;
  uint32          sig;
  uint32          leaf_count;
  StringView      obj_name;
}

struct CV_ObjInfo
{
  uint32     sig;
  StringView name;
}

////////////////////////////////
// Accels

struct CV_Line
{
  uint64 voff;
  uint32 file_off;
  uint32 line_num;
  uint16 col_num;
}

struct CV_LinesAccel
{
  uint64      map_count;
  CV_Line* map;
}

struct CV_InlineeLinesAccel
{
  uint64                        bucket_count;
  uint64                        bucket_max;
  CV_C13InlineeLinesParsed** buckets;
}

struct CV_InlineBinaryAnnotsParsed
{
  uint64           lines_count;
  CV_LineArray* lines;
  Rng1U64List   code_ranges;
}

struct CV_C13InlineeLinesParsedList
{
  CV_C13InlineeLinesParsedNode* first;
  CV_C13InlineeLinesParsedNode* last;
  uint64                           count;
}

////////////////////////////////

enum CV_C13SubSectionIdxKind : uint32
{
  CV_C13SubSectionIdxKind_NULL,
#define X(N,c) CV_C13SubSectionIdxKind_##N,
  CV_C13SubSectionKindXList(X)
#undef X
  CV_C13SubSectionIdxKind_COUNT
}

struct CV_C13SubSectionList
{
  CV_C13SubSectionNode* first;
  CV_C13SubSectionNode* last;
  uint64                   count;
}

////////////////////////////////

struct CV_DebugS
{
  String8List data_list[CV_C13SubSectionIdxKind_COUNT];
}

struct CV_DebugT
{
  uint64  size;
  uint64  count;
  uint8** v;
}

////////////////////////////////
//~ Leaf Helpers

struct CV_Leaf
{
  CV_LeafKind kind;
  StringView     data;
}

struct CV_LeafNode
{
  struct CV_LeafNode* next;
  CV_Leaf             data;
}

struct CV_LeafList
{
  uint64          count;
  CV_LeafNode* first;
  CV_LeafNode* last;
}

////////////////////////////////
//~ String Hash Table

struct CV_StringTableRange
{
  struct CV_StringTableRange* next;
  Rng1U64                     range;
  uint64                         debug_s_idx;
}

struct CV_StringBucket
{
  StringView string;
  [Union]
  struct {
    struct {
      uint32 idx0;
      uint32 idx1;
    };
    uint64 offset;
  } u;
}

struct CV_StringHashTable
{
  uint64               total_string_size;
  uint64               total_insert_count;
  uint64               bucket_cap;
  CV_StringBucket** buckets;
}

struct CV_StringHashTableResult
{
  uint64               string_count;
  CV_StringBucket** buckets;
}

////////////////////////////////
//~ Task Contexts

struct
{
  uint64              cap;
  [Union]
  struct {
    CV_SymbolNode*** buckets;
    CV_SymbolNode**  deref_buckets;
  } u;
  Rng1U64*         ranges;
  CV_SymbolNode**  symbols;
}

struct
{
  CV_SymbolList*  list_arr;
  Rng1U64*        list_range_arr;
  uint64*            symbol_base_arr;
  CV_SymbolNode** symbol_arr;
}

struct
{
  CV_DebugS*            arr;
  CV_StringTableRange** range_lists;
  uint64*                  string_counts;
  uint64                   bucket_cap;
  CV_StringBucket**     buckets;
  uint64                   total_string_size;
  uint64                   total_insert_count;
}

struct
{
  uint8*               buffer;
  Rng1U64*          ranges;
  CV_StringBucket** buckets;
}

struct
{
  CV_DebugT    debug_t;
  Rng1U64*     ranges;
  String8List* lists;
  String8Node* nodes;
}

////////////////////////////////

CV_ObjInfo        cv_obj_info_from_symbol(CV_Symbol symbol);
CV_TypeServerInfo cv_type_server_info_from_leaf(CV_Leaf leaf);
CV_PrecompInfo    cv_precomp_info_from_leaf(CV_Leaf leaf);

////////////////////////////////
//~ Leaf Helpers

uint64     cv_compute_leaf_record_size(StringView data, uint64 align);
uint64     cv_serialize_leaf_to_buffer(uint8* buffer, uint64 buffer_cursor, uint64 buffer_size, CV_LeafKind kind, StringView data, uint64 align);
StringView cv_serialize_raw_leaf(Arena* arena, CV_LeafKind kind, StringView data, uint64 align);
StringView cv_serialize_leaf(Arena* arena, CV_Leaf* leaf, uint64 align);
CV_Leaf cv_make_leaf(Arena* arena, CV_LeafKind kind, StringView data);
uint64     cv_deserial_leaf(StringView raw_data, uint64 off, uint64 align, CV_Leaf* leaf_out);
CV_Leaf cv_leaf_from_string(StringView raw_data);

////////////////////////////////
//~ Symbol Helpers

uint64     cv_compute_symbol_record_size(CV_Symbol* symbol, uint64 align);
uint64     cv_serialize_symbol_to_buffer(uint8* buffer, uint64 buffer_cursor, uint64 buffer_size, CV_Symbol* symbol, uint64 align);
StringView cv_serialize_symbol(Arena* arena, CV_Symbol* symbol, uint64 align);

StringView       cv_make_symbol(Arena* arena, CV_SymKind kind, StringView data);
StringView       cv_make_obj_name(Arena* arena, StringView obj_path, uint32 sig);
StringView       cv_make_comp3(Arena* arena,
                                     CV_Compile3Flags flags, CV_Language lang, CV_Arch arch, 
                                     uint16 ver_fe_major, uint16 ver_fe_minor, uint16 ver_fe_build, uint16 ver_feqfe,
                                     uint16 ver_major, uint16 ver_minor, uint16 ver_build, uint16 ver_qfe,
                                     StringView version_string);
StringView       cv_make_envblock(Arena* arena, String8List string_list);
CV_Symbol     cv_make_proc_ref(Arena* arena, CV_ModIndex imod, uint32 stream_offset, StringView name, B32 is_local);
CV_Symbol     cv_make_pub32(Arena* arena, CV_Pub32Flags flags, uint32 off, uint16 isect, StringView name);
CV_SymbolList cv_make_proc_refs(Arena* arena, CV_ModIndex imod, CV_SymbolList symbol_list);

////////////////////////////////
// .debug$S Helpers

CV_DebugS    cv_parse_debug_s_c13(Arena* arena, StringView raw_debug_s);
CV_DebugS    cv_parse_debug_s_c13_list(Arena* arena, String8List raw_debug_s);
CV_DebugS    cv_parse_debug_s(Arena* arena, StringView raw_debug_s);
void         cv_debug_s_concat_in_place(CV_DebugS* dst, CV_DebugS* src);
String8List  cv_data_c13_from_debug_s(Arena* arena, CV_DebugS* debug_s, B32 write_sig);

CV_C13SubSectionIdxKind cv_c13_sub_section_idx_from_kind(CV_C13SubSectionKind kind);
String8List *           cv_sub_section_ptr_from_debug_s(CV_DebugS* debug_s, CV_C13SubSectionKind kind);
String8List             cv_sub_section_from_debug_s(CV_DebugS debug_s, CV_C13SubSectionKind kind);
StringView                 cv_string_table_from_debug_s(CV_DebugS debug_s);
StringView                 cv_file_chksms_from_debug_s(CV_DebugS debug_s);

////////////////////////////////
//~ .debug$T helpers

CV_DebugT       cv_debug_t_from_data_arr(Arena* arena, String8Array data_arr, uint64 align);
CV_DebugT       cv_debug_t_from_data(Arena* arena, StringView data, uint64 align);
CV_Leaf         cv_debug_t_get_leaf(CV_DebugT debug_t, uint64 leaf_idx);
StringView         cv_debug_t_get_raw_leaf(CV_DebugT debug_t, uint64 leaf_idx);
CV_LeafHeader * cv_debug_t_get_leaf_header(CV_DebugT debug_t, uint64 leaf_idx);
B32             cv_debug_t_is_pch(CV_DebugT debug_t);
B32             cv_debug_t_is_type_server(CV_DebugT debug_t);
uint64             cv_debug_t_array_count_leaves(uint64 count, CV_DebugT* arr);

String8List cv_str8_list_from_debug_t_parallel(TP_Context* tp, Arena* arena, CV_DebugT types);

////////////////////////////////
//~ Sub Section helpers

// $$Symbols
void              cv_parse_symbol_sub_section(Arena* arena, CV_SymbolList* list, uint64 offset_base, StringView data, uint64 align);
void              cv_symbol_list_push_node(CV_SymbolList* list, CV_SymbolNode* node);
CV_SymbolNode *   cv_symbol_list_push(Arena* arena, CV_SymbolList* list);
CV_SymbolNode *   cv_symbol_list_push_data(Arena* arena, CV_SymbolList* list, CV_SymKind kind, StringView data);
CV_SymbolNode *   cv_symbol_list_push_many(Arena* arena, CV_SymbolList* list, uint64 count);
void              cv_symbol_list_remove_node(CV_SymbolList* list, CV_SymbolNode* node);
void              cv_symbol_list_concat_in_place(CV_SymbolList* list, CV_SymbolList* to_concat);
void              cv_symbol_list_concat_in_place_arr(CV_SymbolList* list, uint64 count, CV_SymbolList* to_concat);
uint64               cv_symbol_list_arr_get_count(uint64 count, CV_SymbolList* list_arr);
String8List       cv_data_from_symbol_list(Arena* arena, CV_SymbolList symbol_list, uint64 align);
CV_SymbolList     cv_global_scope_symbols_from_list(Arena* arena, CV_SymbolList list);
CV_SymbolPtrArray cv_symbol_ptr_array_from_list(Arena* arena, TP_Context* tp, uint64 count, CV_SymbolList* symbol_list_arr);

// $$FileChksms
#define CV_MAP_STRING_TO_OFFSET_FUNC(name) uint64 name(void* ud, StringView string)
typedef CV_MAP_STRING_TO_OFFSET_FUNC(CV_MapStringToOffsetFunc);

void        cv_c13_patch_string_offsets_in_checksum_list(CV_ChecksumList checksum_list, StringView string_data, uint64 string_data_base_offset, CV_StringHashTable string_ht);
String8List cv_c13_collect_source_file_names(Arena* arena, CV_ChecksumList checksum_list, StringView string_data);

// $$Lines
CV_C13LinesHeaderList cv_c13_lines_from_sub_sections(Arena* arena, StringView c13_data, Rng1U64 ss_range);
CV_LineArray     cv_c13_line_array_from_data(Arena* arena, StringView c13_data, uint64 sec_base, CV_C13LinesHeader parsed_lines);

// $$InlineeLines
CV_C13InlineeLinesParsedList cv_c13_inlinee_lines_from_sub_sections(Arena* arena, String8List raw_inlinee_lines);
CV_InlineBinaryAnnotsParsed  cv_c13_parse_inline_binary_annots(Arena* arena, uint64 parent_voff, CV_C13InlineeLinesParsed* inlinee_parsed, StringView binary_annots);

// $$FrameData
void cv_c13_patch_checksum_offsets_in_frame_data_list(String8List frame_data, uint32 checksum_rebase);

////////////////////////////////
// $$Lines Accel

void            cv_make_c13_files(Arena* arena, StringView c13_data, CV_C13SubSectionList lines, uint64* file_count_out, CV_C13File** files_out);
CV_LinesAccel * cv_make_lines_accel(Arena* arena, uint64 lines_count, CV_LineArray* lines);
CV_Line *       cv_line_from_voff(CV_LinesAccel* accel, uint64 voff, uint64* out_line_count);

////////////////////////////////
// $$InlineeLines Accel

uint64                        cv_c13_inlinee_lines_accel_hash(void* buffer, uint64 size);
B32                        cv_c13_inlinee_lines_accel_push(CV_InlineeLinesAccel* accel, CV_C13InlineeLinesParsed* parsed);
CV_C13InlineeLinesParsed * cv_c13_inlinee_lines_accel_find(CV_InlineeLinesAccel* accel, CV_ItemId inlinee);
CV_InlineeLinesAccel *     cv_c13_make_inlinee_lines_accel(Arena* arena, CV_C13InlineeLinesParsedList sub_sects);

////////////////////////////////
// String Hash Table

uint64                      cv_string_hash_table_hash(StringView string);
CV_StringHashTable       cv_dedup_string_tables(TP_Arena* arena, TP_Context* tp, uint64 count, CV_DebugS* arr);
CV_StringHashTableResult cv_serialize_string_hash_table(Arena* arena, TP_Context* tp, CV_StringHashTable string_ht);
StringView                  cv_pack_string_hash_table(Arena* arena, TP_Context* tp, CV_StringHashTable string_ht);

////////////////////////////////

Rng1U64List cv_make_defined_range_list_from_gaps(Arena* arena, Rng1U64 defrange, CV_LvarAddrGap* gaps, uint64 gap_count);



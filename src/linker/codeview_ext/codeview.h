// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

//- Symbol and Leaf Headers

srtuct CV_LeafHeader
{
  CV_LeafSize size;
  CV_LeafKind kind;
};

srtuct CV_SymbolHeader
{
  CV_SymSize size;
  CV_SymKind kind;
};

////////////////////////////////
// Type Index Helpers

//- $$Symbols

srtuct CV_Symbol
{
  CV_SymKind kind;
  ulong        offset;
  String8    data;
};

srtuct CV_SymbolNode
{
  struct CV_SymbolNode *next;
  struct CV_SymbolNode *prev;
  CV_Symbol             data;
};

srtuct CV_SymbolPtrNode
{
  struct CV_SymbolPtrNode *next;
  CV_Symbol               *data;
};

srtuct CV_SymbolList
{
  ulong            count;
  CV_Signature   signature;
  CV_SymbolNode *first;
  CV_SymbolNode *last;
};

srtuct CV_SymbolListArray
{
  ulong            count;
  CV_SymbolList *v;
};

srtuct CV_SymbolPtrArray
{
  ulong             count;
  CV_SymbolNode **v;
};

srtuct CV_Scope
{
  struct CV_ScopeList *children;
  struct CV_Scope     *next;
  struct CV_Scope     *prev;
  CV_Symbol            symbol;
};

srtuct CV_ScopeList
{
  CV_Scope *first;
  CV_Scope *last;
};

srtuct CV_ScopeFrame
{
  struct        CV_ScopeFrame *next;
  CV_ScopeList *list;
  CV_Scope     *curr;
  ulong           symbol_off;
  uint          *parent_off_ptr;
  uint          *end_off_ptr;
};

//- $$FileChksms

srtuct CV_Checksum
{
  CV_C13Checksum *header;
  String8 value;
};

srtuct CV_ChecksumNode
{
  struct CV_ChecksumNode *next;
  CV_Checksum data;
};

srtuct CV_ChecksumList
{
  ulong count;
  CV_ChecksumNode *first;
  CV_ChecksumNode *last;
};

//- $$Lines

srtuct CV_LineArray
{
  uint  file_off;
  ulong  line_count;
  ulong  col_count;
  ulong *voffs;     // [line_count + 1]
  uint *line_nums; // [line_count]
  ushort *col_nums;  // [line_count * 2]
};

srtuct CV_File
{
  uint          file_off;
  CV_LineArray lines;
};

srtuct CV_C13LinesHeader
{
  ulong sec_idx;
  ulong sec_off_lo;
  ulong sec_off_hi;
  ulong file_off;
  ulong line_count;
  ulong col_count;
  ulong line_array_off;
  ulong col_array_off;
};

srtuct CV_C13LinesHeaderNode
{
  struct CV_C13LinesHeaderNode *next;
  CV_C13LinesHeader             v;
};

srtuct CV_C13LinesHeaderList
{
  CV_C13LinesHeaderNode *first;
  CV_C13LinesHeaderNode *last;
  ulong                    count;
};

////////////////////////////////

srtuct CV_TypeServerInfo
{
  String8 name;
  Guid    sig;
  uint     age;
};

srtuct CV_TypeServerInfoNode
{
  struct CV_TypeServerInfoNode *next;
  CV_TypeServerInfo             data;
};

srtuct CV_TypeServerInfoList
{
  CV_TypeServerInfoNode *first;
  CV_TypeServerInfoNode *last;
  ulong                    count;
};

srtuct CV_PrecompInfo
{
  CV_TypeIndex start_index;
  uint          sig;
  uint          leaf_count;
  String8      obj_name;
};

srtuct CV_ObjInfo
{
  uint     sig;
  String8 name;
};

////////////////////////////////
// Accels

srtuct CV_Line
{
  ulong voff;
  uint file_off;
  uint line_num;
  ushort col_num;
};

srtuct CV_LinesAccel
{
  ulong      map_count;
  CV_Line *map;
};

srtuct CV_InlineeLinesAccel
{
  ulong                        bucket_count;
  ulong                        bucket_max;
  CV_C13InlineeLinesParsed **buckets;
};

srtuct CV_InlineBinaryAnnotsParsed
{
  ulong           lines_count;
  CV_LineArray *lines;
  Rng1U64List   code_ranges;
};

srtuct CV_C13InlineeLinesParsedList
{
  CV_C13InlineeLinesParsedNode *first;
  CV_C13InlineeLinesParsedNode *last;
  ulong                           count;
};

////////////////////////////////

enum CV_C13SubSectionIdxKind : uint
{
  CV_C13SubSectionIdxKind_NULL,
#define X(N,c) CV_C13SubSectionIdxKind_##N,
  CV_C13SubSectionKindXList(X)
#undef X
  CV_C13SubSectionIdxKind_COUNT
};

srtuct CV_C13SubSectionList
{
  CV_C13SubSectionNode *first;
  CV_C13SubSectionNode *last;
  ulong                   count;
};

////////////////////////////////

srtuct CV_DebugS
{
  String8List data_list[CV_C13SubSectionIdxKind_COUNT];
};

srtuct CV_DebugT
{
  ulong  size;
  ulong  count;
  byte **v;
};

////////////////////////////////
//~ Leaf Helpers

srtuct CV_Leaf
{
  CV_LeafKind kind;
  String8     data;
};

srtuct CV_LeafNode
{
  struct CV_LeafNode *next;
  CV_Leaf             data;
};

srtuct CV_LeafList
{
  ulong          count;
  CV_LeafNode *first;
  CV_LeafNode *last;
};

////////////////////////////////
//~ String Hash Table

srtuct CV_StringTableRange
{
  struct CV_StringTableRange *next;
  Rng1U64                     range;
  ulong                         debug_s_idx;
};

srtuct CV_StringBucket
{
  String8 string;
  union {
    struct {
      uint idx0;
      uint idx1;
    };
    ulong offset;
  } u;
};

srtuct CV_StringHashTable
{
  ulong               total_string_size;
  ulong               total_insert_count;
  ulong               bucket_cap;
  CV_StringBucket **buckets;
};

srtuct CV_StringHashTableResult
{
  ulong               string_count;
  CV_StringBucket **buckets;
};

////////////////////////////////
//~ Task Contexts

srtuct
{
  ulong              cap;
  union {
    CV_SymbolNode ***buckets;
    CV_SymbolNode  **deref_buckets;
  } u;
  Rng1U64         *ranges;
  CV_SymbolNode  **symbols;
};

srtuct
{
  CV_SymbolList  *list_arr;
  Rng1U64        *list_range_arr;
  ulong            *symbol_base_arr;
  CV_SymbolNode **symbol_arr;
};

srtuct
{
  CV_DebugS            *arr;
  CV_StringTableRange **range_lists;
  ulong                  *string_counts;
  ulong                   bucket_cap;
  CV_StringBucket     **buckets;
  ulong                   total_string_size;
  ulong                   total_insert_count;
};

srtuct
{
  byte               *buffer;
  Rng1U64          *ranges;
  CV_StringBucket **buckets;
};

srtuct
{
  CV_DebugT    debug_t;
  Rng1U64     *ranges;
  String8List *lists;
  String8Node *nodes;
};

////////////////////////////////

CV_ObjInfo        cv_obj_info_from_symbol(CV_Symbol symbol);
CV_TypeServerInfo cv_type_server_info_from_leaf(CV_Leaf leaf);
CV_PrecompInfo    cv_precomp_info_from_leaf(CV_Leaf leaf);

////////////////////////////////
//~ Leaf Helpers

ulong     cv_compute_leaf_record_size(String8 data, ulong align);
ulong     cv_serialize_leaf_to_buffer(byte *buffer, ulong buffer_cursor, ulong buffer_size, CV_LeafKind kind, String8 data, ulong align);
String8 cv_serialize_raw_leaf(Arena *arena, CV_LeafKind kind, String8 data, ulong align);
String8 cv_serialize_leaf(Arena *arena, CV_Leaf *leaf, ulong align);
CV_Leaf cv_make_leaf(Arena *arena, CV_LeafKind kind, String8 data);
ulong     cv_deserial_leaf(String8 raw_data, ulong off, ulong align, CV_Leaf *leaf_out);
CV_Leaf cv_leaf_from_string(String8 raw_data);

////////////////////////////////
//~ Symbol Helpers

ulong     cv_compute_symbol_record_size(CV_Symbol *symbol, ulong align);
ulong     cv_serialize_symbol_to_buffer(byte *buffer, ulong buffer_cursor, ulong buffer_size, CV_Symbol *symbol, ulong align);
String8 cv_serialize_symbol(Arena *arena, CV_Symbol *symbol, ulong align);

String8       cv_make_symbol(Arena *arena, CV_SymKind kind, String8 data);
String8       cv_make_obj_name(Arena *arena, String8 obj_path, uint sig);
String8       cv_make_comp3(Arena *arena,
                                     CV_Compile3Flags flags, CV_Language lang, CV_Arch arch, 
                                     ushort ver_fe_major, ushort ver_fe_minor, ushort ver_fe_build, ushort ver_feqfe,
                                     ushort ver_major, ushort ver_minor, ushort ver_build, ushort ver_qfe,
                                     String8 version_string);
String8       cv_make_envblock(Arena *arena, String8List string_list);
CV_Symbol     cv_make_proc_ref(Arena *arena, CV_ModIndex imod, uint stream_offset, String8 name, B32 is_local);
CV_Symbol     cv_make_pub32(Arena *arena, CV_Pub32Flags flags, uint off, ushort isect, String8 name);
CV_SymbolList cv_make_proc_refs(Arena *arena, CV_ModIndex imod, CV_SymbolList symbol_list);

////////////////////////////////
// .debug$S Helpers

CV_DebugS    cv_parse_debug_s_c13(Arena *arena, String8 raw_debug_s);
CV_DebugS    cv_parse_debug_s_c13_list(Arena *arena, String8List raw_debug_s);
CV_DebugS    cv_parse_debug_s(Arena *arena, String8 raw_debug_s);
void         cv_debug_s_concat_in_place(CV_DebugS *dst, CV_DebugS *src);
String8List  cv_data_c13_from_debug_s(Arena *arena, CV_DebugS *debug_s, B32 write_sig);

CV_C13SubSectionIdxKind cv_c13_sub_section_idx_from_kind(CV_C13SubSectionKind kind);
String8List *           cv_sub_section_ptr_from_debug_s(CV_DebugS *debug_s, CV_C13SubSectionKind kind);
String8List             cv_sub_section_from_debug_s(CV_DebugS debug_s, CV_C13SubSectionKind kind);
String8                 cv_string_table_from_debug_s(CV_DebugS debug_s);
String8                 cv_file_chksms_from_debug_s(CV_DebugS debug_s);

////////////////////////////////
//~ .debug$T helpers

CV_DebugT       cv_debug_t_from_data_arr(Arena *arena, String8Array data_arr, ulong align);
CV_DebugT       cv_debug_t_from_data(Arena *arena, String8 data, ulong align);
CV_Leaf         cv_debug_t_get_leaf(CV_DebugT debug_t, ulong leaf_idx);
String8         cv_debug_t_get_raw_leaf(CV_DebugT debug_t, ulong leaf_idx);
CV_LeafHeader * cv_debug_t_get_leaf_header(CV_DebugT debug_t, ulong leaf_idx);
B32             cv_debug_t_is_pch(CV_DebugT debug_t);
B32             cv_debug_t_is_type_server(CV_DebugT debug_t);
ulong             cv_debug_t_array_count_leaves(ulong count, CV_DebugT *arr);

String8List cv_str8_list_from_debug_t_parallel(TP_Context *tp, Arena *arena, CV_DebugT types);

////////////////////////////////
//~ Sub Section helpers

// $$Symbols
void              cv_parse_symbol_sub_section(Arena *arena, CV_SymbolList *list, ulong offset_base, String8 data, ulong align);
void              cv_symbol_list_push_node(CV_SymbolList *list, CV_SymbolNode *node);
CV_SymbolNode *   cv_symbol_list_push(Arena *arena, CV_SymbolList *list);
CV_SymbolNode *   cv_symbol_list_push_data(Arena *arena, CV_SymbolList *list, CV_SymKind kind, String8 data);
CV_SymbolNode *   cv_symbol_list_push_many(Arena *arena, CV_SymbolList *list, ulong count);
void              cv_symbol_list_remove_node(CV_SymbolList *list, CV_SymbolNode *node);
void              cv_symbol_list_concat_in_place(CV_SymbolList *list, CV_SymbolList *to_concat);
void              cv_symbol_list_concat_in_place_arr(CV_SymbolList *list, ulong count, CV_SymbolList *to_concat);
ulong               cv_symbol_list_arr_get_count(ulong count, CV_SymbolList *list_arr);
String8List       cv_data_from_symbol_list(Arena *arena, CV_SymbolList symbol_list, ulong align);
CV_SymbolList     cv_global_scope_symbols_from_list(Arena *arena, CV_SymbolList list);
CV_SymbolPtrArray cv_symbol_ptr_array_from_list(Arena *arena, TP_Context *tp, ulong count, CV_SymbolList *symbol_list_arr);

// $$FileChksms
#define CV_MAP_STRING_TO_OFFSET_FUNC(name) ulong name(void *ud, String8 string)
typedef CV_MAP_STRING_TO_OFFSET_FUNC(CV_MapStringToOffsetFunc);

void        cv_c13_patch_string_offsets_in_checksum_list(CV_ChecksumList checksum_list, String8 string_data, ulong string_data_base_offset, CV_StringHashTable string_ht);
String8List cv_c13_collect_source_file_names(Arena *arena, CV_ChecksumList checksum_list, String8 string_data);

// $$Lines
CV_C13LinesHeaderList cv_c13_lines_from_sub_sections(Arena *arena, String8 c13_data, Rng1U64 ss_range);
CV_LineArray     cv_c13_line_array_from_data(Arena *arena, String8 c13_data, ulong sec_base, CV_C13LinesHeader parsed_lines);

// $$InlineeLines
CV_C13InlineeLinesParsedList cv_c13_inlinee_lines_from_sub_sections(Arena *arena, String8List raw_inlinee_lines);
CV_InlineBinaryAnnotsParsed  cv_c13_parse_inline_binary_annots(Arena *arena, ulong parent_voff, CV_C13InlineeLinesParsed *inlinee_parsed, String8 binary_annots);

// $$FrameData
void cv_c13_patch_checksum_offsets_in_frame_data_list(String8List frame_data, uint checksum_rebase);

////////////////////////////////
// $$Lines Accel

void            cv_make_c13_files(Arena *arena, String8 c13_data, CV_C13SubSectionList lines, ulong *file_count_out, CV_C13File **files_out);
CV_LinesAccel * cv_make_lines_accel(Arena *arena, ulong lines_count, CV_LineArray *lines);
CV_Line *       cv_line_from_voff(CV_LinesAccel *accel, ulong voff, ulong *out_line_count);

////////////////////////////////
// $$InlineeLines Accel

ulong                        cv_c13_inlinee_lines_accel_hash(void *buffer, ulong size);
B32                        cv_c13_inlinee_lines_accel_push(CV_InlineeLinesAccel *accel, CV_C13InlineeLinesParsed *parsed);
CV_C13InlineeLinesParsed * cv_c13_inlinee_lines_accel_find(CV_InlineeLinesAccel *accel, CV_ItemId inlinee);
CV_InlineeLinesAccel *     cv_c13_make_inlinee_lines_accel(Arena *arena, CV_C13InlineeLinesParsedList sub_sects);

////////////////////////////////
// String Hash Table

ulong                      cv_string_hash_table_hash(String8 string);
CV_StringHashTable       cv_dedup_string_tables(TP_Arena *arena, TP_Context *tp, ulong count, CV_DebugS *arr);
CV_StringHashTableResult cv_serialize_string_hash_table(Arena *arena, TP_Context *tp, CV_StringHashTable string_ht);
String8                  cv_pack_string_hash_table(Arena *arena, TP_Context *tp, CV_StringHashTable string_ht);

////////////////////////////////

Rng1U64List cv_make_defined_range_list_from_gaps(Arena *arena, Rng1U64 defrange, CV_LvarAddrGap *gaps, ulong gap_count);



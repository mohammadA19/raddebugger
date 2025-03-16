// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////

struct LNK_PchInfo
{
  CV_TypeIndex ti_lo;
  CV_TypeIndex ti_hi;
  uint64          debug_p_obj_idx;
}

struct LNK_CodeViewSymbolsInput
{
  uint64            obj_idx;
  CV_SymbolList* symbol_list;
  String8        raw_symbols;
}

struct LNK_CodeViewInput
{
  uint64             count;
  uint64             internal_count;
  uint64             external_count;
  uint64             type_server_count;
  String8*        type_server_path_arr; // [type_server_count]
  String8*        type_server_data_arr; // [type_server_count]
  U64List*        ts_to_obj_arr;        // [type_server_count]
  LNK_Obj*        obj_arr;              // [count]
  LNK_PchInfo*    pch_arr;              // [count]
  CV_DebugS*      debug_s_arr;          // [count]
  CV_DebugT*      debug_p_arr;          // [count]
  CV_DebugT*      debug_t_arr;          // [count]
  CV_DebugT*      merged_debug_t_p_arr; // [count]

  uint64                       total_symbol_input_count;
  LNK_CodeViewSymbolsInput* symbol_inputs;  // [total_symbol_input_count]
  CV_SymbolListArray*       parsed_symbols; // [count]

  LNK_Obj*                  internal_obj_arr;         // [internal_count]
  CV_DebugS*                internal_debug_s_arr;     // [internal_count]
  CV_DebugT*                internal_debug_t_arr;     // [internal_count]
  CV_DebugT*                internal_debug_p_arr;     // [internal_count]
  uint64                      internal_total_symbol_input_count;
  LNK_CodeViewSymbolsInput* internal_symbol_inputs;   // [internal_total_symbol_input_count]
  CV_SymbolListArray*       internal_parsed_symbols;  // [internal_count]

  LNK_Obj*                   external_obj_arr;           // [external_count]
  CV_DebugS*                 external_debug_s_arr;       // [external_count]
  CV_DebugT*                 external_debug_t_arr;       // [external_count]
  CV_DebugT*                 external_debug_p_arr;       // [external_count]
  uint64                        external_total_symbol_input_count;
  LNK_CodeViewSymbolsInput*  external_symbol_inputs;     // [exteranl_total_symbol_input_count]
  CV_SymbolListArray*        external_parsed_symbols;    // [external_count]
  Rng1U64**                  external_ti_ranges;         // [type_server_count]
  CV_DebugT**                external_leaves;            // [type_server_count]
  uint64*                       external_obj_to_ts_idx_arr; // [external_count]
  Rng1U64                    external_obj_range;
}

////////////////////////////////

enum
{
  LNK_LeafLocType_Internal,
  LNK_LeafLocType_External,
  LNK_LeafLocType_Count
}

#define LNK_LeafRefFlag_LocIdxExternal (1 << 31)
#define LNK_LeafRefFlag_LeafIdxIPI     (1 << 31)
struct
{
  uint32 enc_loc_idx;
  uint32 enc_leaf_idx;
}

struct LNK_LeafRange
{
  struct LNK_LeafRange* next;
  Rng1U64               range;
  CV_DebugT*            debug_t;
}

struct LNK_LeafRangeList
{
  uint64            count;
  LNK_LeafRange* first;
  LNK_LeafRange* last;
}

struct
{
  LNK_LeafRef  leaf_ref;
  CV_TypeIndex type_index;
}

struct
{
  uint64              count;
  LNK_LeafBucket** v;
}

struct
{
  uint64              cap;
  LNK_LeafBucket** bucket_arr;
}

typedef union
{
  struct {
    U128Array** internal_hashes;
    U128Array** external_hashes;
  };
  U128Array** v[CV_TypeIndexSource_COUNT];
}

////////////////////////////////

struct
{
  LNK_Obj**      obj_arr;
  LNK_ChunkList* sect_list_arr;
  CV_DebugS*     debug_s_arr;
}

struct
{
  LNK_Obj**     obj_arr;
  String8Array* data_arr_arr;
}

struct
{
  LNK_Obj**     obj_arr;
  String8Array* data_arr_arr;
  CV_DebugT*    debug_t_arr;
}

struct
{
  String8Array   data_arr;
  MSF_Parsed**   msf_parse_arr;
}

struct
{
  CV_TypeServerInfo*  ts_info_arr;
  MSF_Parsed**        msf_parse_arr;
  Rng1U64**           external_ti_ranges;
  CV_DebugT**         external_leaves;
  B8*                 is_corrupted;
}

////////////////////////////////

struct
{
  LNK_LeafRangeList*  leaf_ranges_per_task;
  uint64**               count_arr_arr;
}

struct
{
  LNK_CodeViewInput* input;
  LNK_LeafHashes*    hashes;
  Arena**            fixed_arenas;
  CV_DebugT*         debug_t_arr;
}

struct
{
  LNK_CodeViewInput*  input;
  LNK_LeafHashes*     hashes;
  LNK_LeafHashTable*  leaf_ht_arr;
  CV_DebugT*          debug_t_arr;
}

struct
{
  LNK_CodeViewInput*  input;
  LNK_LeafHashes*     hashes;
  LNK_LeafHashTable*  leaf_ht_arr;
  CV_TypeIndexSource  dedup_ti_source;
}

struct
{
  LNK_LeafHashTable*   ht;
  uint64*                 count_arr;
  Rng1U64*             range_arr;
  uint64*                 offset_arr;
  LNK_LeafBucketArray  result;
}

struct
{
  uint64             loc_idx_bit_count_0;
  uint64             loc_idx_bit_count_1;
  uint64             loc_idx_bit_count_2;
  uint64             counts_max;
  uint32**           counts_arr;
  Rng1U64*        ranges;
  LNK_LeafBucket** dst;
  LNK_LeafBucket** src;
  uint64             loc_idx_max;
  uint64             pass_idx;
}

struct
{
  uint32*             counts;
  uint32*             offsets;
  LNK_LeafBucket** dst;
  LNK_LeafBucket** src;
  Rng1U64*         ranges;
}

struct
{
  Rng1U64*            range_arr;
  CV_TypeIndex        min_type_index;
  LNK_LeafBucketArray bucket_arr;
}

struct
{
  LNK_CodeViewInput*  input;
  LNK_LeafBucket**    bucket_arr;
  uint8**                raw_leaf_arr;
  Rng1U64*            range_arr;
}

struct
{
  LNK_CodeViewInput*  input;
  LNK_LeafHashes*     hashes;
  LNK_LeafHashTable*  leaf_ht_arr;
  CV_SymbolList*      symbol_list_arr;
  Arena**             arena_arr;
}

struct
{
  LNK_CodeViewInput* input;
  LNK_LeafHashes*    hashes;
  LNK_LeafHashTable* leaf_ht_arr;
  CV_DebugS*         debug_s_arr;
}

struct
{
  LNK_CodeViewInput*  input;
  LNK_LeafHashes*     hashes;
  LNK_LeafHashTable*  leaf_ht_arr;
  LNK_LeafBucket**    bucket_arr;
  Rng1U64*            range_arr;
  Arena**             fixed_arena_arr;
}

////////////////////////////////

struct
{
  String8List* data_list_arr;
}

struct
{
  String8List* data_list_arr;
  String8List* source_file_names_list_arr;
}

struct
{
  LNK_CodeViewSymbolsInput* inputs;
}

struct
{
  uint64                        total_symbol_input_count;
  LNK_CodeViewSymbolsInput*  symbol_inputs;
  CV_SymbolListArray*        parsed_symbols;
  PDB_DbiModule**            mod_arr;
  String8List*               symbol_data_arr;
  CV_SymbolList*             gsi_list_arr;
}

struct
{
  CV_DebugS*          debug_s_arr;
  MSF_Context*        msf;
  PDB_DbiModule**      dbi_mod_arr;
  String8List*        c13_data_arr;
  String8List*        source_file_names_list_arr;
  uint64                 string_data_base_offset;
  CV_StringHashTable  string_ht;
}

struct
{
  MSF_Context*    msf;
  PDB_DbiModule** mod_arr;
  String8List*    symbol_data_arr;
  String8List*    c11_data_list_arr;
  String8List*    c13_data_list_arr;
  String8List*    globrefs_arr;
}

struct
{
  LNK_Obj*                    obj_arr;
  LNK_Section**               sect_id_map;
  PDB_DbiModule**             mod_arr;
  PDB_DbiSectionContribList*  sc_list;
  String8                     image_data;
}

struct
{
  U32Array*      hash_arr_arr;
  CV_SymbolList* list_arr;
}

struct
{
  uint64*            hash_arr;
  CV_SymbolNode** arr;
  Rng1U64*        range_arr;
}

struct
{
  LNK_Section**                 sect_id_map;
  LNK_SymbolHashTrieChunkList*  chunk_lists;
  CV_SymbolList*                pub_list_arr;

  Rng1U64*           symbol_ranges;
  PDB_GsiContext*    gsi;
  CV_SymbolPtrArray  symbols;
  uint32*               hashes;
}

struct
{
  CV_TypeIndex              ipi_min_type_index;
  CV_DebugT                 ipi_types;
  LNK_CodeViewSymbolsInput* symbol_inputs;
  CV_SymbolListArray*       parsed_symbols;
}

struct
{
  Rng1U64*           range_arr;
  CV_SymbolPtrNode** bucket_arr;
  CV_SymbolPtrNode** out_arr;
  uint64*               out_count_arr;
}

struct
{
  Rng1U64*           range_arr;
  CV_SymbolPtrNode** bucket_arr;
  uint64*               symbol_base_arr;
  CV_SymbolNode**    symbol_arr;
}

struct
{
  CV_DebugT    debug_t;
  Rng1U64*     ranges;
  uint64          hash_length;
  B32          make_map;
  TP_Arena*    map_arena;
  String8List* maps;
}

////////////////////////////////
// RAD Debug Info

struct
{
  String8 name;
  uint64     leaf_idx;
}

struct
{
  CV_DebugT           debug_t;
  Rng1U64*            ranges;
  uint64                 buckets_cap;
  LNK_UDTNameBucket** buckets;
}

struct
{
  CV_DebugT           debug_t;
  CV_TypeIndex        ti_lo;
  Rng1U64*            ranges;
  uint64                 udt_name_buckets_cap;
  LNK_UDTNameBucket** udt_name_buckets;
  CV_TypeIndex*       fwdmap;
}

////////////////////////////////

struct
{
  CV_DebugT*               types;
  uint64                      type_cap;
  uint64                      udt_cap;
  RDIB_TypeRef             variadic_type_ref;
  Rng1U64*                 itype_ranges;
  uint64                      udt_name_bucket_cap;
  LNK_UDTNameBucket**      udt_name_buckets;
  RDIB_Type**              tpi_itype_map;
  RDIB_TypeChunkList*      rdib_types_lists;
  RDIB_TypeChunkList*      rdib_types_struct_lists;
  RDIB_TypeChunkList*      rdib_types_union_lists;
  RDIB_TypeChunkList*      rdib_types_enum_lists;
  RDIB_TypeChunkList*      rdib_types_params_lists;
  RDIB_TypeChunkList*      rdib_types_udt_members_lists;
  RDIB_TypeChunkList*      rdib_types_enum_members_lists;
  RDIB_UDTMemberChunkList* rdib_udt_members_lists;
  RDIB_UDTMemberChunkList* rdib_enum_members_lists;
  Rng1U64*                 ranges;
}

struct
{
  uint64              obj_idx;
  RDIB_SourceFile* src_file;
}

struct
{
  LNK_Obj*               obj_arr;
  CV_DebugS*             debug_s_arr;
  uint64                    total_src_file_count;
  LNK_SourceFileBucket** src_file_buckets;
  uint64                    src_file_buckets_cap;
}

struct
{
  CV_Arch     arch;
  CV_Language language;
  String8     compiler_name;
}

struct
{
  LNK_SectionArray          image_sects;
  LNK_Section**             sect_id_map;
  LNK_Obj*                  obj_arr;
  CV_DebugS*                debug_s_arr;
  CV_DebugT                 ipi;
  LNK_CodeViewSymbolsInput* symbol_inputs;
  CV_SymbolListArray*       parsed_symbols;
  Rng1U64                   ipi_itype_range;
  Rng1U64                   tpi_itype_range;
  RDIB_Type**               tpi_itype_map;
  uint64                       src_file_buckets_cap;
  LNK_SourceFileBucket**    src_file_buckets;
  LNK_UDTNameBucket**       udt_name_buckets;
  uint64                       line_table_cap;
  uint64                       udt_name_buckets_cap;
  uint64                       src_file_chunk_cap;
  uint64                       symbol_chunk_cap;
  uint64                       unit_chunk_cap;
  uint64                       inline_site_cap;
  RDIB_LineTable*           null_line_table;
  HashTable*                extern_symbol_voff_ht;
  LNK_CodeViewCompilerInfo* comp_info_arr;
  CV_InlineeLinesAccel**    inlinee_lines_accel_arr;

  RDIB_InlineSiteChunk** inline_site_chunks;

  // output
  RDIB_UnitChunk*           units;
  RDIB_VariableChunkList*   locals;
  RDIB_ScopeChunkList*      scopes;
  RDIB_VariableChunkList*   extern_gvars;
  RDIB_VariableChunkList*   static_gvars;
  RDIB_VariableChunkList*   extern_tvars;
  RDIB_VariableChunkList*   static_tvars;
  RDIB_ProcedureChunkList*  extern_procs;
  RDIB_ProcedureChunkList*  static_procs;
  RDIB_InlineSiteChunkList* inline_sites;
  RDIB_LineTableChunkList*  line_tables;
}

////////////////////////////////
// CodeView

CV_DebugS *       lnk_parse_debug_s_sections(TP_Context* tp, TP_Arena* arena, uint64 obj_count, LNK_Obj** obj_arr, LNK_ChunkList* sect_list_arr);
CV_DebugT *       lnk_parse_debug_t_sections(TP_Context* tp, TP_Arena* arena, uint64 obj_count, LNK_Obj** obj_arr, LNK_ChunkList* debug_t_list_arr);
CV_SymbolList *   lnk_cv_symbol_list_arr_from_debug_s_arr(TP_Context* tp, TP_Arena* arena, uint64 obj_count, CV_DebugS* debug_s_arr);
LNK_PchInfo *     lnk_setup_pch(Arena* arena, uint64 obj_count, LNK_Obj* obj_arr, CV_DebugT* debug_t_arr, CV_DebugT* debug_p_arr, CV_SymbolListArray* parsed_symbols);

LNK_CodeViewInput lnk_make_code_view_input(TP_Context* tp, TP_Arena* tp_arena, String8List lib_dir_list, LNK_ObjList obj_list);

LNK_LeafRef      lnk_leaf_ref(uint32 idx, uint32 leaf_idx);
LNK_LeafRef      lnk_obj_leaf_ref(uint32 obj_idx, uint32 leaf_idx);
LNK_LeafRef      lnk_ts_leaf_ref(CV_TypeIndexSource ti_source, uint32 ts_idx, uint32 leaf_idx);
int              lnk_leaf_ref_compare(LNK_LeafRef a, LNK_LeafRef b);
LNK_LeafLocType  lnk_loc_type_from_leaf_ref(LNK_LeafRef leaf_ref);
LNK_LeafLocType  lnk_loc_type_from_obj_idx(LNK_CodeViewInput* input, uint64 obj_idx);
uint64              lnk_loc_idx_from_obj_idx(LNK_CodeViewInput* input, uint64 obj_idx);
CV_TypeIndex     lnk_ti_lo_from_loc(LNK_CodeViewInput* input, LNK_LeafLocType loc_type, uint64 loc_idx, CV_TypeIndexSource ti_source);
CV_TypeIndex     lnk_ti_lo_from_leaf_ref(LNK_CodeViewInput* input, LNK_LeafRef leaf_ref);
String8          lnk_data_from_leaf_ref(LNK_CodeViewInput* input, LNK_LeafRef leaf_ref);
CV_Leaf          lnk_cv_leaf_from_leaf_ref(LNK_CodeViewInput* input, LNK_LeafRef leaf_ref);
U128             lnk_hash_from_leaf_ref(LNK_LeafHashes* hashes, LNK_LeafRef leaf_ref);
LNK_LeafRef      lnk_leaf_ref_from_loc_idx_and_ti(LNK_CodeViewInput* input, LNK_LeafLocType loc_type, CV_TypeIndexSource ti_source, uint64 loc_idx, CV_TypeIndex obj_ti);
B32              lnk_match_leaf_ref(LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafRef a, LNK_LeafRef b);
B32              lnk_match_leaf_ref_deep(Arena* arena, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafRef a, LNK_LeafRef b);
U128             lnk_hash_cv_leaf(Arena* arena, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafLocType loc_type, uint32 loc_idx, Rng1U64* ti_ranges, CV_TypeIndex curr_ti, CV_Leaf leaf, CV_TypeIndexInfoList ti_info_list);
void             lnk_hash_cv_leaf_deep(Arena* arena, LNK_CodeViewInput* input, Rng1U64* ti_ranges, CV_DebugT* leaves, LNK_LeafHashes* hashes, LNK_LeafLocType loc_type, uint32 loc_idx, CV_TypeIndexInfoList ti_info_list, String8 data);
LNK_LeafBucket * lnk_leaf_hash_table_insert_or_update(LNK_LeafHashTable* leaf_ht, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, U128 hash, LNK_LeafBucket* new_bucket);
LNK_LeafBucket * lnk_leaf_hash_table_search(LNK_LeafHashTable* ht, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafRef leaf_ref);

void                lnk_cv_debug_t_count_leaves_per_source(TP_Context* tp, uint64 count, CV_DebugT* debug_t_arr, uint64* per_source_count_arr);
void                lnk_hash_debug_t_arr(TP_Context* tp, Arena* arena, uint64 obj_count, CV_DebugT* debug_t_arr, U128Array* hash_arr_arr);
LNK_LeafBucketArray lnk_present_bucket_array_from_leaf_hash_table(TP_Context* tp, Arena* arena, LNK_LeafHashTable* ht);
void                lnk_leaf_bucket_array_sort_radix_subset_parallel(TP_Context* tp, uint64 bucket_count, uint64 loc_idx_max, LNK_LeafBucket** dst, LNK_LeafBucket** src);
void                lnk_leaf_bucket_array_sort_radix_parallel(TP_Context* tp, LNK_LeafBucketArray arr, uint64 obj_count, uint64 type_server_count);
void                lnk_assign_type_indices(TP_Context* tp, LNK_LeafBucketArray bucket_arr, CV_TypeIndex min_type_index);
void                lnk_patch_symbols(TP_Context* tp, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafHashTable* leaf_ht_arr);
void                lnk_patch_inlines(TP_Context* tp, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafHashTable* leaf_ht_arr, uint64 obj_count, CV_DebugS* debug_s_arr);
void                lnk_patch_leaves(TP_Context* tp, LNK_CodeViewInput* input, LNK_LeafHashes* hashes, LNK_LeafHashTable* leaf_ht_arr, LNK_LeafBucketArray bucket_arr);
String8Node *       lnk_copy_raw_leaf_arr_to_type_server(TP_Context* tp, CV_DebugT types, PDB_TypeServer* type_server);
CV_DebugT *         lnk_import_types(TP_Context* tp, TP_Arena* tp_temp, LNK_CodeViewInput* input);

void lnk_replace_type_names_with_hashes(TP_Context* tp, TP_Arena* arena, CV_DebugT debug_t, LNK_TypeNameHashMode mode, uint64 hash_length, String8 map_name);

////////////////////////////////
// RAD Debug info

uint64                  lnk_udt_name_hash_table_hash(String8 string);
LNK_UDTNameBucket ** lnk_udt_name_hash_table_from_debug_t(TP_Context* tp, TP_Arena* arena, CV_DebugT debug_t, uint64* buckets_cap_out);
LNK_UDTNameBucket *  lnk_udt_name_hash_table_lookup(LNK_UDTNameBucket** buckets, uint64 cap, String8 name);
CV_TypeIndex *       lnk_build_udt_fwdmap(TP_Context* tp, Arena* arena, CV_DebugT debug_t, CV_TypeIndex ti_lo, LNK_UDTNameBucket** udt_name_buckets, uint64 udt_name_buckets_cap);

RDIB_TypeRef           lnk_rdib_type_from_itype(LNK_ConvertTypesToRDI* task, CV_TypeIndex itype);
RDI_MemberKind         lnk_rdib_method_kind_from_cv_prop(CV_MethodProp prop);
LNK_SourceFileBucket * lnk_src_file_hash_table_hash(String8 file_path, CV_C13ChecksumKind checksum_kind, String8 checksum_bytes);
LNK_SourceFileBucket * lnk_src_file_hash_table_lookup_slot(LNK_SourceFileBucket** src_file_buckets, uint64 src_file_buckets_cap, uint64 hash, String8 file_path, CV_C13ChecksumKind checksum_kind, String8 checksum_bytes);

String8List lnk_build_rad_debug_info(TP_Context*               tp,
                                              TP_Arena*                 tp_arena,
                                              OperatingSystem           os,
                                              RDI_Arch                  arch,
                                              String8                   image_name,
                                              String8                   image_data,
                                              LNK_SectionArray          image_sects,
                                              LNK_Section**             sect_id_map,
                                              uint64                       obj_count,
                                              LNK_Obj*                  obj_arr,
                                              CV_DebugS*                debug_s_arr,
                                              uint64                       total_symbol_input_count,
                                              LNK_CodeViewSymbolsInput* symbol_inputs,
                                              CV_SymbolListArray*       parsed_symbols,
                                              CV_DebugT                 types[CV_TypeIndexSource_COUNT]);

////////////////////////////////
// PDB

LNK_ProcessedCodeViewC11Data lnk_process_c11_data(TP_Context* tp, TP_Arena* arena, uint64 obj_count, CV_DebugS* debug_s_arr, uint64 string_data_base_offset, CV_StringHashTable string_ht, MSF_Context* msf, PDB_DbiModule** mod_arr);
LNK_ProcessedCodeViewC13Data lnk_process_c13_data(TP_Context* tp, TP_Arena* arena, uint64 obj_count, CV_DebugS* debug_s_arr, uint64 string_data_base_offset, CV_StringHashTable string_ht, MSF_Context* msf, PDB_DbiModule** mod_arr);
uint64 *                        lnk_hash_cv_symbol_ptr_arr(TP_Context* tp, Arena* arena, CV_SymbolPtrArray arr);
CV_SymbolPtrArray            lnk_dedup_gsi_symbols(TP_Context* tp, Arena* arena, PDB_GsiContext* gsi, uint64 obj_count, CV_SymbolList* symbol_list_arr);

void lnk_build_pdb_public_symbols(TP_Context*            tp,
                                           TP_Arena*              arena,
                                           LNK_SymbolTable*       symtab,
                                           LNK_Section**          sect_id_map,
                                           PDB_PsiContext*        psi);

String8List lnk_build_pdb(TP_Context*               tp,
                                   TP_Arena*                 tp_arena,
                                   String8                   image_data,
                                   Guid                      guid,
                                   COFF_MachineType          machine,
                                   COFF_TimeStamp            time_stamp,
                                   uint32                       age,
                                   uint64                       page_size,
                                   String8                   pdb_name,
                                   String8List               lib_dir_list,
                                   String8List               natvis_list,
                                   LNK_SymbolTable*          symtab,
                                   LNK_Section**             sect_id_map,
                                   uint64                       obj_count,
                                   LNK_Obj*                  obj_arr,
                                   CV_DebugS*                debug_s_arr,
                                   uint64                       total_symbol_input_count,
                                   LNK_CodeViewSymbolsInput* symbol_inputs,
                                   CV_SymbolListArray*       parsed_symbols,
                                   CV_DebugT                 types[CV_TypeIndexSource_COUNT]);

////////////////////////////////
// RAD Debug Info

uint64                  lnk_udt_name_hash_table_hash(String8 string);
LNK_UDTNameBucket ** lnk_udt_name_hash_table_from_debug_t(TP_Context* tp, TP_Arena* arena, CV_DebugT debug_t, uint64* buckets_cap_out);
LNK_UDTNameBucket *  lnk_udt_name_hash_table_lookup(LNK_UDTNameBucket** buckets, uint64 cap, String8 name);

CV_TypeIndex * lnk_build_udt_fwdmap(TP_Context*         tp,
                                             Arena*              arena,
                                             CV_DebugT           debug_t,
                                             CV_TypeIndex        ti_lo,
                                             LNK_UDTNameBucket** udt_name_buckets,
                                             uint64                 udt_name_buckets_cap);

void           lnk_init_rdib_itype_map(Arena* arena, RDI_Arch arch, RDIB_Type** itype_map, RDIB_TypeChunkList* rdib_types_list);
RDIB_TypeRef   lnk_rdib_type_from_itype(LNK_ConvertTypesToRDI* task, CV_TypeIndex itype);
RDI_MemberKind lnk_rdib_method_kind_from_cv_prop(CV_MethodProp prop);


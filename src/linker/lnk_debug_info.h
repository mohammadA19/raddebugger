// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

// --- Code View Input ---------------------------------------------------------

typedef struct LNK_PchInfo
{
  CV_TypeIndex ti_lo;
  CV_TypeIndex ti_hi;
  U64          debug_p_obj_idx;
} LNK_PchInfo;

typedef struct LNK_CodeViewSymbolsInput
{
  U64            obj_idx;
  CV_SymbolList *symbol_list;
  String8        raw_symbols;
} LNK_CodeViewSymbolsInput;

typedef struct LNK_CodeViewInput
{
  U64             count;
  U64             internal_count;
  U64             external_count;
  U64             type_server_count;
  String8        *type_server_path_arr; // [type_server_count]
  String8        *type_server_data_arr; // [type_server_count]
  U64List        *ts_to_obj_arr;        // [type_server_count]
  LNK_Obj        *obj_arr;              // [count]
  LNK_PchInfo    *pch_arr;              // [count]
  CV_DebugS      *debug_s_arr;          // [count]
  CV_DebugT      *debug_p_arr;          // [count]
  CV_DebugT      *debug_t_arr;          // [count]
  CV_DebugT      *merged_debug_t_p_arr; // [count]

  U64                       total_symbol_input_count;
  LNK_CodeViewSymbolsInput *symbol_inputs;  // [total_symbol_input_count]
  CV_SymbolListArray       *parsed_symbols; // [count]

  LNK_Obj                  *internal_obj_arr;         // [internal_count]
  CV_DebugS                *internal_debug_s_arr;     // [internal_count]
  CV_DebugT                *internal_debug_t_arr;     // [internal_count]
  CV_DebugT                *internal_debug_p_arr;     // [internal_count]
  U64                      internal_total_symbol_input_count;
  LNK_CodeViewSymbolsInput *internal_symbol_inputs;   // [internal_total_symbol_input_count]
  CV_SymbolListArray       *internal_parsed_symbols;  // [internal_count]

  LNK_Obj                   *external_obj_arr;           // [external_count]
  CV_DebugS                 *external_debug_s_arr;       // [external_count]
  CV_DebugT                 *external_debug_t_arr;       // [external_count]
  CV_DebugT                 *external_debug_p_arr;       // [external_count]
  U64                        external_total_symbol_input_count;
  LNK_CodeViewSymbolsInput  *external_symbol_inputs;     // [exteranl_total_symbol_input_count]
  CV_SymbolListArray        *external_parsed_symbols;    // [external_count]
  Rng1U64                  **external_ti_ranges;         // [type_server_count]
  CV_DebugT                **external_leaves;            // [type_server_count]
  U64                       *external_obj_to_ts_idx_arr; // [external_count]
  Rng1U64                    external_obj_range;
} LNK_CodeViewInput;

// --- Leaf Ref ----------------------------------------------------------------

typedef enum
{
  LNK_LeafLocType_Internal,
  LNK_LeafLocType_External,
  LNK_LeafLocType_Count
} LNK_LeafLocType;

#define LNK_LeafRefFlag_LocIdxExternal (1 << 31)
#define LNK_LeafRefFlag_LeafIdxIPI     (1 << 31)
typedef struct
{
  U32 enc_loc_idx;
  U32 enc_leaf_idx;
} LNK_LeafRef;

typedef struct LNK_LeafRange
{
  struct LNK_LeafRange *next;
  Rng1U64               range;
  CV_DebugT            *debug_t;
} LNK_LeafRange;

typedef struct LNK_LeafRangeList
{
  U64            count;
  LNK_LeafRange *first;
  LNK_LeafRange *last;
} LNK_LeafRangeList;

typedef struct
{
  LNK_LeafRef  leaf_ref;
  CV_TypeIndex type_index;
} LNK_LeafBucket;

typedef struct
{
  U64              count;
  LNK_LeafBucket **v;
} LNK_LeafBucketArray;

typedef struct
{
  U64              cap;
  LNK_LeafBucket **bucket_arr;
} LNK_LeafHashTable;

typedef union
{
  struct {
    U128Array **internal_hashes;
    U128Array **external_hashes;
  };
  U128Array **v[CV_TypeIndexSource_COUNT];
} LNK_LeafHashes;

// --- Symbol Parsing Tasks ----------------------------------------------------

typedef struct
{
  LNK_Obj    **obj_arr;
  String8List *sect_list_arr;
  CV_DebugS   *debug_s_arr;
} LNK_ParseDebugSTaskData;

typedef struct
{
  LNK_Obj     **obj_arr;
  String8Array *data_arr_arr;
} LNK_CheckDebugTSigTaskData;

typedef struct
{
  LNK_Obj     **obj_arr;
  String8Array *data_arr_arr;
  CV_DebugT    *debug_t_arr;
} LNK_ParseDebugTTaskData;

typedef struct
{
  String8Array   data_arr;
  MSF_Parsed   **msf_parse_arr;
} LNK_MsfParsedFromDataTask;

typedef struct
{
  CV_TypeServerInfo  *ts_info_arr;
  MSF_Parsed        **msf_parse_arr;
  Rng1U64           **external_ti_ranges;
  CV_DebugT         **external_leaves;
  B8                 *is_corrupted;
} LNK_GetExternalLeavesTask;

// --- Leaf Deduping Tasks -----------------------------------------------------

typedef struct
{
  LNK_LeafRangeList  *leaf_ranges_per_task;
  U64               **count_arr_arr;
} LNK_CountPerSourceLeafTask;

typedef struct
{
  LNK_CodeViewInput *input;
  LNK_LeafHashes    *hashes;
  Arena            **fixed_arenas;
  CV_DebugT         *debug_t_arr;
} LNK_LeafHasherTask;

typedef struct
{
  LNK_CodeViewInput  *input;
  LNK_LeafHashes     *hashes;
  LNK_LeafHashTable  *leaf_ht_arr;
  CV_DebugT          *debug_t_arr;
} LNK_LeafDedupInternal;

typedef struct
{
  LNK_CodeViewInput  *input;
  LNK_LeafHashes     *hashes;
  LNK_LeafHashTable  *leaf_ht_arr;
  CV_TypeIndexSource  dedup_ti_source;
} LNK_LeafDedupExternal;

typedef struct
{
  LNK_LeafHashTable   *ht;
  U64                 *count_arr;
  Rng1U64             *range_arr;
  U64                 *offset_arr;
  LNK_LeafBucketArray  result;
} LNK_GetPresentBucketsTask;

typedef struct
{
  U64             loc_idx_bit_count_0;
  U64             loc_idx_bit_count_1;
  U64             loc_idx_bit_count_2;
  U64             counts_max;
  U32           **counts_arr;
  Rng1U64        *ranges;
  LNK_LeafBucket **dst;
  LNK_LeafBucket **src;
  U64             loc_idx_max;
  U64             pass_idx;
} LNK_LeafRadixSortTask;

typedef struct
{
  U32             *counts;
  U32             *offsets;
  LNK_LeafBucket **dst;
  LNK_LeafBucket **src;
  Rng1U64         *ranges;
} LNK_LeafLocRadixSortTask;

typedef struct
{
  Rng1U64            *range_arr;
  CV_TypeIndex        min_type_index;
  LNK_LeafBucketArray bucket_arr;
} LNK_AssignTypeIndicesTask;

typedef struct
{
  LNK_CodeViewInput  *input;
  LNK_LeafBucket    **bucket_arr;
  U8                **raw_leaf_arr;
  Rng1U64            *range_arr;
} LNK_UnbucketRawLeavesTask;

typedef struct
{
  LNK_CodeViewInput  *input;
  LNK_LeafHashes     *hashes;
  LNK_LeafHashTable  *leaf_ht_arr;
  CV_SymbolList      *symbol_list_arr;
  Arena             **arena_arr;
} LNK_PatchSymbolTypesTask;

typedef struct
{
  LNK_CodeViewInput *input;
  LNK_LeafHashes    *hashes;
  LNK_LeafHashTable *leaf_ht_arr;
  CV_DebugS         *debug_s_arr;
} LNK_PatchInlinesTask;

typedef struct
{
  LNK_CodeViewInput  *input;
  LNK_LeafHashes     *hashes;
  LNK_LeafHashTable  *leaf_ht_arr;
  LNK_LeafBucket    **bucket_arr;
  Rng1U64            *range_arr;
  Arena             **fixed_arena_arr;
} LNK_PatchLeavesTask;

// --- Code View Processing Trasks ---------------------------------------------

typedef struct
{
  String8List *data_list_arr;
} LNK_ProcessedCodeViewC11Data;

typedef struct
{
  String8List *data_list_arr;
  String8List *source_file_names_list_arr;
} LNK_ProcessedCodeViewC13Data;

typedef struct
{
  LNK_CodeViewSymbolsInput *inputs;
} LNK_ParseCVSymbolsTaskData;

typedef struct
{
  U64                        total_symbol_input_count;
  LNK_CodeViewSymbolsInput  *symbol_inputs;
  CV_SymbolListArray        *parsed_symbols;
  PDB_DbiModule            **mod_arr;
  String8List               *symbol_data_arr;
  CV_SymbolList             *gsi_list_arr;
} LNK_ProcessSymDataTaskData;

typedef struct
{
  CV_DebugS          *debug_s_arr;
  MSF_Context        *msf;
  PDB_DbiModule      **dbi_mod_arr;
  String8List        *c13_data_arr;
  String8List        *source_file_names_list_arr;
  U64                 string_data_base_offset;
  CV_StringHashTable  string_ht;
} LNK_ProcessC13DataTask;

typedef struct
{
  MSF_Context    *msf;
  PDB_DbiModule **mod_arr;
  String8List    *symbol_data_arr;
  String8List    *c11_data_list_arr;
  String8List    *c13_data_list_arr;
  String8List    *globrefs_arr;
} LNK_WriteModuleDataTask;

typedef struct
{
  LNK_Obj                    *obj_arr;
  PDB_DbiModule             **mod_arr;
  PDB_DbiSectionContribList  *sc_list;
  String8                     image_data;
  Rng1U64Array                image_section_file_ranges;
  Rng1U64Array                image_section_virt_ranges;
} LNK_PushDbiSecContribTaskData;

typedef struct
{
  U32Array      *hash_arr_arr;
  CV_SymbolList *list_arr;
} LNK_HashCVSymbolListTask;

typedef struct
{
  U64            *hash_arr;
  CV_SymbolNode **arr;
  Rng1U64        *range_arr;
} LNK_CvSymbolPtrArrayHasher;

typedef struct
{
  LNK_SymbolHashTrieChunkList  *chunk_lists;
  CV_SymbolList                *pub_list_arr;

  Rng1U64           *symbol_ranges;
  PDB_GsiContext    *gsi;
  CV_SymbolPtrArray  symbols;
  U32               *hashes;
} LNK_BuildPublicSymbolsTask;

typedef struct
{
  CV_TypeIndex              ipi_min_type_index;
  CV_DebugT                 ipi_types;
  LNK_CodeViewSymbolsInput *symbol_inputs;
  CV_SymbolListArray       *parsed_symbols;
} LNK_PostProcessCvSymbolsTask;

typedef struct
{
  Rng1U64           *range_arr;
  CV_SymbolPtrNode **bucket_arr;
  CV_SymbolPtrNode **out_arr;
  U64               *out_count_arr;
} LNK_GsiDeduper;

typedef struct
{
  Rng1U64           *range_arr;
  CV_SymbolPtrNode **bucket_arr;
  U64               *symbol_base_arr;
  CV_SymbolNode    **symbol_arr;
} LNK_GsiUnbucket;

typedef struct
{
  CV_DebugT    debug_t;
  Rng1U64     *ranges;
  U64          hash_length;
  B32          make_map;
  TP_Arena    *map_arena;
  String8List *maps;
} LNK_TypeNameReplacer;

// --- RAD Debug Info ----------------------------------------------------------

typedef struct
{
  String8 name;
  U64     leaf_idx;
} LNK_UDTNameBucket;

typedef struct
{
  CV_DebugT           debug_t;
  Rng1U64            *ranges;
  U64                 buckets_cap;
  LNK_UDTNameBucket **buckets;
} LNK_BuildUDTNameHashTableTask;

typedef struct
{
  CV_DebugT           debug_t;
  CV_TypeIndex        ti_lo;
  Rng1U64            *ranges;
  U64                 udt_name_buckets_cap;
  LNK_UDTNameBucket **udt_name_buckets;
  CV_TypeIndex       *fwdmap;
} LNK_BuildUDTFwdMapTask;

// --- RDI Conversion Tasks ----------------------------------------------------

typedef struct
{
  CV_DebugT               *types;
  U64                      type_cap;
  U64                      udt_cap;
  RDIB_TypeRef             variadic_type_ref;
  Rng1U64                 *itype_ranges;
  U64                      udt_name_bucket_cap;
  LNK_UDTNameBucket      **udt_name_buckets;
  RDIB_Type              **tpi_itype_map;
  RDIB_TypeChunkList      *rdib_types_lists;
  RDIB_TypeChunkList      *rdib_types_struct_lists;
  RDIB_TypeChunkList      *rdib_types_union_lists;
  RDIB_TypeChunkList      *rdib_types_enum_lists;
  RDIB_TypeChunkList      *rdib_types_params_lists;
  RDIB_TypeChunkList      *rdib_types_udt_members_lists;
  RDIB_TypeChunkList      *rdib_types_enum_members_lists;
  RDIB_UDTMemberChunkList *rdib_udt_members_lists;
  RDIB_UDTMemberChunkList *rdib_enum_members_lists;
  Rng1U64                 *ranges;
} LNK_ConvertTypesToRDI;

typedef struct
{
  U64              obj_idx;
  RDIB_SourceFile *src_file;
} LNK_SourceFileBucket;

typedef struct
{
  LNK_Obj               *obj_arr;
  CV_DebugS             *debug_s_arr;
  U64                    total_src_file_count;
  LNK_SourceFileBucket **src_file_buckets;
  U64                    src_file_buckets_cap;
} LNK_ConvertSourceFilesToRDITask;

typedef struct
{
  CV_Arch     arch;
  CV_Language language;
  String8     compiler_name;
} LNK_CodeViewCompilerInfo;

typedef struct
{
  COFF_SectionHeaderArray   image_sects;
  LNK_Obj                  *obj_arr;
  CV_DebugS                *debug_s_arr;
  CV_DebugT                 ipi;
  LNK_CodeViewSymbolsInput *symbol_inputs;
  CV_SymbolListArray       *parsed_symbols;
  Rng1U64                   ipi_itype_range;
  Rng1U64                   tpi_itype_range;
  RDIB_Type               **tpi_itype_map;
  U64                       src_file_buckets_cap;
  LNK_SourceFileBucket    **src_file_buckets;
  LNK_UDTNameBucket       **udt_name_buckets;
  U64                       line_table_cap;
  U64                       udt_name_buckets_cap;
  U64                       src_file_chunk_cap;
  U64                       symbol_chunk_cap;
  U64                       unit_chunk_cap;
  U64                       inline_site_cap;
  RDIB_LineTable           *null_line_table;
  HashTable                *extern_symbol_voff_ht;
  LNK_CodeViewCompilerInfo *comp_info_arr;
  CV_InlineeLinesAccel    **inlinee_lines_accel_arr;

  RDIB_InlineSiteChunk **inline_site_chunks;

  // output
  RDIB_UnitChunk           *units;
  RDIB_VariableChunkList   *locals;
  RDIB_ScopeChunkList      *scopes;
  RDIB_VariableChunkList   *extern_gvars;
  RDIB_VariableChunkList   *static_gvars;
  RDIB_VariableChunkList   *extern_tvars;
  RDIB_VariableChunkList   *static_tvars;
  RDIB_ProcedureChunkList  *extern_procs;
  RDIB_ProcedureChunkList  *static_procs;
  RDIB_InlineSiteChunkList *inline_sites;
  RDIB_LineTableChunkList  *line_tables;
} LNK_ConvertUnitToRDITask;

// --- CodeView ----------------------------------------------------------------

// --- RAD Debug info ----------------------------------------------------------

internal String8List lnk_build_rad_debug_info(TP_Context               *tp,
                                              TP_Arena                 *tp_arena,
                                              OperatingSystem           os,
                                              RDI_Arch                  arch,
                                              String8                   image_name,
                                              String8                   image_data,
                                              U64                       obj_count,
                                              LNK_Obj                  *obj_arr,
                                              CV_DebugS                *debug_s_arr,
                                              U64                       total_symbol_input_count,
                                              LNK_CodeViewSymbolsInput *symbol_inputs,
                                              CV_SymbolListArray       *parsed_symbols,
                                              CV_DebugT                 types[CV_TypeIndexSource_COUNT]);

// --- PDB ---------------------------------------------------------------------

internal String8List lnk_build_pdb(TP_Context               *tp,
                                   TP_Arena                 *tp_arena,
                                   String8                   image_data,
                                   LNK_Config               *config,
                                   LNK_SymbolTable          *symtab,
                                   U64                       obj_count,
                                   LNK_Obj                  *obj_arr,
                                   CV_DebugS                *debug_s_arr,
                                   U64                       total_symbol_input_count,
                                   LNK_CodeViewSymbolsInput *symbol_inputs,
                                   CV_SymbolListArray       *parsed_symbols,
                                   CV_DebugT                 types[CV_TypeIndexSource_COUNT]);

// --- RAD Debug Info ----------------------------------------------------------

internal CV_TypeIndex * lnk_build_udt_fwdmap(TP_Context         *tp,
                                             Arena              *arena,
                                             CV_DebugT           debug_t,
                                             CV_TypeIndex        ti_lo,
                                             LNK_UDTNameBucket **udt_name_buckets,
                                             U64                 udt_name_buckets_cap);


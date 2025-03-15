// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_FROM_PDB_H
#define RDI_FROM_PDB_H

////////////////////////////////
//~ rjf: Export Artifact Flags

P2R_ConvertFlags :: enum U32
{
  P2R_ConvertFlag_Strings                 = (1<<0),
  P2R_ConvertFlag_IndexRuns               = (1<<1),
  P2R_ConvertFlag_BinarySections          = (1<<2),
  P2R_ConvertFlag_Units                   = (1<<3),
  P2R_ConvertFlag_Procedures              = (1<<4),
  P2R_ConvertFlag_GlobalVariables         = (1<<5),
  P2R_ConvertFlag_ThreadVariables         = (1<<6),
  P2R_ConvertFlag_Scopes                  = (1<<7),
  P2R_ConvertFlag_Locals                  = (1<<8),
  P2R_ConvertFlag_Types                   = (1<<9),
  P2R_ConvertFlag_UDTs                    = (1<<10),
  P2R_ConvertFlag_LineInfo                = (1<<11),
  P2R_ConvertFlag_GlobalVariableNameMap   = (1<<12),
  P2R_ConvertFlag_ThreadVariableNameMap   = (1<<13),
  P2R_ConvertFlag_ProcedureNameMap        = (1<<14),
  P2R_ConvertFlag_TypeNameMap             = (1<<15),
  P2R_ConvertFlag_LinkNameProcedureNameMap= (1<<16),
  P2R_ConvertFlag_NormalSourcePathNameMap = (1<<17),
  P2R_ConvertFlag_Deterministic           = (1<<18),
  P2R_ConvertFlag_All = 0xffffffff,
}

////////////////////////////////
//~ rjf: Conversion Stage Inputs/Outputs

P2R_User2Convert :: struct
{
  String8 input_pdb_name;
  String8 input_pdb_data;
  String8 input_exe_name;
  String8 input_exe_data;
  String8 output_name;
  P2R_ConvertFlags flags;
  String8List errors;
}

P2R_Convert2Bake :: struct
{
  RDIM_BakeParams bake_params;
}

P2R_Bake2Serialize :: struct
{
  RDIM_BakeResults bake_results;
}

P2R_Serialize2File :: struct
{
  RDIM_SerializedSectionBundle bundle;
}

////////////////////////////////
//~ rjf: Initial PDB Information Extraction & Conversion Preparation Task Types

//- rjf: tpi hash parsing

P2R_TPIHashParseIn :: struct
{
  PDB_Strtbl *strtbl;
  PDB_TpiParsed *tpi;
  String8 hash_data;
  String8 aux_data;
}

//- rjf: tpi leaves parsing

P2R_TPILeafParseIn :: struct
{
  String8 leaf_data;
  CV_TypeId itype_first;
}

//- rjf: exe hashing

P2R_EXEHashIn :: struct
{
  String8 exe_data;
}

//- rjf: symbol stream parsing

P2R_SymbolStreamParseIn :: struct
{
  String8 data;
}

//- rjf: c13 line info stream parsing

P2R_C13StreamParseIn :: struct
{
  String8 data;
  String8 strtbl;
  COFF_SectionHeaderArray coff_sections;
}

//- rjf: comp unit parsing

P2R_CompUnitParseIn :: struct
{
  String8 data;
}

//- rjf: comp unit contribution table parsing

P2R_CompUnitContributionsParseIn :: struct
{
  String8 data;
  COFF_SectionHeaderArray coff_sections;
}

////////////////////////////////
//~ rjf: Conversion Data Structure & Task Types

//- rjf: link name map (voff -> string)

P2R_LinkNameNode :: struct
{
  P2R_LinkNameNode *next;
  U64 voff;
  String8 name;
}

P2R_LinkNameMap :: struct
{
  P2R_LinkNameNode **buckets;
  U64 buckets_count;
  U64 bucket_collision_count;
  U64 link_name_count;
}

//- rjf: normalized file path -> source file map

P2R_SrcFileNode :: struct
{
  P2R_SrcFileNode *next;
  RDIM_SrcFile *src_file;
}

P2R_SrcFileMap :: struct
{
  P2R_SrcFileNode **slots;
  U64 slots_count;
}

//- rjf: unit conversion tasks

P2R_UnitConvertIn :: struct
{
  PDB_Strtbl *pdb_strtbl;
  COFF_SectionHeaderArray coff_sections;
  PDB_CompUnitArray *comp_units;
  PDB_CompUnitContributionArray *comp_unit_contributions;
  CV_SymParsed **comp_unit_syms;
  CV_C13Parsed **comp_unit_c13s;
}

P2R_UnitConvertOut :: struct
{
  RDIM_UnitChunkList units;
  RDIM_SrcFileChunkList src_files;
  RDIM_LineTableChunkList line_tables;
  RDIM_LineTable **units_first_inline_site_line_tables;
}

//- rjf: link name map building tasks

P2R_LinkNameMapBuildIn :: struct
{
  CV_SymParsed *sym;
  COFF_SectionHeaderArray coff_sections;
  P2R_LinkNameMap *link_name_map;
}

//- rjf: type forward resolution map build

P2R_ITypeFwdMapFillIn :: struct
{
  PDB_TpiHashParsed *tpi_hash;
  CV_LeafParsed *tpi_leaf;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  CV_TypeId *itype_fwd_map;
}

//- rjf: itype chain build

P2R_TypeIdChain :: struct
{
  P2R_TypeIdChain *next;
  CV_TypeId itype;
}

P2R_ITypeChainBuildIn :: struct
{
  CV_LeafParsed *tpi_leaf;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  CV_TypeId *itype_fwd_map;
  P2R_TypeIdChain **itype_chains;
}

//- rjf: udt conversion

P2R_UDTConvertIn :: struct
{
  CV_LeafParsed *tpi_leaf;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  CV_TypeId *itype_fwd_map;
  RDIM_Type **itype_type_ptrs;
}

//- rjf: symbol stream conversion

P2R_SymbolStreamConvertIn :: struct
{
  RDI_Arch arch;
  COFF_SectionHeaderArray coff_sections;
  PDB_TpiHashParsed *tpi_hash;
  CV_LeafParsed *tpi_leaf;
  CV_LeafParsed *ipi_leaf;
  CV_SymParsed *sym;
  U64 sym_ranges_first;
  U64 sym_ranges_opl;
  CV_TypeId *itype_fwd_map;
  RDIM_Type **itype_type_ptrs;
  P2R_LinkNameMap *link_name_map;
  RDIM_LineTable *first_inline_site_line_table;
}

P2R_SymbolStreamConvertOut :: struct
{
  RDIM_SymbolChunkList procedures;
  RDIM_SymbolChunkList global_variables;
  RDIM_SymbolChunkList thread_variables;
  RDIM_ScopeChunkList scopes;
  RDIM_InlineSiteChunkList inline_sites;
}

////////////////////////////////
//~ rjf: Baking Task Types

//- rjf: line table baking task types

P2R_BakeLineTablesIn :: struct
{
  RDIM_LineTableChunkList *line_tables;
}

//- rjf: string map baking task types

P2R_BakeSrcFilesStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_SrcFileChunkList *list;
}

P2R_BakeUnitsStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_UnitChunkList *list;
}

P2R_BakeTypesStringsInNode :: struct
{
  P2R_BakeTypesStringsInNode *next;
  RDIM_Type *v;
  RDI_U64 count;
}

P2R_BakeTypesStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  P2R_BakeTypesStringsInNode *first;
  P2R_BakeTypesStringsInNode *last;
}

P2R_BakeUDTsStringsInNode :: struct
{
  P2R_BakeUDTsStringsInNode *next;
  RDIM_UDT *v;
  RDI_U64 count;
}

P2R_BakeUDTsStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  P2R_BakeUDTsStringsInNode *first;
  P2R_BakeUDTsStringsInNode *last;
}

P2R_BakeSymbolsStringsInNode :: struct
{
  P2R_BakeSymbolsStringsInNode *next;
  RDIM_Symbol *v;
  RDI_U64 count;
}

P2R_BakeSymbolsStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  P2R_BakeSymbolsStringsInNode *first;
  P2R_BakeSymbolsStringsInNode *last;
}

P2R_BakeInlineSiteStringsInNode :: struct
{
  P2R_BakeInlineSiteStringsInNode *next;
  RDIM_InlineSite *v;
  RDI_U64 count;
}

P2R_BakeInlineSiteStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  P2R_BakeInlineSiteStringsInNode *first;
  P2R_BakeInlineSiteStringsInNode *last;
}

P2R_BakeScopesStringsInNode :: struct
{
  P2R_BakeScopesStringsInNode *next;
  RDIM_Scope *v;
  RDI_U64 count;
}

P2R_BakeScopesStringsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  P2R_BakeScopesStringsInNode *first;
  P2R_BakeScopesStringsInNode *last;
}

//- rjf: string map joining task types

P2R_JoinBakeStringMapSlotsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **src_maps;
  U64 src_maps_count;
  RDIM_BakeStringMapLoose *dst_map;
  Rng1U64 slot_idx_range;
}

//- rjf: string map sorting task types

P2R_SortBakeStringMapSlotsIn :: struct
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose *src_map;
  RDIM_BakeStringMapLoose *dst_map;
  U64 slot_idx;
  U64 slot_count;
}

//- rjf: OLD string map baking types

P2R_BuildBakeStringMapIn :: struct
{
  RDIM_BakePathTree *path_tree;
  RDIM_BakeParams *params;
}

P2R_BuildBakeNameMapIn :: struct
{
  RDI_NameMapKind k;
  RDIM_BakeParams *params;
}

//- rjf: debug info baking task types

P2R_BakeUnitsIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakePathTree *path_tree;
  RDIM_UnitChunkList *units;
}

P2R_BakeUnitVMapIn :: struct
{
  RDIM_UnitChunkList *units;
}

P2R_BakeSrcFilesIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakePathTree *path_tree;
  RDIM_SrcFileChunkList *src_files;
}

P2R_BakeUDTsIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_UDTChunkList *udts;
}

P2R_BakeGlobalVariablesIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *global_variables;
}

P2R_BakeGlobalVMapIn :: struct
{
  RDIM_SymbolChunkList *global_variables;
}

P2R_BakeThreadVariablesIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *thread_variables;
}

P2R_BakeProceduresIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *procedures;
}

P2R_BakeScopesIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_ScopeChunkList *scopes;
}

P2R_BakeScopeVMapIn :: struct
{
  RDIM_ScopeChunkList *scopes;
}

P2R_BakeInlineSitesIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_InlineSiteChunkList *inline_sites;
}

P2R_BakeFilePathsIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakePathTree *path_tree;
}

P2R_BakeStringsIn :: struct
{
  RDIM_BakeStringMapTight *strings;
}

P2R_BakeTypeNodesIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakeIdxRunMap *idx_runs;
  RDIM_TypeChunkList *types;
}

P2R_BakeNameMapIn :: struct
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakeIdxRunMap *idx_runs;
  RDIM_BakeNameMap *map;
  RDI_NameMapKind kind;
}

P2R_BakeIdxRunsIn :: struct
{
  RDIM_BakeIdxRunMap *idx_runs;
}

////////////////////////////////
//~ rjf: Top-Level State

P2R_State :: struct
{
  Arena *arena;
  U64 work_thread_arenas_count;
  Arena **work_thread_arenas;
}

////////////////////////////////
//~ rjf: Globals

static P2R_State *p2r_state = 0;

////////////////////////////////
//~ rjf: Basic Helpers

U64 p2r_end_of_cplusplus_container_name(String8 str);
U64 p2r_hash_from_voff(U64 voff);

////////////////////////////////
//~ rjf: Command Line -> Conversion Inputs

P2R_User2Convert *p2r_user2convert_from_cmdln(Arena *arena, CmdLine *cmdline);

////////////////////////////////
//~ rjf: COFF => RDI Canonical Conversions

RDI_BinarySectionFlags p2r_rdi_binary_section_flags_from_coff_section_flags(COFF_SectionFlags flags);

////////////////////////////////
//~ rjf: CodeView => RDI Canonical Conversions

RDI_Arch     p2r_rdi_arch_from_cv_arch(CV_Arch arch);
RDI_RegCode  p2r_rdi_reg_code_from_cv_reg_code(RDI_Arch arch, CV_Reg reg_code);
RDI_Language p2r_rdi_language_from_cv_language(CV_Language language);
RDI_TypeKind p2r_rdi_type_kind_from_cv_basic_type(CV_BasicType basic_type);

////////////////////////////////
//~ rjf: Location Info Building Helpers

RDIM_Location *p2r_location_from_addr_reg_off(Arena *arena, RDI_Arch arch, RDI_RegCode reg_code, U32 reg_byte_size, U32 reg_byte_pos, S64 offset, B32 extra_indirection);
RDI_RegCode p2r_reg_code_from_arch_encoded_fp_reg(RDI_Arch arch, CV_EncodedFramePtrReg encoded_reg);
void p2r_location_over_lvar_addr_range(Arena *arena, RDIM_ScopeChunkList *scopes, RDIM_LocationSet *locset, RDIM_Location *location, CV_LvarAddrRange *range, COFF_SectionHeader *section, CV_LvarAddrGap *gaps, U64 gap_count);

////////////////////////////////
//~ rjf: Initial Parsing & Preparation Pass Tasks

ASYNC_WORK_DEF(p2r_exe_hash_work);
ASYNC_WORK_DEF(p2r_tpi_hash_parse_work);
ASYNC_WORK_DEF(p2r_tpi_leaf_work);
ASYNC_WORK_DEF(p2r_symbol_stream_parse_work);
ASYNC_WORK_DEF(p2r_c13_stream_parse_work);
ASYNC_WORK_DEF(p2r_comp_unit_parse_work);
ASYNC_WORK_DEF(p2r_comp_unit_contributions_parse_work);

////////////////////////////////
//~ rjf: Unit Conversion Tasks

ASYNC_WORK_DEF(p2r_units_convert_work);

////////////////////////////////
//~ rjf: Link Name Map Building Tasks

ASYNC_WORK_DEF(p2r_link_name_map_build_work);

////////////////////////////////
//~ rjf: Type Parsing/Conversion Tasks

ASYNC_WORK_DEF(p2r_itype_fwd_map_fill_work);
ASYNC_WORK_DEF(p2r_itype_chain_build_work);

////////////////////////////////
//~ rjf: UDT Conversion Tasks

ASYNC_WORK_DEF(p2r_udt_convert_work);

////////////////////////////////
//~ rjf: Symbol Stream Conversion Tasks

ASYNC_WORK_DEF(p2r_symbol_stream_convert_work);

////////////////////////////////
//~ rjf: Top-Level Conversion Entry Point

P2R_Convert2Bake *p2r_convert(Arena *arena, P2R_User2Convert *in);

////////////////////////////////
//~ rjf: Baking Stage Tasks

//- rjf: unsorted bake string map building
ASYNC_WORK_DEF(p2r_bake_src_files_strings_work);
ASYNC_WORK_DEF(p2r_bake_units_strings_work);
ASYNC_WORK_DEF(p2r_bake_types_strings_work);
ASYNC_WORK_DEF(p2r_bake_udts_strings_work);
ASYNC_WORK_DEF(p2r_bake_symbols_strings_work);
ASYNC_WORK_DEF(p2r_bake_scopes_strings_work);
ASYNC_WORK_DEF(p2r_bake_line_tables_work);

//- rjf: bake string map joining
ASYNC_WORK_DEF(p2r_bake_string_map_join_work);

//- rjf: bake string map sorting
ASYNC_WORK_DEF(p2r_bake_string_map_sort_work);

//- rjf: pass 1: interner/deduper map builds
ASYNC_WORK_DEF(p2r_build_bake_name_map_work);

//- rjf: pass 2: string-map-dependent debug info stream builds
ASYNC_WORK_DEF(p2r_bake_units_work);
ASYNC_WORK_DEF(p2r_bake_unit_vmap_work);
ASYNC_WORK_DEF(p2r_bake_src_files_work);
ASYNC_WORK_DEF(p2r_bake_udts_work);
ASYNC_WORK_DEF(p2r_bake_global_variables_work);
ASYNC_WORK_DEF(p2r_bake_global_vmap_work);
ASYNC_WORK_DEF(p2r_bake_thread_variables_work);
ASYNC_WORK_DEF(p2r_bake_procedures_work);
ASYNC_WORK_DEF(p2r_bake_scopes_work);
ASYNC_WORK_DEF(p2r_bake_scope_vmap_work);
ASYNC_WORK_DEF(p2r_bake_file_paths_work);
ASYNC_WORK_DEF(p2r_bake_strings_work);

//- rjf: pass 3: idx-run-map-dependent debug info stream builds
ASYNC_WORK_DEF(p2r_bake_type_nodes_work);
ASYNC_WORK_DEF(p2r_bake_name_map_work);
ASYNC_WORK_DEF(p2r_bake_idx_runs_work);

////////////////////////////////
//~ rjf: Top-Level Initialization

void p2r_init();

////////////////////////////////
//~ rjf: Top-Level Baking Entry Point

P2R_Bake2Serialize *p2r_bake(Arena *arena, P2R_Convert2Bake *in);

////////////////////////////////
//~ rjf: Top-Level Compression Entry Point

P2R_Serialize2File *p2r_compress(Arena *arena, P2R_Serialize2File *in);

////////////////////////////////

B32 p2r_has_symbol_ref(String8 msf_data, String8List symbol_list, MSF_RawStreamTable *st);
B32 p2r_has_file_ref(String8 msf_data, String8List file_list, MSF_RawStreamTable *st);
B32 p2r_has_symbol_or_file_ref(String8 msf_data, String8List symbol_list, String8List file_list);

#endif // RDI_FROM_PDB_H

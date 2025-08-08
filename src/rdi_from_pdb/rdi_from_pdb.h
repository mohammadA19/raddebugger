// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_FROM_PDB_H
#define RDI_FROM_PDB_H

////////////////////////////////
//~ rjf: Conversion Stage Inputs/Outputs

struct P2R_ConvertParams
{
    StringView input_pdb_name;
    StringView input_pdb_data;
    StringView input_exe_name;
    StringView input_exe_data;
    RDIM_SubsetFlags subset_flags;
    B32 deterministic;
};

////////////////////////////////
//~ rjf: Initial PDB Information Extraction & Conversion Preparation Task Types

//- rjf: tpi hash parsing

struct P2R_TPIHashParseIn
{
    PDB_Strtbl *strtbl;
    PDB_TpiParsed *tpi;
    StringView hash_data;
    StringView aux_data;
};

//- rjf: tpi leaves parsing

struct P2R_TPILeafParseIn
{
    StringView leaf_data;
    CV_TypeId itype_first;
};

//- rjf: exe hashing

struct P2R_EXEHashIn
{
    StringView exe_data;
};

//- rjf: symbol stream parsing

struct P2R_SymbolStreamParseIn
{
    StringView data;
};

//- rjf: c13 line info stream parsing

struct P2R_C13StreamParseIn
{
    StringView data;
    StringView strtbl;
    COFF_SectionHeaderArray coff_sections;
};

//- rjf: comp unit parsing

struct P2R_CompUnitParseIn
{
    StringView data;
};

//- rjf: comp unit contribution table parsing

struct P2R_CompUnitContributionsParseIn
{
    StringView data;
    COFF_SectionHeaderArray coff_sections;
};

//- rjf: comp unit contribution table bucketing by unit

struct P2R_CompUnitContributionsBucketIn
{
    U64 comp_unit_count;
    PDB_CompUnitContributionArray contributions;
};

struct P2R_CompUnitContributionsBucketOut
{
    RDIM_Rng1U64ChunkList *unit_ranges;
};

////////////////////////////////
//~ rjf: Conversion Data Structure & Task Types

//- rjf: link name map (voff -> string)

struct P2R_LinkNameNode
{
    P2R_LinkNameNode *next;
    U64 voff;
    StringView name;
};

struct P2R_LinkNameMap
{
    P2R_LinkNameNode **buckets;
    U64 buckets_count;
    U64 bucket_collision_count;
    U64 link_name_count;
};

//- rjf: normalized file path -> source file map

struct P2R_SrcFileNode
{
    P2R_SrcFileNode *next;
    RDIM_SrcFile *src_file;
};

struct P2R_SrcFileMap
{
    P2R_SrcFileNode **slots;
    U64 slots_count;
};

//- rjf: per-unit source files conversion tasks

struct P2R_GatherUnitSrcFilesIn
{
    PDB_Strtbl *pdb_strtbl;
    COFF_SectionHeaderArray coff_sections;
    PDB_CompUnit *comp_unit;
    CV_SymParsed *comp_unit_syms;
    CV_C13Parsed *comp_unit_c13s;
};

struct P2R_GatherUnitSrcFilesOut
{
    String8Array src_file_paths;
};

//- rjf: unit conversion tasks

struct P2R_UnitConvertIn
{
    U64 comp_unit_idx;
    PDB_Strtbl *pdb_strtbl;
    COFF_SectionHeaderArray coff_sections;
    PDB_CompUnit *comp_unit;
    RDIM_Rng1U64ChunkList comp_unit_ranges;
    CV_SymParsed *comp_unit_syms;
    CV_C13Parsed *comp_unit_c13s;
    P2R_SrcFileMap *src_file_map;
};

struct P2R_UnitConvertOut
{
    RDIM_UnitChunkList units;
    RDIM_LineTableChunkList line_tables;
    RDIM_LineTable *unit_first_inline_site_line_table;
};

//- rjf: src file sequence equipping task

struct P2R_SrcFileSeqEquipIn
{
    RDIM_SrcFileChunkList src_files;
    RDIM_LineTableChunkList line_tables;
};

//- rjf: link name map building tasks

struct P2R_LinkNameMapBuildIn
{
    CV_SymParsed *sym;
    COFF_SectionHeaderArray coff_sections;
    P2R_LinkNameMap *link_name_map;
};

//- rjf: type forward resolution map build

struct P2R_ITypeFwdMapFillIn
{
    PDB_TpiHashParsed *tpi_hash;
    CV_LeafParsed *tpi_leaf;
    CV_TypeId itype_first;
    CV_TypeId itype_opl;
    CV_TypeId *itype_fwd_map;
};

//- rjf: itype chain build

struct P2R_TypeIdChain
{
    P2R_TypeIdChain *next;
    CV_TypeId itype;
};

struct P2R_ITypeChainBuildIn
{
    CV_LeafParsed *tpi_leaf;
    CV_TypeId itype_first;
    CV_TypeId itype_opl;
    CV_TypeId *itype_fwd_map;
    P2R_TypeIdChain **itype_chains;
};

//- rjf: udt conversion

struct P2R_UDTConvertIn
{
    CV_LeafParsed *tpi_leaf;
    CV_TypeId itype_first;
    CV_TypeId itype_opl;
    CV_TypeId *itype_fwd_map;
    RDIM_Type **itype_type_ptrs;
};

//- rjf: symbol stream conversion

struct P2R_SymbolStreamConvertIn
{
    B32 parsing_global_stream;
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
};

struct P2R_SymbolStreamConvertOut
{
    RDIM_SymbolChunkList procedures;
    RDIM_SymbolChunkList global_variables;
    RDIM_SymbolChunkList thread_variables;
    RDIM_SymbolChunkList constants;
    RDIM_ScopeChunkList scopes;
    RDIM_InlineSiteChunkList inline_sites;
    RDIM_TypeChunkList typedefs;
};

////////////////////////////////
//~ rjf: Globals

global ASYNC_Root *p2r_async_root = 0;

////////////////////////////////
//~ rjf: Basic Helpers


////////////////////////////////
//~ rjf: COFF => RDI Canonical Conversions


////////////////////////////////
//~ rjf: CodeView => RDI Canonical Conversions


////////////////////////////////
//~ rjf: Location Info Building Helpers


////////////////////////////////
//~ rjf: Initial Parsing & Preparation Pass Tasks

ASYNC_WORK_DEF(p2r_exe_hash_work);
ASYNC_WORK_DEF(p2r_tpi_hash_parse_work);
ASYNC_WORK_DEF(p2r_tpi_leaf_work);
ASYNC_WORK_DEF(p2r_symbol_stream_parse_work);
ASYNC_WORK_DEF(p2r_c13_stream_parse_work);
ASYNC_WORK_DEF(p2r_comp_unit_parse_work);
ASYNC_WORK_DEF(p2r_comp_unit_contributions_parse_work);
ASYNC_WORK_DEF(p2r_comp_unit_contributions_bucket_work);

////////////////////////////////
//~ rjf: Unit Source File Gathering Tasks

ASYNC_WORK_DEF(p2r_gather_unit_src_file_work);

////////////////////////////////
//~ rjf: Unit Conversion Tasks

ASYNC_WORK_DEF(p2r_unit_convert_work);

////////////////////////////////
//~ rjf: Source File Sequence Equipping Task

ASYNC_WORK_DEF(p2r_src_file_seq_equip_work);

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


#endif // RDI_FROM_PDB_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Export Artifact Flags

enum P2R_ConvertFlags : U32
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

struct P2R_User2Convert
{
  String8 input_pdb_name;
  String8 input_pdb_data;
  String8 input_exe_name;
  String8 input_exe_data;
  String8 output_name;
  P2R_ConvertFlags flags;
  String8List errors;
}

struct P2R_Convert2Bake
{
  RDIM_BakeParams bake_params;
}

struct P2R_Bake2Serialize
{
  RDIM_BakeResults bake_results;
}

struct P2R_Serialize2File
{
  RDIM_SerializedSectionBundle bundle;
}

////////////////////////////////
//~ rjf: Initial PDB Information Extraction & Conversion Preparation Task Types

//- rjf: tpi hash parsing

struct P2R_TPIHashParseIn
{
  PDB_Strtbl* strtbl;
  PDB_TpiParsed* tpi;
  String8 hash_data;
  String8 aux_data;
}

//- rjf: tpi leaves parsing

struct P2R_TPILeafParseIn
{
  String8 leaf_data;
  CV_TypeId itype_first;
}

//- rjf: exe hashing

struct P2R_EXEHashIn
{
  String8 exe_data;
}

//- rjf: symbol stream parsing

struct P2R_SymbolStreamParseIn
{
  String8 data;
}

//- rjf: c13 line info stream parsing

struct P2R_C13StreamParseIn
{
  String8 data;
  String8 strtbl;
  COFF_SectionHeaderArray coff_sections;
}

//- rjf: comp unit parsing

struct P2R_CompUnitParseIn
{
  String8 data;
}

//- rjf: comp unit contribution table parsing

struct P2R_CompUnitContributionsParseIn
{
  String8 data;
  COFF_SectionHeaderArray coff_sections;
}

////////////////////////////////
//~ rjf: Conversion Data Structure & Task Types

//- rjf: link name map (voff -> string)

struct P2R_LinkNameNode
{
  P2R_LinkNameNode* next;
  U64 voff;
  String8 name;
}

struct P2R_LinkNameMap
{
  P2R_LinkNameNode** buckets;
  U64 buckets_count;
  U64 bucket_collision_count;
  U64 link_name_count;
}

//- rjf: normalized file path -> source file map

struct P2R_SrcFileNode
{
  P2R_SrcFileNode* next;
  RDIM_SrcFile* src_file;
}

struct P2R_SrcFileMap
{
  P2R_SrcFileNode** slots;
  U64 slots_count;
}

//- rjf: unit conversion tasks

struct P2R_UnitConvertIn
{
  PDB_Strtbl* pdb_strtbl;
  COFF_SectionHeaderArray coff_sections;
  PDB_CompUnitArray* comp_units;
  PDB_CompUnitContributionArray* comp_unit_contributions;
  CV_SymParsed** comp_unit_syms;
  CV_C13Parsed** comp_unit_c13s;
}

struct P2R_UnitConvertOut
{
  RDIM_UnitChunkList units;
  RDIM_SrcFileChunkList src_files;
  RDIM_LineTableChunkList line_tables;
  RDIM_LineTable** units_first_inline_site_line_tables;
}

//- rjf: link name map building tasks

struct P2R_LinkNameMapBuildIn
{
  CV_SymParsed* sym;
  COFF_SectionHeaderArray coff_sections;
  P2R_LinkNameMap* link_name_map;
}

//- rjf: type forward resolution map build

struct P2R_ITypeFwdMapFillIn
{
  PDB_TpiHashParsed* tpi_hash;
  CV_LeafParsed* tpi_leaf;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  CV_TypeId* itype_fwd_map;
}

//- rjf: itype chain build

struct P2R_TypeIdChain
{
  P2R_TypeIdChain* next;
  CV_TypeId itype;
}

struct P2R_ITypeChainBuildIn
{
  CV_LeafParsed* tpi_leaf;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  CV_TypeId* itype_fwd_map;
  P2R_TypeIdChain** itype_chains;
}

//- rjf: udt conversion

struct P2R_UDTConvertIn
{
  CV_LeafParsed* tpi_leaf;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  CV_TypeId* itype_fwd_map;
  RDIM_Type** itype_type_ptrs;
}

//- rjf: symbol stream conversion

struct P2R_SymbolStreamConvertIn
{
  B32 parsing_global_stream;
  RDI_Arch arch;
  COFF_SectionHeaderArray coff_sections;
  PDB_TpiHashParsed* tpi_hash;
  CV_LeafParsed* tpi_leaf;
  CV_LeafParsed* ipi_leaf;
  CV_SymParsed* sym;
  U64 sym_ranges_first;
  U64 sym_ranges_opl;
  CV_TypeId* itype_fwd_map;
  RDIM_Type** itype_type_ptrs;
  P2R_LinkNameMap* link_name_map;
  RDIM_LineTable* first_inline_site_line_table;
}

struct P2R_SymbolStreamConvertOut
{
  RDIM_SymbolChunkList procedures;
  RDIM_SymbolChunkList global_variables;
  RDIM_SymbolChunkList thread_variables;
  RDIM_ScopeChunkList scopes;
  RDIM_InlineSiteChunkList inline_sites;
  RDIM_TypeChunkList typedefs;
}

////////////////////////////////
//~ rjf: Top-Level State

struct P2R_State
{
  Arena* arena;
  U64 work_thread_arenas_count;
  Arena** work_thread_arenas;
}

////////////////////////////////
//~ rjf: Globals

global P2R_State *p2r_state = 0;

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



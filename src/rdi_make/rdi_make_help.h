// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDIM_MAKE_HELP
#define RDIM_MAKE_HELP

//- rjf: line table baking task types

struct RDIM_BakeLineTablesIn
{
  RDIM_LineTableChunkList* line_tables;
}

//- rjf: string map baking task types

struct RDIM_BakeSrcFilesStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_SrcFileChunkList* list;
}

struct RDIM_BakeUnitsStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_UnitChunkList* list;
}

struct RDIM_BakeUDTsStringsInNode
{
  RDIM_BakeUDTsStringsInNode* next;
  RDIM_UDT* v;
  RDI_U64 count;
}

struct RDIM_BakeTypesStringsInNode
{
  RDIM_BakeTypesStringsInNode* next;
  RDIM_Type* v;
  RDI_U64 count;
}

struct RDIM_BakeTypesStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_BakeTypesStringsInNode* first;
  RDIM_BakeTypesStringsInNode* last;
}

struct RDIM_BakeUDTsStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_BakeUDTsStringsInNode* first;
  RDIM_BakeUDTsStringsInNode* last;
}

struct RDIM_BakeSymbolsStringsInNode
{
  RDIM_BakeSymbolsStringsInNode* next;
  RDIM_Symbol* v;
  RDI_U64 count;
}

struct RDIM_BakeSymbolsStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_BakeSymbolsStringsInNode* first;
  RDIM_BakeSymbolsStringsInNode* last;
}

struct RDIM_BakeInlineSiteStringsInNode
{
  RDIM_BakeInlineSiteStringsInNode* next;
  RDIM_InlineSite* v;
  RDI_U64 count;
}

struct RDIM_BakeInlineSiteStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_BakeInlineSiteStringsInNode* first;
  RDIM_BakeInlineSiteStringsInNode* last;
}

struct RDIM_BakeScopesStringsInNode
{
  RDIM_BakeScopesStringsInNode* next;
  RDIM_Scope* v;
  RDI_U64 count;
}

struct RDIM_BakeScopesStringsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** maps;
  RDIM_BakeScopesStringsInNode* first;
  RDIM_BakeScopesStringsInNode* last;
}

//- rjf: OLD string map baking types

struct RDIM_BuildBakeStringMapIn
{
  RDIM_BakePathTree* path_tree;
  RDIM_BakeParams* params;
}

struct RDIM_BuildBakeNameMapIn
{
  RDI_NameMapKind k;
  RDI_U64* type_indices;
  RDIM_BakeParams* params;
}

//- rjf: string map joining task types

struct RDIM_JoinBakeStringMapSlotsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose** src_maps;
  U64 src_maps_count;
  RDIM_BakeStringMapLoose* dst_map;
  Rng1U64 slot_idx_range;
}

//- rjf: string map sorting task types

struct RDIM_SortBakeStringMapSlotsIn
{
  RDIM_BakeStringMapTopology* top;
  RDIM_BakeStringMapLoose* src_map;
  RDIM_BakeStringMapLoose* dst_map;
  U64 slot_idx;
  U64 slot_count;
}

//- rjf: debug info baking task types

struct RDIM_BakeUnitsIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_BakePathTree* path_tree;
  RDIM_UnitChunkList* units;
}

struct RDIM_BakeUnitVMapIn
{
  RDIM_UnitChunkList* units;
}

struct RDIM_BakeSrcFilesIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_BakePathTree* path_tree;
  RDIM_SrcFileChunkList* src_files;
}

struct RDIM_BakeUDTsIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_UDTChunkList* udts;
  RDI_U64* type_indices;
}

struct RDIM_BakeGlobalVariablesIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_SymbolChunkList* global_variables;
  RDI_U64* type_indices;
}

struct RDIM_BakeGlobalVMapIn
{
  RDIM_SymbolChunkList* global_variables;
}

struct RDIM_BakeThreadVariablesIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_SymbolChunkList* thread_variables;
  RDI_U64* type_indices;
}

struct RDIM_BakeProceduresIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_SymbolChunkList* procedures;
  RDI_U64* type_indices;
  RDIM_String8List* location_blocks;
  RDIM_String8List* location_data_blobs;
}

struct RDIM_BakeScopesIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_ScopeChunkList* scopes;
  RDI_U64* type_indices;
  RDIM_String8List* location_blocks;
  RDIM_String8List* location_data_blobs;
}

struct RDIM_BakeScopeVMapIn
{
  RDIM_ScopeChunkList* scopes;
}

struct RDIM_BakeInlineSitesIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_InlineSiteChunkList* inline_sites;
  RDI_U64* type_indices;
}

struct RDIM_BakeFilePathsIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_BakePathTree* path_tree;
}

struct RDIM_BakeStringsIn
{
  RDIM_BakeStringMapTight* strings;
}

struct RDIM_BakeTypeNodesIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_BakeIdxRunMap* idx_runs;
  RDIM_TypeChunkList* types;
  RDI_U64* type_indices;
}

struct RDIM_BakeNameMapIn
{
  RDIM_BakeStringMapTight* strings;
  RDIM_BakeIdxRunMap* idx_runs;
  RDIM_BakeNameMap* map;
  RDI_NameMapKind kind;
}

struct RDIM_BakeIdxRunsIn
{
  RDIM_BakeIdxRunMap* idx_runs;
}

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

struct RDIM_HelpState
{
  Arena* arena;
  U64 work_thread_arenas_count;
  Arena** work_thread_arenas;
}

////////////////////////////////

global RDIM_HelpState *rdim_help_state = 0;

////////////////////////////////


ELP

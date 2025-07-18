// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_MAKE_LOCAL_H
#define RDI_MAKE_LOCAL_H

//- rjf: base layer memory ops
#define RDIM_MEMSET_OVERRIDE
#define RDIM_MEMCPY_OVERRIDE
#define rdim_memset MemorySet
#define rdim_memcpy MemoryCopy

//- rjf: base layer string overrides
#define RDI_STRING8_OVERRIDE
#define RDIM_String8            String8
#define RDIM_String8_BaseMember str
#define RDIM_String8_SizeMember size
#define RDI_STRING8LIST_OVERRIDE
#define RDIM_String8Node                 String8Node
#define RDIM_String8Node_NextPtrMember   next
#define RDIM_String8Node_StringMember    string
#define RDIM_String8List                 String8List
#define RDIM_String8List_FirstMember     first
#define RDIM_String8List_LastMember      last
#define RDIM_String8List_NodeCountMember node_count
#define RDIM_String8List_TotalSizeMember total_size

//- rjf: base layer arena overrides
#define RDIM_ARENA_OVERRIDE
#define RDIM_Arena Arena
#define rdim_arena_alloc     arena_alloc
#define rdim_arena_release   arena_release
#define rdim_arena_pos       arena_pos
#define rdim_arena_push      arena_push
#define rdim_arena_pop_to    arena_pop_to

//- rjf: base layer scratch arena overrides
#define RDIM_SCRATCH_OVERRIDE
#define RDIM_Temp Temp
#define rdim_temp_arena(t)   ((t).arena)
#define rdim_scratch_begin   scratch_begin
#define rdim_scratch_end     scratch_end

//- rjf: base layer profiling markup overrides
#define RDIM_ProfBegin(...) ProfBeginDynamic(__VA_ARGS__)
#define RDIM_ProfEnd(...) ProfEnd()

//- rjf: main library
#include "lib_rdi_make/rdi_make.h"

//- rjf: line table baking task types

struct RDIM_BakeLineTablesIn
{
  RDIM_LineTableChunkList *line_tables;
};

//- rjf: string map baking task types

struct RDIM_BakeSrcFilesStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_SrcFileChunkList *list;
};

struct RDIM_BakeUnitsStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_UnitChunkList *list;
};

struct RDIM_BakeUDTsStringsInNode
{
  RDIM_BakeUDTsStringsInNode *next;
  RDIM_UDT *v;
  RDI_uint64 count;
};

struct RDIM_BakeTypesStringsInNode
{
  RDIM_BakeTypesStringsInNode *next;
  RDIM_Type *v;
  RDI_uint64 count;
};

struct RDIM_BakeTypesStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_BakeTypesStringsInNode *first;
  RDIM_BakeTypesStringsInNode *last;
};

struct RDIM_BakeUDTsStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_BakeUDTsStringsInNode *first;
  RDIM_BakeUDTsStringsInNode *last;
};

struct RDIM_BakeSymbolsStringsInNode
{
  RDIM_BakeSymbolsStringsInNode *next;
  RDIM_Symbol *v;
  RDI_uint64 count;
};

struct RDIM_BakeSymbolsStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_BakeSymbolsStringsInNode *first;
  RDIM_BakeSymbolsStringsInNode *last;
};

struct RDIM_BakeInlineSiteStringsInNode
{
  RDIM_BakeInlineSiteStringsInNode *next;
  RDIM_InlineSite *v;
  RDI_uint64 count;
};

struct RDIM_BakeInlineSiteStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_BakeInlineSiteStringsInNode *first;
  RDIM_BakeInlineSiteStringsInNode *last;
};

struct RDIM_BakeScopesStringsInNode
{
  RDIM_BakeScopesStringsInNode *next;
  RDIM_Scope *v;
  RDI_uint64 count;
};

struct RDIM_BakeScopesStringsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **maps;
  RDIM_BakeScopesStringsInNode *first;
  RDIM_BakeScopesStringsInNode *last;
};

//- rjf: OLD string map baking types

struct RDIM_BuildBakeStringMapIn
{
  RDIM_BakePathTree *path_tree;
  RDIM_BakeParams *params;
};

struct RDIM_BuildBakeNameMapIn
{
  RDI_NameMapKind k;
  RDIM_BakeParams *params;
};

//- rjf: string map joining task types

struct RDIM_JoinBakeStringMapSlotsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose **src_maps;
  uint64 src_maps_count;
  RDIM_BakeStringMapLoose *dst_map;
  Rng1uint64 slot_idx_range;
};

//- rjf: string map sorting task types

struct RDIM_SortBakeStringMapSlotsIn
{
  RDIM_BakeStringMapTopology *top;
  RDIM_BakeStringMapLoose *src_map;
  RDIM_BakeStringMapLoose *dst_map;
  uint64 slot_idx;
  uint64 slot_count;
};

//- rjf: debug info baking task types

struct RDIM_BakeUnitsIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakePathTree *path_tree;
  RDIM_UnitChunkList *units;
};

struct RDIM_BakeUnitVMapIn
{
  RDIM_UnitChunkList *units;
};

struct RDIM_BakeSrcFilesIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakePathTree *path_tree;
  RDIM_SrcFileChunkList *src_files;
};

struct RDIM_BakeUDTsIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_UDTChunkList *udts;
};

struct RDIM_BakeGlobalVariablesIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *global_variables;
};

struct RDIM_BakeConstantsIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *constants;
};

struct RDIM_BakeGlobalVMapIn
{
  RDIM_SymbolChunkList *global_variables;
};

struct RDIM_BakeThreadVariablesIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *thread_variables;
};

struct RDIM_BakeProceduresIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_SymbolChunkList *procedures;
  RDIM_String8List *location_blocks;
  RDIM_String8List *location_data_blobs;
};

struct RDIM_BakeScopesIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_ScopeChunkList *scopes;
  RDIM_String8List *location_blocks;
  RDIM_String8List *location_data_blobs;
};

struct RDIM_BakeScopeVMapIn
{
  RDIM_ScopeChunkList *scopes;
};

struct RDIM_BakeInlineSitesIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_InlineSiteChunkList *inline_sites;
};

struct RDIM_BakeFilePathsIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakePathTree *path_tree;
};

struct RDIM_BakeStringsIn
{
  RDIM_BakeStringMapTight *strings;
};

struct RDIM_BakeTypeNodesIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakeIdxRunMap *idx_runs;
  RDIM_TypeChunkList *types;
};

struct RDIM_BakeNameMapIn
{
  RDIM_BakeStringMapTight *strings;
  RDIM_BakeIdxRunMap *idx_runs;
  RDIM_BakeNameMap *map;
  RDI_NameMapKind kind;
};

struct RDIM_BakeIdxRunsIn
{
  RDIM_BakeIdxRunMap *idx_runs;
};

////////////////////////////////

internal RDIM_DataModel rdim_infer_data_model(OperatingSystem os, RDI_Arch arch);

////////////////////////////////
//~ rjf: Baking Stage Tasks

//- rjf: unsorted bake string map building
ASYNC_WORK_DEF(rdim_bake_src_files_strings_work);
ASYNC_WORK_DEF(rdim_bake_units_strings_work);
ASYNC_WORK_DEF(rdim_bake_types_strings_work);
ASYNC_WORK_DEF(rdim_bake_udts_strings_work);
ASYNC_WORK_DEF(rdim_bake_symbols_strings_work);
ASYNC_WORK_DEF(rdim_bake_scopes_strings_work);
ASYNC_WORK_DEF(rdim_bake_line_tables_work);

//- rjf: bake string map joining
ASYNC_WORK_DEF(rdim_bake_string_map_join_work);

//- rjf: bake string map sorting
ASYNC_WORK_DEF(rdim_bake_string_map_sort_work);

//- rjf: pass 1: interner/deduper map builds
ASYNC_WORK_DEF(rdim_build_bake_name_map_work);

//- rjf: pass 2: string-map-dependent debug info stream builds
ASYNC_WORK_DEF(rdim_bake_units_work);
ASYNC_WORK_DEF(rdim_bake_unit_vmap_work);
ASYNC_WORK_DEF(rdim_bake_src_files_work);
ASYNC_WORK_DEF(rdim_bake_udts_work);
ASYNC_WORK_DEF(rdim_bake_global_variables_work);
ASYNC_WORK_DEF(rdim_bake_global_vmap_work);
ASYNC_WORK_DEF(rdim_bake_thread_variables_work);
ASYNC_WORK_DEF(rdim_bake_constants_work);
ASYNC_WORK_DEF(rdim_bake_procedures_work);
ASYNC_WORK_DEF(rdim_bake_scopes_work);
ASYNC_WORK_DEF(rdim_bake_scope_vmap_work);
ASYNC_WORK_DEF(rdim_bake_file_paths_work);
ASYNC_WORK_DEF(rdim_bake_strings_work);

//- rjf: pass 3: idx-run-map-dependent debug info stream builds
ASYNC_WORK_DEF(rdim_bake_type_nodes_work);
ASYNC_WORK_DEF(rdim_bake_name_map_work);
ASYNC_WORK_DEF(rdim_bake_idx_runs_work);

////////////////////////////////
//~ rjf: Globals

global ASYNC_Root *rdim_local_async_root = 0;

////////////////////////////////

internal RDIM_DataModel    rdim_infer_data_model(OperatingSystem os, RDI_Arch arch);
internal RDIM_TopLevelInfo rdim_make_top_level_info(String8 image_name, Arch arch, uint64 exe_hash, RDIM_BinarySectionList sections);

////////////////////////////////

internal RDIM_BakeResults             rdim_bake(Arena *arena, ASYNC_Root *async_root, RDIM_BakeParams *in);
internal RDIM_SerializedSectionBundle rdim_compress(Arena *arena, RDIM_SerializedSectionBundle *in);

#endif // RDI_MAKE_LOCAL_H

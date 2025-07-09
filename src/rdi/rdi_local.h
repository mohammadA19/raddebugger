// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_FORMAT_LOCAL_H
#define RDI_FORMAT_LOCAL_H

#include "lib_rdi/rdi.h"
#include "lib_rdi/rdi_parse.h"

////////////////////////////////
//~ rjf: RDI Dumping Flags

#define RDI_DumpSubset_XList \
X(DataSections,        data_sections,               "DATA SECTIONS")\
X(TopLevelInfo,        top_level_info,              "TOP LEVEL INFO")\
X(BinarySections,      binary_sections,             "BINARY SECTIONS")\
X(FilePaths,           file_paths,                  "FILE PATHS")\
X(SourceFiles,         source_files,                "SOURCE FILES")\
X(LineTables,          line_tables,                 "LINE TABLES")\
X(SourceLineMaps,      source_line_maps,            "SOURCE LINE MAPS")\
X(Units,               units,                       "UNITS")\
X(UnitVMap,            unit_vmap,                   "UNIT VMAP")\
X(TypeNodes,           type_nodes,                  "TYPE NODES")\
X(UserDefinedTypes,    user_defined_types,          "USER DEFINED TYPES")\
X(GlobalVariables,     global_variables,            "GLOBAL VARIABLES")\
X(GlobalVariablesVMap, global_variables_vmap,       "GLOBAL VARIABLE VMAP")\
X(ThreadVariables,     thread_variables,            "THREAD VARIABLES")\
X(Constants,           constants,                   "CONSTANTS")\
X(Procedures,          procedures,                  "PROCEDURES")\
X(Scopes,              scopes,                      "SCOPES")\
X(ScopeVMap,           scope_vmap,                  "SCOPE VMAP")\
X(InlineSites,         inline_sites,                "INLINE SITES")\
X(NameMaps,            name_maps,                   "NAME MAPS")\
X(Strings,             strings,                     "STRINGS")\

typedef enum RDI_DumpSubset
{
#define X(name, name_lower, title) RDI_DumpSubset_##name,
  RDI_DumpSubset_XList
#undef X
}
RDI_DumpSubset;

typedef U32 RDI_DumpSubsetFlags;
enum
{
#define X(name, name_lower, title) RDI_DumpSubsetFlag_##name = (1<<RDI_DumpSubset_##name),
  RDI_DumpSubset_XList
#undef X
  RDI_DumpSubsetFlag_All = 0xffffffffu,
};

@(rodata) global String8 rdi_name_lowercase_from_dump_subset_table[] =
{
#define X(name, name_lower, title) str8_lit_comp(#name_lower),
  RDI_DumpSubset_XList
#undef X
};

@(rodata) global String8 rdi_name_title_from_dump_subset_table[] =
{
#define X(name, name_lower, title) str8_lit_comp(title),
  RDI_DumpSubset_XList
#undef X
};

////////////////////////////////
//~ rjf: Lookup Helpers

////////////////////////////////
//~ rjf: String <=> Enum

#if 0 // TODO(rjf): conflicts with RDI...
#endif

////////////////////////////////
//~ rjf: RDI Dumping

#endif // RDI_FORMAT_LOCAL_H

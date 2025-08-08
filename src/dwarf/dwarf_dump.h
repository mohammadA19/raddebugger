// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DWARF_DUMP_H
#define DWARF_DUMP_H

////////////////////////////////
//~ rjf: Dump Subset Types

#define DW_DumpSubset_XList \
X(DebugInfo,           debug_info,          "DEBUG INFO")\
X(DebugAbbrev,         debug_abbrev,        "DEBUG ABBREV")\
X(DebugLine,           debug_line,          "DEBUG LINE")\
X(DebugStr,            debug_str,           "DEBUG STR")\
X(DebugLoc,            debug_loc,           "DEBUG LOC")\
X(DebugRanges,         debug_ranges,        "DEBUG RANGES")\
X(DebugARanges,        debug_aranges,       "DEBUG ARANGES")\
X(DebugAddr,           debug_addr,          "DEBUG ADDR")\
X(DebugLocLists,       debug_loclists,      "DEBUG LOCLISTS")\
X(DebugRngLists,       debug_rnglists,      "DEBUG RNGLISTS")\
X(DebugPubNames,       debug_pubnames,      "DEBUG PUBNAMES")\
X(DebugPubTypes,       debug_pubtypes,      "DEBUG PUBTYPES")\
X(DebugLineStr,        debug_linestr,       "DEBUG LINESTR")\
X(DebugStrOffsets,     debug_stroff,        "DEBUG STROFF")\

enum DW_DumpSubset
{
#define X(name, name_lower, title) DW_DumpSubset_##name,
  DW_DumpSubset_XList
#undef X
}
DW_DumpSubset;

enum DW_DumpSubsetFlags : U32
{
#define X(name, name_lower, title) DW_DumpSubsetFlag_##name = (1<<DW_DumpSubset_##name),
  DW_DumpSubset_XList
#undef X
  DW_DumpSubsetFlag_All = 0xffffffffu,
};

read_only global String8 dw_name_lowercase_from_dump_subset_table[] =
{
#define X(name, name_lower, title) str8_lit_comp(#name_lower),
  DW_DumpSubset_XList
#undef X
};

read_only global String8 dw_name_title_from_dump_subset_table[] =
{
#define X(name, name_lower, title) str8_lit_comp(title),
  DW_DumpSubset_XList
#undef X
};

////////////////////////////////
//~ rjf: Stringification Helpers


#if 0

#endif

////////////////////////////////
//~ rjf: Dump Entry Point


#endif // DWARF_DUMP_H

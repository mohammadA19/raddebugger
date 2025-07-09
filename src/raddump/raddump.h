// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




#define RD_INDENT_WIDTH 2
#define RD_INDENT_MAX   4096

#define rd_printf(f, ...) str8_list_pushf(arena, out, "%S" f, indent, __VA_ARGS__)
#define rd_newline()      str8_list_pushf(arena, out, "");
#define rd_errorf(f, ...)   rd_stderr("ERROR: "f, __VA_ARGS__)
#define rd_warningf(f, ...) rd_stderr("WARNING: "f, __VA_ARGS__)
#define rd_indent()       do { if (indent.size + RD_INDENT_WIDTH <= RD_INDENT_MAX) { indent.size += RD_INDENT_WIDTH; } else { assert(!"indent overflow");   } } while (0)
#define rd_unindent()     do { if (indent.size >= RD_INDENT_WIDTH)                 { indent.size -= RD_INDENT_WIDTH; } else { assert(!"unbalanced indent"); } } while (0)

typedef u64 RD_Option;

#define RD_Option_Help       (1ull << 0)
#define RD_Option_Version    (1ull << 1)
#define RD_Option_Headers    (1ull << 2)
#define RD_Option_Sections   (1ull << 3)
#define RD_Option_Debug      (1ull << 4)
#define RD_Option_Imports    (1ull << 5)
#define RD_Option_Exports    (1ull << 6)
#define RD_Option_Disasm     (1ull << 7)
#define RD_Option_Rawdata    (1ull << 8)
#define RD_Option_Tls        (1ull << 9)
#define RD_Option_Codeview   (1ull << 10)
#define RD_Option_Symbols    (1ull << 11)
#define RD_Option_Relocs     (1ull << 12)
#define RD_Option_Exceptions (1ull << 13)
#define RD_Option_LoadConfig (1ull << 14)
#define RD_Option_Resources  (1ull << 15)
#define RD_Option_LongNames  (1ull << 16)
// DWARF
#define RD_Option_DebugInfo       (1ull << 17)
#define RD_Option_DebugAbbrev     (1ull << 18)
#define RD_Option_DebugLine       (1ull << 19)
#define RD_Option_DebugStr        (1ull << 20)
#define RD_Option_DebugLoc        (1ull << 21)
#define RD_Option_DebugRanges     (1ull << 22)
#define RD_Option_DebugARanges    (1ull << 23)
#define RD_Option_DebugAddr       (1ull << 24)
#define RD_Option_DebugLocLists   (1ull << 25)
#define RD_Option_DebugRngLists   (1ull << 26)
#define RD_Option_DebugPubNames   (1ull << 27)
#define RD_Option_DebugPubTypes   (1ull << 28)
#define RD_Option_DebugLineStr    (1ull << 29)
#define RD_Option_DebugStrOffsets (1ull << 30)
#define RD_Option_Dwarf                       \
(RD_Option_DebugInfo     | \
RD_Option_DebugAbbrev   | \
RD_Option_DebugLine     | \
RD_Option_DebugStr      | \
RD_Option_DebugLoc      | \
RD_Option_DebugRanges   | \
RD_Option_DebugARanges  | \
RD_Option_DebugAddr     | \
RD_Option_DebugLocLists | \
RD_Option_DebugRngLists | \
RD_Option_DebugPubNames | \
RD_Option_DebugPubTypes | \
RD_Option_DebugLineStr  | \
RD_Option_DebugStrOffsets)
#define RD_Option_RelaxDwarfParser (1ull << 31ull)
// RDI
#define RD_Option_NoRdi               (1ull << 32ull)
#define RD_Option_RdiDataSections     (1ull << 33ull)
#define RD_Option_RdiTopLevelInfo     (1ull << 34ull)
#define RD_Option_RdiBinarySections   (1ull << 35ull)
#define RD_Option_RdiFilePaths        (1ull << 36ull)
#define RD_Option_RdiSourceFiles      (1ull << 37ull)
#define RD_Option_RdiLineTables       (1ull << 38ull)
#define RD_Option_RdiSourceLineMaps   (1ull << 39ull)
#define RD_Option_RdiUnits            (1ull << 40ull)
#define RD_Option_RdiUnitVMap         (1ull << 41ull)
#define RD_Option_RdiTypeNodes        (1ull << 42ull)
#define RD_Option_RdiUserDefinedTypes (1ull << 43ull)
#define RD_Option_RdiGlobalVars       (1ull << 44ull)
#define RD_Option_RdiGlobalVarsVMap   (1ull << 45ull)
#define RD_Option_RdiThreadVars       (1ull << 46ull)
#define RD_Option_RdiConstants        (1ull << 47ull)
#define RD_Option_RdiProcedures       (1ull << 48ull)
#define RD_Option_RdiScopes           (1ull << 49ull)
#define RD_Option_RdiScopeVMap        (1ull << 50ull)
#define RD_Option_RdiInlineSites      (1ull << 51ull)
#define RD_Option_RdiNameMaps         (1ull << 52ull)
#define RD_Option_RdiStrings          (1ull << 53ull)
#define RD_Option_RdiAll              (RD_Option_RdiDataSections     | \
RD_Option_RdiTopLevelInfo     | \
RD_Option_RdiBinarySections   | \
RD_Option_RdiFilePaths        | \
RD_Option_RdiSourceFiles      | \
RD_Option_RdiLineTables       | \
RD_Option_RdiSourceLineMaps   | \
RD_Option_RdiUnits            | \
RD_Option_RdiUnitVMap         | \
RD_Option_RdiTypeNodes        | \
RD_Option_RdiUserDefinedTypes | \
RD_Option_RdiGlobalVars       | \
RD_Option_RdiGlobalVarsVMap   | \
RD_Option_RdiThreadVars       | \
RD_Option_RdiConstants        | \
RD_Option_RdiProcedures       | \
RD_Option_RdiScopes           | \
RD_Option_RdiScopeVMap        | \
RD_Option_RdiInlineSites      | \
RD_Option_RdiNameMaps         | \
RD_Option_RdiStrings)

typedef struct RD_Marker
{
  u64     off;
  String8 string;
} RD_Marker;

typedef struct RD_MarkerArray
{
  u64        count;
  RD_Marker *v;
} RD_MarkerArray;

typedef struct MarkerNode
{
  struct MarkerNode *next;
  RD_Marker          v;
} RD_MarkerNode;

typedef struct RD_MarkerList
{
  u64            count;
  RD_MarkerNode *first;
  RD_MarkerNode *last;
} RD_MarkerList;

typedef struct RD_DisasmResult
{
  String8 text;
  u64     size;
} RD_DisasmResult;

typedef struct RD_Section
{
  String8 name;
  String8 raw_data;
} RD_Section;

typedef struct RD_SectionArray
{
  u64         count;
  RD_Section *v;
} RD_SectionArray;

typedef struct RD_Line
{
  String8 file_path;
  u32     line_num;
} RD_Line;

////////////////////////////////

// raddump

// Markers

// Sections

// Disasm

// Raw Data

// CodeView

// MSVC CRT

// PE




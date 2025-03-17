// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADDUMP_H
#define RADDUMP_H

#define RD_INDENT_WIDTH 2
#define RD_INDENT_MAX   4096

#define rd_printf(f, ...) str8_list_pushf(arena, out, "%S" f, indent, __VA_ARGS__)
#define rd_newline()      str8_list_pushf(arena, out, "");
#define rd_errorf(f, ...)   rd_stderr("ERROR: "f, __VA_ARGS__)
#define rd_warningf(f, ...) rd_stderr("WARNING: "f, __VA_ARGS__)
#define rd_indent()       do { if (indent.size + RD_INDENT_WIDTH <= RD_INDENT_MAX) { indent.size += RD_INDENT_WIDTH; } else { Assert(!"indent overflow");   } } while (0)
#define rd_unindent()     do { if (indent.size >= RD_INDENT_WIDTH)                 { indent.size -= RD_INDENT_WIDTH; } else { Assert(!"unbalanced indent"); } } while (0)

typedef uint64 RD_Option;

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
#define RD_Option_NoRdi            (1ull << 32ull)
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
#define RD_Option_RdiProcedures       (1ull << 48ull)
#define RD_Option_RdiScopes           (1ull << 49ull)
#define RD_Option_RdiScopeVMap        (1ull << 50ull)
#define RD_Option_RdiInlineSites      (1ull << 51ull)
#define RD_Option_RdiNameMaps         (1ull << 52ull)
#define RD_Option_RdiStrings          (1ull << 53ull)

struct RD_Marker
{
  uint64     off;
  StringView str;
}

struct RD_MarkerArray
{
  uint64        count;
  RD_Marker* v;
}

struct MarkerNode
{
  struct MarkerNode* next;
  RD_Marker          v;
}

struct RD_MarkerList
{
  uint64            count;
  RD_MarkerNode* first;
  RD_MarkerNode* last;
}

struct RD_DisasmResult
{
  StringView text;
  uint64     size;
}

struct RD_Section
{
  StringView name;
  StringView raw_data;
}

struct RD_SectionArray
{
  uint64         count;
  RD_Section* v;
}

struct RD_Line
{
  StringView file_path;
  uint32     line_num;
}

////////////////////////////////

// raddump

B32 rd_is_pe (StringView raw_data);
B32 rd_is_rdi(StringView raw_data);

StringView rd_string_from_flags(Arena* arena, String8List list, uint64 remaining_flags);

void rd_format_preamble(Arena* arena, String8List* out, StringView indent, StringView input_path, StringView raw_data);

// Markers

RD_MarkerArray * rd_section_markers_from_coff_symbol_table(Arena* arena, StringView raw_data, uint64 string_table_off, uint64 section_count, COFF_Symbol32Array symbols);

// Sections

RD_SectionArray rd_sections_from_coff_section_table(Arena* arnea, StringView raw_image, uint64 string_table_off, uint64 section_count, COFF_SectionHeader* sections);

// Disasm

RD_DisasmResult rd_disasm_next_instruction(Arena* arena, Arch arch, uint64 addr, StringView raw_code);
void            rd_print_disasm           (Arena* arena, String8List* out, StringView indent, Arch arch, uint64 image_base, uint64 sect_off, uint64 marker_count, RD_Marker* markers, StringView raw_code);

// Raw Data

StringView rd_format_hex_array(Arena* arena, uint8* ptr, uint64 size);
void    rd_print_raw_data  (Arena* arena, String8List* out, StringView indent, uint64 bytes_per_row, uint64 marker_count, RD_Marker* markers, StringView raw_data);

// RDI

StringView rdi_string_from_data_section_kind(Arena* arena, RDI_SectionKind v);
StringView rdi_string_from_arch             (Arena* arena, RDI_Arch        v);
StringView rdi_string_from_language         (Arena* arena, RDI_Language    v);
StringView rdi_string_from_local_kind       (Arena* arena, RDI_LocalKind   v);
StringView rdi_string_from_type_kind        (Arena* arena, RDI_TypeKind    v);
StringView rdi_string_from_member_kind      (Arena* arena, RDI_MemberKind  v);

StringView rdi_string_from_binary_section_flags(Arena* arena, RDI_BinarySectionFlags flags);
StringView rdi_string_from_type_modifier       (Arena* arena, RDI_TypeModifierFlags  flags);
StringView rdi_string_from_udt_flags           (Arena* arena, RDI_UDTFlags           flags);
StringView rdi_string_from_link_flags          (Arena* arena, RDI_LinkFlags          flags);

void rdi_print_data_sections  (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi);
void rdi_print_top_level_info (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_TopLevelInfo*   tli);
void rdi_print_binary_section (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_BinarySection*  bin_section);
void rdi_print_file_path      (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_FilePathNode*   file_path);
void rdi_print_source_file    (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_SourceFile*     source_file);
void rdi_print_line_table     (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_LineTable*      line_table);
void rdi_print_source_line_map(Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_SourceLineMap*  map);
void rdi_print_unit           (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_Unit*           unit);
void rdi_print_type_node      (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_TypeNode*       type);
void rdi_print_udt            (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_UDT*            udt);
void rdi_print_global_variable(Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_GlobalVariable* gvar);
void rdi_print_thread_variable(Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_ThreadVariable* tvar);
void rdi_print_procedure      (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_Procedure*      proc);
void rdi_print_scope          (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_Scope*          scope, RDI_Arch arch);
void rdi_print_inline_site    (Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, RDI_InlineSite*     inline_site);
void rdi_print_vmap_entry     (Arena* arena, String8List* out, StringView indent, RDI_VMapEntry* v);

// DWARF

String8List dw_string_list_from_expression  (Arena* arena, StringView raw_data, uint64 address_size, Arch arch, DW_Version ver, DW_Ext ext, B32 is_dwarf64);
StringView     dw_format_expression_single_line(Arena* arena, StringView raw_data, uint64 address_size, Arch arch, DW_Version ver, DW_Ext ext, B32 is_dwarf64);
StringView     dw_format_eh_ptr_enc            (Arena* arena, DW_EhPtrEnc enc);
void        dw_print_cfi_program            (Arena* arena, String8List* out, StringView indent, StringView raw_data, DW_CIEUnpacked* cie, DW_EhPtrCtx* ptr_ctx, Arch arch, DW_Version ver, DW_Ext ext, B32 is_dwarf64);

void dw_print_eh_frame         (Arena* arena, String8List* out, StringView indent, StringView raw_eh_frame, Arch arch, DW_Version ver, DW_Ext ext, DW_EhPtrCtx* ptr_ctx);
void dw_print_debug_info       (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections, Arch arch, ImageType image_type, B32 relaxed);
void dw_print_debug_abbrev     (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_line       (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections, B32 relaxed);
void dw_print_debug_str        (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_loc        (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections, Arch arch, ImageType image_type, B32 relaxed);
void dw_print_debug_ranges     (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections, Arch arch, ImageType image_type, B32 relaxed);
void dw_print_debug_aranges    (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_addr       (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_loclists   (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections, Rng1U64Array segment_vranges, Arch arch);
void dw_print_debug_rnglists   (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections, Rng1U64Array segment_vranges);
void dw_print_debug_pubnames   (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_pubtypes   (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_line_str   (Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);
void dw_print_debug_str_offsets(Arena* arena, String8List* out, StringView indent, DW_SectionArray* sections);

// CodeView

void cv_print_binary_annots  (Arena* arena, String8List* out, StringView indent, CV_Arch arch, StringView raw_data);
void cv_print_lvar_addr_range(Arena* arena, String8List* out, StringView indent, CV_LvarAddrRange range);
void cv_print_lvar_addr_gap  (Arena* arena, String8List* out, StringView indent, StringView raw_data);
void cv_print_lvar_attr      (Arena* arena, String8List* out, StringView indent, CV_LocalVarAttr attr);
void cv_print_symbol         (Arena* arena, String8List* out, StringView indent, CV_Arch arch, CV_TypeIndex min_itype, CV_SymKind type, StringView raw_symbol);
uint64  cv_print_leaf           (Arena* arena, String8List* out, StringView indent, CV_TypeIndex min_itype, CV_LeafKind kind, StringView raw_leaf);
void cv_print_debug_t        (Arena* arena, String8List* out, StringView indent, CV_DebugT debug_t);
void cv_print_symbols_c13    (Arena* arena, String8List* out, StringView indent, CV_Arch arch, StringView raw_data);
void cv_print_lines_c13      (Arena* arena, String8List* out, StringView indent, StringView raw_lines);
void cv_print_file_checksums (Arena* arena, String8List* out, StringView indent, StringView raw_chksums);
void cv_print_string_table   (Arena* arena, String8List* out, StringView indent, StringView raw_strtab);
void cv_print_inlinee_lines  (Arena* arena, String8List* out, StringView indent, StringView raw_data);
void cv_print_symbols_section(Arena* arena, String8List* out, StringView indent, CV_Arch arch, StringView raw_ss);

// COFF

void coff_print_archive_member_header(Arena* arena, String8List* out, StringView indent, COFF_ParsedArchiveMemberHeader header, StringView long_names);
void coff_print_seciton_table        (Arena* arena, String8List* out, StringView indent, StringView raw_data, uint64 string_table_off, COFF_Symbol32Array symbols, uint64 sect_count, COFF_SectionHeader* sect_headers);
void coff_disasm_sections            (Arena* arena, String8List* out, StringView indent, StringView raw_data, COFF_MachineType machine, uint64 image_base, B32 is_obj, RD_MarkerArray* section_markers, uint64 section_count, COFF_SectionHeader* sections);
void coff_raw_data_sections          (Arena* arena, String8List* out, StringView indent, StringView raw_data, B32 is_obj, RD_MarkerArray* section_markers, uint64 section_count, COFF_SectionHeader* sections);
void coff_print_relocs               (Arena* arena, String8List* out, StringView indent, StringView raw_data, uint64 string_table_off, COFF_MachineType machine, uint64 sect_count, COFF_SectionHeader* sect_headers, COFF_Symbol32Array symbols);
void coff_print_symbol_table         (Arena* arena, String8List* out, StringView indent, StringView raw_data, B32 is_big_obj, uint64 string_table_off, COFF_Symbol32Array symbols);
void coff_print_big_obj_header       (Arena* arena, String8List* out, StringView indent, COFF_BigObjHeader* header);
void coff_print_file_header          (Arena* arena, String8List* out, StringView indent, COFF_FileHeader* header);
void coff_print_import               (Arena* arena, String8List* out, StringView indent, COFF_ParsedArchiveImportHeader* header);
void coff_print_big_obj              (Arena* arena, String8List* out, StringView indent, StringView raw_data, RD_Option opts);
void coff_print_obj                  (Arena* arena, String8List* out, StringView indent, StringView raw_data, RD_Option opts);
void coff_print_archive              (Arena* arena, String8List* out, StringView indent, StringView raw_archive, RD_Option opts);

// MSVC CRT

void mscrt_print_eh_handler_type32(Arena* arena, String8List* out, StringView indent, RDI_Parsed* rdi, MSCRT_EhHandlerType32* handler);

// PE

void pe_print_data_directory_ranges(Arena* arena, String8List* out, StringView indent, uint64 count, PE_DataDirectory* dirs);
void pe_print_optional_header32    (Arena* arena, String8List* out, StringView indent, PE_OptionalHeader32* opt_header, PE_DataDirectory* dirs);
void pe_print_optional_header32plus(Arena* arena, String8List* out, StringView indent, PE_OptionalHeader32Plus* opt_header, PE_DataDirectory* dirs);
void pe_print_load_config32        (Arena* arena, String8List* out, StringView indent, PE_LoadConfig32* lc);
void pe_print_load_config64        (Arena* arena, String8List* out, StringView indent, PE_LoadConfig64* lc);
void pe_print_tls                  (Arena* arena, String8List* out, StringView indent, PE_ParsedTLS tls);
void pe_print_debug_diretory       (Arena* arena, String8List* out, StringView indent, StringView raw_data, StringView raw_dir);
void pe_print_export_table         (Arena* arena, String8List* out, StringView indent, PE_ParsedExportTable exptab);
void pe_print_static_import_table  (Arena* arena, String8List* out, StringView indent, uint64 image_base, PE_ParsedStaticImportTable imptab);
void pe_print_delay_import_table   (Arena* arena, String8List* out, StringView indent, uint64 image_base, PE_ParsedDelayImportTable imptab);
void pe_print_resources            (Arena* arena, String8List* out, StringView indent, PE_ResourceDir* root);
void pe_print_exceptions_x8664     (Arena* arena, String8List* out, StringView indent, uint64 section_count, COFF_SectionHeader* sections, StringView raw_data, Rng1U64 except_frange, RDI_Parsed* rdi);
void pe_print_exceptions           (Arena* arena, String8List* out, StringView indent, COFF_MachineType machine, uint64 section_count, COFF_SectionHeader* sections, StringView raw_data, Rng1U64 except_frange, RDI_Parsed* rdi);
void pe_print_base_relocs          (Arena* arena, String8List* out, StringView indent, COFF_MachineType machine, uint64 image_base, uint64 section_count, COFF_SectionHeader* sections, StringView raw_data, Rng1U64 base_reloc_franges, RDI_Parsed* rdi);
void pe_print                      (Arena* arena, String8List* out, StringView indent, StringView raw_data, RD_Option opts, RDI_Parsed* rdi);

#endif // RADDUMP_H


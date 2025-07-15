// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#if 0
static void coff_print_archive_member_header(Arena *arena, String8List *out, String8 indent, COFF_ParsedArchiveMemberHeader header, String8 long_names);
static void coff_print_section_table        (Arena *arena, String8List *out, String8 indent, String8 string_table, COFF_Symbol32Array symbols, U64 sect_count, COFF_SectionHeader *sect_headers);
static void coff_disasm_sections            (Arena *arena, String8List *out, String8 indent, String8 raw_data, COFF_MachineType machine, U64 image_base, B32 is_obj, RD_MarkerArray *section_markers, U64 section_count, COFF_SectionHeader *sections);
static void coff_raw_data_sections          (Arena *arena, String8List *out, String8 indent, String8 raw_data, B32 is_obj, RD_MarkerArray *section_markers, U64 section_count, COFF_SectionHeader *sections);
static void coff_print_relocs               (Arena *arena, String8List *out, String8 indent, String8 raw_data, String8 string_table, COFF_MachineType machine, U64 sect_count, COFF_SectionHeader *sect_headers, COFF_Symbol32Array symbols);
static void coff_print_symbol_table         (Arena *arena, String8List *out, String8 indent, String8 raw_data, B32 is_big_obj, String8 string_table, COFF_Symbol32Array symbols);
static void coff_print_big_obj_header       (Arena *arena, String8List *out, String8 indent, COFF_BigObjHeader *header);
static void coff_print_file_header          (Arena *arena, String8List *out, String8 indent, COFF_FileHeader *header);
static void coff_print_import               (Arena *arena, String8List *out, String8 indent, COFF_ParsedArchiveImportHeader *header);
static void coff_print_big_obj              (Arena *arena, String8List *out, String8 indent, String8 raw_data, RD_Option opts);
static void coff_print_obj                  (Arena *arena, String8List *out, String8 indent, String8 raw_data, RD_Option opts);
static void coff_print_archive              (Arena *arena, String8List *out, String8 indent, String8 raw_archive, RD_Option opts);
#endif


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_DUMP_H
#define RDI_DUMP_H

////////////////////////////////
//~ rjf: RADDBG Stringize Helper Types

typedef struct RDI_FilePathBundle RDI_FilePathBundle;
struct RDI_FilePathBundle
{
  RDI_FilePathNode *file_paths;
  ulong file_path_count;
}

typedef struct RDI_UDTMemberBundle RDI_UDTMemberBundle;
struct RDI_UDTMemberBundle
{
  RDI_Member *members;
  RDI_EnumMember *enum_members;
  uint member_count;
  uint enum_member_count;
}

typedef struct RDI_ScopeBundle RDI_ScopeBundle;
struct RDI_ScopeBundle
{
  RDI_Scope *scopes;
  ulong *scope_voffs;
  RDI_Local *locals;
  RDI_LocationBlock *location_blocks;
  byte *location_data;
  uint scope_count;
  uint scope_voff_count;
  uint local_count;
  uint location_block_count;
  uint location_data_size;
}

////////////////////////////////
//~ rjf: RDI Enum -> String Functions

internal String8 rdi_string_from_reg_code_x86(ulong reg_code);
internal String8 rdi_string_from_reg_code_x64(ulong reg_code);
internal String8 rdi_string_from_reg_code(RDI_Arch arch, ulong reg_code);
internal String8 rdi_string_from_data_section_kind(RDI_SectionKind v);
internal String8 rdi_string_from_arch(RDI_Arch v);
internal String8 rdi_string_from_language(RDI_Language v);
internal String8 rdi_string_from_type_kind(RDI_TypeKind v);
internal String8 rdi_string_from_member_kind(RDI_MemberKind v);
internal String8 rdi_string_from_local_kind(RDI_LocalKind v);

////////////////////////////////
//~ rjf: RDI Flags -> String Functions

internal void rdi_stringize_binary_section_flags(Arena *arena, String8List *out, RDI_BinarySectionFlags flags);
internal void rdi_stringize_type_modifier_flags(Arena *arena, String8List *out, RDI_TypeModifierFlags flags);
internal void rdi_stringize_udt_flags(Arena *arena, String8List *out, RDI_UDTFlags flags);
internal void rdi_stringize_link_flags(Arena *arena, String8List *out, RDI_LinkFlags flags);

////////////////////////////////

internal String8 rdi_format_reg_code(Arena *arena, RDI_Arch arch, ulong reg_code);

////////////////////////////////
//~ rjf: RDI Compound Stringize Functions

internal void rdi_stringize_data_sections(Arena *arena, String8List *out, RDI_Parsed *rdi, uint indent_level);
internal void rdi_stringize_top_level_info(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_TopLevelInfo *tli, uint indent_level);
internal void rdi_stringize_binary_section(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_BinarySection *bin_section, uint indent_level);
internal void rdi_stringize_file_path(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_FilePathBundle *bundle, RDI_FilePathNode *file_path, uint indent_level);
internal void rdi_stringize_source_file(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_SourceFile *source_file, uint indent_level);
internal void rdi_stringize_line_table(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_LineTable *line_table, uint indent_level);
internal void rdi_stringize_source_line_map(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_SourceLineMap *map, uint indent_level);
internal void rdi_stringize_unit(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_Unit *unit, uint indent_level);
internal void rdi_stringize_type_node(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_TypeNode *type, uint indent_level);
internal void rdi_stringize_udt(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_UDTMemberBundle *bundle, RDI_UDT *udt, uint indent_level);
internal void rdi_stringize_global_variable(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_GlobalVariable *global_variable, uint indent_level);
internal void rdi_stringize_thread_variable(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_ThreadVariable *thread_var, uint indent_level);
internal void rdi_stringize_procedure(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_Procedure *proc, uint indent_level);
internal void rdi_stringize_scope(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_Arch arch, RDI_ScopeBundle *bundle, RDI_Scope *scope, uint indent_level);
internal void rdi_stringize_inline_site(Arena *arena, String8List *out, RDI_Parsed *rdi, RDI_InlineSite *inline_site, uint indent_level);

#endif // RDI_DUMP_H

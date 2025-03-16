// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_DUMP_H
#define RDI_DUMP_H

////////////////////////////////
//~ rjf: RADDBG Stringize Helper Types

struct RDI_FilePathBundle
{
  RDI_FilePathNode* file_paths;
  uint64 file_path_count;
}

struct RDI_UDTMemberBundle
{
  RDI_Member* members;
  RDI_EnumMember* enum_members;
  uint32 member_count;
  uint32 enum_member_count;
}

struct RDI_ScopeBundle
{
  RDI_Scope* scopes;
  uint64* scope_voffs;
  RDI_Local* locals;
  RDI_LocationBlock* location_blocks;
  uint8* location_data;
  uint32 scope_count;
  uint32 scope_voff_count;
  uint32 local_count;
  uint32 location_block_count;
  uint32 location_data_size;
}

////////////////////////////////
//~ rjf: RDI Enum . String Functions

String8 rdi_string_from_reg_code_x86(uint64 reg_code);
String8 rdi_string_from_reg_code_x64(uint64 reg_code);
String8 rdi_string_from_reg_code(RDI_Arch arch, uint64 reg_code);
String8 rdi_string_from_data_section_kind(RDI_SectionKind v);
String8 rdi_string_from_arch(RDI_Arch v);
String8 rdi_string_from_language(RDI_Language v);
String8 rdi_string_from_type_kind(RDI_TypeKind v);
String8 rdi_string_from_member_kind(RDI_MemberKind v);
String8 rdi_string_from_local_kind(RDI_LocalKind v);

////////////////////////////////
//~ rjf: RDI Flags . String Functions

void rdi_stringize_binary_section_flags(Arena* arena, String8List* out, RDI_BinarySectionFlags flags);
void rdi_stringize_type_modifier_flags(Arena* arena, String8List* out, RDI_TypeModifierFlags flags);
void rdi_stringize_udt_flags(Arena* arena, String8List* out, RDI_UDTFlags flags);
void rdi_stringize_link_flags(Arena* arena, String8List* out, RDI_LinkFlags flags);

////////////////////////////////

String8 rdi_format_reg_code(Arena* arena, RDI_Arch arch, uint64 reg_code);

////////////////////////////////
//~ rjf: RDI Compound Stringize Functions

void rdi_stringize_data_sections(Arena* arena, String8List* out, RDI_Parsed* rdi, uint32 indent_level);
void rdi_stringize_top_level_info(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_TopLevelInfo* tli, uint32 indent_level);
void rdi_stringize_binary_section(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_BinarySection* bin_section, uint32 indent_level);
void rdi_stringize_file_path(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_FilePathBundle* bundle, RDI_FilePathNode* file_path, uint32 indent_level);
void rdi_stringize_source_file(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_SourceFile* source_file, uint32 indent_level);
void rdi_stringize_line_table(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_LineTable* line_table, uint32 indent_level);
void rdi_stringize_source_line_map(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_SourceLineMap* map, uint32 indent_level);
void rdi_stringize_unit(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_Unit* unit, uint32 indent_level);
void rdi_stringize_type_node(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_TypeNode* type, uint32 indent_level);
void rdi_stringize_udt(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_UDTMemberBundle* bundle, RDI_UDT* udt, uint32 indent_level);
void rdi_stringize_global_variable(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_GlobalVariable* global_variable, uint32 indent_level);
void rdi_stringize_thread_variable(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_ThreadVariable* thread_var, uint32 indent_level);
void rdi_stringize_procedure(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_Procedure* proc, uint32 indent_level);
void rdi_stringize_scope(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_Arch arch, RDI_ScopeBundle* bundle, RDI_Scope* scope, uint32 indent_level);
void rdi_stringize_inline_site(Arena* arena, String8List* out, RDI_Parsed* rdi, RDI_InlineSite* inline_site, uint32 indent_level);

#endif // RDI_DUMP_H

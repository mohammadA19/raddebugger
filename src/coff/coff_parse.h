// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef COFF_PARSE_H
#define COFF_PARSE_H

srtuct COFF_FileHeaderInfo
{
  B32              is_big_obj;
  COFF_MachineType machine;
  ulong              header_size;
  ulong              section_array_off;
  ulong              section_count_no_null;
  ulong              string_table_off;
  ulong              symbol_size;
  ulong              symbol_off;
  ulong              symbol_count;
};

////////////////////////////////

srtuct COFF_SectionHeaderArray
{
  ulong                 count;
  COFF_SectionHeader *v;
};

////////////////////////////////

srtuct COFF_Symbol16Node
{
  struct COFF_Symbol16Node *next;
  COFF_Symbol16             data;
};

srtuct COFF_Symbol16List
{
  ulong                count;
  COFF_Symbol16Node *first;
  COFF_Symbol16Node *last;
};

srtuct COFF_Symbol32Array
{
  ulong            count;
  COFF_Symbol32 *v;
};

srtuct COFF_ParsedSymbol
{
  String8              name;
  uint                  value;
  uint                  section_number;
  COFF_SymbolType      type;
  COFF_SymStorageClass storage_class;
  byte                   aux_symbol_count;
};

enum COFF_SymbolValueInterpType : uint
{
  COFF_SymbolValueInterp_Regular,   // symbol has section and offset.
  COFF_SymbolValueInterp_Weak,      // symbol is overridable.
  COFF_SymbolValueInterp_Undefined, // symbol doesn't have a reference section.
  COFF_SymbolValueInterp_Common,    // symbol has no section but still has size.
  COFF_SymbolValueInterp_Abs,       // symbol has an absolute (non-relocatable) value and is not an address.
  COFF_SymbolValueInterp_Debug      // symbol is used to provide general type of debugging information.
};

////////////////////////////////

srtuct COFF_RelocNode
{
  struct COFF_RelocNode *next;
  COFF_Reloc             data;
};

srtuct COFF_RelocList
{
  ulong             count;
  COFF_RelocNode *first;
  COFF_RelocNode *last;
};

srtuct COFF_RelocArray
{
  ulong         count;
  COFF_Reloc *v;
};

srtuct COFF_RelocInfo
{
  ulong array_off;
  ulong count;
};

////////////////////////////////

enum COFF_ResourceIDType : uint  COFF_ResourceIDTypeEnum
{
  COFF_ResourceIDType_Null,
  COFF_ResourceIDType_Number,
  COFF_ResourceIDType_String,
  COFF_ResourceIDType_Count
};

srtuct COFF_ResourceID16
{
  COFF_ResourceIDType type;
  union {
    ushort      number;
    String16 string;
  } u;
};

srtuct COFF_ResourceID
{
  COFF_ResourceIDType type;
  union {
    ushort     number;
    String8 string;
  } u;
};

srtuct COFF_ParsedResource
{
  COFF_ResourceID          type;
  COFF_ResourceID          name;
  uint                      data_version;
  COFF_ResourceMemoryFlags memory_flags;
  ushort                      language_id;
  uint                      version;
  uint                      characteristics;
  String8                  data;
};

srtuct COFF_ParsedResourceNode
{
  struct COFF_ParsedResourceNode *next;
  COFF_ParsedResource             data;
};

srtuct COFF_ParsedResourceList
{
  ulong                      count;
  COFF_ParsedResourceNode *first;
  COFF_ParsedResourceNode *last;
};

////////////////////////////////

enum
{
  COFF_DataType_Null,
  COFF_DataType_Obj,
  COFF_DataType_BigObj,
  COFF_DataType_Import
};

enum
{
  COFF_Archive_Null,
  COFF_Archive_Regular,
  COFF_Archive_Thin
};

srtuct COFF_ParsedArchiveMemberHeader
{
  String8        name;           // padded to 16 bytes with spaces
  COFF_TimeStamp time_stamp;
  uint            user_id;        // unix artifact that does not have meaning on windows
  uint            group_id;       // unix artifact that does not have meaning on windows
  String8        mode;           // octal representation the members file mode
  B32            is_end_correct; // set to true if found correct signature after header
  Rng1U64        data_range;
};

srtuct COFF_ParsedArchiveImportHeader
{
  B32               is_sig_correct;
  ushort               version;
  COFF_MachineType  machine;
  COFF_TimeStamp    time_stamp;
  uint               data_size;
  ushort               hint_or_ordinal;
  COFF_ImportType   type;
  COFF_ImportByType import_by;
  String8           func_name;
  String8           dll_name;
};

srtuct COFF_ArchiveMember
{
  COFF_ParsedArchiveMemberHeader header;
  ulong                      offset;
  String8                  data;
};

srtuct COFF_ArchiveFirstMember
{
  uint      symbol_count;
  ulong      member_offset_count;
  uint     *member_offsets;
  String8  string_table;
};

srtuct COFF_ArchiveSecondMember
{
  uint      member_count;
  uint      symbol_count;
  ulong      member_offset_count;
  uint     *member_offsets;
  ulong      symbol_index_count;
  ushort     *symbol_indices;
  String8  string_table;
};

srtuct COFF_ArchiveMemberNode
{
  struct COFF_ArchiveMemberNode *next;
  COFF_ArchiveMember             data;
};

srtuct COFF_ArchiveMemberList
{
  ulong                     count;
  COFF_ArchiveMemberNode *first;
  COFF_ArchiveMemberNode *last;
};

srtuct COFF_ArchiveParse
{
  B32                      has_second_header;
  B32                      has_long_names;
  COFF_ArchiveFirstMember  first_member;
  COFF_ArchiveSecondMember second_member;
  String8                  long_names;
  String8                  error;
};

////////////////////////////////
// Obj Header

B32 coff_is_big_obj(String8 raw_coff);
B32 coff_is_obj    (String8 raw_coff);
COFF_FileHeaderInfo coff_file_header_info_from_data(String8 raw_coff);

////////////////////////////////
// Symbol

COFF_ParsedSymbol coff_parse_symbol32(String8 raw_coff, ulong string_table_off, COFF_Symbol32 *sym32);
COFF_ParsedSymbol coff_parse_symbol16(String8 raw_coff, ulong string_table_off, COFF_Symbol16 *sym16);

COFF_Symbol32Array coff_symbol_array_from_data_16(Arena *arena, String8 data, ulong symbol_array_off, ulong symbol_count);
COFF_Symbol32Array coff_symbol_array_from_data_32(Arena *arena, String8 data, ulong symbol_array_off, ulong symbol_count);
COFF_Symbol32Array coff_symbol_array_from_data   (Arena *arena, String8 data, ulong symbol_array_off, ulong symbol_count, ulong symbol_size);

COFF_Symbol16Node *coff_symbol16_list_push(Arena *arena, COFF_Symbol16List *list, COFF_Symbol16 symbol);

COFF_SymbolValueInterpType coff_interp_symbol(uint section_number, uint value, COFF_SymStorageClass storage_class);

////////////////////////////////
// Reloc

COFF_RelocInfo coff_reloc_info_from_section_header(String8 data, COFF_SectionHeader *header);

////////////////////////////////
// Resource

String8         coff_resource_string_from_str16 (Arena *arena, String16 string);
String8         coff_resource_string_from_str8  (Arena *arena, String8 string);
String8         coff_resource_number_from_u16   (Arena *arena, ushort number);
COFF_ResourceID coff_utf8_resource_id_from_utf16(Arena *arena, COFF_ResourceID16 *id_16);

ulong                     coff_read_resource_id_utf16 (String8 raw_res, ulong off, COFF_ResourceID16 *id_out);
ulong                     coff_read_resource          (Arena *arena, String8 raw_res, ulong off, COFF_ParsedResource *res_out);
COFF_ParsedResourceList coff_resource_list_from_data(Arena *arena, String8 data);

String8 coff_write_resource_id(Arena *arena, COFF_ResourceID id);
String8 coff_write_resource   (Arena *arena, COFF_ResourceID type, COFF_ResourceID name, uint data_version, COFF_ResourceMemoryFlags memory_flags, ushort language_id, uint version, uint characteristics, String8 data);

int coff_resource_id_compar(void *raw_a, void *raw_b); // COFF_ResourceID

////////////////////////////////
// Archive

B32              coff_is_import             (String8 raw_archive_member);
COFF_DataType    coff_data_type_from_data   (String8 raw_archive_member);
B32              coff_is_regular_archive    (String8 raw_archive);
B32              coff_is_thin_archive       (String8 raw_archive);
COFF_ArchiveType coff_archive_type_from_data(String8 raw_archive);

ulong                      coff_parse_archive_member_header(String8 raw_archive, ulong offset, COFF_ParsedArchiveMemberHeader *header_out);
COFF_ArchiveFirstMember  coff_parse_first_archive_member (COFF_ArchiveMember *member);
COFF_ArchiveSecondMember coff_parse_second_archive_member(COFF_ArchiveMember *member);
String8                  coff_parse_long_name            (String8 long_names, String8 name);
ulong                      coff_parse_import               (String8 raw_archive_member, ulong offset, COFF_ParsedArchiveImportHeader *header_out);

COFF_ArchiveMember             coff_archive_member_from_offset(String8 raw_archive, ulong offset);
COFF_ArchiveMember             coff_archive_member_from_data  (String8 raw_archive_member);
COFF_ParsedArchiveImportHeader coff_archive_import_from_data  (String8 raw_archive_member);

ulong coff_regular_archive_member_iter_init(String8 raw_archive);
B32 coff_regular_archive_member_iter_next(String8 raw_archive, ulong *offset, COFF_ArchiveMember *member_out);

ulong coff_thin_archive_member_iter_init(String8 raw_archive);
B32 coff_thin_archive_member_iter_next(String8 raw_archive, ulong *offset, COFF_ArchiveMember *member_out);

COFF_ArchiveParse coff_regular_archive_parse_from_member_list(COFF_ArchiveMemberList list);
COFF_ArchiveParse coff_thin_archive_parse_from_data          (String8 raw_archive);
COFF_ArchiveParse coff_regular_archive_parse_from_data       (String8 raw_archive);
COFF_ArchiveParse coff_archive_parse_from_data               (String8 raw_archive);

#endif // COFF_PARSE_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

typedef struct COFF_FileHeaderInfo
{
  B32              is_big_obj;
  COFF_MachineType machine;
  U64              header_size;
  U64              section_count_no_null;
  U64              symbol_size;
  U64              symbol_count;
  Rng1U64          header_range;
  Rng1U64          section_table_range;
  Rng1U64          symbol_table_range;
  Rng1U64          string_table_range;
} COFF_FileHeaderInfo;

////////////////////////////////

typedef struct COFF_SectionHeaderArray
{
  U64                 count;
  COFF_SectionHeader* v;
} COFF_SectionHeaderArray;

////////////////////////////////

typedef struct COFF_Symbol16Node
{
  COFF_Symbol16Node* next;
  COFF_Symbol16             data;
} COFF_Symbol16Node;

typedef struct COFF_Symbol16List
{
  U64                count;
  COFF_Symbol16Node* first;
  COFF_Symbol16Node* last;
} COFF_Symbol16List;

typedef struct COFF_Symbol32Array
{
  U64            count;
  COFF_Symbol32* v;
} COFF_Symbol32Array;

typedef struct COFF_ParsedSymbol
{
  String8              name;
  U32                  value;
  U32                  section_number;
  COFF_SymbolType      type;
  COFF_SymStorageClass storage_class;
  U8                   aux_symbol_count;
} COFF_ParsedSymbol;

enum COFF_SymbolValueInterpType : U32
{
  COFF_SymbolValueInterp_Regular,   // symbol has section and offset.
  COFF_SymbolValueInterp_Weak,      // symbol is overridable.
  COFF_SymbolValueInterp_Undefined, // symbol doesn't have a reference section.
  COFF_SymbolValueInterp_Common,    // symbol has no section but still has size.
  COFF_SymbolValueInterp_Abs,       // symbol has an absolute (non-relocatable) value and is not an address.
  COFF_SymbolValueInterp_Debug      // symbol is used to provide general type of debugging information.
}

////////////////////////////////

typedef struct COFF_RelocNode
{
  COFF_RelocNode* next;
  COFF_Reloc             data;
} COFF_RelocNode;

typedef struct COFF_RelocList
{
  U64             count;
  COFF_RelocNode* first;
  COFF_RelocNode* last;
} COFF_RelocList;

typedef struct COFF_RelocArray
{
  U64         count;
  COFF_Reloc* v;
} COFF_RelocArray;

typedef struct COFF_RelocInfo
{
  U64 array_off;
  U64 count;
} COFF_RelocInfo;

////////////////////////////////

enum COFF_ResourceIDType : U32  COFF_ResourceIDTypeEnum
{
  COFF_ResourceIDType_Null,
  COFF_ResourceIDType_Number,
  COFF_ResourceIDType_String,
  COFF_ResourceIDType_Count
}

typedef struct COFF_ResourceID16
{
  COFF_ResourceIDType type;
  union {
    U16      number;
    String16 string;
  } u;
} COFF_ResourceID16;

typedef struct COFF_ResourceID
{
  COFF_ResourceIDType type;
  union {
    U16     number;
    String8 string;
  } u;
} COFF_ResourceID;

typedef struct COFF_ParsedResource
{
  COFF_ResourceID          type;
  COFF_ResourceID          name;
  U32                      data_version;
  COFF_ResourceMemoryFlags memory_flags;
  U16                      language_id;
  U32                      version;
  U32                      characteristics;
  String8                  data;
} COFF_ParsedResource;

typedef struct COFF_ParsedResourceNode
{
  COFF_ParsedResourceNode* next;
  COFF_ParsedResource             data;
} COFF_ParsedResourceNode;

typedef struct COFF_ParsedResourceList
{
  U64                      count;
  COFF_ParsedResourceNode* first;
  COFF_ParsedResourceNode* last;
} COFF_ParsedResourceList;

////////////////////////////////

enum
{
  COFF_DataType_Null,
  COFF_DataType_Obj,
  COFF_DataType_BigObj,
  COFF_DataType_Import
} COFF_DataType;

enum
{
  COFF_Archive_Null,
  COFF_Archive_Regular,
  COFF_Archive_Thin
} COFF_ArchiveType;

typedef struct COFF_ParsedArchiveMemberHeader
{
  String8        name;           // padded to 16 bytes with spaces
  COFF_TimeStamp time_stamp;
  U32            user_id;        // unix artifact that does not have meaning on windows
  U32            group_id;       // unix artifact that does not have meaning on windows
  String8        mode;           // octal representation the members file mode
  B32            is_end_correct; // set to true if found correct signature after header
  Rng1U64        data_range;
} COFF_ParsedArchiveMemberHeader;

typedef struct COFF_ParsedArchiveImportHeader
{
  B32               is_sig_correct;
  U16               version;
  COFF_MachineType  machine;
  COFF_TimeStamp    time_stamp;
  U32               data_size;
  U16               hint_or_ordinal;
  COFF_ImportType   type;
  COFF_ImportByType import_by;
  String8           func_name;
  String8           dll_name;
} COFF_ParsedArchiveImportHeader;

typedef struct COFF_ArchiveMember
{
  COFF_ParsedArchiveMemberHeader header;
  U64                      offset;
  String8                  data;
} COFF_ArchiveMember;

typedef struct COFF_ArchiveFirstMember
{
  U32      symbol_count;
  U64      member_offset_count;
  U32*     member_offsets;
  String8  string_table;
} COFF_ArchiveFirstMember;

typedef struct COFF_ArchiveSecondMember
{
  U32      member_count;
  U32      symbol_count;
  U64      member_offset_count;
  U32*     member_offsets;
  U64      symbol_index_count;
  U16*     symbol_indices;
  String8  string_table;
} COFF_ArchiveSecondMember;

typedef struct COFF_ArchiveMemberNode
{
  COFF_ArchiveMemberNode* next;
  COFF_ArchiveMember             data;
} COFF_ArchiveMemberNode;

typedef struct COFF_ArchiveMemberList
{
  U64                     count;
  COFF_ArchiveMemberNode* first;
  COFF_ArchiveMemberNode* last;
} COFF_ArchiveMemberList;

typedef struct COFF_ArchiveParse
{
  B32                      has_second_header;
  B32                      has_long_names;
  COFF_ArchiveFirstMember  first_member;
  COFF_ArchiveSecondMember second_member;
  String8                  long_names;
  String8                  error;
} COFF_ArchiveParse;

////////////////////////////////
// Obj Header


////////////////////////////////
// Symbol





////////////////////////////////
// Reloc


////////////////////////////////
// Resource





////////////////////////////////
// Archive








// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




typedef struct COFF_FileHeaderInfo
{
  b32              is_big_obj;
  COFF_MachineType machine;
  u64              header_size;
  u64              section_count_no_null;
  u64              symbol_size;
  u64              symbol_count;
  Rng1U64          header_range;
  Rng1U64          section_table_range;
  Rng1U64          symbol_table_range;
  Rng1U64          string_table_range;
} COFF_FileHeaderInfo;

////////////////////////////////

typedef struct COFF_SectionHeaderArray
{
  u64                 count;
  COFF_SectionHeader *v;
} COFF_SectionHeaderArray;

////////////////////////////////

typedef struct COFF_Symbol16Node
{
  struct COFF_Symbol16Node *next;
  COFF_Symbol16             data;
} COFF_Symbol16Node;

typedef struct COFF_Symbol16List
{
  u64                count;
  COFF_Symbol16Node *first;
  COFF_Symbol16Node *last;
} COFF_Symbol16List;

typedef struct COFF_Symbol32Array
{
  u64            count;
  COFF_Symbol32 *v;
} COFF_Symbol32Array;

typedef struct COFF_ParsedSymbol
{
  String8               name;
  u64                   value;
  u32                   section_number;
  COFF_SymbolType       type;
  COFF_SymStorageClass  storage_class;
  u8                    aux_symbol_count;
  void                 *raw_symbol;
} COFF_ParsedSymbol;

typedef u32 COFF_SymbolValueInterpType;
enum
{
  COFF_SymbolValueInterp_Regular,   // symbol has section and offset.
  COFF_SymbolValueInterp_Weak,      // symbol is overridable.
  COFF_SymbolValueInterp_Undefined, // symbol doesn't have a reference section.
  COFF_SymbolValueInterp_Common,    // symbol has no section but still has size.
  COFF_SymbolValueInterp_Abs,       // symbol has an absolute (non-relocatable) value and is not an address.
  COFF_SymbolValueInterp_Debug      // symbol is used to provide general type of debugging information.
};

////////////////////////////////

typedef struct COFF_RelocNode
{
  struct COFF_RelocNode *next;
  COFF_Reloc             data;
} COFF_RelocNode;

typedef struct COFF_RelocList
{
  u64             count;
  COFF_RelocNode *first;
  COFF_RelocNode *last;
} COFF_RelocList;

typedef struct COFF_RelocArray
{
  u64         count;
  COFF_Reloc *v;
} COFF_RelocArray;

typedef struct COFF_RelocInfo
{
  u64 array_off;
  u64 count;
} COFF_RelocInfo;

////////////////////////////////

typedef u32 COFF_ResourceIDType;
enum  COFF_ResourceIDTypeEnum
{
  COFF_ResourceIDType_Null,
  COFF_ResourceIDType_Number,
  COFF_ResourceIDType_String,
  COFF_ResourceIDType_Count
};

typedef struct COFF_ResourceID16
{
  COFF_ResourceIDType type;
  union {
    u16      number;
    String16 string;
  } u;
} COFF_ResourceID16;

typedef struct COFF_ResourceID
{
  COFF_ResourceIDType type;
  union {
    u16     number;
    String8 string;
  } u;
} COFF_ResourceID;

typedef struct COFF_ParsedResource
{
  COFF_ResourceID          type;
  COFF_ResourceID          name;
  u32                      data_version;
  COFF_ResourceMemoryFlags memory_flags;
  u16                      language_id;
  u32                      version;
  u32                      characteristics;
  String8                  data;
} COFF_ParsedResource;

typedef struct COFF_ParsedResourceNode
{
  struct COFF_ParsedResourceNode *next;
  COFF_ParsedResource             data;
} COFF_ParsedResourceNode;

typedef struct COFF_ParsedResourceList
{
  u64                      count;
  COFF_ParsedResourceNode *first;
  COFF_ParsedResourceNode *last;
} COFF_ParsedResourceList;

////////////////////////////////

typedef enum
{
  COFF_DataType_Null,
  COFF_DataType_Obj,
  COFF_DataType_BigObj,
  COFF_DataType_Import
} COFF_DataType;

typedef enum
{
  COFF_Archive_Null,
  COFF_Archive_Regular,
  COFF_Archive_Thin
} COFF_ArchiveType;

typedef struct COFF_ParsedArchiveMemberHeader
{
  String8        name;           // padded to 16 bytes with spaces
  COFF_TimeStamp time_stamp;
  u32            user_id;        // unix artifact that does not have meaning on windows
  u32            group_id;       // unix artifact that does not have meaning on windows
  String8        mode;           // octal representation the members file mode
  b32            is_end_correct; // set to true if found correct signature after header
  Rng1U64        data_range;
} COFF_ParsedArchiveMemberHeader;

typedef struct COFF_ParsedArchiveImportHeader
{
  b32               is_sig_correct;
  u16               version;
  COFF_MachineType  machine;
  COFF_TimeStamp    time_stamp;
  u32               data_size;
  u16               hint_or_ordinal;
  COFF_ImportType   type;
  COFF_ImportByType import_by;
  String8           func_name;
  String8           dll_name;
} COFF_ParsedArchiveImportHeader;

typedef struct COFF_ArchiveMember
{
  COFF_ParsedArchiveMemberHeader header;
  u64                            offset;
  String8                        data;
} COFF_ArchiveMember;

typedef struct COFF_ArchiveFirstMember
{
  u32      symbol_count;
  u64      member_offset_count;
  u32     *member_offsets;
  String8  string_table;
} COFF_ArchiveFirstMember;

typedef struct COFF_ArchiveSecondMember
{
  u32      member_count;
  u32      symbol_count;
  u64      member_offset_count;
  u32     *member_offsets;
  u64      symbol_index_count;
  u16     *symbol_indices;
  String8  string_table;
} COFF_ArchiveSecondMember;

typedef struct COFF_ArchiveMemberNode
{
  struct COFF_ArchiveMemberNode *next;
  COFF_ArchiveMember             data;
} COFF_ArchiveMemberNode;

typedef struct COFF_ArchiveMemberList
{
  u64                     count;
  COFF_ArchiveMemberNode *first;
  COFF_ArchiveMemberNode *last;
} COFF_ArchiveMemberList;

typedef struct COFF_ArchiveParse
{
  b32                      has_second_header;
  b32                      has_long_names;
  COFF_ArchiveFirstMember  first_member;
  COFF_ArchiveSecondMember second_member;
  String8                  long_names;
  String8                  error;
} COFF_ArchiveParse;

////////////////////////////////
// Obj Header





////////////////////////////////
// Section



////////////////////////////////
// Symbol
















////////////////////////////////
// Reloc



////////////////////////////////
// Resource













 // COFF_ResourceID

////////////////////////////////
// Archive






























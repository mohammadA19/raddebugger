// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef COFF_PARSE_H
#define COFF_PARSE_H

typedef struct COFF_FileHeaderInfo
{
  B32              is_big_obj;
  COFF_MachineType machine;
  uint64              header_size;
  uint64              section_count_no_null;
  uint64              symbol_size;
  uint64              symbol_count;
  Rng1uint64          header_range;
  Rng1uint64          section_table_range;
  Rng1uint64          symbol_table_range;
  Rng1uint64          string_table_range;
} COFF_FileHeaderInfo;

////////////////////////////////

typedef struct COFF_SectionHeaderArray
{
  uint64                 count;
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
  uint64                count;
  COFF_Symbol16Node *first;
  COFF_Symbol16Node *last;
} COFF_Symbol16List;

typedef struct COFF_Symbol32Array
{
  uint64            count;
  COFF_Symbol32 *v;
} COFF_Symbol32Array;

typedef struct COFF_ParsedSymbol
{
  String8               name;
  uint64                   value;
  uint32                   section_number;
  COFF_SymbolType       type;
  COFF_SymStorageClass  storage_class;
  uint8                    aux_symbol_count;
  void                 *raw_symbol;
} COFF_ParsedSymbol;

enum COFF_SymbolValueInterpType : uint32
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
  uint64             count;
  COFF_RelocNode *first;
  COFF_RelocNode *last;
} COFF_RelocList;

typedef struct COFF_RelocArray
{
  uint64         count;
  COFF_Reloc *v;
} COFF_RelocArray;

typedef struct COFF_RelocInfo
{
  uint64 array_off;
  uint64 count;
} COFF_RelocInfo;

////////////////////////////////

enum COFF_ResourceIDType : uint32  COFF_ResourceIDTypeEnum
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
    uint16      number;
    String16 string;
  } u;
} COFF_ResourceID16;

typedef struct COFF_ResourceID
{
  COFF_ResourceIDType type;
  union {
    uint16     number;
    String8 string;
  } u;
} COFF_ResourceID;

typedef struct COFF_ParsedResource
{
  COFF_ResourceID          type;
  COFF_ResourceID          name;
  uint32                      data_version;
  COFF_ResourceMemoryFlags memory_flags;
  uint16                      language_id;
  uint32                      version;
  uint32                      characteristics;
  String8                  data;
} COFF_ParsedResource;

typedef struct COFF_ParsedResourceNode
{
  struct COFF_ParsedResourceNode *next;
  COFF_ParsedResource             data;
} COFF_ParsedResourceNode;

typedef struct COFF_ParsedResourceList
{
  uint64                      count;
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
  uint32            user_id;        // unix artifact that does not have meaning on windows
  uint32            group_id;       // unix artifact that does not have meaning on windows
  String8        mode;           // octal representation the members file mode
  B32            is_end_correct; // set to true if found correct signature after header
  Rng1uint64        data_range;
} COFF_ParsedArchiveMemberHeader;

typedef struct COFF_ParsedArchiveImportHeader
{
  B32               is_sig_correct;
  uint16               version;
  COFF_MachineType  machine;
  COFF_TimeStamp    time_stamp;
  uint32               data_size;
  uint16               hint_or_ordinal;
  COFF_ImportType   type;
  COFF_ImportByType import_by;
  String8           func_name;
  String8           dll_name;
} COFF_ParsedArchiveImportHeader;

typedef struct COFF_ArchiveMember
{
  COFF_ParsedArchiveMemberHeader header;
  uint64                            offset;
  String8                        data;
} COFF_ArchiveMember;

typedef struct COFF_ArchiveFirstMember
{
  uint32      symbol_count;
  uint64      member_offset_count;
  uint32     *member_offsets;
  String8  string_table;
} COFF_ArchiveFirstMember;

typedef struct COFF_ArchiveSecondMember
{
  uint32      member_count;
  uint32      symbol_count;
  uint64      member_offset_count;
  uint32     *member_offsets;
  uint64      symbol_index_count;
  uint16     *symbol_indices;
  String8  string_table;
} COFF_ArchiveSecondMember;

typedef struct COFF_ArchiveMemberNode
{
  struct COFF_ArchiveMemberNode *next;
  COFF_ArchiveMember             data;
} COFF_ArchiveMemberNode;

typedef struct COFF_ArchiveMemberList
{
  uint64                     count;
  COFF_ArchiveMemberNode *first;
  COFF_ArchiveMemberNode *last;
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
// Section


////////////////////////////////
// Symbol






////////////////////////////////
// Reloc


////////////////////////////////
// Resource




internal int coff_resource_id_compar(void *raw_a, void *raw_b); // COFF_ResourceID

////////////////////////////////
// Archive







#endif // COFF_PARSE_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PDB_H
#define PDB_H

// https://github.com/microsoft/microsoft-pdb/tree/master/PDB

////////////////////////////////
//~ PDB String Table Types

#define PDB_INVALID_STRING_INDEX max_U32
typedef uint PDB_StringIndex;
typedef uint PDB_StringOffset;

enum
{
  PDB_StringTableHeader_MAGIC = 0xEFFEEFFE
};

enum
{
  PDB_StringTableHeader_Version1       = 1,
  PDB_StringTableHeader_CurrentVersion = PDB_StringTableHeader_Version1
};

srtuct PDB_StringTableHeader
{
  uint magic;
  uint version;
};

////////////////////////////////

enum PDB_FixedStream
{
  PDB_FixedStream_Info = 1,
  PDB_FixedStream_Tpi  = 2,
  PDB_FixedStream_Dbi  = 3,
  PDB_FixedStream_Ipi  = 4
};

////////////////////////////////
//~ PDB Info Types

enum PDB_InfoVersion : uint{
  PDB_InfoVersion_VC2      = 19941610,
  PDB_InfoVersion_VC4      = 19950623,
  PDB_InfoVersion_VC41     = 19950814,
  PDB_InfoVersion_VC50     = 19960307,
  PDB_InfoVersion_VC98     = 19970604,
  PDB_InfoVersion_VC70_DEP = 19990604,
  PDB_InfoVersion_VC70     = 20000404,
  PDB_InfoVersion_VC80     = 20030901,
  PDB_InfoVersion_VC110    = 20091201,
  PDB_InfoVersion_VC140    = 20140508
};

// referenced in PDB1::loadPdbStream
enum
{
  PDB_FeatureSig_NULL               = 0,
  PDB_FeatureSig_VC110              = PDB_InfoVersion_VC110,
  PDB_FeatureSig_VC140              = PDB_InfoVersion_VC140,
  PDB_FeatureSig_NO_TYPE_MERGE      = 0x4D544F4E,
  PDB_FeatureSig_MINIMAL_DEBUG_INFO = 0x494E494D,
};
typedef uint PDB_FeatureSig;

enum
{
  PDB_FeatureFlag_HAS_ID_STREAM    = (1 << 0),
  PDB_FeatureFlag_NO_TYPE_MERGE    = (1 << 1),
  PDB_FeatureFlag_MINIMAL_DBG_INFO = (1 << 2),
};
typedef uint PDB_FeatureFlags;

#pragma pack(push,1)
srtuct PDB_InfoHeaderV70
{
  PDB_InfoVersion    version;
  COFF_TimeStamp     time_stamp;
  uint                age;
  Guid               guid;
  // PDB_HashTable   named_stream_hash_table
  // PDB_FeatureFlag features[*]
};

#pragma pack(pop)
StaticAssert(sizeof(PDB_InfoHeaderV70) == 28, pdb_info_header_v70_size_check);

#define PDB_SRC_HEADER_BLOCK_STREAM_NAME str8_lit("/src/headerblock")
#define PDB_LINK_INFO_STREAM_NAME        str8_lit("/LinkInfo")
#define PDB_NAMES_STREAM_NAME            str8_lit("/names")

////////////////////////////////
// SRC Header Block

#define PDB_SRC_HEADER_BLOCK_MAGIC_V1 19980827

srtuct PDB_SrcHeaderBlockHeader
{
  uint version;
  uint stream_size;
  ulong file_time;
  uint age;
  byte  pad[44];
};

enum
{
  PDB_SrcComp_NULL,
  PDB_SrcComp_RUN_LENGTH_RECORD,
  PDB_SrcComp_HUFFMAN,
  PDB_SrcComp_LZ,
  PDB_SrcComp_DOTNET
};
typedef byte PDB_SrcCompType;

enum
{
  PDB_SrcHeaderBlockEntryFlag_IS_VIRTUAL = (1 << 0)
};
typedef byte PDB_SrcHeaderFlags;

// (PDB/include/pdb.h: SrcHeaderOut)
srtuct PDB_SrcHeaderBlockEntry
{
  uint                size;
  uint                version;
  uint                file_crc;
  uint                file_size;
  PDB_StringOffset   file_path;
  PDB_StringOffset   obj;
  PDB_StringOffset   virt_path;
  PDB_SrcCompType    comp;
  PDB_SrcHeaderFlags flags;
  byte                 pad[2];
  byte                 reserved[8];
};

////////////////////////////////
//~ PDB Format DBI Types

enum PDB_DbiStream : uint
{
  PDB_DbiStream_FPO,
  PDB_DbiStream_EXCEPTION,
  PDB_DbiStream_FIXUP,
  PDB_DbiStream_OMAP_TO_SRC,
  PDB_DbiStream_OMAP_FROM_SRC,
  PDB_DbiStream_SECTION_HEADER,
  PDB_DbiStream_TOKEN_RDI_MAP,
  PDB_DbiStream_XDATA,
  PDB_DbiStream_PDATA,
  PDB_DbiStream_NEW_FPO,
  PDB_DbiStream_SECTION_HEADER_ORIG,
  PDB_DbiStream_COUNT
};

enum PDB_DbiHeaderSignature : uint
{
  PDB_DbiHeaderSignature_V1 = 0xFFFFFFFF
};

enum PDB_DbiVersion : uint
{
  PDB_DbiVersion_41  =   930803,
  PDB_DbiVersion_50  = 19960307,
  PDB_DbiVersion_60  = 19970606,
  PDB_DbiVersion_70  = 19990903,
  PDB_DbiVersion_110 = 20091201,
};

typedef ushort PDB_DbiBuildNumber;
#define PDB_DbiBuildNumberNewFormatFlag 0x8000
#define PDB_DbiBuildNumberMinor(bn)     ((bn)&0xFF)
#define PDB_DbiBuildNumberMajor(bn)     (((bn) >> 8)&0x7F)
#define PDB_DbiBuildNumberNewFormat(bn) (!!((bn)&PDB_DbiBuildNumberNewFormatFlag))
#define PDB_DbiMakeBuildNumber(maj, min) (PDB_DbiBuildNumber)(PDB_DbiBuildNumberNewFormatFlag | ((min)&0xFF) | (((maj)&0x7F) << 16))

enum PDB_DbiHeaderFlags : ushort
{
  PDB_DbiHeaderFlag_Incremental = 0x1,
  PDB_DbiHeaderFlag_Stripped    = 0x2,
  PDB_DbiHeaderFlag_CTypes      = 0x4
};

srtuct PDB_DbiHeader
{
  PDB_DbiHeaderSignature sig;
  PDB_DbiVersion version;
  uint age;
  MSF_StreamNumber gsi_sn;
  PDB_DbiBuildNumber build_number;
  
  MSF_StreamNumber psi_sn;
  ushort pdb_version;
  
  MSF_StreamNumber sym_sn;
  ushort pdb_version2;
  
  uint module_info_size;
  uint sec_con_size;
  uint sec_map_size;
  uint file_info_size;
  
  uint tsm_size;
  uint mfc_index;
  uint dbg_header_size;
  uint ec_info_size;
  
  PDB_DbiHeaderFlags flags;
  COFF_MachineType machine;
  
  uint reserved;
};

// "ModuleInfo" DBI range

typedef uint PDB_DbiSectionContribVersion;
#define PDB_DbiSectionContribVersion_1 (0xeffe0000u + 19970605u)
#define PDB_DbiSectionContribVersion_2 (0xeffe0000u + 20140516u)

srtuct PDB_DbiSectionContrib40
{
  CV_SectionIndex sec;
  ushort pad0;
  uint sec_off;
  uint size;
  uint flags;
  CV_ModIndex mod;
  ushort pad1;
};

srtuct PDB_DbiSectionContrib
{
  PDB_DbiSectionContrib40 base;
  uint data_crc;
  uint reloc_crc;
};

srtuct PDB_DbiSectionContrib2
{
  PDB_DbiSectionContrib40 base;
  uint data_crc;
  uint reloc_crc;
  uint sec_coff;
};

srtuct PDB_DbiCompUnitHeader
{
  uint unused;
  PDB_DbiSectionContrib contribution;
  ushort flags; // unknown
  
  MSF_StreamNumber sn;
  uint symbols_size;
  uint c11_lines_size;
  uint c13_lines_size;
  
  ushort num_contrib_files;
  ushort unused2;
  uint file_names_offset;
  
  PDB_StringIndex src_file;
  PDB_StringIndex pdb_file;
  
  // byte[] module_name (null terminated)
  // byte[] obj_name (null terminated)
};

////////////////////////////////

enum
{
  PDB_DbiOMF_NONE          = 0,
  PDB_DbiOMF_READ          = (1 << 0),
  PDB_DbiOMF_WRITE         = (1 << 1),
  PDB_DbiOMF_EXEC          = (1 << 2),
  PDB_DbiOMF_IS_32BIT_ADDR = (1 << 3), // Descritor is 32-bit address
  PDB_DbiOMF_IS_SELECTOR   = (1 << 8), // Frame is a selector
  PDB_DbiOMF_IS_ABS_ADDR   = (1 << 9), // Frame is absolute address
  PDB_DbiOMF_IS_GROUP      = (1 << 10) // Descriptor is a group
};
typedef ushort PDB_DbiOMF;

srtuct PDB_DbiSecMapEntry
{
  PDB_DbiOMF flags;
  ushort        ovl;
  ushort        group;
  ushort        frame;
  ushort        sec_name;
  ushort        class_name;
  uint        offset;
  uint        sec_size;
};

srtuct PDB_DbiSecMapHeader
{
  ushort section_count;
  ushort segment_count;
};

////////////////////////////////
//~ PDB Format TPI/IPI Types

enum PDB_TpiVersion : uint
{
  PDB_TpiVersion_INTV_VC2       = 920924,
  PDB_TpiVersion_IMPV40         = 19950410,
  PDB_TpiVersion_IMPV41         = 19951122,
  PDB_TpiVersion_IMPV50_INTERIM = 19960307,
  PDB_TpiVersion_IMPV50         = 19961031,
  PDB_TpiVersion_IMPV70         = 19990903,
  PDB_TpiVersion_IMPV80         = 20040203,
};

enum
{
  PDB_TYPE_SERVER_HASH_BUCKET_COUNT_V7   = 0x1000,
  PDB_TYPE_SERVER_HASH_BUCKET_COUNT_V8   = 0x3FFF,
  PDB_TYPE_SERVER_HASH_BUCKET_COUNT_INIT = 0x1000,
  PDB_TYPE_SERVER_HASH_BUCKET_COUNT_MAX  = 0x40000,
  
  PDB_TYPE_SERVER_HASH_BUCKET_COUNT_CURRENT = PDB_TYPE_SERVER_HASH_BUCKET_COUNT_V8,
};

#define PDB_TYPE_OFFSET_MAX  max_U32
typedef uint PDB_TypeOffset;

srtuct PDB_TpiOffHint
{
  CV_TypeId itype;
  PDB_TypeOffset off;
};

srtuct PDB_OffsetSize
{
  uint off;
  uint size;
};

srtuct PDB_TpiHeader
{
  //   (HDR)
  PDB_TpiVersion version;
  uint header_size;
  uint ti_lo;
  uint ti_hi;
  uint leaf_data_size;
  
  //   (PdbTpiHash)
  MSF_StreamNumber hash_sn;
  MSF_StreamNumber hash_sn_aux;
  uint hash_key_size;
  uint hash_bucket_count;
  PDB_OffsetSize hash_vals;
  PDB_OffsetSize itype_offs;
  PDB_OffsetSize hash_adj;
};


////////////////////////////////
//~ PDB Format GSI Types

enum PDB_GsiSignature : uint
{
  PDB_GsiSignature_Basic = 0xffffffff,
};

enum PDB_GsiVersion : uint
{
  PDB_GsiVersion_V70 = 0xeffe0000 + 19990810,
};

srtuct PDB_GsiHeader
{
  PDB_GsiSignature signature;
  PDB_GsiVersion version;
  uint hash_record_arr_size;
  uint bucket_data_size;
};

srtuct PDB_GsiHashRecord
{
  uint symbol_off;
  uint cref;
};

srtuct PDB_GsiHashRecordOffsetCalc
{
  uint next;
  uint off;
  uint cref;
};

srtuct PDB_PsiHeader
{
  uint sym_hash_size;
  uint addr_map_size;
  uint thunk_count;
  uint thunk_size;
  CV_SectionIndex isec_thunk_table;
  ushort padding;
  uint sec_thunk_table_off;
  uint sec_count;
};

////////////////////////////////

uint pdb_hash_v1(String8 string);

#endif // PDB_H

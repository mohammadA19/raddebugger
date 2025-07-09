// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ PDB Parser String Table Types

typedef struct PDB_Strtbl
{
  String8 data;
  u32 bucket_count;
  u32 strblock_min;
  u32 strblock_max;
  u32 buckets_min;
  u32 buckets_max;
} PDB_Strtbl;

////////////////////////////////
//~ PDB Parser Info Types

typedef enum PDB_NamedStream
{
  PDB_NamedStream_HeaderBlock,
  PDB_NamedStream_StringTable,
  PDB_NamedStream_LinkInfo,
  PDB_NamedStream_Count
} PDB_NamedStream;

typedef struct PDB_NamedStreamTable
{
  MSF_StreamNumber sn[PDB_NamedStream_Count];
} PDB_NamedStreamTable;

typedef struct PDB_InfoNode
{
  struct PDB_InfoNode *next;
  String8 string;
  MSF_StreamNumber sn;
} PDB_InfoNode;

typedef struct PDB_Info
{
  PDB_InfoNode *first;
  PDB_InfoNode *last;
  Guid auth_guid;
  PDB_FeatureFlags features;
} PDB_Info;

typedef struct PDB_InfoHeader
{
  PDB_InfoVersion version;
  u32 time;
  u32 age;
} PDB_InfoHeader;

////////////////////////////////
//~ PDB Parser DBI Types

//  (this is not "literally" defined by the format - but helpful to have)
typedef enum PDB_DbiRange
{
  PDB_DbiRange_ModuleInfo,
  PDB_DbiRange_SecCon,
  PDB_DbiRange_SecMap,
  PDB_DbiRange_FileInfo,
  PDB_DbiRange_TSM,
  PDB_DbiRange_EcInfo,
  PDB_DbiRange_DbgHeader,
  PDB_DbiRange_COUNT
} PDB_DbiRange;

//  (this is not "literally" defined by the format - but helpful to have)
typedef enum
{
  PDB_DbiCompUnitRange_Symbols,
  PDB_DbiCompUnitRange_C11,
  PDB_DbiCompUnitRange_C13,
  PDB_DbiCompUnitRange_COUNT
} PDB_DbiCompUnitRange;

typedef struct PDB_DbiParsed
{
  String8 data;
  COFF_MachineType machine_type;
  MSF_StreamNumber gsi_sn;
  MSF_StreamNumber psi_sn;
  MSF_StreamNumber sym_sn;
  u64 range_off[(u64)(PDB_DbiRange_COUNT) + 1];
  MSF_StreamNumber dbg_streams[PDB_DbiStream_COUNT];
} PDB_DbiParsed;

typedef struct PDB_CompUnit
{
  MSF_StreamNumber sn;
  u32 range_off[(u32)(PDB_DbiCompUnitRange_COUNT) + 1];
  
  String8 obj_name;
  String8 group_name;
} PDB_CompUnit;

typedef struct PDB_CompUnitNode
{
  struct PDB_CompUnitNode *next;
  PDB_CompUnit unit;
} PDB_CompUnitNode;

typedef struct PDB_CompUnitArray
{
  PDB_CompUnit **units;
  u64 count;
} PDB_CompUnitArray;

typedef struct PDB_CompUnitContribution
{
  u32 mod;
  u64 voff_first;
  u64 voff_opl;
} PDB_CompUnitContribution;

typedef struct PDB_CompUnitContributionArray
{
  PDB_CompUnitContribution *contributions;
  u64 count;
} PDB_CompUnitContributionArray;

////////////////////////////////
//~ PDB Parser TPI/IPI Types

typedef struct PDB_TpiParsed
{
  String8 data;
  
  // leaf info
  u64 leaf_first;
  u64 leaf_opl;
  u32 itype_first;
  u32 itype_opl;
  
  // hash info
  MSF_StreamNumber hash_sn;
  MSF_StreamNumber hash_sn_aux;
  u32 hash_key_size;
  u32 hash_bucket_count;
  u32 hash_vals_off;
  u32 hash_vals_size;
  u32 itype_off;
  u32 itype_size;
  u32 hash_adj_off;
  u32 hash_adj_size;
  
} PDB_TpiParsed;

typedef struct PDB_TpiHashBlock
{
  struct PDB_TpiHashBlock *next;
  u32 local_count;
  CV_TypeId itypes[13]; // 13 = (64 - 12)/4
} PDB_TpiHashBlock;

typedef struct PDB_TpiHashParsed
{
  String8 data;
  String8 aux_data;
  
  PDB_TpiHashBlock **buckets;
  u32 bucket_count;
  u32 bucket_mask;
} PDB_TpiHashParsed;

////////////////////////////////
//~ PDB Parser GSI Types

typedef struct PDB_GsiBucket
{
  u32 *offs;
  u64 count;
} PDB_GsiBucket;

typedef struct PDB_GsiParsed
{
  PDB_GsiBucket buckets[4096];
} PDB_GsiParsed;

////////////////////////////////
//~ PDB Parser Functions

internal PDB_TpiHashParsed*   pdb_tpi_hash_from_data(Arena *arena,
                                                     PDB_Strtbl *strtbl,
                                                     PDB_TpiParsed *tpi,
                                                     String8 tpi_hash_data,
                                                     String8 tpi_hash_aux_data);

internal PDB_CompUnitArray*   pdb_comp_unit_array_from_data(Arena *arena,
                                                            String8 module_info_data);

internal PDB_CompUnitContributionArray*
pdb_comp_unit_contribution_array_from_data(Arena *arena, String8 seccontrib_data,
                                           COFF_SectionHeaderArray sections);

////////////////////////////////
//~ PDB Dbi Functions

internal String8              pdb_data_from_unit_range(MSF_Parsed *msf, PDB_CompUnit *unit,
                                                       PDB_DbiCompUnitRange range);

////////////////////////////////
//~ PDB Tpi Functions

internal CV_TypeIdArray       pdb_tpi_itypes_from_name(Arena *arena,
                                                       PDB_TpiHashParsed *tpi_hash,
                                                       CV_LeafParsed *tpi_leaf,
                                                       String8 name,
                                                       b32 compare_unique_name,
                                                       u32 output_cap);

internal CV_TypeId            pdb_tpi_first_itype_from_name(PDB_TpiHashParsed *tpi_hash,
                                                            CV_LeafParsed *tpi_leaf,
                                                            String8 name,
                                                            b32 compare_unique_name);

////////////////////////////////
//~ PDB Strtbl Functions

internal String8              pdb_strtbl_string_from_index(PDB_Strtbl *strtbl,
                                                           PDB_StringIndex idx);

////////////////////////////////
//~ rjf: Thin Lookup Fast Paths



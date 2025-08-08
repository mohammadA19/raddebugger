// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PDB_PARSE_H
#define PDB_PARSE_H

////////////////////////////////
//~ PDB Parser String Table Types

typedef struct PDB_Strtbl
{
    StringView data;
    U32 bucket_count;
    U32 strblock_min;
    U32 strblock_max;
    U32 buckets_min;
    U32 buckets_max;
} PDB_Strtbl;

////////////////////////////////
//~ PDB Parser Info Types

enum PDB_NamedStream
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
    StringView string;
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
    U32 time;
    U32 age;
} PDB_InfoHeader;

////////////////////////////////
//~ PDB Parser DBI Types

//  (this is not "literally" defined by the format - but helpful to have)
enum PDB_DbiRange
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
enum
{
    PDB_DbiCompUnitRange_Symbols,
    PDB_DbiCompUnitRange_C11,
    PDB_DbiCompUnitRange_C13,
    PDB_DbiCompUnitRange_COUNT
} PDB_DbiCompUnitRange;

typedef struct PDB_DbiParsed
{
    StringView data;
    COFF_MachineType machine_type;
    MSF_StreamNumber gsi_sn;
    MSF_StreamNumber psi_sn;
    MSF_StreamNumber sym_sn;
    U64 range_off[(U64)(PDB_DbiRange_COUNT) + 1];
    MSF_StreamNumber dbg_streams[PDB_DbiStream_COUNT];
} PDB_DbiParsed;

typedef struct PDB_CompUnit
{
    MSF_StreamNumber sn;
    U32 range_off[(U32)(PDB_DbiCompUnitRange_COUNT) + 1];
    
    StringView obj_name;
    StringView group_name;
} PDB_CompUnit;

typedef struct PDB_CompUnitNode
{
    struct PDB_CompUnitNode *next;
    PDB_CompUnit unit;
} PDB_CompUnitNode;

typedef struct PDB_CompUnitArray
{
    PDB_CompUnit **units;
    U64 count;
} PDB_CompUnitArray;

typedef struct PDB_CompUnitContribution
{
    U32 mod;
    U64 voff_first;
    U64 voff_opl;
} PDB_CompUnitContribution;

typedef struct PDB_CompUnitContributionArray
{
    PDB_CompUnitContribution *contributions;
    U64 count;
} PDB_CompUnitContributionArray;


////////////////////////////////
//~ PDB Parser TPI/IPI Types

typedef struct PDB_TpiParsed
{
    StringView data;
    
    // leaf info
    U64 leaf_first;
    U64 leaf_opl;
    U32 itype_first;
    U32 itype_opl;
    
    // hash info
    MSF_StreamNumber hash_sn;
    MSF_StreamNumber hash_sn_aux;
    U32 hash_key_size;
    U32 hash_bucket_count;
    U32 hash_vals_off;
    U32 hash_vals_size;
    U32 itype_off;
    U32 itype_size;
    U32 hash_adj_off;
    U32 hash_adj_size;
    
} PDB_TpiParsed;

typedef struct PDB_TpiHashBlock
{
    struct PDB_TpiHashBlock *next;
    U32 local_count;
    CV_TypeId itypes[13]; // 13 = (64 - 12)/4
} PDB_TpiHashBlock;

typedef struct PDB_TpiHashParsed
{
    StringView data;
    StringView aux_data;
    
    PDB_TpiHashBlock **buckets;
    U32 bucket_count;
    U32 bucket_mask;
} PDB_TpiHashParsed;

////////////////////////////////
//~ PDB Parser GSI Types

typedef struct PDB_GsiBucket
{
    U32 *offs;
    U64 count;
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
                                                                                                          StringView tpi_hash_data,
                                                                                                          StringView tpi_hash_aux_data);


internal PDB_CompUnitArray*   pdb_comp_unit_array_from_data(Arena *arena,
                                                                                                                        StringView module_info_data);

internal PDB_CompUnitContributionArray*
pdb_comp_unit_contribution_array_from_data(Arena *arena, StringView seccontrib_data,
                                                                                      COFF_SectionHeaderArray sections);

////////////////////////////////
//~ PDB Dbi Functions

internal StringView              pdb_data_from_unit_range(MSF_Parsed *msf, PDB_CompUnit *unit,
                                                                                                              PDB_DbiCompUnitRange range);

////////////////////////////////
//~ PDB Tpi Functions


internal CV_TypeIdArray       pdb_tpi_itypes_from_name(Arena *arena,
                                                                                                              PDB_TpiHashParsed *tpi_hash,
                                                                                                              CV_LeafParsed *tpi_leaf,
                                                                                                              StringView name,
                                                                                                              B32 compare_unique_name,
                                                                                                              U32 output_cap);

internal CV_TypeId            pdb_tpi_first_itype_from_name(PDB_TpiHashParsed *tpi_hash,
                                                                                                                        CV_LeafParsed *tpi_leaf,
                                                                                                                        StringView name,
                                                                                                                        B32 compare_unique_name);

////////////////////////////////
//~ PDB Strtbl Functions

internal StringView              pdb_strtbl_string_from_index(PDB_Strtbl *strtbl,
                                                                                                                      PDB_StringIndex idx);

////////////////////////////////
//~ rjf: Thin Lookup Fast Paths


#endif // PDB_PARSE_H

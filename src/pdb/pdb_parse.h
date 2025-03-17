// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PDB_PARSE_H
#define PDB_PARSE_H

////////////////////////////////
//~ PDB Parser String Table Types

struct PDB_Strtbl
{
  StringView data;
  uint32 bucket_count;
  uint32 strblock_min;
  uint32 strblock_max;
  uint32 buckets_min;
  uint32 buckets_max;
}

////////////////////////////////
//~ PDB Parser Info Types

enum PDB_NamedStream
{
  PDB_NamedStream_HeaderBlock,
  PDB_NamedStream_StringTable,
  PDB_NamedStream_LinkInfo,
  PDB_NamedStream_Count
}

struct PDB_NamedStreamTable
{
  MSF_StreamNumber sn[PDB_NamedStream_Count];
}

struct PDB_InfoNode
{
  struct PDB_InfoNode* next;
  StringView str;
  MSF_StreamNumber sn;
}

struct PDB_Info
{
  PDB_InfoNode* first;
  PDB_InfoNode* last;
  Guid auth_guid;
}

struct PDB_InfoHeader
{
  PDB_InfoVersion version;
  uint32 time;
  uint32 age;
}

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
}

//  (this is not "literally" defined by the format - but helpful to have)
enum
{
  PDB_DbiCompUnitRange_Symbols,
  PDB_DbiCompUnitRange_C11,
  PDB_DbiCompUnitRange_C13,
  PDB_DbiCompUnitRange_COUNT
}

struct PDB_DbiParsed
{
  StringView data;
  COFF_MachineType machine_type;
  MSF_StreamNumber gsi_sn;
  MSF_StreamNumber psi_sn;
  MSF_StreamNumber sym_sn;
  uint64 range_off[(uint64)(PDB_DbiRange_COUNT) + 1];
  MSF_StreamNumber dbg_streams[PDB_DbiStream_COUNT];
}

struct PDB_CompUnit
{
  MSF_StreamNumber sn;
  uint32 range_off[(uint32)(PDB_DbiCompUnitRange_COUNT) + 1];
  
  StringView obj_name;
  StringView group_name;
}

struct PDB_CompUnitNode
{
  struct PDB_CompUnitNode* next;
  PDB_CompUnit unit;
}

struct PDB_CompUnitArray
{
  PDB_CompUnit** units;
  uint64 count;
}

struct PDB_CompUnitContribution
{
  uint32 mod;
  uint64 voff_first;
  uint64 voff_opl;
}

struct PDB_CompUnitContributionArray
{
  PDB_CompUnitContribution* contributions;
  uint64 count;
}


////////////////////////////////
//~ PDB Parser TPI/IPI Types

struct PDB_TpiParsed
{
  StringView data;
  
  // leaf info
  uint64 leaf_first;
  uint64 leaf_opl;
  uint32 itype_first;
  uint32 itype_opl;
  
  // hash info
  MSF_StreamNumber hash_sn;
  MSF_StreamNumber hash_sn_aux;
  uint32 hash_key_size;
  uint32 hash_bucket_count;
  uint32 hash_vals_off;
  uint32 hash_vals_size;
  uint32 itype_off;
  uint32 itype_size;
  uint32 hash_adj_off;
  uint32 hash_adj_size;
  
}

struct PDB_TpiHashBlock
{
  struct PDB_TpiHashBlock* next;
  uint32 local_count;
  CV_TypeId itypes[13]; // 13 = (64 - 12)/4
}

struct PDB_TpiHashParsed
{
  StringView data;
  StringView aux_data;
  
  PDB_TpiHashBlock** buckets;
  uint32 bucket_count;
  uint32 bucket_mask;
}

////////////////////////////////
//~ PDB Parser GSI Types

struct PDB_GsiBucket
{
  uint32* offs;
  uint64 count;
}

struct PDB_GsiParsed
{
  PDB_GsiBucket buckets[4096];
}

////////////////////////////////
//~ PDB Parser Functions

PDB_Info*            pdb_info_from_data(Arena* arena, StringView pdb_info_data);
PDB_NamedStreamTable*pdb_named_stream_table_from_info(Arena* arena, PDB_Info* info);
PDB_Strtbl*          pdb_strtbl_from_data(Arena* arena, StringView strtbl_data);

PDB_DbiParsed*       pdb_dbi_from_data(Arena* arena, StringView dbi_data);
PDB_TpiParsed*       pdb_tpi_from_data(Arena* arena, StringView tpi_data);
PDB_TpiHashParsed*   pdb_tpi_hash_from_data(Arena* arena,
                                                     PDB_Strtbl* strtbl,
                                                     PDB_TpiParsed* tpi,
                                                     StringView tpi_hash_data,
                                                     StringView tpi_hash_aux_data);
PDB_GsiParsed*       pdb_gsi_from_data(Arena* arena, StringView gsi_data);
uint64                  pdb_gsi_symbol_from_string(PDB_GsiParsed* gsi, StringView symbol_data, StringView str);

COFF_SectionHeaderArray pdb_coff_section_array_from_data(Arena* arena, StringView section_data);

PDB_CompUnitArray*   pdb_comp_unit_array_from_data(Arena* arena,
                                                            StringView module_info_data);

PDB_CompUnitContributionArray*
pdb_comp_unit_contribution_array_from_data(Arena* arena, StringView seccontrib_data,
                                           COFF_SectionHeaderArray sections);

////////////////////////////////
//~ PDB Dbi Functions

StringView              pdb_data_from_dbi_range(PDB_DbiParsed* dbi, PDB_DbiRange range);
StringView              pdb_data_from_unit_range(MSF_Parsed* msf, PDB_CompUnit* unit,
                                                       PDB_DbiCompUnitRange range);

////////////////////////////////
//~ PDB Tpi Functions

StringView              pdb_leaf_data_from_tpi(PDB_TpiParsed* tpi);

CV_TypeIdArray       pdb_tpi_itypes_from_name(Arena* arena,
                                                       PDB_TpiHashParsed* tpi_hash,
                                                       CV_LeafParsed* tpi_leaf,
                                                       StringView name,
                                                       B32 compare_unique_name,
                                                       uint32 output_cap);

CV_TypeId            pdb_tpi_first_itype_from_name(PDB_TpiHashParsed* tpi_hash,
                                                            CV_LeafParsed* tpi_leaf,
                                                            StringView name,
                                                            B32 compare_unique_name);

////////////////////////////////
//~ PDB Strtbl Functions

StringView              pdb_strtbl_string_from_off(PDB_Strtbl* strtbl, uint32 off);
StringView              pdb_strtbl_string_from_index(PDB_Strtbl* strtbl,
                                                           PDB_StringIndex idx);
uint32                  pdb_strtbl_off_from_string(PDB_Strtbl* strtbl, StringView str);


#endif // PDB_PARSE_H


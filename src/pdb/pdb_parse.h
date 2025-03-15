// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PDB_PARSE_H
#define PDB_PARSE_H

////////////////////////////////
//~ PDB Parser String Table Types

srtuct PDB_Strtbl
{
  String8 data;
  U32 bucket_count;
  U32 strblock_min;
  U32 strblock_max;
  U32 buckets_min;
  U32 buckets_max;
}

////////////////////////////////
//~ PDB Parser Info Types

PDB_NamedStream :: enum {
  PDB_NamedStream_HeaderBlock,
  PDB_NamedStream_StringTable,
  PDB_NamedStream_LinkInfo,
  PDB_NamedStream_Count
}

srtuct PDB_NamedStreamTable
{
  MSF_StreamNumber sn[PDB_NamedStream_Count];
}

srtuct PDB_InfoNode
{
  struct PDB_InfoNode *next;
  String8 string;
  MSF_StreamNumber sn;
}

srtuct PDB_Info
{
  PDB_InfoNode *first;
  PDB_InfoNode *last;
  Guid auth_guid;
}

srtuct PDB_InfoHeader
{
  PDB_InfoVersion version;
  U32 time;
  U32 age;
}

////////////////////////////////
//~ PDB Parser DBI Types

//  (this is not "literally" defined by the format - but helpful to have)
PDB_DbiRange :: enum {
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

srtuct PDB_DbiParsed
{
  String8 data;
  COFF_MachineType machine_type;
  MSF_StreamNumber gsi_sn;
  MSF_StreamNumber psi_sn;
  MSF_StreamNumber sym_sn;
  U64 range_off[(U64)(PDB_DbiRange_COUNT) + 1];
  MSF_StreamNumber dbg_streams[PDB_DbiStream_COUNT];
}

srtuct PDB_CompUnit
{
  MSF_StreamNumber sn;
  U32 range_off[(U32)(PDB_DbiCompUnitRange_COUNT) + 1];
  
  String8 obj_name;
  String8 group_name;
}

srtuct PDB_CompUnitNode
{
  struct PDB_CompUnitNode *next;
  PDB_CompUnit unit;
}

srtuct PDB_CompUnitArray
{
  PDB_CompUnit **units;
  U64 count;
}

srtuct PDB_CompUnitContribution
{
  U32 mod;
  U64 voff_first;
  U64 voff_opl;
}

srtuct PDB_CompUnitContributionArray
{
  PDB_CompUnitContribution *contributions;
  U64 count;
}


////////////////////////////////
//~ PDB Parser TPI/IPI Types

srtuct PDB_TpiParsed
{
  String8 data;
  
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
  
}

srtuct PDB_TpiHashBlock
{
  struct PDB_TpiHashBlock *next;
  U32 local_count;
  CV_TypeId itypes[13]; // 13 = (64 - 12)/4
}

srtuct PDB_TpiHashParsed
{
  String8 data;
  String8 aux_data;
  
  PDB_TpiHashBlock **buckets;
  U32 bucket_count;
  U32 bucket_mask;
}

////////////////////////////////
//~ PDB Parser GSI Types

srtuct PDB_GsiBucket
{
  U32 *offs;
  U64 count;
}

srtuct PDB_GsiParsed
{
  PDB_GsiBucket buckets[4096];
}

////////////////////////////////
//~ PDB Parser Functions

PDB_Info*            pdb_info_from_data(Arena *arena, String8 pdb_info_data);
PDB_NamedStreamTable*pdb_named_stream_table_from_info(Arena *arena, PDB_Info *info);
PDB_Strtbl*          pdb_strtbl_from_data(Arena *arena, String8 strtbl_data);

PDB_DbiParsed*       pdb_dbi_from_data(Arena *arena, String8 dbi_data);
PDB_TpiParsed*       pdb_tpi_from_data(Arena *arena, String8 tpi_data);
PDB_TpiHashParsed*   pdb_tpi_hash_from_data(Arena *arena,
                                                     PDB_Strtbl *strtbl,
                                                     PDB_TpiParsed *tpi,
                                                     String8 tpi_hash_data,
                                                     String8 tpi_hash_aux_data);
PDB_GsiParsed*       pdb_gsi_from_data(Arena *arena, String8 gsi_data);
U64                  pdb_gsi_symbol_from_string(PDB_GsiParsed *gsi, String8 symbol_data, String8 string);

COFF_SectionHeaderArray pdb_coff_section_array_from_data(Arena *arena, String8 section_data);

PDB_CompUnitArray*   pdb_comp_unit_array_from_data(Arena *arena,
                                                            String8 module_info_data);

PDB_CompUnitContributionArray*
pdb_comp_unit_contribution_array_from_data(Arena *arena, String8 seccontrib_data,
                                           COFF_SectionHeaderArray sections);

////////////////////////////////
//~ PDB Dbi Functions

String8              pdb_data_from_dbi_range(PDB_DbiParsed *dbi, PDB_DbiRange range);
String8              pdb_data_from_unit_range(MSF_Parsed *msf, PDB_CompUnit *unit,
                                                       PDB_DbiCompUnitRange range);

////////////////////////////////
//~ PDB Tpi Functions

String8              pdb_leaf_data_from_tpi(PDB_TpiParsed *tpi);

CV_TypeIdArray       pdb_tpi_itypes_from_name(Arena *arena,
                                                       PDB_TpiHashParsed *tpi_hash,
                                                       CV_LeafParsed *tpi_leaf,
                                                       String8 name,
                                                       B32 compare_unique_name,
                                                       U32 output_cap);

CV_TypeId            pdb_tpi_first_itype_from_name(PDB_TpiHashParsed *tpi_hash,
                                                            CV_LeafParsed *tpi_leaf,
                                                            String8 name,
                                                            B32 compare_unique_name);

////////////////////////////////
//~ PDB Strtbl Functions

String8              pdb_strtbl_string_from_off(PDB_Strtbl *strtbl, U32 off);
String8              pdb_strtbl_string_from_index(PDB_Strtbl *strtbl,
                                                           PDB_StringIndex idx);
U32                  pdb_strtbl_off_from_string(PDB_Strtbl *strtbl, String8 string);


#endif // PDB_PARSE_H


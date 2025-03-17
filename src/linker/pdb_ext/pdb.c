// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

uint32
pdb_hash_udt(CV_UDTInfo udt_info, StringView data)
{
  B32 is_fwdref       = !!(udt_info.props & CV_TypeProp_FwdRef);
  B32 is_scoped       = !!(udt_info.props & CV_TypeProp_Scoped);
  B32 has_unique_name = !!(udt_info.props & CV_TypeProp_HasUniqueName);
  B32 is_anon         = has_unique_name && cv_is_udt_name_anon(udt_info.name);
  
  uint32 hash = 0;
  // dbi/tpi.cpp:1918
  if (!is_fwdref && !is_scoped && !is_anon) {
    hash = pdb_hash_v1(udt_info.name);
  }
  // dbi/tpi.cpp:1937
  else if (!is_fwdref && has_unique_name && is_scoped && !is_anon) {
    hash = pdb_hash_v1(udt_info.unique_name);
  }
  // dbi/tpi.cpp 1338
  else {
    hash = pdb_hash_v1(data);
  }
  
  return hash;
}

uint32
pdb_crc32_from_string(StringView str)
{
  return ~update_crc32(~0, str.Ptr, str.Length);
}


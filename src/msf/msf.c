// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal b32
msf_check_magic_20(String8 data)
{
  b32 is_msf = data.size >= sizeof(msf_msf20_magic) && str8_match(data, str8_lit(msf_msf20_magic), StringMatchFlag_RightSideSloppy);
  return is_msf;
}

internal b32
msf_check_magic_70(String8 data)
{
  b32 is_msf = data.size >= sizeof(msf_msf70_magic) && str8_match(data, str8_lit(msf_msf70_magic), StringMatchFlag_RightSideSloppy);
  return is_msf;
}

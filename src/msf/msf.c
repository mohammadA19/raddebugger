// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal B32
msf_check_magic_20(StringView data)
{
    B32 is_msf = data.size >= sizeof(msf_msf20_magic) && str8_match(data, (msf_msf20_magic), StringMatchFlag_RightSideSloppy);
    return is_msf;
}

internal B32
msf_check_magic_70(StringView data)
{
    B32 is_msf = data.size >= sizeof(msf_msf70_magic) && str8_match(data, (msf_msf70_magic), StringMatchFlag_RightSideSloppy);
    return is_msf;
}

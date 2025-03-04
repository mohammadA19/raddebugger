// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

srtuct MD5Hash
{
  U8 value[16];
};

internal MD5Hash md5_hash_from_string(String8 data);


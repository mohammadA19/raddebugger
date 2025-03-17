// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

uint32 update_crc32(uint32 crc, uint8* ptr, uint64 size);
uint32 crc32_from_string(StringView string);


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define MemoryCopyStr8(dst, s) MemoryCopy(dst, (s).str, (s).size)


#define str8_list_push_struct(a,l,d) str8_list_push_raw(a, l, d, sizeof(*d))

// TODO: remove



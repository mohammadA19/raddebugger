// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once 

StringView make_file_name_with_ext(Arena* arena, StringView file_name, StringView ext);
StringView path_convert_slashes(Arena* arena, StringView path, PathStyle path_style);
StringView path_canon_from_regular_path(Arena* arena, StringView path);
PathStyle path_style_from_string(StringView string);


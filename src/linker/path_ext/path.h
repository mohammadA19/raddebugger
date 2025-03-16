// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once 

String8 make_file_name_with_ext(Arena* arena, String8 file_name, String8 ext);
String8 path_convert_slashes(Arena* arena, String8 path, PathStyle path_style);
String8 path_canon_from_regular_path(Arena* arena, String8 path);
PathStyle path_style_from_string(String8 string);


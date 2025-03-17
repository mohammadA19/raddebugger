// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PATH_H
#define PATH_H

////////////////////////////////
//~ allen: Path Helper Functions

StringMatchFlags path_match_flags_from_os(OperatingSystem os);
StringView path_relative_dst_from_absolute_dst_src(Arena* arena, StringView dst, StringView src);
StringView path_absolute_dst_from_relative_dst_src(Arena* arena, StringView dst, StringView src);
String8List path_normalized_list_from_string(Arena* arena, StringView path, PathStyle* style_out);
StringView path_normalized_from_string(Arena* arena, StringView path);
B32 path_match_normalized(StringView left, StringView right);

#endif //PATH_H

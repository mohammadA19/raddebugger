// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

StringView
make_file_name_with_ext(Arena* arena, StringView file_name, StringView ext)
{
  StringView file_name_no_ext = str8_chop_last_dot(file_name);
  StringView result           = push_str8f(arena, "%S.%S", file_name_no_ext, ext);
  return result;
}

StringView
path_char_from_style(PathStyle style)
{
  StringView result = StringView();
  switch (style) {
  case PathStyle_Null:     break;
  case PathStyle_Relative: break;
  case PathStyle_WindowsAbsolute: result = ("\\"); break;
  case PathStyle_UnixAbsolute:    result = ("/");  break;
  }
  return result;
}

StringView
path_convert_slashes(Arena* arena, StringView path, PathStyle path_style)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = str8_split_path(scratch.arena, path);
  StringJoin join = {0};
  join.sep = path_char_from_style(path_style);
  StringView result = str8_list_join(arena, &list, &join);
  scratch_end(scratch);
  return result;
}

StringView
path_canon_from_regular_path(Arena* arena, StringView path)
{
  Temp scratch = scratch_begin(&arena, 1);
  StringView result;
  result = lower_from_str8(scratch.arena, path);
  result = path_convert_slashes(arena, result, PathStyle_UnixAbsolute);
  scratch_end(scratch);
  return result;
}

struct {
  StringView   string;
  PathStyle path_style;
} g_path_style_map[] = {
  { ("windows"), PathStyle_WindowsAbsolute },
  { ("unix"),    PathStyle_UnixAbsolute    },
  { ("system"),  PathStyle_SystemAbsolute  },
};

PathStyle
path_style_from_string(StringView string)
{
  for (uint64 i = 0; i < ArrayCount(g_path_style_map); ++i) {
    if (str8_match(g_path_style_map[i].string, string, StringMatchFlag_CaseInsensitive)) {
      return g_path_style_map[i].path_style;
    }
  }
  return PathStyle_Null;
}


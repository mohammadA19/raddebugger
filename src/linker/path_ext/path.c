// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal String8
make_file_name_with_ext(Arena *arena, String8 file_name, String8 ext)
{
  String8 file_name_no_ext = str8_chop_last_dot(file_name);
  String8 result           = push_str8f(arena, "%S.%S", file_name_no_ext, ext);
  return result;
}

internal String8
path_char_from_style(PathStyle style)
{
  String8 result = str8_zero();
  switch (style) {
  case PathStyle.Null:     break;
  case PathStyle.Relative: break;
  case PathStyle.WindowsAbsolute: result = ("\\"); break;
  case PathStyle.UnixAbsolute:    result = ("/");  break;
  }
  return result;
}

internal String8
path_convert_slashes(Arena *arena, String8 path, PathStyle path_style)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = str8_split_path(scratch.arena, path);
  StringJoin join = {0};
  join.sep = path_char_from_style(path_style);
  String8 result = str8_list_join(arena, &list, &join);
  scratch_end(scratch);
  return result;
}

internal String8
path_canon_from_regular_path(Arena *arena, String8 path)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8 result;
  result = lower_from_str8(scratch.arena, path);
  result = path_convert_slashes(arena, result, PathStyle.UnixAbsolute);
  scratch_end(scratch);
  return result;
}

struct {
  String8   string;
  PathStyle path_style;
} g_path_style_map[] = {
  { ("windows"), PathStyle.WindowsAbsolute },
  { ("unix"),    PathStyle.UnixAbsolute    },
  { ("system"),  PathStyle.SystemAbsolute  },
};

internal PathStyle
path_style_from_string(String8 string)
{
  for (U64 i = 0; i < ArrayCount(g_path_style_map); ++i) {
    if (str8_match(g_path_style_map[i].string, string, StringMatchFlags.CaseInsensitive)) {
      return g_path_style_map[i].path_style;
    }
  }
  return PathStyle.Null;
}


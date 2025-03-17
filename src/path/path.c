// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ allen: Path Helper Functions

StringMatchFlags
path_match_flags_from_os(OperatingSystem os)
{
  StringMatchFlags flags = StringMatchFlag_SlashInsensitive;
  switch(os)
  {
    default:{}break;
    case OperatingSystem_Windows:
    {
      flags |= StringMatchFlag_CaseInsensitive;
    }break;
    case OperatingSystem_Linux:
    case OperatingSystem_Mac:
    {
      // NOTE(rjf): no-op
    }break;
  }
  return flags;
}

StringView
path_relative_dst_from_absolute_dst_src(Arena* arena, StringView dst, StringView src)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  // rjf: gather path parts
  StringView dst_name = str8_skip_last_slash(dst);
  StringView src_folder = str8_chop_last_slash(src);
  StringView dst_folder = str8_chop_last_slash(dst);
  String8List src_folders = str8_split_path(scratch.arena, src_folder);
  String8List dst_folders = str8_split_path(scratch.arena, dst_folder);
  
  // rjf: count # of backtracks to get from src . dest
  uint64 num_backtracks = src_folders.node_count;
  for(String8Node* src_n = src_folders.first, *bp_n = dst_folders.first;
      src_n != 0 && bp_n != 0;
      src_n = src_n.next, bp_n = bp_n.next)
  {
    if(str8_match(src_n.str, bp_n.str, path_match_flags_from_os(operating_system_from_context())))
    {
      num_backtracks -= 1;
    }
    else
    {
      break;
    }
  }
  
  // rjf: only build relative str if # of backtracks is not the entire `src`.
  // if getting to `dst` from `src` requires erasing the entire `src`, then the
  // only possible way to get to `dst` from `src` is via absolute path.
  StringView dst_path = {0};
  if(num_backtracks >= src_folders.node_count)
  {
    dst_path = path_normalized_from_string(arena, dst);
  }
  else
  {
    // rjf: build backtrack parts
    String8List dst_path_strs = {0};
    for(uint64 idx = 0; idx < num_backtracks; idx += 1)
    {
      str8_list_push(scratch.arena, &dst_path_strs, (".."));
    }
    
    // rjf: build parts of dst which are unique from src
    {
      B32 unique_from_src = 0;
      for(String8Node* src_n = src_folders.first, *bp_n = dst_folders.first;
          bp_n != 0;
          bp_n = bp_n.next)
      {
        if(!unique_from_src && (src_n == 0 || !str8_match(src_n.str, bp_n.str, path_match_flags_from_os(operating_system_from_context()))))
        {
          unique_from_src = 1;
        }
        if(unique_from_src)
        {
          str8_list_push(scratch.arena, &dst_path_strs, bp_n.str);
        }
        if(src_n != 0)
        {
          src_n = src_n.next;
        }
      }
    }
    
    // rjf: build file name
    str8_list_push(scratch.arena, &dst_path_strs, dst_name);
    
    // rjf: join
    StringJoin join = {0};
    {
      join.sep = ("/");
    }
    dst_path = str8_list_join(arena, &dst_path_strs, &join);
  }
  scratch_end(scratch);
  return dst_path;
}

StringView
path_absolute_dst_from_relative_dst_src(Arena* arena, StringView dst, StringView src)
{
  StringView result = dst;
  PathStyle dst_style = path_style_from_str8(dst);
  if(dst_style == PathStyle_Relative)
  {
    Temp scratch = scratch_begin(&arena, 1);
    StringView dst_from_src_absolute = push_str8f(scratch.arena, "%S/%S", src, dst);
    StringView dst_from_src_absolute_normalized = path_normalized_from_string(arena, dst_from_src_absolute);
    result = dst_from_src_absolute_normalized;
    scratch_end(scratch);
  }
  return result;
}

String8List
path_normalized_list_from_string(Arena* arena, StringView path_string, PathStyle* style_out){
  // analyze path
  PathStyle path_style = path_style_from_str8(path_string);
  String8List path = str8_split_path(arena, path_string);
  
  // prepend current path to convert relative . absolute
  PathStyle path_style_full = path_style;
  if (path.node_count != 0 && path_style == PathStyle_Relative){
    StringView current_path_string = os_get_current_path(arena);
    
    PathStyle current_path_style = path_style_from_str8(current_path_string);
    Assert(current_path_style != PathStyle_Relative);
    
    String8List current_path = str8_split_path(arena, current_path_string);
    str8_list_concat_in_place(&current_path, &path);
    path = current_path;
    path_style_full = current_path_style;
  }
  
  // resolve dots
  str8_path_list_resolve_dots_in_place(&path, path_style_full);
  
  // return
  if (style_out != 0){
    *style_out = path_style_full;
  }
  return(path);
}

StringView
path_normalized_from_string(Arena* arena, StringView path_string){
  Temp scratch = scratch_begin(&arena, 1);
  
  PathStyle style = PathStyle_Relative;
  String8List path = path_normalized_list_from_string(scratch.arena, path_string, &style);
  
  StringView result = str8_path_list_join_by_style(arena, &path, style);
  scratch_end(scratch);
  return(result);
}

B32
path_match_normalized(StringView left, StringView right)
{
  B32 result = 0;
  {
    Temp scratch = scratch_begin(0, 0);
    StringView left_normalized = path_normalized_from_string(scratch.arena, left);
    StringView right_normalized = path_normalized_from_string(scratch.arena, right);
    result = str8_match(left_normalized, right_normalized, StringMatchFlag_CaseInsensitive);
    scratch_end(scratch);
  }
  return result;
}

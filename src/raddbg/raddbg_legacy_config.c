// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal RD_CfgList
rd_cfg_tree_list_from_string__pre_0_9_16(Arena *arena, string file_path, string data)
{
  RD_CfgList result = {0};
  {
    Temp scratch = scratch_begin(&arena, 1);
    string folder_path = str8_skip_last_slash(file_path);
    MD_Node *src_root = md_parse_from_text(scratch.arena, file_path, data).root;
    {
      for MD_EachNode(tln, src_root.first)
      {
        //- rjf: targets
        if (str8_match(tln.string, ("target"), 0))
        {
          string disabled_string   = md_child_from_string(tln, ("disabled"), 0)->first.string;
          string executable        = md_child_from_string(tln, ("executable"), 0)->first.string;
          string arguments         = md_child_from_string(tln, ("arguments"), 0)->first.string;
          string working_directory = md_child_from_string(tln, ("working_directory"), 0)->first.string;
          string entry_point       = md_child_from_string(tln, ("entry_point"), 0)->first.string;
          string stdout_path       = md_child_from_string(tln, ("stdout_path"), 0)->first.string;
          string stderr_path       = md_child_from_string(tln, ("stderr_path"), 0)->first.string;
          string stdin_path        = md_child_from_string(tln, ("stdin_path"), 0)->first.string;
          string debug_subprocesses= md_child_from_string(tln, ("debug_subprocesses"), 0)->first.string;
          RD_Cfg *dst_root = rd_cfg_new(&rd_nil_cfg, ("target"));
          rd_cfg_list_push(arena, &result, dst_root);
          {
            if (executable.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("executable")), path_absolute_dst_from_relative_dst_src(scratch.arena, executable, folder_path)); }
            if (arguments.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("arguments")), raw_from_escaped_str8(scratch.arena, arguments)); }
            if (working_directory.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("working_directory")), path_absolute_dst_from_relative_dst_src(scratch.arena, working_directory, folder_path)); }
            if (entry_point.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("entry_point")), raw_from_escaped_str8(scratch.arena, entry_point)); }
            if (stdout_path.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("stdout_path")), path_absolute_dst_from_relative_dst_src(scratch.arena, stdout_path, folder_path)); }
            if (stderr_path.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("stderr_path")), path_absolute_dst_from_relative_dst_src(scratch.arena, stderr_path, folder_path)); }
            if (stdin_path.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("stdin_path")), path_absolute_dst_from_relative_dst_src(scratch.arena, stdin_path, folder_path)); }
            if (debug_subprocesses.size != 0) { rd_cfg_new(rd_cfg_new(dst_root, ("debug_subprocesses")), raw_from_escaped_str8(scratch.arena, debug_subprocesses)); }
            if (!str8_match(disabled_string, ("1"), 0))
            {
              rd_cfg_new(rd_cfg_new(dst_root, ("enabled")), ("1"));
            }
          }
        }
        
        //- rjf: recent files / projects
        if (str8_match(tln.string, ("recent_file"), 0) ||
           str8_match(tln.string, ("recent_project"), 0))
        {
          RD_Cfg *dst_root = rd_cfg_new(&rd_nil_cfg, tln.string);
          rd_cfg_list_push(arena, &result, dst_root);
          rd_cfg_new(rd_cfg_new(dst_root, ("path")), path_absolute_dst_from_relative_dst_src(scratch.arena, tln.first.string, folder_path));
        }
        
        //- rjf: file path maps
        if (str8_match(tln.string, ("file_path_map"), 0))
        {
          string source = md_child_from_string(tln, ("source"), 0)->first.string;
          string dest = md_child_from_string(tln, ("dest"), 0)->first.string;
          RD_Cfg *dst_root = rd_cfg_new(&rd_nil_cfg, tln.string);
          rd_cfg_list_push(arena, &result, dst_root);
          rd_cfg_new(rd_cfg_new(dst_root, ("source")), path_absolute_dst_from_relative_dst_src(scratch.arena, source, folder_path));
          rd_cfg_new(rd_cfg_new(dst_root, ("dest")), path_absolute_dst_from_relative_dst_src(scratch.arena, dest, folder_path));
        }
      }
    }
    scratch_end(scratch);
  }
  return result;
}

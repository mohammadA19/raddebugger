// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct LNK_CmdOption
{
  struct LNK_CmdOption* next;
  String8               string;
  String8List           value_strings;
}

struct LNK_CmdLine
{
  U64            option_count;
  LNK_CmdOption* first_option;
  LNK_CmdOption* last_option;
  String8List    input_list;
}

String8List     lnk_arg_list_parse_windows_rules(Arena* arena, String8 string);
LNK_CmdLine     lnk_cmd_line_parse_windows_rules(Arena* arena, String8List arg_list);
LNK_CmdOption * lnk_cmd_line_option_from_string(LNK_CmdLine cmd_line, String8 string);
B32             lnk_cmd_line_has_option_string(LNK_CmdLine cmd_line, String8 string);
B32             lnk_cmd_line_has_option(LNK_CmdLine cmd_line, char* string);

LNK_CmdOption * lnk_cmd_line_push_option(Arena* arena, LNK_CmdLine* cmd_line, char* string, char* value);
LNK_CmdOption * lnk_cmd_line_push_option_if_not_present(Arena* arena, LNK_CmdLine* cmd_line, char* string, char* value);

String8List lnk_unwrap_rsp(Arena* arena, String8List arg_list);

String8List lnk_data_from_cmd_line(Arena* arena, LNK_CmdLine cmd_line);


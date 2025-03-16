// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_COMMAND_LINE_H
#define BASE_COMMAND_LINE_H

////////////////////////////////
//~ rjf: Parsed Command Line Types

struct CmdLineOpt
{
  CmdLineOpt* next;
  CmdLineOpt* hash_next;
  uint64 hash;
  String8 string;
  String8List value_strings;
  String8 value_string;
}

struct CmdLineOptList
{
  uint64 count;
  CmdLineOpt* first;
  CmdLineOpt* last;
}

struct CmdLine
{
  String8 exe_name;
  CmdLineOptList options;
  String8List inputs;
  uint64 option_table_size;
  CmdLineOpt** option_table;
  uint64 argc;
  char** argv;
}

////////////////////////////////
//~ NOTE(rjf): Command Line Option Parsing

uint64              cmd_line_hash_from_string(String8 string);
CmdLineOpt**     cmd_line_slot_from_string(CmdLine* cmd_line, String8 string);
CmdLineOpt*      cmd_line_opt_from_slot(CmdLineOpt** slot, String8 string);
void             cmd_line_push_opt(CmdLineOptList* list, CmdLineOpt* var);
CmdLineOpt*      cmd_line_insert_opt(Arena* arena, CmdLine* cmd_line, String8 string, String8List values);
CmdLine          cmd_line_from_string_list(Arena* arena, String8List arguments);
CmdLineOpt*      cmd_line_opt_from_string(CmdLine* cmd_line, String8 name);
String8List      cmd_line_strings(CmdLine* cmd_line, String8 name);
String8          cmd_line_string(CmdLine* cmd_line, String8 name);
B32              cmd_line_has_flag(CmdLine* cmd_line, String8 name);
B32              cmd_line_has_argument(CmdLine* cmd_line, String8 name);

#endif // BASE_COMMAND_LINE_H

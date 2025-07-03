// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

////////////////////////////////
//~ rjf: Parsed Command Line Types

struct CmdLineOpt
{
  CmdLineOpt *next;
  CmdLineOpt *hash_next;
  U64 hash;
  String8 string;
  String8List value_strings;
  String8 value_string;
};

struct CmdLineOptList
{
  U64 count;
  CmdLineOpt *first;
  CmdLineOpt *last;
};

struct CmdLine
{
  String8 exe_name;
  CmdLineOptList options;
  String8List inputs;
  U64 option_table_size;
  CmdLineOpt **option_table;
  U64 argc;
  char **argv;
};

////////////////////////////////
//~ rjf: Command Line Parsing Functions

static CmdLineOpt**     cmd_line_slot_from_string(CmdLine *cmd_line, String8 string);
static CmdLineOpt*      cmd_line_opt_from_slot(CmdLineOpt **slot, String8 string);
static void             cmd_line_push_opt(CmdLineOptList *list, CmdLineOpt *var);
static CmdLineOpt*      cmd_line_insert_opt(Arena *arena, CmdLine *cmd_line, String8 string, String8List values);
static CmdLine          cmd_line_from_string_list(Arena *arena, String8List arguments);
static CmdLineOpt*      cmd_line_opt_from_string(CmdLine *cmd_line, String8 name);
static String8List      cmd_line_strings(CmdLine *cmd_line, String8 name);
static String8          cmd_line_string(CmdLine *cmd_line, String8 name);
static B32              cmd_line_has_flag(CmdLine *cmd_line, String8 name);
static B32              cmd_line_has_argument(CmdLine *cmd_line, String8 name);

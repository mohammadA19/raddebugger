// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Parsed Command Line Types



struct CmdLineOpt
{
  CmdLineOpt *next;
  CmdLineOpt *hash_next;
  u64 hash;
  String8 string;
  String8List value_strings;
  String8 value_string;
};



struct CmdLineOptList
{
  u64 count;
  CmdLineOpt *first;
  CmdLineOpt *last;
};



struct CmdLine
{
  String8 exe_name;
  CmdLineOptList options;
  String8List inputs;
  u64 option_table_size;
  CmdLineOpt **option_table;
  u64 argc;
  char **argv;
};

////////////////////////////////
//~ NOTE(rjf): Command Line Option Parsing















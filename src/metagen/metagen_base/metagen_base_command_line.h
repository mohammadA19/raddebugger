// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
//~ rjf: Parsed Command Line Types

typedef struct CmdLineOpt CmdLineOpt;
struct CmdLineOpt
{
  CmdLineOpt *next;
  CmdLineOpt *hash_next;
  U64 hash;
  String8 string;
  String8List value_strings;
  String8 value_string;
};

typedef struct CmdLineOptList CmdLineOptList;
struct CmdLineOptList
{
  U64 count;
  CmdLineOpt *first;
  CmdLineOpt *last;
};

typedef struct CmdLine CmdLine;
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
//~ NOTE(rjf): Command Line Option Parsing



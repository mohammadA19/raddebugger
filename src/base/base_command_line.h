// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




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














// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct LNK_CmdOption
{
  struct LNK_CmdOption *next;
  String8               string;
  String8List           value_strings;
} LNK_CmdOption;

typedef struct LNK_CmdLine
{
  U64            option_count;
  LNK_CmdOption *first_option;
  LNK_CmdOption *last_option;
  String8List    input_list;
} LNK_CmdLine;






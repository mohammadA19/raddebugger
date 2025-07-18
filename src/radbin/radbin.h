// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADBIN_H
#define RADBIN_H

////////////////////////////////
//~ rjf: Generated Code

#include "radbin/generated/radbin.meta.h"

////////////////////////////////
//~ rjf: File Types

enum RB_FileFormatFlags : uint32
{
  RB_FileFormatFlag_HasDWARF = (1<<0),
};

struct RB_File
{
  RB_FileFormat format;
  RB_FileFormatFlags format_flags;
  String8 path;
  String8 data;
};

struct RB_FileNode
{
  RB_FileNode *next;
  RB_File *v;
};

struct RB_FileList
{
  RB_FileNode *first;
  RB_FileNode *last;
  uint64 count;
};

read_only global RB_File rb_file_nil = {0};
#define rb_file_list_first(list) ((list)->first ? (list)->first->v : &rb_file_nil)

////////////////////////////////
//~ rjf: Top-Level Entry Point


#endif //RADBIN_H

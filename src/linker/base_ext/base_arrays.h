// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct uint64Node
{
  struct uint64Node *next;
  uint64             data;
} uint64Node;

typedef struct uint64List
{
  uint64      count;
  uint64Node *first;
  uint64Node *last;
} uint64List;

typedef struct VoidNode
{
  struct VoidNode *next;
  void            *v;
} VoidNode;

////////////////////////////////








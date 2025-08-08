// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct U32Node
{
  struct U32Node *next;
  U32             data;
} U32Node;

typedef struct U64Node
{
  struct U64Node *next;
  U64             data;
} U64Node;

typedef struct U64List
{
  U64      count;
  U64Node *first;
  U64Node *last;
} U64List;

typedef struct VoidNode
{
  struct VoidNode *next;
  void            *v;
} VoidNode;

////////////////////////////////








// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct u64Node
{
  struct u64Node *next;
  u64             data;
} u64Node;

typedef struct u64List
{
  u64      count;
  u64Node *first;
  u64Node *last;
} u64List;

typedef struct VoidNode
{
  struct VoidNode *next;
  void            *v;
} VoidNode;

////////////////////////////////



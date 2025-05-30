// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct U64Node
{
  U64Node* next;
  U64             data;
} U64Node;

typedef struct U64List
{
  U64      count;
  U64Node* first;
  U64Node* last;
} U64List;

typedef struct VoidNode
{
  VoidNode* next;
  void*            v;
} VoidNode;

////////////////////////////////








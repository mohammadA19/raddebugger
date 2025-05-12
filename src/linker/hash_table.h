// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct KeyValuePair
{
  union {
    String8 key_string;
    void   *key_raw;
    U32     key_u32;
    U64     key_u64;
  };
  union {
    String8  value_string;
    void    *value_raw;
    U32      value_u32;
    U64      value_u64;
  };
} KeyValuePair;

typedef struct BucketNode
{
  struct BucketNode *next;
  KeyValuePair       v;
} BucketNode;

typedef struct BucketList
{
  BucketNode *first;
  BucketNode *last;
} BucketList;

typedef struct HashTable
{
  U64         count;
  U64         cap;
  BucketList *buckets;
  BucketList  free_buckets;
} HashTable;

////////////////////////////////

////////////////////////////////



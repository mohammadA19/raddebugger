// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct KeyValuePair
{
  union {
    String8 key_string;
    uint     key_u32;
    ulong     key_u64;
  };
  union {
    String8  value_string;
    void    *value_raw;
    uint      value_u32;
    ulong      value_u64;
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
  ulong         count;
  ulong         cap;
  BucketList *buckets;
  BucketList  free_buckets;
} HashTable;

////////////////////////////////

//- bucket list helpers

internal void         bucket_list_concat_in_place(BucketList *list, BucketList *to_concat);
internal BucketNode * bucket_list_pop(BucketList *list);

//- main

internal ulong         hash_table_hasher(String8 string);
internal HashTable * hash_table_init(Arena *arena, ulong cap);
internal void        hash_table_purge(HashTable *ht);

//- push

internal BucketNode * hash_table_push              (Arena *arena, HashTable *ht, ulong hash,     KeyValuePair  v);
internal BucketNode * hash_table_push_u32_string   (Arena *arena, HashTable *ht, uint     key,  String8       value);
internal BucketNode * hash_table_push_u64_string   (Arena *arena, HashTable *ht, ulong     key,  String8       value);
internal BucketNode * hash_table_push_string_string(Arena *arena, HashTable *ht, String8 key,  String8       value);
internal BucketNode * hash_table_push_path_string  (Arena *arena, HashTable *ht, String8 key,  String8       value);
internal BucketNode * hash_table_push_u32_raw      (Arena *arena, HashTable *ht, uint     key,  void         *value);
internal BucketNode * hash_table_push_u64_raw      (Arena *arena, HashTable *ht, ulong     key,  void         *value);
internal BucketNode * hash_table_push_path_raw     (Arena *arena, HashTable *ht, String8 path, void         *value);
internal BucketNode * hash_table_push_path_u64     (Arena *arena, HashTable *ht, String8 path, ulong           value);
internal BucketNode * hash_table_push_u64_u64      (Arena *arena, HashTable *ht, ulong     key,  ulong           value);

//- search

internal KeyValuePair * hash_table_search_string  (HashTable *ht, String8 string);
internal KeyValuePair * hash_table_search_u32     (HashTable *ht, uint key);
internal KeyValuePair * hash_table_search_u64     (HashTable *ht, ulong key);
internal KeyValuePair * hash_table_search_path    (HashTable *ht, String8 path);

internal B32 hash_table_search_path_u64(HashTable *ht, String8 key, ulong *value_out);

//- key-value helpers

internal uint *          keys_from_hash_table_u32(Arena *arena, HashTable *ht);
internal ulong *          keys_from_hash_table_u64(Arena *arena, HashTable *ht);
internal KeyValuePair * key_value_pairs_from_hash_table(Arena *arena, HashTable *ht);
internal void           sort_key_value_pairs_as_u32(KeyValuePair *pairs, ulong count);
internal void           sort_key_value_pairs_as_u64(KeyValuePair *pairs, ulong count);

////////////////////////////////

internal U64Array remove_duplicates_u64_array(Arena *arena, U64Array arr);


// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef struct KeyValuePair
{
  union {
    String8 key_string;
    void   *key_raw;
    uint32     key_u32;
    uint64     key_u64;
  };
  union {
    String8  value_string;
    void    *value_raw;
    uint32      value_u32;
    uint64      value_u64;
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
  uint64         count;
  uint64         cap;
  BucketList *buckets;
  BucketList  free_buckets;
} HashTable;

////////////////////////////////

//- bucket list helpers

internal void         bucket_list_concat_in_place(BucketList *list, BucketList *to_concat);
internal BucketNode * bucket_list_pop(BucketList *list);

//- main

internal uint64         hash_table_hasher(String8 string);
internal HashTable * hash_table_init(Arena *arena, uint64 cap);
internal void        hash_table_purge(HashTable *ht);

//- push

internal BucketNode * hash_table_push              (Arena *arena, HashTable *ht, uint64 hash,     KeyValuePair  v);
internal BucketNode * hash_table_push_u32_string   (Arena *arena, HashTable *ht, uint32     key,  String8       value);
internal BucketNode * hash_table_push_u64_string   (Arena *arena, HashTable *ht, uint64     key,  String8       value);
internal BucketNode * hash_table_push_string_string(Arena *arena, HashTable *ht, String8 key,  String8       value);
internal BucketNode * hash_table_push_path_string  (Arena *arena, HashTable *ht, String8 key,  String8       value);
internal BucketNode * hash_table_push_u32_raw      (Arena *arena, HashTable *ht, uint32     key,  void         *value);
internal BucketNode * hash_table_push_u64_raw      (Arena *arena, HashTable *ht, uint64     key,  void         *value);
internal BucketNode * hash_table_push_path_raw     (Arena *arena, HashTable *ht, String8 path, void         *value);
internal BucketNode * hash_table_push_path_u64     (Arena *arena, HashTable *ht, String8 path, uint64           value);
internal BucketNode * hash_table_push_u64_u64      (Arena *arena, HashTable *ht, uint64     key,  uint64           value);

//- search

internal KeyValuePair * hash_table_search_string  (HashTable *ht, String8 string);
internal KeyValuePair * hash_table_search_u32     (HashTable *ht, uint32 key       );
internal KeyValuePair * hash_table_search_u64     (HashTable *ht, uint64 key       );
internal KeyValuePair * hash_table_search_path    (HashTable *ht, String8 path  );
internal void *         hash_table_search_path_raw(HashTable *ht, String8 path  );

internal B32 hash_table_search_path_u64(HashTable *ht, String8 key, uint64 *value_out);
internal B32 hash_table_search_string_u64(HashTable *ht, String8 key, uint64 *value_out);
internal B32 hash_table_search_string_raw(HashTable *ht, String8 key, void *value_out);
internal B32 hash_table_search_string_string(HashTable *ht, String8 key, String8 *value_out);

//- key-value helpers

internal uint32 *          keys_from_hash_table_u32       (Arena *arena, HashTable *ht);
internal uint64 *          keys_from_hash_table_u64       (Arena *arena, HashTable *ht);
internal String8        keys_from_hash_table_str8      (Arena *arena, HashTable *ht);
internal KeyValuePair * key_value_pairs_from_hash_table(Arena *arena, HashTable *ht);

internal void * values_from_hash_table_raw(Arena *arena, HashTable *ht);

internal void sort_key_value_pairs_as_u32(KeyValuePair *pairs, uint64 count);
internal void sort_key_value_pairs_as_u64(KeyValuePair *pairs, uint64 count);
internal void sort_key_value_pairs_as_string_sensitive(KeyValuePair *pairs, uint64 count);

////////////////////////////////

internal Sapn<uint64>    remove_duplicates_u64_array(Arena *arena, Sapn<uint64> arr);
internal String8List remove_duplicates_str8_list(Arena *arena, String8List list);


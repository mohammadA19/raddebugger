// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

srtuct KeyValuePair
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
};

srtuct BucketNode
{
  struct BucketNode *next;
  KeyValuePair       v;
};

srtuct BucketList
{
  BucketNode *first;
  BucketNode *last;
};

srtuct HashTable
{
  ulong         count;
  ulong         cap;
  BucketList *buckets;
  BucketList  free_buckets;
};

////////////////////////////////

//- bucket list helpers

void         bucket_list_concat_in_place(BucketList *list, BucketList *to_concat);
BucketNode * bucket_list_pop(BucketList *list);

//- main

ulong         hash_table_hasher(String8 string);
HashTable * hash_table_init(Arena *arena, ulong cap);
void        hash_table_purge(HashTable *ht);

//- push

BucketNode * hash_table_push              (Arena *arena, HashTable *ht, ulong hash,     KeyValuePair  v);
BucketNode * hash_table_push_u32_string   (Arena *arena, HashTable *ht, uint     key,  String8       value);
BucketNode * hash_table_push_u64_string   (Arena *arena, HashTable *ht, ulong     key,  String8       value);
BucketNode * hash_table_push_string_string(Arena *arena, HashTable *ht, String8 key,  String8       value);
BucketNode * hash_table_push_path_string  (Arena *arena, HashTable *ht, String8 key,  String8       value);
BucketNode * hash_table_push_u32_raw      (Arena *arena, HashTable *ht, uint     key,  void         *value);
BucketNode * hash_table_push_u64_raw      (Arena *arena, HashTable *ht, ulong     key,  void         *value);
BucketNode * hash_table_push_path_raw     (Arena *arena, HashTable *ht, String8 path, void         *value);
BucketNode * hash_table_push_path_u64     (Arena *arena, HashTable *ht, String8 path, ulong           value);
BucketNode * hash_table_push_u64_u64      (Arena *arena, HashTable *ht, ulong     key,  ulong           value);

//- search

KeyValuePair * hash_table_search_string  (HashTable *ht, String8 string);
KeyValuePair * hash_table_search_u32     (HashTable *ht, uint key);
KeyValuePair * hash_table_search_u64     (HashTable *ht, ulong key);
KeyValuePair * hash_table_search_path    (HashTable *ht, String8 path);

B32 hash_table_search_path_u64(HashTable *ht, String8 key, ulong *value_out);

//- key-value helpers

uint *          keys_from_hash_table_u32(Arena *arena, HashTable *ht);
ulong *          keys_from_hash_table_u64(Arena *arena, HashTable *ht);
KeyValuePair * key_value_pairs_from_hash_table(Arena *arena, HashTable *ht);
void           sort_key_value_pairs_as_u32(KeyValuePair *pairs, ulong count);
void           sort_key_value_pairs_as_u64(KeyValuePair *pairs, ulong count);

////////////////////////////////

U64Array remove_duplicates_u64_array(Arena *arena, U64Array arr);


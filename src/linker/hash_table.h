// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

struct KeyValuePair
{
  [Union]
  struct {
    StringView key_string;
    uint32     key_u32;
    uint64     key_u64;
  };
  [Union]
  struct {
    StringView  value_string;
    void*    value_raw;
    uint32      value_u32;
    uint64      value_u64;
  };
}

struct BucketNode
{
  struct BucketNode* next;
  KeyValuePair       v;
}

struct BucketList
{
  BucketNode* first;
  BucketNode* last;
}

struct HashTable
{
  uint64         count;
  uint64         cap;
  BucketList* buckets;
  BucketList  free_buckets;
}

////////////////////////////////

//- bucket list helpers

void         bucket_list_concat_in_place(BucketList* list, BucketList* to_concat);
BucketNode * bucket_list_pop(BucketList* list);

//- main

uint64         hash_table_hasher(StringView str);
HashTable * hash_table_init(Arena* arena, uint64 cap);
void        hash_table_purge(HashTable* ht);

//- push

BucketNode * hash_table_push              (Arena* arena, HashTable* ht, uint64 hash,     KeyValuePair  v);
BucketNode * hash_table_push_u32_string   (Arena* arena, HashTable* ht, uint32     key,  StringView       value);
BucketNode * hash_table_push_u64_string   (Arena* arena, HashTable* ht, uint64     key,  StringView       value);
BucketNode * hash_table_push_string_string(Arena* arena, HashTable* ht, StringView key,  StringView       value);
BucketNode * hash_table_push_path_string  (Arena* arena, HashTable* ht, StringView key,  StringView       value);
BucketNode * hash_table_push_u32_raw      (Arena* arena, HashTable* ht, uint32     key,  void*         value);
BucketNode * hash_table_push_u64_raw      (Arena* arena, HashTable* ht, uint64     key,  void*         value);
BucketNode * hash_table_push_path_raw     (Arena* arena, HashTable* ht, StringView path, void*         value);
BucketNode * hash_table_push_path_u64     (Arena* arena, HashTable* ht, StringView path, uint64           value);
BucketNode * hash_table_push_u64_u64      (Arena* arena, HashTable* ht, uint64     key,  uint64           value);

//- search

KeyValuePair * hash_table_search_string  (HashTable* ht, StringView str);
KeyValuePair * hash_table_search_u32     (HashTable* ht, uint32 key);
KeyValuePair * hash_table_search_u64     (HashTable* ht, uint64 key);
KeyValuePair * hash_table_search_path    (HashTable* ht, StringView path);

B32 hash_table_search_path_u64(HashTable* ht, StringView key, uint64* value_out);

//- key-value helpers

uint32 *          keys_from_hash_table_u32(Arena* arena, HashTable* ht);
uint64 *          keys_from_hash_table_u64(Arena* arena, HashTable* ht);
KeyValuePair * key_value_pairs_from_hash_table(Arena* arena, HashTable* ht);
void           sort_key_value_pairs_as_u32(KeyValuePair* pairs, uint64 count);
void           sort_key_value_pairs_as_u64(KeyValuePair* pairs, uint64 count);

////////////////////////////////

U64Array remove_duplicates_u64_array(Arena* arena, U64Array arr);


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

void
bucket_list_concat_in_place(BucketList* list, BucketList* to_concat)
{
  SLLConcatInPlaceNoCount(list, to_concat);
}

BucketNode *
bucket_list_pop(BucketList* list)
{
  BucketNode* result = list.first;
  SLLQueuePop(list.first, list.last);
  return result;
}

////////////////////////////////

uint64
hash_table_hasher(StringView string)
{
  XXH64_hash_t hash64 = XXH3_64bits(string.str, string.size);
  return hash64;
}

HashTable *
hash_table_init(Arena* arena, uint64 cap)
{
  HashTable* ht = push_array(arena, HashTable, 1);
  ht.cap       = cap;
  ht.buckets   = push_array(arena, BucketList, cap);
  return ht;
}

void
hash_table_purge(HashTable* ht)
{
  // reset key count
  ht.count = 0;

  // concat buckets
  for (uint64 ibucket = 0; ibucket < ht.cap; ++ibucket) {
    bucket_list_concat_in_place(&ht.free_buckets, &ht.buckets[ibucket]);
  }
}

BucketNode *
hash_table_push(Arena* arena, HashTable* ht, uint64 hash, KeyValuePair v)
{
  BucketNode* node;
  if (ht.free_buckets.first != 0) {
    node = bucket_list_pop(&ht.free_buckets);
  } else {
    node = push_array(arena, BucketNode, 1);
  }
  node.next = 0;
  node.v    = v;
  
  uint64 ibucket = hash % ht.cap;
  SLLQueuePush(ht.buckets[ibucket].first, ht.buckets[ibucket].last, node);
  ++ht.count;
  
  return node;
}

BucketNode *
hash_table_push_string_string(Arena* arena, HashTable* ht, StringView key, StringView value)
{
  uint64 hash = hash_table_hasher(key);
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_string = key, .value_string = value });
}

BucketNode *
hash_table_push_string_raw(Arena* arena, HashTable* ht, StringView key, void* value)
{
  uint64 hash = hash_table_hasher(key);
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_string = key, .value_raw = value });
}

BucketNode *
hash_table_push_string_u64(Arena* arena, HashTable* ht, StringView key, uint64 value)
{
  uint64 hash = hash_table_hasher(key);
  return hash_table_push(arena, ht, hash, (KeyValuePair){.key_string = key, .value_u64 = value });
}

BucketNode *
hash_table_push_u32_raw(Arena* arena, HashTable* ht, uint32 key, void* value)
{
  uint64 hash = hash_table_hasher(str8_struct(&key));
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_u32 = key, .value_raw = value });
}

BucketNode *
hash_table_push_u32_string(Arena* arena, HashTable* ht, uint32 key, StringView value)
{
  uint64 hash = hash_table_hasher(str8_struct(&key));
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_u32 = key, .value_string = value });
}

BucketNode *
hash_table_push_u64_raw(Arena* arena, HashTable* ht, uint64 key, void* value)
{
  uint64 hash = hash_table_hasher(str8_struct(&key));
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_u64 = key, .value_raw = value });
}

BucketNode *
hash_table_push_u64_string(Arena* arena, HashTable* ht, uint64 key, StringView value)
{
  uint64 hash = hash_table_hasher(str8_struct(&key));
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_u64 = key, .value_string = value });
}

BucketNode *
hash_table_push_u64_u64(Arena* arena, HashTable* ht, uint64 key, uint64 value)
{
  uint64 hash = hash_table_hasher(str8_struct(&key));
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_u64 = key, .value_u64 = value });
}

BucketNode *
hash_table_push_path_string(Arena* arena, HashTable* ht, StringView path, StringView value)
{
  StringView path_canon = path_canon_from_regular_path(arena, path); 
  return hash_table_push_string_string(arena, ht, path_canon, value);
}

BucketNode *
hash_table_push_path_u64(Arena* arena, HashTable* ht, StringView path, uint64 value)
{
  StringView path_canon = path_canon_from_regular_path(arena, path);
  uint64 hash = hash_table_hasher(path_canon);
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_string = path_canon, .value_u64 = value });
}

BucketNode *
hash_table_push_path_raw(Arena* arena, HashTable* ht, StringView path, void* value)
{
  StringView path_canon = path_canon_from_regular_path(arena, path);
  uint64 hash = hash_table_hasher(path_canon);
  return hash_table_push(arena, ht, hash, (KeyValuePair){ .key_string = path_canon, .value_raw = value });
}

////////////////////////////////

KeyValuePair *
hash_table_search_string(HashTable* ht, StringView key_string)
{
  uint64         hash    = hash_table_hasher(key_string);
  uint64         ibucket = hash % ht.cap;
  BucketList* bucket  = ht.buckets + ibucket;
  for (BucketNode* n = bucket.first; n != 0; n = n.next) {
    if (str8_match(n.v.key_string, key_string, 0)) {
      return &n.v;
    }
  }
  return 0;
}

KeyValuePair *
hash_table_search_u32(HashTable* ht, uint32 key_u32)
{
  uint64         hash    = hash_table_hasher(str8_struct(&key_u32));
  uint64         ibucket = hash % ht.cap;
  BucketList* bucket  = ht.buckets + ibucket;
  for (BucketNode* n = bucket.first; n != 0; n = n.next) {
    if (n.v.key_u32 == key_u32) {
      return &n.v;
    }
  }
  return 0;
}

KeyValuePair *
hash_table_search_u64(HashTable* ht, uint64 key_u64)
{
  uint64         hash    = hash_table_hasher(str8_struct(&key_u64));
  uint64         ibucket = hash % ht.cap;
  BucketList* bucket  = ht.buckets + ibucket;
  for (BucketNode* n = bucket.first; n != 0; n = n.next) {
    if (n.v.key_u64 == key_u64) {
      return &n.v;
    }
  }
  return 0;
}

KeyValuePair *
hash_table_search_path(HashTable* ht, StringView path)
{
  Temp scratch = scratch_begin(0,0);
  StringView path_canon = path;
  path_canon = lower_from_str8(scratch.arena, path_canon);
  path_canon = path_convert_slashes(scratch.arena, path_canon, PathStyle_UnixAbsolute);
  KeyValuePair* result = hash_table_search_string(ht, path_canon);
  scratch_end(scratch);
  return result;
}

B32
hash_table_search_path_u64(HashTable* ht, StringView key, uint64* value_out)
{
  KeyValuePair* result = hash_table_search_path(ht, key);
  if (result != 0) {
    if (value_out != 0) {
      *value_out = result.value_u64;
    }
    return 1;
  }
  return 0;
}

B32
hash_table_search_string_u64(HashTable* ht, StringView key, uint64* value_out)
{
  KeyValuePair* result = hash_table_search_string(ht, key);
  if (result != 0) {
    if (value_out != 0) {
      *value_out = result.value_u64;
    }
    return 1;
  }
  return 0;
}

////////////////////////////////

int
key_value_pair_is_before_u32(void* raw_a, void* raw_b)
{
  KeyValuePair* a = raw_a;
  KeyValuePair* b = raw_b;
  return a.key_u32 < b.key_u32;
}

int
key_value_pair_is_before_u64(void* raw_a, void* raw_b)
{
  KeyValuePair* a = raw_a;
  KeyValuePair* b = raw_b;
  return a.key_u64 < b.key_u64;
}

uint32 *
keys_from_hash_table_u32(Arena* arena, HashTable* ht)
{
  uint32* result = push_array_no_zero(arena, uint32, ht.count);
  for (uint64 bucket_idx = 0, cursor = 0; bucket_idx < ht.cap; ++bucket_idx) {
    for (BucketNode* n = ht.buckets[bucket_idx].first; n != 0; n = n.next) {
      Assert(cursor < ht.count);
      result[cursor++] = n.v.key_u32;
    }
  }
  return result;
}

uint64 *
keys_from_hash_table_u64(Arena* arena, HashTable* ht)
{
  uint64* result = push_array_no_zero(arena, uint64, ht.count);
  for (uint64 bucket_idx = 0, cursor = 0; bucket_idx < ht.cap; ++bucket_idx) {
    for (BucketNode* n = ht.buckets[bucket_idx].first; n != 0; n = n.next) {
      Assert(cursor < ht.count);
      result[cursor++] = n.v.key_u64;
    }
  }
  return result;
}

KeyValuePair *
key_value_pairs_from_hash_table(Arena* arena, HashTable* ht)
{
  KeyValuePair* pairs = push_array_no_zero(arena, KeyValuePair, ht.count);
  for (uint64 bucket_idx = 0, cursor = 0; bucket_idx < ht.cap; ++bucket_idx) {
    for (BucketNode* n = ht.buckets[bucket_idx].first; n != 0; n = n.next) {
      Assert(cursor < ht.count);
      pairs[cursor++] = n.v;
    }
  }
  return pairs;
}

void
sort_key_value_pairs_as_u32(KeyValuePair* pairs, uint64 count)
{
  radsort(pairs, count, key_value_pair_is_before_u32);
}

void
sort_key_value_pairs_as_u64(KeyValuePair* pairs, uint64 count)
{
  radsort(pairs, count, key_value_pair_is_before_u64);
}

U64Array
remove_duplicates_u64_array(Arena* arena, U64Array arr)
{
  Temp scratch = scratch_begin(&arena, 1);

  HashTable* ht = hash_table_init(scratch.arena, ((uint64)(double)arr.count * 0.5));

  for (uint64 i = 0; i < arr.count; ++i) {
    KeyValuePair* is_present = hash_table_search_u64(ht, arr.v[i]);
    if (!is_present) {
      hash_table_push_u64_raw(scratch.arena, ht, arr.v[i], 0);
    }
  }

  U64Array result = {0};
  result.count    = ht.count;
  result.v        = keys_from_hash_table_u64(arena, ht);

  scratch_end(scratch);
  return result;
}

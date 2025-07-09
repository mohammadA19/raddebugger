// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal u16
safe_cast_u16x(u64 x)
{
  ensure(x <= max_U16);
  return (u16)x;
}

internal u64
u128_mod64(u128 a, u64 b)
{
  return a.u64[1] % b;
}

internal Version
make_version(u64 major, u64 minor)
{
  Version version;
  version.major = major;
  version.minor = minor;
  return version;
}

internal int
version_compar(Version a, Version b)
{
  int cmp = 0;
  if (a.major < b.major) {
    cmp = -1;
  } else if (a.major > b.major) {
    cmp = +1;
  } else if (a.major == b.major) {
    if (a.minor < b.minor) {
      cmp = -1;
    } else if (a.minor > b.minor) {
      cmp = +1;
    }
  }
  return cmp;
}

internal ISectOff
isect_off(u32 isect, u32 off)
{
  ISectOff result = { isect, off };
  return result;
}

internal int
u16_compar(const void *raw_a, const void *raw_b)
{
  u16 a = *(u16*)raw_a;
  u16 b = *(u16*)raw_b;
  int result = a < b  ? -1 :
               a > b  ? +1 :
               0;
  return result;
}

internal int
u32_compar(const void *raw_a, const void *raw_b)
{
  u32 a = *(u32*)raw_a;
  u32 b = *(u32*)raw_b;
  int result = a < b  ? -1 :
  a > b  ? +1 :
  0;
  return result;
}

internal int
u64_compar(const void *raw_a, const void *raw_b)
{
  u64 a = *(const u64*)raw_a;
  u64 b = *(const u64*)raw_b;
  int result = a < b  ? -1 : a > b  ? +1 : 0;
  return result;
}

internal int
u64_compar_inv(const void *raw_a, const void *raw_b)
{
  u64 a = *(const u64*)raw_a;
  u64 b = *(const u64*)raw_b;
  int result = a < b  ? +1 : a > b  ? -1 : 0;
  return result;
}

internal int
u16_compar_is_before(void *raw_a, void *raw_b)
{
  u16 *a = (u16 *)raw_a;
  u16 *b = (u16 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u32_compar_is_before(void *raw_a, void *raw_b)
{
  u32 *a = (u32 *)raw_a;
  u32 *b = (u32 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u64_compar_is_before(void *raw_a, void *raw_b)
{
  u64 *a = (u64 *)raw_a;
  u64 *b = (u64 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u8_is_before(void *raw_a, void *raw_b)
{
  u8 *a = (u8 *) raw_a;
  u8 *b = (u8 *) raw_b;
  return *a < *b;
}

internal int
u16_is_before(void *raw_a, void *raw_b)
{
  u16 *a = (u16 *) raw_a;
  u16 *b = (u16 *) raw_b;
  return *a < *b;
}

internal int
u32_is_before(void *raw_a, void *raw_b)
{
  u32 *a = (u32 *) raw_a;
  u32 *b = (u32 *) raw_b;
  return *a < *b;
}

internal int
u64_is_before(void *raw_a, void *raw_b)
{
  u64 *a = (u64 *) raw_a;
  u64 *b = (u64 *) raw_b;
  return *a < *b;
}

internal int
pair_u32_is_before_v0(void *raw_a, void *raw_b)
{
  PairU32 *a = raw_a;
  PairU32 *b = raw_b;
  return a->v0 < b->v0;
}

internal int
pair_u32_is_before(void *raw_a, void *raw_b)
{
  PairU32 *a = raw_a;
  PairU32 *b = raw_b;
  return a->v1 < b->v1;
}

internal int
pair_u64_is_before_v0(void *raw_a, void *raw_b)
{
  PairU64 *a = raw_a;
  PairU64 *b = raw_b;
  return a->v0 < b->v0;
}

internal int
pair_u64_is_before_v1(void *raw_a, void *raw_b)
{
  PairU64 *a = raw_a;
  PairU64 *b = raw_b;
  return a->v1 < b->v1;
}

internal int
pair_u32_compar_v0(const void *raw_a, const void *raw_b)
{
  const PairU32 *a = raw_a;
  const PairU32 *b = raw_b;
  return u32_compar(&a->v0, &b->v0);
}

internal int
pair_u64_compar_v0(const void *raw_a, const void *raw_b)
{
  const PairU64 *a = raw_a;
  const PairU64 *b = raw_b;
  return u64_compar(&a->v0, &b->v0);
}

internal int
pair_u64_compar_v1(const void *raw_a, const void *raw_b)
{
  const PairU64 *a = raw_a;
  const PairU64 *b = raw_b;
  return u64_compar(&a->v1, &b->v1);
}

internal u64
pair_u64_nearest_v0(PairU64 *arr, u64 count, u64 v)
{
  u64 result = max_U64;

  if (count > 1 && arr[0].v0 <= v && v < arr[count-1].v0) {
    u64 l = 0;
    u64 r = count - 1;
    for (; l <= r; ) {
      u64 m = l + (r - l) / 2;
      if (arr[m].v0 == v) {
        return m;
      } else if (arr[m].v0 < v) {
        l = m + 1;
      } else {
        r = m - 1;
      }
    }
    result = l;
  } else if (count == 1 && arr[0].v0 == v) {
    result = 0;
  } else if (count > 0 && v >= arr[count-1].v0) {
    result = count-1;
  }

  return result;
}

internal void
str8_list_concat_in_place_array(String8List *list, String8List *arr, u64 count)
{
  for (u64 i = 0; i < count; ++i) {
    str8_list_concat_in_place(list, &arr[i]);
  }
}




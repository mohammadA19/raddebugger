// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal uint16
safe_cast_u16x(uint64 x)
{
  AssertAlways(x <= max_uint16);
  return (uint16)x;
}

internal uint64
u128_mod64(uint128 a, uint64 b)
{
  return a.u64[1] % b;
}

internal Version
make_version(uint64 major, uint64 minor)
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
isect_off(uint32 isect, uint32 off)
{
  ISectOff result = { isect, off };
  return result;
}

internal int
u16_compar(const void *raw_a, const void *raw_b)
{
  uint16 a = *(uint16*)raw_a;
  uint16 b = *(uint16*)raw_b;
  int result = a < b  ? -1 :
               a > b  ? +1 :
               0;
  return result;
}

internal int
u32_compar(const void *raw_a, const void *raw_b)
{
  uint32 a = *(uint32*)raw_a;
  uint32 b = *(uint32*)raw_b;
  int result = a < b  ? -1 :
  a > b  ? +1 :
  0;
  return result;
}

internal int
u64_compar(const void *raw_a, const void *raw_b)
{
  uint64 a = *(const uint64*)raw_a;
  uint64 b = *(const uint64*)raw_b;
  int result = a < b  ? -1 : a > b  ? +1 : 0;
  return result;
}

internal int
u64_compar_inv(const void *raw_a, const void *raw_b)
{
  uint64 a = *(const uint64*)raw_a;
  uint64 b = *(const uint64*)raw_b;
  int result = a < b  ? +1 : a > b  ? -1 : 0;
  return result;
}

internal int
u16_compar_is_before(void *raw_a, void *raw_b)
{
  uint16 *a = (uint16 *)raw_a;
  uint16 *b = (uint16 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u32_compar_is_before(void *raw_a, void *raw_b)
{
  uint32 *a = (uint32 *)raw_a;
  uint32 *b = (uint32 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u64_compar_is_before(void *raw_a, void *raw_b)
{
  uint64 *a = (uint64 *)raw_a;
  uint64 *b = (uint64 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u8_is_before(void *raw_a, void *raw_b)
{
  uint8 *a = (uint8 *) raw_a;
  uint8 *b = (uint8 *) raw_b;
  return *a < *b;
}

internal int
u16_is_before(void *raw_a, void *raw_b)
{
  uint16 *a = (uint16 *) raw_a;
  uint16 *b = (uint16 *) raw_b;
  return *a < *b;
}

internal int
u32_is_before(void *raw_a, void *raw_b)
{
  uint32 *a = (uint32 *) raw_a;
  uint32 *b = (uint32 *) raw_b;
  return *a < *b;
}

internal int
u64_is_before(void *raw_a, void *raw_b)
{
  uint64 *a = (uint64 *) raw_a;
  uint64 *b = (uint64 *) raw_b;
  return *a < *b;
}

internal int
pair_u32_is_before_v0(void *raw_a, void *raw_b)
{
  Pairuint32 *a = raw_a;
  Pairuint32 *b = raw_b;
  return a->v0 < b->v0;
}

internal int
pair_u32_is_before(void *raw_a, void *raw_b)
{
  Pairuint32 *a = raw_a;
  Pairuint32 *b = raw_b;
  return a->v1 < b->v1;
}

internal int
pair_u64_is_before_v0(void *raw_a, void *raw_b)
{
  Pairuint64 *a = raw_a;
  Pairuint64 *b = raw_b;
  return a->v0 < b->v0;
}

internal int
pair_u64_is_before_v1(void *raw_a, void *raw_b)
{
  Pairuint64 *a = raw_a;
  Pairuint64 *b = raw_b;
  return a->v1 < b->v1;
}

internal int
pair_u32_compar_v0(const void *raw_a, const void *raw_b)
{
  const Pairuint32 *a = raw_a;
  const Pairuint32 *b = raw_b;
  return u32_compar(&a->v0, &b->v0);
}

internal int
pair_u64_compar_v0(const void *raw_a, const void *raw_b)
{
  const Pairuint64 *a = raw_a;
  const Pairuint64 *b = raw_b;
  return u64_compar(&a->v0, &b->v0);
}

internal int
pair_u64_compar_v1(const void *raw_a, const void *raw_b)
{
  const Pairuint64 *a = raw_a;
  const Pairuint64 *b = raw_b;
  return u64_compar(&a->v1, &b->v1);
}

internal uint64
pair_u64_nearest_v0(Pairuint64 *arr, uint64 count, uint64 v)
{
  uint64 result = max_uint64;

  if (count > 1 && arr[0].v0 <= v && v < arr[count-1].v0) {
    uint64 l = 0;
    uint64 r = count - 1;
    for (; l <= r; ) {
      uint64 m = l + (r - l) / 2;
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
str8_list_concat_in_place_array(String8List *list, String8List *arr, uint64 count)
{
  for (uint64 i = 0; i < count; ++i) {
    str8_list_concat_in_place(list, &arr[i]);
  }
}




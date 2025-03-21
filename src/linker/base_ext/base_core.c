// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal ushort
safe_cast_u16x(ulong x)
{
  AssertAlways(x <= max_U16);
  return (ushort)x;
}

////////////////////////////////

internal ulong
u128_mod64(U128 a, ulong b)
{
  return a.u64[1] % b;
}

////////////////////////////////

internal Version
make_version(ulong major, ulong minor)
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

////////////////////////////////

internal ISectOff
isect_off(uint isect, uint off)
{
  ISectOff result = { isect, off };
  return result;
}

////////////////////////////////

internal int
u16_compar(const void *raw_a, const void *raw_b)
{
  ushort a = *(ushort*)raw_a;
  ushort b = *(ushort*)raw_b;
  int result = a < b  ? -1 :
               a > b  ? +1 :
               0;
  return result;
}

internal int
u32_compar(const void *raw_a, const void *raw_b)
{
  uint a = *(uint*)raw_a;
  uint b = *(uint*)raw_b;
  int result = a < b  ? -1 :
  a > b  ? +1 :
  0;
  return result;
}

internal int
u64_compar(const void *raw_a, const void *raw_b)
{
  ulong a = *(const ulong*)raw_a;
  ulong b = *(const ulong*)raw_b;
  int result = a < b  ? -1 : a > b  ? +1 : 0;
  return result;
}

internal int
u64_compar_inv(const void *raw_a, const void *raw_b)
{
  ulong a = *(const ulong*)raw_a;
  ulong b = *(const ulong*)raw_b;
  int result = a < b  ? +1 : a > b  ? -1 : 0;
  return result;
}

internal int
u16_compar_is_before(void *raw_a, void *raw_b)
{
  ushort *a = (ushort *)raw_a;
  ushort *b = (ushort *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u32_compar_is_before(void *raw_a, void *raw_b)
{
  uint *a = (uint *)raw_a;
  uint *b = (uint *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

internal int
u64_compar_is_before(void *raw_a, void *raw_b)
{
  ulong *a = (ulong *)raw_a;
  ulong *b = (ulong *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}


internal int
u8_is_before(void *raw_a, void *raw_b)
{
  byte *a = (byte *) raw_a;
  byte *b = (byte *) raw_b;
  return *a < *b;
}

internal int
u16_is_before(void *raw_a, void *raw_b)
{
  ushort *a = (ushort *) raw_a;
  ushort *b = (ushort *) raw_b;
  return *a < *b;
}

internal int
u32_is_before(void *raw_a, void *raw_b)
{
  uint *a = (uint *) raw_a;
  uint *b = (uint *) raw_b;
  return *a < *b;
}

internal int
u64_is_before(void *raw_a, void *raw_b)
{
  ulong *a = (ulong *) raw_a;
  ulong *b = (ulong *) raw_b;
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


////////////////////////////////

internal void
str8_list_concat_in_place_array(String8List *list, String8List *arr, ulong count)
{
  for (ulong i = 0; i < count; ++i) {
    str8_list_concat_in_place(list, &arr[i]);
  }
}




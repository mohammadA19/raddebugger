// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

uint16
safe_cast_u16x(uint64 x)
{
  AssertAlways(x <= max_U16);
  return (uint16)x;
}

////////////////////////////////

uint64
u128_mod64(U128 a, uint64 b)
{
  return a.u64[1] % b;
}

////////////////////////////////

Version
make_version(uint64 major, uint64 minor)
{
  Version version;
  version.major = major;
  version.minor = minor;
  return version;
}

int
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

ISectOff
isect_off(uint32 isect, uint32 off)
{
  ISectOff result = { isect, off };
  return result;
}

////////////////////////////////

int
u16_compar(const void* raw_a, const void* raw_b)
{
  uint16 a = *(uint16*)raw_a;
  uint16 b = *(uint16*)raw_b;
  int result = a < b  ? -1 :
               a > b  ? +1 :
               0;
  return result;
}

int
u32_compar(const void* raw_a, const void* raw_b)
{
  uint32 a = *(uint32*)raw_a;
  uint32 b = *(uint32*)raw_b;
  int result = a < b  ? -1 :
  a > b  ? +1 :
  0;
  return result;
}

int
u64_compar(const void* raw_a, const void* raw_b)
{
  uint64 a = *(const uint64*)raw_a;
  uint64 b = *(const uint64*)raw_b;
  int result = a < b  ? -1 : a > b  ? +1 : 0;
  return result;
}

int
u64_compar_inv(const void* raw_a, const void* raw_b)
{
  uint64 a = *(const uint64*)raw_a;
  uint64 b = *(const uint64*)raw_b;
  int result = a < b  ? +1 : a > b  ? -1 : 0;
  return result;
}

int
u16_compar_is_before(void* raw_a, void* raw_b)
{
  uint16* a = (uint16 *)raw_a;
  uint16* b = (uint16 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

int
u32_compar_is_before(void* raw_a, void* raw_b)
{
  uint32* a = (uint32 *)raw_a;
  uint32* b = (uint32 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}

int
u64_compar_is_before(void* raw_a, void* raw_b)
{
  uint64* a = (uint64 *)raw_a;
  uint64* b = (uint64 *)raw_b;
  int is_before = *a < *b;
  return is_before; 
}


int
u8_is_before(void* raw_a, void* raw_b)
{
  uint8* a = (uint8 *) raw_a;
  uint8* b = (uint8 *) raw_b;
  return* a < *b;
}

int
u16_is_before(void* raw_a, void* raw_b)
{
  uint16* a = (uint16 *) raw_a;
  uint16* b = (uint16 *) raw_b;
  return* a < *b;
}

int
u32_is_before(void* raw_a, void* raw_b)
{
  uint32* a = (uint32 *) raw_a;
  uint32* b = (uint32 *) raw_b;
  return* a < *b;
}

int
u64_is_before(void* raw_a, void* raw_b)
{
  uint64* a = (uint64 *) raw_a;
  uint64* b = (uint64 *) raw_b;
  return* a < *b;
}

int
pair_u32_is_before_v0(void* raw_a, void* raw_b)
{
  PairU32* a = raw_a;
  PairU32* b = raw_b;
  return a.v0 < b.v0;
}

int
pair_u32_is_before(void* raw_a, void* raw_b)
{
  PairU32* a = raw_a;
  PairU32* b = raw_b;
  return a.v1 < b.v1;
}

int
pair_u64_is_before_v0(void* raw_a, void* raw_b)
{
  PairU64* a = raw_a;
  PairU64* b = raw_b;
  return a.v0 < b.v0;
}

int
pair_u64_is_before_v1(void* raw_a, void* raw_b)
{
  PairU64* a = raw_a;
  PairU64* b = raw_b;
  return a.v1 < b.v1;
}

int
pair_u32_compar_v0(const void* raw_a, const void* raw_b)
{
  const PairU32* a = raw_a;
  const PairU32* b = raw_b;
  return u32_compar(&a.v0, &b.v0);
}

int
pair_u64_compar_v0(const void* raw_a, const void* raw_b)
{
  const PairU64* a = raw_a;
  const PairU64* b = raw_b;
  return u64_compar(&a.v0, &b.v0);
}

int
pair_u64_compar_v1(const void* raw_a, const void* raw_b)
{
  const PairU64* a = raw_a;
  const PairU64* b = raw_b;
  return u64_compar(&a.v1, &b.v1);
}


////////////////////////////////

void
str8_list_concat_in_place_array(String8List* list, String8List* arr, uint64 count)
{
  for (uint64 i = 0; i < count; ++i) {
    str8_list_concat_in_place(list, &arr[i]);
  }
}




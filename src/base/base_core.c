// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Safe Casts

uint16
safe_cast_u16(uint32 x)
{
  AssertAlways(x <= max_U16);
  uint16 result = (uint16)x;
  return result;
}

uint32
safe_cast_u32(uint64 x)
{
  AssertAlways(x <= max_U32);
  uint32 result = (uint32)x;
  return result;
}

int32
safe_cast_s32(int64 x)
{
  AssertAlways(x <= max_S32);
  int32 result = (int32)x;
  return result;
}

////////////////////////////////
//~ rjf: Large Base Type Functions

U128
u128_zero()
{
  U128 v = {0};
  return v;
}

U128
u128_make(uint64 v0, uint64 v1)
{
  U128 v = {v0, v1};
  return v;
}

B32
u128_match(U128 a, U128 b)
{
  return MemoryMatchStruct(&a, &b);
}

////////////////////////////////
//~ rjf: Bit Patterns

uint32
u32_from_u64_saturate(uint64 x){
  uint32 x32 = (x > max_U32)?max_U32:(uint32)x;
  return(x32);
}

uint64
u64_up_to_pow2(uint64 x){
  if (x == 0){
    x = 1;
  }
  else{
    x -= 1;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> 32);
    x += 1;
  }
  return(x);
}

int32
extend_sign32(uint32 x, uint32 size){
  uint32 high_bit = size * 8;
  uint32 shift = 32 - high_bit;
  int32 result = ((int32)x << shift) >> shift;
  return result;
}

int64
extend_sign64(uint64 x, uint64 size){
  uint64 high_bit = size * 8;
  uint64 shift = 64 - high_bit;
  int64 result = ((int64)x << shift) >> shift;
  return result;
}

float
inf32(){
  [Union]
  struct { uint32 u; float f; } x;
  x.u = exponent32;
  return(x.f);
}

float
neg_inf32(){
  [Union]
  struct { uint32 u; float f; } x;
  x.u = sign32 | exponent32;
  return(x.f);
}

uint16
bswap_u16(uint16 x)
{
  uint16 result = (((x & 0xFF00) >> 8) |
                ((x & 0x00FF) << 8));
  return result;
}

uint32
bswap_u32(uint32 x)
{
  uint32 result = (((x & 0xFF000000) >> 24) |
                ((x & 0x00FF0000) >> 8)  |
                ((x & 0x0000FF00) << 8)  |
                ((x & 0x000000FF) << 24));
  return result;
}

uint64
bswap_u64(uint64 x)
{
  // TODO(nick): naive bswap, replace with something that is faster like an intrinsic
  uint64 result = (((x & 0xFF00000000000000ULL) >> 56) |
                ((x & 0x00FF000000000000ULL) >> 40) |
                ((x & 0x0000FF0000000000ULL) >> 24) |
                ((x & 0x000000FF00000000ULL) >> 8)  |
                ((x & 0x00000000FF000000ULL) << 8)  |
                ((x & 0x0000000000FF0000ULL) << 24) |
                ((x & 0x000000000000FF00ULL) << 40) |
                ((x & 0x00000000000000FFULL) << 56));
  return result;
}

#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)

uint64
count_bits_set32(uint32 val)
{
  return __popcnt(val);
}

uint64
count_bits_set64(uint64 val)
{
  return __popcnt64(val);
}

uint64
ctz32(uint32 mask)
{
  unsigned long idx;
  _BitScanForward(&idx, mask);
  return idx;
}

uint64
ctz64(uint64 mask)
{
  unsigned long idx;
  _BitScanForward64(&idx, mask);
  return idx;
}

uint64
clz32(uint32 mask)
{
  unsigned long idx;
  _BitScanReverse(&idx, mask);
  return 31 - idx;
}

uint64
clz64(uint64 mask)
{
  unsigned long idx;
  _BitScanReverse64(&idx, mask);
  return 63 - idx;
}

#elif COMPILER_CLANG || COMPILER_GCC

uint64
count_bits_set32(uint32 val)
{
  return __builtin_popcount(val);
}

uint64
count_bits_set64(uint64 val)
{
  return __builtin_popcountll(val);
}

uint64
ctz32(uint32 val)
{
  return __builtin_ctz(val);
}

uint64
clz32(uint32 val)
{
  return __builtin_clz(val);
}

uint64
ctz64(uint64 val)
{
  return __builtin_ctzll(val);
}

uint64
clz64(uint64 val)
{
  return __builtin_clzll(val);
}

#else
# error "Bit intrinsic functions not defined for this compiler."
#endif

////////////////////////////////
//~ rjf: Enum -> Sign

int32
sign_from_side_S32(Side side){
  return((side == Side_Min)?-1:1);
}

float
sign_from_side_F32(Side side){
  return((side == Side_Min)?-1.f:1.f);
}

////////////////////////////////
//~ rjf: Memory Functions

B32
memory_is_zero(void* ptr, uint64 size){
  B32 result = 1;
  
  // break down size
  uint64 extra = (size&0x7);
  uint64 count8 = (size >> 3);
  
  // check with 8-byte stride
  uint64* p64 = (uint64*)ptr;
  if(result)
  {
    for (uint64 i = 0; i < count8; i += 1, p64 += 1){
      if (*p64 != 0){
        result = 0;
        goto done;
      }
    }
  }
  
  // check extra
  if(result)
  {
    uint8* p8 = (uint8*)p64;
    for (uint64 i = 0; i < extra; i += 1, p8 += 1){
      if (*p8 != 0){
        result = 0;
        goto done;
      }
    }
  }
  
  done:;
  return(result);
}

////////////////////////////////
//~ rjf: Text 2D Coordinate/Range Functions

TxtPt
txt_pt(int64 line, int64 column)
{
  TxtPt p = {0};
  p.line = line;
  p.column = column;
  return p;
}

B32
txt_pt_match(TxtPt a, TxtPt b)
{
  return a.line == b.line && a.column == b.column;
}

B32
txt_pt_less_than(TxtPt a, TxtPt b)
{
  B32 result = 0;
  if(a.line < b.line)
  {
    result = 1;
  }
  else if(a.line == b.line)
  {
    result = a.column < b.column;
  }
  return result;
}

TxtPt
txt_pt_min(TxtPt a, TxtPt b)
{
  TxtPt result = b;
  if(txt_pt_less_than(a, b))
  {
    result = a;
  }
  return result;
}

TxtPt
txt_pt_max(TxtPt a, TxtPt b)
{
  TxtPt result = a;
  if(txt_pt_less_than(a, b))
  {
    result = b;
  }
  return result;
}

TxtRng
txt_rng(TxtPt min, TxtPt max)
{
  TxtRng range = {0};
  if(txt_pt_less_than(min, max))
  {
    range.min = min;
    range.max = max;
  }
  else
  {
    range.min = max;
    range.max = min;
  }
  return range;
}

TxtRng
txt_rng_intersect(TxtRng a, TxtRng b)
{
  TxtRng result = {0};
  result.min = txt_pt_max(a.min, b.min);
  result.max = txt_pt_min(a.max, b.max);
  if(txt_pt_less_than(result.max, result.min))
  {
    MemoryZeroStruct(&result);
  }
  return result;
}

TxtRng
txt_rng_union(TxtRng a, TxtRng b)
{
  TxtRng result = {0};
  result.min = txt_pt_min(a.min, b.min);
  result.max = txt_pt_max(a.max, b.max);
  return result;
}

B32
txt_rng_contains(TxtRng r, TxtPt pt)
{
  B32 result = ((txt_pt_less_than(r.min, pt) || txt_pt_match(r.min, pt)) &&
                txt_pt_less_than(pt, r.max));
  return result;
}

////////////////////////////////
//~ rjf: Toolchain/Environment Enum Functions

uint64
bit_size_from_arch(Arch arch)
{
  // TODO(rjf): metacode
  uint64 arch_bitsize = 0;
  switch(arch)
  {
    case Arch_x64:   arch_bitsize = 64; break;
    case Arch_x86:   arch_bitsize = 32; break;
    case Arch_arm64: arch_bitsize = 64; break;
    case Arch_arm32: arch_bitsize = 32; break;
    default: break;
  }
  return arch_bitsize;
}

uint64
max_instruction_size_from_arch(Arch arch)
{
  // TODO(rjf): make this real
  return 64;
}

OperatingSystem
operating_system_from_context(){
  OperatingSystem os = OperatingSystem_Null;
#if OS_WINDOWS
  os = OperatingSystem_Windows;
#elif OS_LINUX
  os = OperatingSystem_Linux;
#elif OS_MAC
  os = OperatingSystem_Mac;
#endif
  return os;
}

Arch
arch_from_context(){
  Arch arch = Arch_Null;
#if ARCH_X64
  arch = Arch_x64;
#elif ARCH_X86
  arch = Arch_x86;
#elif ARCH_ARM64
  arch = Arch_arm64;
#elif ARCH_ARM32
  arch = Arch_arm32;
#endif
  return arch;
}

Compiler
compiler_from_context(){
  Compiler compiler = Compiler_Null;
#if COMPILER_MSVC
  compiler = Compiler_msvc;
#elif COMPILER_GCC
  compiler = Compiler_gcc;
#elif COMPILER_CLANG
  compiler = Compiler_clang;
#endif
  return compiler;
}

////////////////////////////////
//~ rjf: Time Functions

DenseTime
dense_time_from_date_time(DateTime date_time){
  DenseTime result = 0;
  result += date_time.year;
  result *= 12;
  result += date_time.mon;
  result *= 31;
  result += date_time.day;
  result *= 24;
  result += date_time.hour;
  result *= 60;
  result += date_time.min;
  result *= 61;
  result += date_time.sec;
  result *= 1000;
  result += date_time.msec;
  return(result);
}

DateTime
date_time_from_dense_time(DenseTime time){
  DateTime result = {0};
  result.msec = time%1000;
  time /= 1000;
  result.sec  = time%61;
  time /= 61;
  result.min  = time%60;
  time /= 60;
  result.hour = time%24;
  time /= 24;
  result.day  = time%31;
  time /= 31;
  result.mon  = time%12;
  time /= 12;
  Assert(time <= max_U32);
  result.year = (uint32)time;
  return(result);
}

DateTime
date_time_from_micro_seconds(uint64 time){
  DateTime result = {0};
  result.micro_sec = time%1000;
  time /= 1000;
  result.msec = time%1000;
  time /= 1000;
  result.sec = time%60;
  time /= 60;
  result.min = time%60;
  time /= 60;
  result.hour = time%24;
  time /= 24;
  result.day = time%31;
  time /= 31;
  result.mon = time%12;
  time /= 12;
  Assert(time <= max_U32);
  result.year = (uint32)time;
  return(result);
}

DateTime
date_time_from_unix_time(uint64 unix_time)
{
  DateTime date = {0};
  date.year     = 1970;
  date.day      = 1 + (unix_time / 86400);
  date.sec      = (uint32)unix_time % 60;
  date.min      = (uint32)(unix_time / 60) % 60;
  date.hour     = (uint32)(unix_time / 3600) % 24;

  for(;;)
  {
    for(date.month = 0; date.month < 12; ++date.month)
    {
      uint64 c = 0;
      switch(date.month)
      {
        case Month_Jan: c = 31; break;
        case Month_Feb:
        {
          if((date.year % 4 == 0) && ((date.year % 100) != 0 || (date.year % 400) == 0))
          {
            c = 29;
          }
          else
          {
            c = 28;
          }
        } break;
        case Month_Mar: c = 31; break;
        case Month_Apr: c = 30; break;
        case Month_May: c = 31; break;
        case Month_Jun: c = 30; break;
        case Month_Jul: c = 31; break;
        case Month_Aug: c = 31; break;
        case Month_Sep: c = 30; break;
        case Month_Oct: c = 31; break;
        case Month_Nov: c = 30; break;
        case Month_Dec: c = 31; break;
        default: InvalidPath;
      }
      if(date.day <= c)
      {
        goto exit;
      }
      date.day -= c;
    }
    ++date.year;
  }
  exit:;

  return date;
}

////////////////////////////////
//~ rjf: Non-Fancy Ring Buffer Reads/Writes

uint64
ring_write(uint8* ring_base, uint64 ring_size, uint64 ring_pos, void* src_data, uint64 src_data_size)
{
  Assert(src_data_size <= ring_size);
  {
    uint64 ring_off = ring_pos%ring_size;
    uint64 bytes_before_split = ring_size-ring_off;
    uint64 pre_split_bytes = Min(bytes_before_split, src_data_size);
    uint64 pst_split_bytes = src_data_size-pre_split_bytes;
    void* pre_split_data = src_data;
    void* pst_split_data = ((uint8 *)src_data + pre_split_bytes);
    MemoryCopy(ring_base+ring_off, pre_split_data, pre_split_bytes);
    MemoryCopy(ring_base+0, pst_split_data, pst_split_bytes);
  }
  return src_data_size;
}

uint64
ring_read(uint8* ring_base, uint64 ring_size, uint64 ring_pos, void* dst_data, uint64 read_size)
{
  Assert(read_size <= ring_size);
  {
    uint64 ring_off = ring_pos%ring_size;
    uint64 bytes_before_split = ring_size-ring_off;
    uint64 pre_split_bytes = Min(bytes_before_split, read_size);
    uint64 pst_split_bytes = read_size-pre_split_bytes;
    MemoryCopy(dst_data, ring_base+ring_off, pre_split_bytes);
    MemoryCopy((uint8 *)dst_data + pre_split_bytes, ring_base+0, pst_split_bytes);
  }
  return read_size;
}

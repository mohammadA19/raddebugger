// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Safe Casts

internal ushort
safe_cast_u16(uint x)
{
  AssertAlways(x <= max_U16);
  ushort result = (ushort)x;
  return result;
}

internal uint
safe_cast_u32(ulong x)
{
  AssertAlways(x <= max_U32);
  uint result = (uint)x;
  return result;
}

internal int
safe_cast_s32(long x)
{
  AssertAlways(x <= max_S32);
  int result = (int)x;
  return result;
}

////////////////////////////////
//~ rjf: Large Base Type Functions

internal U128
u128_zero(void)
{
  U128 v = {0};
  return v;
}

internal U128
u128_make(ulong v0, ulong v1)
{
  U128 v = {v0, v1};
  return v;
}

internal B32
u128_match(U128 a, U128 b)
{
  return MemoryMatchStruct(&a, &b);
}

////////////////////////////////
//~ rjf: Bit Patterns

internal uint
u32_from_u64_saturate(ulong x){
  uint x32 = (x > max_U32)?max_U32:(uint)x;
  return(x32);
}

internal ulong
u64_up_to_pow2(ulong x){
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

internal int
extend_sign32(uint x, uint size){
  uint high_bit = size * 8;
  uint shift = 32 - high_bit;
  int result = ((int)x << shift) >> shift;
  return result;
}

internal long
extend_sign64(ulong x, ulong size){
  ulong high_bit = size * 8;
  ulong shift = 64 - high_bit;
  long result = ((long)x << shift) >> shift;
  return result;
}

internal float
inf32(void){
  union { uint u; float f; } x;
  x.u = exponent32;
  return(x.f);
}

internal float
neg_inf32(void){
  union { uint u; float f; } x;
  x.u = sign32 | exponent32;
  return(x.f);
}

internal ushort
bswap_u16(ushort x)
{
  ushort result = (((x & 0xFF00) >> 8) |
                ((x & 0x00FF) << 8));
  return result;
}

internal uint
bswap_u32(uint x)
{
  uint result = (((x & 0xFF000000) >> 24) |
                ((x & 0x00FF0000) >> 8)  |
                ((x & 0x0000FF00) << 8)  |
                ((x & 0x000000FF) << 24));
  return result;
}

internal ulong
bswap_u64(ulong x)
{
  // TODO(nick): naive bswap, replace with something that is faster like an intrinsic
  ulong result = (((x & 0xFF00000000000000ULL) >> 56) |
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

internal ulong
count_bits_set16(ushort val)
{
  return __popcnt16(val);
}

internal ulong
count_bits_set32(uint val)
{
  return __popcnt(val);
}

internal ulong
count_bits_set64(ulong val)
{
  return __popcnt64(val);
}

internal ulong
ctz32(uint mask)
{
  unsigned long idx;
  _BitScanForward(&idx, mask);
  return idx;
}

internal ulong
ctz64(ulong mask)
{
  unsigned long idx;
  _BitScanForward64(&idx, mask);
  return idx;
}

internal ulong
clz32(uint mask)
{
  unsigned long idx;
  _BitScanReverse(&idx, mask);
  return 31 - idx;
}

internal ulong
clz64(ulong mask)
{
  unsigned long idx;
  _BitScanReverse64(&idx, mask);
  return 63 - idx;
}

#elif COMPILER_CLANG || COMPILER_GCC

internal ulong
count_bits_set16(ushort val)
{
  NotImplemented;
  return 0;
}

internal ulong
count_bits_set32(uint val)
{
  NotImplemented;
  return 0;
}

internal ulong
count_bits_set64(ulong val)
{
  NotImplemented;
  return 0;
}

internal ulong
ctz32(uint val)
{
  NotImplemented;
  return 0;
}

internal ulong
clz32(uint val)
{
  NotImplemented;
  return 0;
}

internal ulong
clz64(ulong val)
{
  NotImplemented;
  return 0;
}

#else
# error "Bit intrinsic functions not defined for this compiler."
#endif

////////////////////////////////
//~ rjf: Enum -> Sign

internal int
sign_from_side_S32(Side side){
  return((side == Side_Min)?-1:1);
}

internal float
sign_from_side_F32(Side side){
  return((side == Side_Min)?-1.f:1.f);
}

////////////////////////////////
//~ rjf: Memory Functions

internal B32
memory_is_zero(void *ptr, ulong size){
  B32 result = 1;
  
  // break down size
  ulong extra = (size&0x7);
  ulong count8 = (size >> 3);
  
  // check with 8-byte stride
  ulong *p64 = (ulong*)ptr;
  if(result)
  {
    for (ulong i = 0; i < count8; i += 1, p64 += 1){
      if (*p64 != 0){
        result = 0;
        goto done;
      }
    }
  }
  
  // check extra
  if(result)
  {
    byte *p8 = (byte*)p64;
    for (ulong i = 0; i < extra; i += 1, p8 += 1){
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

internal TxtPt
txt_pt(long line, long column)
{
  TxtPt p = {0};
  p.line = line;
  p.column = column;
  return p;
}

internal B32
txt_pt_match(TxtPt a, TxtPt b)
{
  return a.line == b.line && a.column == b.column;
}

internal B32
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

internal TxtPt
txt_pt_min(TxtPt a, TxtPt b)
{
  TxtPt result = b;
  if(txt_pt_less_than(a, b))
  {
    result = a;
  }
  return result;
}

internal TxtPt
txt_pt_max(TxtPt a, TxtPt b)
{
  TxtPt result = a;
  if(txt_pt_less_than(a, b))
  {
    result = b;
  }
  return result;
}

internal TxtRng
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

internal TxtRng
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

internal TxtRng
txt_rng_union(TxtRng a, TxtRng b)
{
  TxtRng result = {0};
  result.min = txt_pt_min(a.min, b.min);
  result.max = txt_pt_max(a.max, b.max);
  return result;
}

internal B32
txt_rng_contains(TxtRng r, TxtPt pt)
{
  B32 result = ((txt_pt_less_than(r.min, pt) || txt_pt_match(r.min, pt)) &&
                txt_pt_less_than(pt, r.max));
  return result;
}

////////////////////////////////
//~ rjf: Toolchain/Environment Enum Functions

internal ulong
bit_size_from_arch(Architecture arch)
{
  // TODO(rjf): metacode
  ulong arch_bitsize = 0;
  switch(arch)
  {
    case Architecture_x64:   arch_bitsize = 64; break;
    case Architecture_x86:   arch_bitsize = 32; break;
    case Architecture_arm64: arch_bitsize = 64; break;
    case Architecture_arm32: arch_bitsize = 32; break;
    default: break;
  }
  return arch_bitsize;
}

internal ulong
max_instruction_size_from_arch(Architecture arch)
{
  // TODO(rjf): make this real
  return 64;
}

internal OperatingSystem
operating_system_from_context(void){
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

internal Architecture
architecture_from_context(void){
  Architecture arch = Architecture_Null;
#if ARCH_X64
  arch = Architecture_x64;
#elif ARCH_X86
  arch = Architecture_x86;
#elif ARCH_ARM64
  arch = Architecture_arm64;
#elif ARCH_ARM32
  arch = Architecture_arm32;
#endif
  return arch;
}

internal Compiler
compiler_from_context(void){
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

internal DenseTime
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

internal DateTime
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
  result.year = (uint)time;
  return(result);
}

internal DateTime
date_time_from_micro_seconds(ulong time){
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
  result.year = (uint)time;
  return(result);
}

////////////////////////////////
//~ rjf: Non-Fancy Ring Buffer Reads/Writes

internal ulong
ring_write(byte *ring_base, ulong ring_size, ulong ring_pos, void *src_data, ulong src_data_size)
{
  Assert(src_data_size <= ring_size);
  {
    ulong ring_off = ring_pos%ring_size;
    ulong bytes_before_split = ring_size-ring_off;
    ulong pre_split_bytes = Min(bytes_before_split, src_data_size);
    ulong pst_split_bytes = src_data_size-pre_split_bytes;
    void *pre_split_data = src_data;
    void *pst_split_data = ((byte *)src_data + pre_split_bytes);
    MemoryCopy(ring_base+ring_off, pre_split_data, pre_split_bytes);
    MemoryCopy(ring_base+0, pst_split_data, pst_split_bytes);
  }
  return src_data_size;
}

internal ulong
ring_read(byte *ring_base, ulong ring_size, ulong ring_pos, void *dst_data, ulong read_size)
{
  Assert(read_size <= ring_size);
  {
    ulong ring_off = ring_pos%ring_size;
    ulong bytes_before_split = ring_size-ring_off;
    ulong pre_split_bytes = Min(bytes_before_split, read_size);
    ulong pst_split_bytes = read_size-pre_split_bytes;
    MemoryCopy(dst_data, ring_base+ring_off, pre_split_bytes);
    MemoryCopy((byte *)dst_data + pre_split_bytes, ring_base+0, pst_split_bytes);
  }
  return read_size;
}

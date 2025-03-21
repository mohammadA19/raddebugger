// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_CORE_H
#define BASE_CORE_H

////////////////////////////////
//~ rjf: Foreign Includes

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

////////////////////////////////
//~ rjf: Codebase Keywords

#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
# define read_only __attribute__((section(".rodata")))
#else
// NOTE(rjf): I don't know of a useful way to do this in GCC land.
// __attribute__((section(".rodata"))) looked promising, but it introduces a
// strange warning about malformed section attributes, and it doesn't look
// like writing to that section reliably produces access violations, strangely
// enough. (It does on Clang)
# define read_only
#endif

#if COMPILER_MSVC
# define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
# define thread_static __thread
#endif

#if COMPILER_MSVC
# define force_inline __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
# define force_inline __attribute__((always_inline))
#endif

////////////////////////////////
//~ rjf: Linkage Keyword Macros

#if OS_WINDOWS
# define shared_function C_LINKAGE __declspec(dllexport)
#else
# define shared_function C_LINKAGE
#endif

#if LANG_CPP
# define C_LINKAGE_BEGIN extern "C"{
# define C_LINKAGE_END }
# define C_LINKAGE extern "C"
#else
# define C_LINKAGE_BEGIN
# define C_LINKAGE_END
# define C_LINKAGE
#endif

////////////////////////////////
//~ rjf: Units

#define KB(n)  (((ulong)(n)) << 10)
#define MB(n)  (((ulong)(n)) << 20)
#define GB(n)  (((ulong)(n)) << 30)
#define TB(n)  (((ulong)(n)) << 40)
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

////////////////////////////////
//~ rjf: Branch Predictor Hints

#if defined(__clang__)
# define Expect(expr, val) __builtin_expect((expr), (val))
#else
# define Expect(expr, val) (expr)
#endif

#define Likely(expr)            Expect(expr,1)
#define Unlikely(expr)          Expect(expr,0)

////////////////////////////////
//~ rjf: Clamps, Mins, Maxes

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define ClampTop(A,X) Min(A,X)
#define ClampBot(X,B) Max(X,B)
#define Clamp(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

////////////////////////////////
//~ rjf: Type -> Alignment

#if COMPILER_MSVC
# define AlignOf(T) __alignof(T)
#elif COMPILER_CLANG
# define AlignOf(T) __alignof(T)
#elif COMPILER_GCC
# define AlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

////////////////////////////////
//~ rjf: Member Offsets

#define Member(T,m)                 (((T*)0)->m)
#define OffsetOf(T,m)               IntFromPtr(&Member(T,m))
#define MemberFromOffset(T,ptr,off) (T)((((byte *)ptr)+(off)))
#define CastFromMember(T,m,ptr)     (T*)(((byte*)ptr) - OffsetOf(T,m))

////////////////////////////////
//~ rjf: For-Loop Construct Macros

#define DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

#define EachIndex(it, count) (ulong it = 0; it < (count); it += 1)
#define EachElement(it, array) (ulong it = 0; it < ArrayCount(array); it += 1)
#define EachEnumVal(type, it) (type it = (type)0; it < type##_COUNT; it = (type)(it+1))
#define EachNonZeroEnumVal(type, it) (type it = (type)1; it < type##_COUNT; it = (type)(it+1))

////////////////////////////////
//~ rjf: Memory Operation Macros

#define MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define MemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define MemoryCompare(a, b, size)     memcmp((a), (b), (size))
#define MemoryStrlen(ptr)             strlen(ptr)

#define MemoryCopyStruct(d,s)  MemoryCopy((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   MemoryCopy((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),sizeof(*(d))*(c))

#define MemoryZero(s,z)       memset((s),0,(z))
#define MemoryZeroStruct(s)   MemoryZero((s),sizeof(*(s)))
#define MemoryZeroArray(a)    MemoryZero((a),sizeof(a))
#define MemoryZeroTyped(m,c)  MemoryZero((m),sizeof(*(m))*(c))

#define MemoryMatch(a,b,z)     (MemoryCompare((a),(b),(z)) == 0)
#define MemoryMatchStruct(a,b)  MemoryMatch((a),(b),sizeof(*(a)))
#define MemoryMatchArray(a,b)   MemoryMatch((a),(b),sizeof(a))

#define MemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define MemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

////////////////////////////////
//~ rjf: Asserts

#if COMPILER_MSVC
# define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

#define AssertAlways(x) do{if(!(x)) {Trap();}}while(0)
#if BUILD_DEBUG
# define Assert(x) AssertAlways(x)
#else
# define Assert(x) ()(x)
#endif
#define InvalidPath        Assert(!"Invalid Path!")
#define NotImplemented     Assert(!"Not Implemented!")
#define NoOp               (()0)
#define StaticAssert(C, ID) static byte Glue(ID, __LINE__)[(C)?1:-1]

////////////////////////////////
//~ rjf: Atomic Operations

#if COMPILER_MSVC
# include <intrin.h>
# if ARCH_X64
#  define ins_atomic_u64_eval(x)                 *((volatile ulong *)(x))
#  define ins_atomic_u64_inc_eval(x)             InterlockedIncrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_dec_eval(x)             InterlockedDecrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_eval_assign(x,c)        InterlockedExchange64((volatile __int64 *)(x),(c))
#  define ins_atomic_u64_add_eval(x,c)           InterlockedAdd64((volatile __int64 *)(x), c)
#  define ins_atomic_u64_eval_cond_assign(x,k,c) InterlockedCompareExchange64((volatile __int64 *)(x),(k),(c))
#  define ins_atomic_u32_eval(x)                 *((volatile uint *)(x))
#  define ins_atomic_u32_inc_eval(x)             InterlockedIncrement((volatile LONG *)x)
#  define ins_atomic_u32_eval_assign(x,c)        InterlockedExchange((volatile LONG *)(x),(c))
#  define ins_atomic_u32_eval_cond_assign(x,k,c) InterlockedCompareExchange((volatile LONG *)(x),(k),(c))
#  define ins_atomic_u32_add_eval(x,c)           InterlockedAdd((volatile LONG *)(x), c)
# else
#  error Atomic intrinsics not defined for this compiler / architecture combination.
# endif
#elif COMPILER_CLANG || COMPILER_GCC
#  define ins_atomic_u64_eval(x)                 __atomic_load_n(x, __ATOMIC_SEQ_CST)
#  define ins_atomic_u64_inc_eval(x)             (__atomic_fetch_add((volatile ulong *)(x), 1, __ATOMIC_SEQ_CST) + 1)
#  define ins_atomic_u64_dec_eval(x)             (__atomic_fetch_sub((volatile ulong *)(x), 1, __ATOMIC_SEQ_CST) - 1)
#  define ins_atomic_u64_eval_assign(x,c)        __atomic_exchange_n(x, c, __ATOMIC_SEQ_CST)
#  define ins_atomic_u64_add_eval(x,c)           (__atomic_fetch_add((volatile ulong *)(x), c, __ATOMIC_SEQ_CST) + (c))
#  define ins_atomic_u64_eval_cond_assign(x,k,c) ({ ulong _new = (c); __atomic_compare_exchange_n((volatile ulong *)(x),&_new,(k),0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST); _new; })
#  define ins_atomic_u32_eval(x)                 __atomic_load_n(x, __ATOMIC_SEQ_CST)
#  define ins_atomic_u32_inc_eval(x)             (__atomic_fetch_add((volatile uint *)(x), 1, __ATOMIC_SEQ_CST) + 1)
#  define ins_atomic_u32_add_eval(x,c)           (__atomic_fetch_add((volatile uint *)(x), c, __ATOMIC_SEQ_CST) + (c))
#  define ins_atomic_u32_eval_assign(x,c)        __atomic_exchange_n(x, c, __ATOMIC_SEQ_CST)
#  define ins_atomic_u32_eval_cond_assign(x,k,c) ({ uint _new = (c); __atomic_compare_exchange_n((volatile uint *)(x),&_new,(k),0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST); _new; })
#else
#  error Atomic intrinsics not defined for this compiler / architecture.
#endif

#if ARCH_64BIT
# define ins_atomic_ptr_eval_cond_assign(x,k,c) (void*)ins_atomic_u64_eval_cond_assign((volatile ulong *)(x), (ulong)(k), (ulong)(c))
# define ins_atomic_ptr_eval_assign(x,c)        (void*)ins_atomic_u64_eval_assign((volatile ulong *)(x), (ulong)(c))
# define ins_atomic_ptr_eval(x)                 (void*)ins_atomic_u64_eval((volatile ulong *)x)
#elif ARCH_32BIT
# define ins_atomic_ptr_eval_cond_assign(x,k,c) (void*)ins_atomic_u32_eval_cond_assign((volatile uint *)(x), (uint)(k), (uint)(c))
# define ins_atomic_ptr_eval_assign(x,c)        (void*)ins_atomic_u32_eval_assign((volatile uint *)(x), (uint)(c))
# define ins_atomic_ptr_eval(x)                 (void*)ins_atomic_u32_eval((volatile uint *)x)
#else
# error Atomic intrinsics for pointers not defined for this architecture.
#endif

////////////////////////////////
//~ rjf: Linked List Building Macros

//- rjf: linked list macro helpers
#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

//- rjf: doubly-linked-lists
#define DLLInsert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ? \
((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :\
CheckNil(nil,p) ? \
((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :\
((p)==(l)) ? \
((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil, (n)->next)) :\
(((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define DLLPushBack_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define DLLPushFront_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define DLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),\
((n) == (l) ? (l) = (l)->prev : (0)),\
(CheckNil(nil,(n)->prev) ? (0) :\
((n)->prev->next = (n)->next)),\
(CheckNil(nil,(n)->next) ? (0) :\
((n)->next->prev = (n)->prev)))

//- rjf: singly-linked, doubly-headed lists (queues)
#define SLLQueuePush_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((l)->next=(n),(l)=(n),SetNil(nil,(n)->next)))
#define SLLQueuePushFront_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((n)->next=(f),(f)=(n)))
#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)?\
(SetNil(nil,f),SetNil(nil,l)):\
((f)=(f)->next))

//- rjf: singly-linked, singly-headed lists (stacks)
#define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLLStackPop_N(f,next) ((f)=(f)->next)

//- rjf: doubly-linked-list helpers
#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(0,f,l,n,next,prev)
#define DLLInsert(f,l,p,n) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack(f,l,n) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront(f,l,n) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove(f,l,n) DLLRemove_NPZ(0,f,l,n,next,prev)

//- rjf: singly-linked, doubly-headed list helpers
#define SLLQueuePush_N(f,l,n,next) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront_N(f,l,n,next) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop_N(f,l,next) SLLQueuePop_NZ(0,f,l,next)
#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop(f,l) SLLQueuePop_NZ(0,f,l,next)

//- rjf: singly-linked, singly-headed list helpers
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop(f) SLLStackPop_N(f,next)

////////////////////////////////
//~ rjf: Address Sanitizer Markup

#if COMPILER_MSVC
# if defined(__SANITIZE_ADDRESS__)
#  define ASAN_ENABLED 1
#  define NO_ASAN __declspec(no_sanitize_address)
# else
#  define NO_ASAN
# endif
#elif COMPILER_CLANG
# if defined(__has_feature)
#  if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#   define ASAN_ENABLED 1
#  endif
# endif
# define NO_ASAN __attribute__((no_sanitize("address")))
#else
# define NO_ASAN
#endif

#if ASAN_ENABLED
#pragma comment(lib, "clang_rt.asan-x86_64.lib")
C_LINKAGE void __asan_poison_memory_region(void const volatile *addr, size_t size);
C_LINKAGE void __asan_unpoison_memory_region(void const volatile *addr, size_t size);
# define AsanPoisonMemoryRegion(addr, size)   __asan_poison_memory_region((addr), (size))
# define AsanUnpoisonMemoryRegion(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
# define AsanPoisonMemoryRegion(addr, size)   (()(addr), ()(size))
# define AsanUnpoisonMemoryRegion(addr, size) (()(addr), ()(size))
#endif

////////////////////////////////
//~ rjf: Misc. Helper Macros

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)

#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define CeilIntegerDiv(a,b) (((a) + (b) - 1)/(b))

#define Swap(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)

#if ARCH_64BIT
# define IntFromPtr(ptr) ((ulong)(ptr))
#elif ARCH_32BIT
# define IntFromPtr(ptr) ((uint)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif
#define PtrFromInt(i) (void*)((byte*)0 + (i))

#define Compose64Bit(a,b)  ((((ulong)a) << 32) | ((ulong)b));
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define ExtractBit(word, idx) (((word) >> (idx)) & 1)

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define this_function_name "unknown"
#else
# define this_function_name __func__
#endif

////////////////////////////////
//~ rjf: Base Types

typedef uint8_t  byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t ulong;
typedef int8_t   sbyte;
typedef int16_t  short;
typedef int32_t  int;
typedef int64_t  long;
typedef sbyte       B8;
typedef short      B16;
typedef int      B32;
typedef long      B64;
typedef float    F32;
typedef double   F64;
typedef void VoidProc();
struct U128
{
  ulong u64[2];
};

////////////////////////////////
//~ rjf: Basic Types & Spaces

enum Dimension
{
  Dimension_X,
  Dimension_Y,
  Dimension_Z,
  Dimension_W,
};

enum Side
{
  Side_Invalid = -1,
  Side_Min,
  Side_Max,
  Side_COUNT,
};
#define side_flip(s) ((Side)(!(s)))

enum Axis2
{
  Axis2_Invalid = -1,
  Axis2_X,
  Axis2_Y,
  Axis2_COUNT,
};
#define axis2_flip(a) ((Axis2)(!(a)))

enum Corner
{
  Corner_Invalid = -1,
  Corner_00,
  Corner_01,
  Corner_10,
  Corner_11,
  Corner_COUNT
};

enum Dir2
{
  Dir2_Invalid = -1,
  Dir2_Left,
  Dir2_Up,
  Dir2_Right,
  Dir2_Down,
  Dir2_COUNT
};
#define axis2_from_dir2(d) (((d) & 1) ? Axis2_Y : Axis2_X)
#define side_from_dir2(d) (((d) < Dir2_Right) ? Side_Min : Side_Max)

////////////////////////////////
//~ rjf: Toolchain/Environment Enums

enum OperatingSystem
{
  OperatingSystem_Null,
  OperatingSystem_Windows,
  OperatingSystem_Linux,
  OperatingSystem_Mac,
  OperatingSystem_COUNT,
};

enum ImageType
{
  Image_Null,
  Image_CoffPe,
  Image_Elf32,
  Image_Elf64,
  Image_Macho
};

enum Arch
{
  Arch_Null,
  Arch_x64,
  Arch_x86,
  Arch_arm64,
  Arch_arm32,
  Arch_COUNT,
};

enum Compiler
{
  Compiler_Null,
  Compiler_msvc,
  Compiler_gcc,
  Compiler_clang,
  Compiler_COUNT,
};

////////////////////////////////
//~ rjf: Text 2D Coordinates & Ranges

struct TxtPt
{
  long line;
  long column;
};

struct TxtRng
{
  TxtPt min;
  TxtPt max;
};

////////////////////////////////
//~ Globally Unique Ids

union Guid
{
  struct
  {
    uint data1;
    ushort data2;
    ushort data3;
    byte  data4[8];
  };
  byte v[16];
};
StaticAssert(sizeof(Guid) == 16, g_guid_size_check);

////////////////////////////////
//~ Arrays

struct U16Array
{
  ulong  count;
  ushort *v;
};
struct U32Array
{
  ulong  count;
  uint *v;
};
struct U64Array
{
  ulong  count;
  ulong *v;
};
struct U128Array
{
  ulong   count;
  U128 *v;
};

////////////////////////////////
//~ NOTE(allen): Constants

static uint sign32     = 0x80000000;
static uint exponent32 = 0x7F800000;
static uint mantissa32 = 0x007FFFFF;

static F32   big_golden32 = 1.61803398875f;
static F32 small_golden32 = 0.61803398875f;

static F32 pi32 = 3.1415926535897f;

static F64 machine_epsilon64 = 4.94065645841247e-324;

static ulong max_U64 = 0xffffffffffffffffull;
static uint max_U32 = 0xffffffff;
static ushort max_U16 = 0xffff;
static byte  max_U8  = 0xff;

static long max_S64 = (long)0x7fffffffffffffffull;
static int max_S32 = (int)0x7fffffff;
static short max_S16 = (short)0x7fff;
static sbyte  max_S8  =  (sbyte)0x7f;

static long min_S64 = (long)0xffffffffffffffffull;
static int min_S32 = (int)0xffffffff;
static short min_S16 = (short)0xffff;
static sbyte  min_S8  =  (sbyte)0xff;

static const uint bitmask1  = 0x00000001;
static const uint bitmask2  = 0x00000003;
static const uint bitmask3  = 0x00000007;
static const uint bitmask4  = 0x0000000f;
static const uint bitmask5  = 0x0000001f;
static const uint bitmask6  = 0x0000003f;
static const uint bitmask7  = 0x0000007f;
static const uint bitmask8  = 0x000000ff;
static const uint bitmask9  = 0x000001ff;
static const uint bitmask10 = 0x000003ff;
static const uint bitmask11 = 0x000007ff;
static const uint bitmask12 = 0x00000fff;
static const uint bitmask13 = 0x00001fff;
static const uint bitmask14 = 0x00003fff;
static const uint bitmask15 = 0x00007fff;
static const uint bitmask16 = 0x0000ffff;
static const uint bitmask17 = 0x0001ffff;
static const uint bitmask18 = 0x0003ffff;
static const uint bitmask19 = 0x0007ffff;
static const uint bitmask20 = 0x000fffff;
static const uint bitmask21 = 0x001fffff;
static const uint bitmask22 = 0x003fffff;
static const uint bitmask23 = 0x007fffff;
static const uint bitmask24 = 0x00ffffff;
static const uint bitmask25 = 0x01ffffff;
static const uint bitmask26 = 0x03ffffff;
static const uint bitmask27 = 0x07ffffff;
static const uint bitmask28 = 0x0fffffff;
static const uint bitmask29 = 0x1fffffff;
static const uint bitmask30 = 0x3fffffff;
static const uint bitmask31 = 0x7fffffff;
static const uint bitmask32 = 0xffffffff;

static const ulong bitmask33 = 0x00000001ffffffffull;
static const ulong bitmask34 = 0x00000003ffffffffull;
static const ulong bitmask35 = 0x00000007ffffffffull;
static const ulong bitmask36 = 0x0000000fffffffffull;
static const ulong bitmask37 = 0x0000001fffffffffull;
static const ulong bitmask38 = 0x0000003fffffffffull;
static const ulong bitmask39 = 0x0000007fffffffffull;
static const ulong bitmask40 = 0x000000ffffffffffull;
static const ulong bitmask41 = 0x000001ffffffffffull;
static const ulong bitmask42 = 0x000003ffffffffffull;
static const ulong bitmask43 = 0x000007ffffffffffull;
static const ulong bitmask44 = 0x00000fffffffffffull;
static const ulong bitmask45 = 0x00001fffffffffffull;
static const ulong bitmask46 = 0x00003fffffffffffull;
static const ulong bitmask47 = 0x00007fffffffffffull;
static const ulong bitmask48 = 0x0000ffffffffffffull;
static const ulong bitmask49 = 0x0001ffffffffffffull;
static const ulong bitmask50 = 0x0003ffffffffffffull;
static const ulong bitmask51 = 0x0007ffffffffffffull;
static const ulong bitmask52 = 0x000fffffffffffffull;
static const ulong bitmask53 = 0x001fffffffffffffull;
static const ulong bitmask54 = 0x003fffffffffffffull;
static const ulong bitmask55 = 0x007fffffffffffffull;
static const ulong bitmask56 = 0x00ffffffffffffffull;
static const ulong bitmask57 = 0x01ffffffffffffffull;
static const ulong bitmask58 = 0x03ffffffffffffffull;
static const ulong bitmask59 = 0x07ffffffffffffffull;
static const ulong bitmask60 = 0x0fffffffffffffffull;
static const ulong bitmask61 = 0x1fffffffffffffffull;
static const ulong bitmask62 = 0x3fffffffffffffffull;
static const ulong bitmask63 = 0x7fffffffffffffffull;
static const ulong bitmask64 = 0xffffffffffffffffull;

static const uint bit1  = (1<<0);
static const uint bit2  = (1<<1);
static const uint bit3  = (1<<2);
static const uint bit4  = (1<<3);
static const uint bit5  = (1<<4);
static const uint bit6  = (1<<5);
static const uint bit7  = (1<<6);
static const uint bit8  = (1<<7);
static const uint bit9  = (1<<8);
static const uint bit10 = (1<<9);
static const uint bit11 = (1<<10);
static const uint bit12 = (1<<11);
static const uint bit13 = (1<<12);
static const uint bit14 = (1<<13);
static const uint bit15 = (1<<14);
static const uint bit16 = (1<<15);
static const uint bit17 = (1<<16);
static const uint bit18 = (1<<17);
static const uint bit19 = (1<<18);
static const uint bit20 = (1<<19);
static const uint bit21 = (1<<20);
static const uint bit22 = (1<<21);
static const uint bit23 = (1<<22);
static const uint bit24 = (1<<23);
static const uint bit25 = (1<<24);
static const uint bit26 = (1<<25);
static const uint bit27 = (1<<26);
static const uint bit28 = (1<<27);
static const uint bit29 = (1<<28);
static const uint bit30 = (1<<29);
static const uint bit31 = (1<<30);
static const uint bit32 = (1<<31);

static const ulong bit33 = (1ull<<32);
static const ulong bit34 = (1ull<<33);
static const ulong bit35 = (1ull<<34);
static const ulong bit36 = (1ull<<35);
static const ulong bit37 = (1ull<<36);
static const ulong bit38 = (1ull<<37);
static const ulong bit39 = (1ull<<38);
static const ulong bit40 = (1ull<<39);
static const ulong bit41 = (1ull<<40);
static const ulong bit42 = (1ull<<41);
static const ulong bit43 = (1ull<<42);
static const ulong bit44 = (1ull<<43);
static const ulong bit45 = (1ull<<44);
static const ulong bit46 = (1ull<<45);
static const ulong bit47 = (1ull<<46);
static const ulong bit48 = (1ull<<47);
static const ulong bit49 = (1ull<<48);
static const ulong bit50 = (1ull<<49);
static const ulong bit51 = (1ull<<50);
static const ulong bit52 = (1ull<<51);
static const ulong bit53 = (1ull<<52);
static const ulong bit54 = (1ull<<53);
static const ulong bit55 = (1ull<<54);
static const ulong bit56 = (1ull<<55);
static const ulong bit57 = (1ull<<56);
static const ulong bit58 = (1ull<<57);
static const ulong bit59 = (1ull<<58);
static const ulong bit60 = (1ull<<59);
static const ulong bit61 = (1ull<<60);
static const ulong bit62 = (1ull<<61);
static const ulong bit63 = (1ull<<62);
static const ulong bit64 = (1ull<<63);

////////////////////////////////
//~ allen: Time

enum WeekDay
{
  WeekDay_Sun,
  WeekDay_Mon,
  WeekDay_Tue,
  WeekDay_Wed,
  WeekDay_Thu,
  WeekDay_Fri,
  WeekDay_Sat,
  WeekDay_COUNT,
};

enum Month
{
  Month_Jan,
  Month_Feb,
  Month_Mar,
  Month_Apr,
  Month_May,
  Month_Jun,
  Month_Jul,
  Month_Aug,
  Month_Sep,
  Month_Oct,
  Month_Nov,
  Month_Dec,
  Month_COUNT,
};

struct DateTime
{
  ushort micro_sec; // [0,999]
  ushort msec; // [0,999]
  ushort sec;  // [0,60]
  ushort min;  // [0,59]
  ushort hour; // [0,24]
  ushort day;  // [0,30]
  union
  {
    WeekDay week_day;
    uint wday;
  };
  union
  {
    Month month;
    uint mon;
  };
  uint year; // 1 = 1 CE, 0 = 1 BC
};

typedef ulong DenseTime;

////////////////////////////////
//~ allen: Files

enum FilePropertyFlags : uint
{
  FilePropertyFlag_IsFolder = (1 << 0),
};

struct FileProperties
{
  ulong size;
  DenseTime modified;
  DenseTime created;
  FilePropertyFlags flags;
};

////////////////////////////////
//~ rjf: Safe Casts

ushort safe_cast_u16(uint x);
uint safe_cast_u32(ulong x);
int safe_cast_s32(long x);

////////////////////////////////
//~ rjf: Large Base Type Functions

U128 u128_zero();
U128 u128_make(ulong v0, ulong v1);
B32 u128_match(U128 a, U128 b);

////////////////////////////////
//~ rjf: Bit Patterns

uint u32_from_u64_saturate(ulong x);
ulong u64_up_to_pow2(ulong x);
int extend_sign32(uint x, uint size);
long extend_sign64(ulong x, ulong size);

F32 inf32();
F32 neg_inf32();

ushort bswap_u16(ushort x);
uint bswap_u32(uint x);
ulong bswap_u64(ulong x);

#if ARCH_LITTLE_ENDIAN
# define from_be_u16(x) bswap_u16(x)
# define from_be_u32(x) bswap_u32(x)
# define from_be_u64(x) bswap_u64(x)
#else
# define from_be_u16(x) (x)
# define from_be_u32(x) (x)
# define from_be_u64(x) (x)
#endif

ulong count_bits_set32(uint val);
ulong count_bits_set64(ulong val);

ulong ctz32(uint val);
ulong ctz64(ulong val);
ulong clz32(uint val);
ulong clz64(ulong val);

////////////////////////////////
//~ rjf: Enum -> Sign

int sign_from_side_S32(Side side);
F32 sign_from_side_F32(Side side);

////////////////////////////////
//~ rjf: Memory Functions

B32 memory_is_zero(void *ptr, ulong size);

////////////////////////////////
//~ rjf: Text 2D Coordinate/Range Functions

TxtPt txt_pt(long line, long column);
B32 txt_pt_match(TxtPt a, TxtPt b);
B32 txt_pt_less_than(TxtPt a, TxtPt b);
TxtPt txt_pt_min(TxtPt a, TxtPt b);
TxtPt txt_pt_max(TxtPt a, TxtPt b);
TxtRng txt_rng(TxtPt min, TxtPt max);
TxtRng txt_rng_intersect(TxtRng a, TxtRng b);
TxtRng txt_rng_union(TxtRng a, TxtRng b);
B32 txt_rng_contains(TxtRng r, TxtPt pt);

////////////////////////////////
//~ rjf: Toolchain/Environment Enum Functions

ulong bit_size_from_arch(Arch arch);
ulong max_instruction_size_from_arch(Arch arch);

OperatingSystem operating_system_from_context();
Arch arch_from_context();
Compiler compiler_from_context();

////////////////////////////////
//~ rjf: Time Functions

DenseTime dense_time_from_date_time(DateTime date_time);
DateTime  date_time_from_dense_time(DenseTime time);
DateTime  date_time_from_micro_seconds(ulong time);
DateTime  date_time_from_unix_time(ulong unix_time);

////////////////////////////////
//~ rjf: Non-Fancy Ring Buffer Reads/Writes

ulong ring_write(byte *ring_base, ulong ring_size, ulong ring_pos, void *src_data, ulong src_data_size);
ulong ring_read(byte *ring_base, ulong ring_size, ulong ring_pos, void *dst_data, ulong read_size);
#define ring_write_struct(ring_base, ring_size, ring_pos, ptr) ring_write((ring_base), (ring_size), (ring_pos), (ptr), sizeof(*(ptr)))
#define ring_read_struct(ring_base, ring_size, ring_pos, ptr) ring_read((ring_base), (ring_size), (ring_pos), (ptr), sizeof(*(ptr)))

////////////////////////////////
//~ rjf: Sorts

#define quick_sort(ptr, count, element_size, cmp_function) qsort((ptr), (count), (element_size), (int (*)(const void *, const void *))(cmp_function))

#endif // BASE_CORE_H

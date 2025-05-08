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

#define internal      static
#define global        static
#define local_persist static

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

template KB(n: untyped) =  cast[U64](n) shl 10
template MB(n: untyped) =  cast[U64](n) shl 20
template GB(n: untyped) =  cast[U64](n) shl 30
template TB(n: untyped) =  cast[U64](n) shl 40
template Thousand(n: untyped) =   n * 1000
template Million(n: untyped) =    n * 1000000
template Billion(n: untyped) =    n * 1000000000

////////////////////////////////
//~ rjf: Clamps, Mins, Maxes

template clampTop(A: untyped, X: untyped) = Min(A,X)
template clampBot(X: untyped, B: untyped) = Max(X,B)

////////////////////////////////
//~ rjf: Member Offsets

#define Member(T,m)                 (((T*)0)->m)
#define OffsetOf(T,m)               IntFromPtr(&Member(T,m))
#define MemberFromOffset(T,ptr,off) (T)((((U8 *)ptr)+(off)))
#define CastFromMember(T,m,ptr)     (T*)(((U8*)ptr) - OffsetOf(T,m))

////////////////////////////////
//~ rjf: For-Loop Construct Macros

#define DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

#define EachIndex(it, count) (U64 it = 0; it < (count); it += 1)
#define EachElement(it, array) (U64 it = 0; it < ArrayCount(array); it += 1)
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
# define Assert(x) (void)(x)
#endif
#define InvalidPath        Assert(!"Invalid Path!")
#define NotImplemented     Assert(!"Not Implemented!")
#define NoOp               ((void)0)
#define StaticAssert(C, ID) global U8 Glue(ID, __LINE__)[(C)?1:-1]

////////////////////////////////
//~ rjf: Atomic Operations

#if COMPILER_MSVC
# include <intrin.h>
# if ARCH_X64
#  define ins_atomic_u64_eval(x)                 *((volatile U64 *)(x))
#  define ins_atomic_u64_inc_eval(x)             InterlockedIncrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_dec_eval(x)             InterlockedDecrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_eval_assign(x,c)        InterlockedExchange64((volatile __int64 *)(x),(c))
#  define ins_atomic_u64_add_eval(x,c)           InterlockedAdd64((volatile __int64 *)(x), c)
#  define ins_atomic_u64_eval_cond_assign(x,k,c) InterlockedCompareExchange64((volatile __int64 *)(x),(k),(c))
#  define ins_atomic_u32_eval(x)                 *((volatile U32 *)(x))
#  define ins_atomic_u32_inc_eval(x)             InterlockedIncrement((volatile LONG *)x)
#  define ins_atomic_u32_eval_assign(x,c)        InterlockedExchange((volatile LONG *)(x),(c))
#  define ins_atomic_u32_eval_cond_assign(x,k,c) InterlockedCompareExchange((volatile LONG *)(x),(k),(c))
#  define ins_atomic_u32_add_eval(x,c)           InterlockedAdd((volatile LONG *)(x), c)
# else
#  error Atomic intrinsics not defined for this compiler / architecture combination.
# endif
#elif COMPILER_CLANG || COMPILER_GCC
#  define ins_atomic_u64_eval(x)                 __atomic_load_n(x, __ATOMIC_SEQ_CST)
#  define ins_atomic_u64_inc_eval(x)             (__atomic_fetch_add((volatile U64 *)(x), 1, __ATOMIC_SEQ_CST) + 1)
#  define ins_atomic_u64_dec_eval(x)             (__atomic_fetch_sub((volatile U64 *)(x), 1, __ATOMIC_SEQ_CST) - 1)
#  define ins_atomic_u64_eval_assign(x,c)        __atomic_exchange_n(x, c, __ATOMIC_SEQ_CST)
#  define ins_atomic_u64_add_eval(x,c)           (__atomic_fetch_add((volatile U64 *)(x), c, __ATOMIC_SEQ_CST) + (c))
#  define ins_atomic_u64_eval_cond_assign(x,k,c) ({ U64 _new = (c); __atomic_compare_exchange_n((volatile U64 *)(x),&_new,(k),0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST); _new; })
#  define ins_atomic_u32_eval(x)                 __atomic_load_n(x, __ATOMIC_SEQ_CST)
#  define ins_atomic_u32_inc_eval(x)             (__atomic_fetch_add((volatile U32 *)(x), 1, __ATOMIC_SEQ_CST) + 1)
#  define ins_atomic_u32_add_eval(x,c)           (__atomic_fetch_add((volatile U32 *)(x), c, __ATOMIC_SEQ_CST) + (c))
#  define ins_atomic_u32_eval_assign(x,c)        __atomic_exchange_n(x, c, __ATOMIC_SEQ_CST)
#  define ins_atomic_u32_eval_cond_assign(x,k,c) ({ U32 _new = (c); __atomic_compare_exchange_n((volatile U32 *)(x),&_new,(k),0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST); _new; })
#else
#  error Atomic intrinsics not defined for this compiler / architecture.
#endif

#if ARCH_64BIT
# define ins_atomic_ptr_eval_cond_assign(x,k,c) (void*)ins_atomic_u64_eval_cond_assign((volatile U64 *)(x), (U64)(k), (U64)(c))
# define ins_atomic_ptr_eval_assign(x,c)        (void*)ins_atomic_u64_eval_assign((volatile U64 *)(x), (U64)(c))
# define ins_atomic_ptr_eval(x)                 (void*)ins_atomic_u64_eval((volatile U64 *)x)
#elif ARCH_32BIT
# define ins_atomic_ptr_eval_cond_assign(x,k,c) (void*)ins_atomic_u32_eval_cond_assign((volatile U32 *)(x), (U32)(k), (U32)(c))
# define ins_atomic_ptr_eval_assign(x,c)        (void*)ins_atomic_u32_eval_assign((volatile U32 *)(x), (U32)(c))
# define ins_atomic_ptr_eval(x)                 (void*)ins_atomic_u32_eval((volatile U32 *)x)
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
# define AsanPoisonMemoryRegion(addr, size)   ((void)(addr), (void)(size))
# define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
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
# define IntFromPtr(ptr) ((U64)(ptr))
#elif ARCH_32BIT
# define IntFromPtr(ptr) ((U32)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif
#define PtrFromInt(i) (void*)((U8*)0 + (i))

#define Compose64Bit(a,b)  ((((U64)a) << 32) | ((U64)b));
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define ExtractBit(word, idx) (((word) >> (idx)) & 1)
#define Extract8(word, pos)   (((word) >> ((pos)*8))  & max_U8)
#define Extract16(word, pos)  (((word) >> ((pos)*16)) & max_U16)
#define Extract32(word, pos)  (((word) >> ((pos)*32)) & max_U32)

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define this_function_name "unknown"
#else
# define this_function_name __func__
#endif

////////////////////////////////
//~ rjf: Base Types

typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;
typedef void VoidProc(void);

type
  U128* {.union.} = object
    u8: array[16, U8]
    u16: array[8, U16]
    u32: array[4, U32]
    u64: array[2, U64]

    U256* {.union.} = object
      u8: array[32, U8]
      u16: array[16, U16]
      u32: array[8, U32]
      u64: array[4, U64]
      u128: array[2, U128]

    U512* {.union.} = object
      u8: array[64, U8]
      u16: array[32, U16]
      u32: array[16, U32]
      u64: array[8, U64]
      u128: array[4, U128]
      u256: array[2, U256]

////////////////////////////////
//~ rjf: Basic Types & Spaces

typedef enum Dimension
{
  Dimension_X,
  Dimension_Y,
  Dimension_Z,
  Dimension_W,
}
Dimension;

typedef enum Side
{
  Side_Invalid = -1,
  Side_Min,
  Side_Max,
  Side_COUNT,
}
Side;
#define side_flip(s) ((Side)(!(s)))

typedef enum Axis2
{
  Axis2_Invalid = -1,
  Axis2_X,
  Axis2_Y,
  Axis2_COUNT,
}
Axis2;
#define axis2_flip(a) ((Axis2)(!(a)))

typedef enum Corner
{
  Corner_Invalid = -1,
  Corner_00,
  Corner_01,
  Corner_10,
  Corner_11,
  Corner_COUNT
}
Corner;

typedef enum Dir2
{
  Dir2_Invalid = -1,
  Dir2_Left,
  Dir2_Up,
  Dir2_Right,
  Dir2_Down,
  Dir2_COUNT
}
Dir2;
#define axis2_from_dir2(d) (((d) & 1) ? Axis2_Y : Axis2_X)
#define side_from_dir2(d) (((d) < Dir2_Right) ? Side_Min : Side_Max)

////////////////////////////////
//~ rjf: Toolchain/Environment Enums

typedef enum OperatingSystem
{
  OperatingSystem_Null,
  OperatingSystem_Windows,
  OperatingSystem_Linux,
  OperatingSystem_Mac,
  OperatingSystem_COUNT,
}
OperatingSystem;

typedef enum ImageType
{
  Image_Null,
  Image_CoffPe,
  Image_Elf32,
  Image_Elf64,
  Image_Macho
} ImageType;

typedef enum Arch
{
  Arch_Null,
  Arch_x64,
  Arch_x86,
  Arch_arm64,
  Arch_arm32,
  Arch_COUNT,
}
Arch;

typedef enum Compiler
{
  Compiler_Null,
  Compiler_msvc,
  Compiler_gcc,
  Compiler_clang,
  Compiler_COUNT,
}
Compiler;

////////////////////////////////
//~ rjf: Text 2D Coordinates & Ranges

typedef struct TxtPt TxtPt;
struct TxtPt
{
  S64 line;
  S64 column;
};

typedef struct TxtRng TxtRng;
struct TxtRng
{
  TxtPt min;
  TxtPt max;
};

////////////////////////////////
//~ Globally Unique Ids

typedef union Guid Guid;
union Guid
{
  struct
  {
    U32 data1;
    U16 data2;
    U16 data3;
    U8  data4[8];
  };
  U8 v[16];
};
StaticAssert(sizeof(Guid) == 16, g_guid_size_check);

////////////////////////////////
//~ Arrays

typedef struct U16Array U16Array;
struct U16Array
{
  U64  count;
  U16 *v;
};
typedef struct U32Array U32Array;
struct U32Array
{
  U64  count;
  U32 *v;
};
typedef struct U64Array U64Array;
struct U64Array
{
  U64  count;
  U64 *v;
};
typedef struct U128Array U128Array;
struct U128Array
{
  U64   count;
  U128 *v;
};

////////////////////////////////
//~ NOTE(allen): Constants

global U32 sign32     = 0x80000000;
global U32 exponent32 = 0x7F800000;
global U32 mantissa32 = 0x007FFFFF;

global F32   big_golden32 = 1.61803398875f;
global F32 small_golden32 = 0.61803398875f;

global F32 pi32 = 3.1415926535897f;

global F64 machine_epsilon64 = 4.94065645841247e-324;

global U64 max_U64 = 0xffffffffffffffffull;
global U32 max_U32 = 0xffffffff;
global U16 max_U16 = 0xffff;
global U8  max_U8  = 0xff;

global S64 max_S64 = (S64)0x7fffffffffffffffull;
global S32 max_S32 = (S32)0x7fffffff;
global S16 max_S16 = (S16)0x7fff;
global S8  max_S8  =  (S8)0x7f;

global S64 min_S64 = (S64)0xffffffffffffffffull;
global S32 min_S32 = (S32)0xffffffff;
global S16 min_S16 = (S16)0xffff;
global S8  min_S8  =  (S8)0xff;

const
  bitmask1 : U32  = 0x00000001
  bitmask2 : U32  = 0x00000003
  bitmask3 : U32  = 0x00000007
  bitmask4 : U32  = 0x0000000f
  bitmask5 : U32  = 0x0000001f
  bitmask6 : U32  = 0x0000003f
  bitmask7 : U32  = 0x0000007f
  bitmask8 : U32  = 0x000000ff
  bitmask9 : U32  = 0x000001ff
  bitmask10 : U32 = 0x000003ff
  bitmask11 : U32 = 0x000007ff
  bitmask12 : U32 = 0x00000fff
  bitmask13 : U32 = 0x00001fff
  bitmask14 : U32 = 0x00003fff
  bitmask15 : U32 = 0x00007fff
  bitmask16 : U32 = 0x0000ffff
  bitmask17 : U32 = 0x0001ffff
  bitmask18 : U32 = 0x0003ffff
  bitmask19 : U32 = 0x0007ffff
  bitmask20 : U32 = 0x000fffff
  bitmask21 : U32 = 0x001fffff
  bitmask22 : U32 = 0x003fffff
  bitmask23 : U32 = 0x007fffff
  bitmask24 : U32 = 0x00ffffff
  bitmask25 : U32 = 0x01ffffff
  bitmask26 : U32 = 0x03ffffff
  bitmask27 : U32 = 0x07ffffff
  bitmask28 : U32 = 0x0fffffff
  bitmask29 : U32 = 0x1fffffff
  bitmask30 : U32 = 0x3fffffff
  bitmask31 : U32 = 0x7fffffff
  bitmask32 : U32 = 0xffffffff

  bitmask33 : U64 = 0x00000001ffffffffull
  bitmask34 : U64 = 0x00000003ffffffffull
  bitmask35 : U64 = 0x00000007ffffffffull
  bitmask36 : U64 = 0x0000000fffffffffull
  bitmask37 : U64 = 0x0000001fffffffffull
  bitmask38 : U64 = 0x0000003fffffffffull
  bitmask39 : U64 = 0x0000007fffffffffull
  bitmask40 : U64 = 0x000000ffffffffffull
  bitmask41 : U64 = 0x000001ffffffffffull
  bitmask42 : U64 = 0x000003ffffffffffull
  bitmask43 : U64 = 0x000007ffffffffffull
  bitmask44 : U64 = 0x00000fffffffffffull
  bitmask45 : U64 = 0x00001fffffffffffull
  bitmask46 : U64 = 0x00003fffffffffffull
  bitmask47 : U64 = 0x00007fffffffffffull
  bitmask48 : U64 = 0x0000ffffffffffffull
  bitmask49 : U64 = 0x0001ffffffffffffull
  bitmask50 : U64 = 0x0003ffffffffffffull
  bitmask51 : U64 = 0x0007ffffffffffffull
  bitmask52 : U64 = 0x000fffffffffffffull
  bitmask53 : U64 = 0x001fffffffffffffull
  bitmask54 : U64 = 0x003fffffffffffffull
  bitmask55 : U64 = 0x007fffffffffffffull
  bitmask56 : U64 = 0x00ffffffffffffffull
  bitmask57 : U64 = 0x01ffffffffffffffull
  bitmask58 : U64 = 0x03ffffffffffffffull
  bitmask59 : U64 = 0x07ffffffffffffffull
  bitmask60 : U64 = 0x0fffffffffffffffull
  bitmask61 : U64 = 0x1fffffffffffffffull
  bitmask62 : U64 = 0x3fffffffffffffffull
  bitmask63 : U64 = 0x7fffffffffffffffull
  bitmask64 : U64 = 0xffffffffffffffffull

  bit1 : U32  = 1 shl 0
  bit2 : U32  = 1 shl 1
  bit3 : U32  = 1 shl 2
  bit4 : U32  = 1 shl 3
  bit5 : U32  = 1 shl 4
  bit6 : U32  = 1 shl 5
  bit7 : U32  = 1 shl 6
  bit8 : U32  = 1 shl 7
  bit9 : U32  = 1 shl 8
  bit10 : U32 = 1 shl 9
  bit11 : U32 = 1 shl 10
  bit12 : U32 = 1 shl 11
  bit13 : U32 = 1 shl 12
  bit14 : U32 = 1 shl 13
  bit15 : U32 = 1 shl 14
  bit16 : U32 = 1 shl 15
  bit17 : U32 = 1 shl 16
  bit18 : U32 = 1 shl 17
  bit19 : U32 = 1 shl 18
  bit20 : U32 = 1 shl 19
  bit21 : U32 = 1 shl 20
  bit22 : U32 = 1 shl 21
  bit23 : U32 = 1 shl 22
  bit24 : U32 = 1 shl 23
  bit25 : U32 = 1 shl 24
  bit26 : U32 = 1 shl 25
  bit27 : U32 = 1 shl 26
  bit28 : U32 = 1 shl 27
  bit29 : U32 = 1 shl 28
  bit30 : U32 = 1 shl 29
  bit31 : U32 = 1 shl 30
  bit32 : U32 = 1 shl 31

  bit33 : U64 = 1 shl 32
  bit34 : U64 = 1 shl 33
  bit35 : U64 = 1 shl 34
  bit36 : U64 = 1 shl 35
  bit37 : U64 = 1 shl 36
  bit38 : U64 = 1 shl 37
  bit39 : U64 = 1 shl 38
  bit40 : U64 = 1 shl 39
  bit41 : U64 = 1 shl 40
  bit42 : U64 = 1 shl 41
  bit43 : U64 = 1 shl 42
  bit44 : U64 = 1 shl 43
  bit45 : U64 = 1 shl 44
  bit46 : U64 = 1 shl 45
  bit47 : U64 = 1 shl 46
  bit48 : U64 = 1 shl 47
  bit49 : U64 = 1 shl 48
  bit50 : U64 = 1 shl 49
  bit51 : U64 = 1 shl 50
  bit52 : U64 = 1 shl 51
  bit53 : U64 = 1 shl 52
  bit54 : U64 = 1 shl 53
  bit55 : U64 = 1 shl 54
  bit56 : U64 = 1 shl 55
  bit57 : U64 = 1 shl 56
  bit58 : U64 = 1 shl 57
  bit59 : U64 = 1 shl 58
  bit60 : U64 = 1 shl 59
  bit61 : U64 = 1 shl 60
  bit62 : U64 = 1 shl 61
  bit63 : U64 = 1 shl 62
  bit64 : U64 = 1 shl 63

////////////////////////////////
//~ allen: Time

typedef enum WeekDay
{
  WeekDay_Sun,
  WeekDay_Mon,
  WeekDay_Tue,
  WeekDay_Wed,
  WeekDay_Thu,
  WeekDay_Fri,
  WeekDay_Sat,
  WeekDay_COUNT,
}
WeekDay;

typedef enum Month
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
}
Month;

typedef struct DateTime DateTime;
struct DateTime
{
  U16 micro_sec; // [0,999]
  U16 msec; // [0,999]
  U16 sec;  // [0,60]
  U16 min;  // [0,59]
  U16 hour; // [0,24]
  U16 day;  // [0,30]
  union
  {
    WeekDay week_day;
    U32 wday;
  };
  union
  {
    Month month;
    U32 mon;
  };
  U32 year; // 1 = 1 CE, 0 = 1 BC
};

typedef U64 DenseTime;

////////////////////////////////
//~ allen: Files

typedef U32 FilePropertyFlags;
enum
{
  FilePropertyFlag_IsFolder = (1 << 0),
};

typedef struct FileProperties FileProperties;
struct FileProperties
{
  U64 size;
  DenseTime modified;
  DenseTime created;
  FilePropertyFlags flags;
};

////////////////////////////////
//~ rjf: Safe Casts

internal U16 safe_cast_u16(U32 x);
internal U32 safe_cast_u32(U64 x);
internal S32 safe_cast_s32(S64 x);

////////////////////////////////
//~ rjf: Large Base Type Functions

internal U128 u128_zero(void);
internal U128 u128_make(U64 v0, U64 v1);
internal B32 u128_match(U128 a, U128 b);

////////////////////////////////
//~ rjf: Bit Patterns

internal U32 u32_from_u64_saturate(U64 x);
internal U64 u64_up_to_pow2(U64 x);
internal S32 extend_sign32(U32 x, U32 size);
internal S64 extend_sign64(U64 x, U64 size);

internal F32 inf32(void);
internal F32 neg_inf32(void);

internal U16 bswap_u16(U16 x);
internal U32 bswap_u32(U32 x);
internal U64 bswap_u64(U64 x);

#if ARCH_LITTLE_ENDIAN
# define from_be_u16(x) bswap_u16(x)
# define from_be_u32(x) bswap_u32(x)
# define from_be_u64(x) bswap_u64(x)
#else
# define from_be_u16(x) (x)
# define from_be_u32(x) (x)
# define from_be_u64(x) (x)
#endif

internal U64 count_bits_set32(U32 val);
internal U64 count_bits_set64(U64 val);

internal U64 ctz32(U32 val);
internal U64 ctz64(U64 val);
internal U64 clz32(U32 val);
internal U64 clz64(U64 val);

////////////////////////////////
//~ rjf: Enum -> Sign

internal S32 sign_from_side_S32(Side side);
internal F32 sign_from_side_F32(Side side);

////////////////////////////////
//~ rjf: Memory Functions

internal B32 memory_is_zero(void *ptr, U64 size);

////////////////////////////////
//~ rjf: Text 2D Coordinate/Range Functions

internal TxtPt txt_pt(S64 line, S64 column);
internal B32 txt_pt_match(TxtPt a, TxtPt b);
internal B32 txt_pt_less_than(TxtPt a, TxtPt b);
internal TxtPt txt_pt_min(TxtPt a, TxtPt b);
internal TxtPt txt_pt_max(TxtPt a, TxtPt b);
internal TxtRng txt_rng(TxtPt min, TxtPt max);
internal TxtRng txt_rng_intersect(TxtRng a, TxtRng b);
internal TxtRng txt_rng_union(TxtRng a, TxtRng b);
internal B32 txt_rng_contains(TxtRng r, TxtPt pt);

////////////////////////////////
//~ rjf: Toolchain/Environment Enum Functions

internal U64 bit_size_from_arch(Arch arch);
internal U64 max_instruction_size_from_arch(Arch arch);

internal OperatingSystem operating_system_from_context(void);
internal Arch arch_from_context(void);
internal Compiler compiler_from_context(void);

////////////////////////////////
//~ rjf: Time Functions

internal DenseTime dense_time_from_date_time(DateTime date_time);
internal DateTime  date_time_from_dense_time(DenseTime time);
internal DateTime  date_time_from_micro_seconds(U64 time);
internal DateTime  date_time_from_unix_time(U64 unix_time);

////////////////////////////////
//~ rjf: Non-Fancy Ring Buffer Reads/Writes

internal U64 ring_write(U8 *ring_base, U64 ring_size, U64 ring_pos, void *src_data, U64 src_data_size);
internal U64 ring_read(U8 *ring_base, U64 ring_size, U64 ring_pos, void *dst_data, U64 read_size);
#define ring_write_struct(ring_base, ring_size, ring_pos, ptr) ring_write((ring_base), (ring_size), (ring_pos), (ptr), sizeof(*(ptr)))
#define ring_read_struct(ring_base, ring_size, ring_pos, ptr) ring_read((ring_base), (ring_size), (ring_pos), (ptr), sizeof(*(ptr)))

////////////////////////////////
//~ rjf: Sorts

#define quick_sort(ptr, count, element_size, cmp_function) qsort((ptr), (count), (element_size), (int (*)(const void *, const void *))(cmp_function))

////////////////////////////////

internal U64 u64_array_bsearch(U64 *arr, U64 count, U64 value);

#endif // BASE_CORE_H

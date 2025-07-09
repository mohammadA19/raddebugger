// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




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

#define KB(n)  (((u64)(n)) << 10)
#define MB(n)  (((u64)(n)) << 20)
#define GB(n)  (((u64)(n)) << 30)
#define TB(n)  (((u64)(n)) << 40)
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
//~ rjf: Member Offsets

#define Member(T,m)                 (((T*)0)->m)
#define OffsetOf(T,m)               IntFromPtr(&Member(T,m))
#define MemberFromOffset(T,ptr,off) (T)((((u8 *)ptr)+(off)))
#define CastFromMember(T,m,ptr)     (T*)(((u8*)ptr) - OffsetOf(T,m))

////////////////////////////////
//~ rjf: For-Loop Construct Macros

#define DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

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
#define StaticAssert(C, ID) global u8 Glue(ID, __LINE__)[(C)?1:-1]

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
//~ rjf: Misc. Helper Macros

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)

#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define CeilIntegerDiv(a,b) (((a) + (b) - 1)/(b))

#define Swap(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)

#if ARCH_64BIT
# define IntFromPtr(ptr) ((u64)(ptr))
#elif ARCH_32BIT
# define IntFromPtr(ptr) ((u32)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif
#define PtrFromInt(i) (void*)((u8*)0 + (i))

#define Compose64Bit(a,b)  ((((u64)a) << 32) | ((u64)b));
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

typedef void VoidProc(void);

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



struct TxtPt
{
  i64 line;
  i64 column;
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
    u32 data1;
    u16 data2;
    u16 data3;
    u8  data4[8];
  };
  u8 v[16];
};
StaticAssert(sizeof(Guid) == 16, g_guid_size_check);

////////////////////////////////
//~ NOTE(allen): Constants

global u32 sign32     = 0x80000000;
global u32 exponent32 = 0x7F800000;
global u32 mantissa32 = 0x007FFFFF;

global f32   big_golden32 = 1.61803398875f;
global f32 small_golden32 = 0.61803398875f;

global f32 pi32 = 3.1415926535897f;

global f64 machine_epsilon64 = 4.94065645841247e-324;

global u64 max_U64 = 0xffffffffffffffffull;
global u32 max_U32 = 0xffffffff;
global u16 max_U16 = 0xffff;
global u8  max_U8  = 0xff;

global i64 max_S64 = (i64)0x7fffffffffffffffull;
global i32 max_S32 = (i32)0x7fffffff;
global i16 max_S16 = (i16)0x7fff;
global i8  max_S8  =  (i8)0x7f;

global i64 min_S64 = (i64)0xffffffffffffffffull;
global i32 min_S32 = (i32)0xffffffff;
global i16 min_S16 = (i16)0xffff;
global i8  min_S8  =  (i8)0xff;

global const u32 bitmask1  = 0x00000001;
global const u32 bitmask2  = 0x00000003;
global const u32 bitmask3  = 0x00000007;
global const u32 bitmask4  = 0x0000000f;
global const u32 bitmask5  = 0x0000001f;
global const u32 bitmask6  = 0x0000003f;
global const u32 bitmask7  = 0x0000007f;
global const u32 bitmask8  = 0x000000ff;
global const u32 bitmask9  = 0x000001ff;
global const u32 bitmask10 = 0x000003ff;
global const u32 bitmask11 = 0x000007ff;
global const u32 bitmask12 = 0x00000fff;
global const u32 bitmask13 = 0x00001fff;
global const u32 bitmask14 = 0x00003fff;
global const u32 bitmask15 = 0x00007fff;
global const u32 bitmask16 = 0x0000ffff;
global const u32 bitmask17 = 0x0001ffff;
global const u32 bitmask18 = 0x0003ffff;
global const u32 bitmask19 = 0x0007ffff;
global const u32 bitmask20 = 0x000fffff;
global const u32 bitmask21 = 0x001fffff;
global const u32 bitmask22 = 0x003fffff;
global const u32 bitmask23 = 0x007fffff;
global const u32 bitmask24 = 0x00ffffff;
global const u32 bitmask25 = 0x01ffffff;
global const u32 bitmask26 = 0x03ffffff;
global const u32 bitmask27 = 0x07ffffff;
global const u32 bitmask28 = 0x0fffffff;
global const u32 bitmask29 = 0x1fffffff;
global const u32 bitmask30 = 0x3fffffff;
global const u32 bitmask31 = 0x7fffffff;
global const u32 bitmask32 = 0xffffffff;

global const u64 bitmask33 = 0x00000001ffffffffull;
global const u64 bitmask34 = 0x00000003ffffffffull;
global const u64 bitmask35 = 0x00000007ffffffffull;
global const u64 bitmask36 = 0x0000000fffffffffull;
global const u64 bitmask37 = 0x0000001fffffffffull;
global const u64 bitmask38 = 0x0000003fffffffffull;
global const u64 bitmask39 = 0x0000007fffffffffull;
global const u64 bitmask40 = 0x000000ffffffffffull;
global const u64 bitmask41 = 0x000001ffffffffffull;
global const u64 bitmask42 = 0x000003ffffffffffull;
global const u64 bitmask43 = 0x000007ffffffffffull;
global const u64 bitmask44 = 0x00000fffffffffffull;
global const u64 bitmask45 = 0x00001fffffffffffull;
global const u64 bitmask46 = 0x00003fffffffffffull;
global const u64 bitmask47 = 0x00007fffffffffffull;
global const u64 bitmask48 = 0x0000ffffffffffffull;
global const u64 bitmask49 = 0x0001ffffffffffffull;
global const u64 bitmask50 = 0x0003ffffffffffffull;
global const u64 bitmask51 = 0x0007ffffffffffffull;
global const u64 bitmask52 = 0x000fffffffffffffull;
global const u64 bitmask53 = 0x001fffffffffffffull;
global const u64 bitmask54 = 0x003fffffffffffffull;
global const u64 bitmask55 = 0x007fffffffffffffull;
global const u64 bitmask56 = 0x00ffffffffffffffull;
global const u64 bitmask57 = 0x01ffffffffffffffull;
global const u64 bitmask58 = 0x03ffffffffffffffull;
global const u64 bitmask59 = 0x07ffffffffffffffull;
global const u64 bitmask60 = 0x0fffffffffffffffull;
global const u64 bitmask61 = 0x1fffffffffffffffull;
global const u64 bitmask62 = 0x3fffffffffffffffull;
global const u64 bitmask63 = 0x7fffffffffffffffull;
global const u64 bitmask64 = 0xffffffffffffffffull;

global const u32 bit1  = (1<<0);
global const u32 bit2  = (1<<1);
global const u32 bit3  = (1<<2);
global const u32 bit4  = (1<<3);
global const u32 bit5  = (1<<4);
global const u32 bit6  = (1<<5);
global const u32 bit7  = (1<<6);
global const u32 bit8  = (1<<7);
global const u32 bit9  = (1<<8);
global const u32 bit10 = (1<<9);
global const u32 bit11 = (1<<10);
global const u32 bit12 = (1<<11);
global const u32 bit13 = (1<<12);
global const u32 bit14 = (1<<13);
global const u32 bit15 = (1<<14);
global const u32 bit16 = (1<<15);
global const u32 bit17 = (1<<16);
global const u32 bit18 = (1<<17);
global const u32 bit19 = (1<<18);
global const u32 bit20 = (1<<19);
global const u32 bit21 = (1<<20);
global const u32 bit22 = (1<<21);
global const u32 bit23 = (1<<22);
global const u32 bit24 = (1<<23);
global const u32 bit25 = (1<<24);
global const u32 bit26 = (1<<25);
global const u32 bit27 = (1<<26);
global const u32 bit28 = (1<<27);
global const u32 bit29 = (1<<28);
global const u32 bit30 = (1<<29);
global const u32 bit31 = (1<<30);
global const u32 bit32 = (1<<31);

global const u64 bit33 = (1ull<<32);
global const u64 bit34 = (1ull<<33);
global const u64 bit35 = (1ull<<34);
global const u64 bit36 = (1ull<<35);
global const u64 bit37 = (1ull<<36);
global const u64 bit38 = (1ull<<37);
global const u64 bit39 = (1ull<<38);
global const u64 bit40 = (1ull<<39);
global const u64 bit41 = (1ull<<40);
global const u64 bit42 = (1ull<<41);
global const u64 bit43 = (1ull<<42);
global const u64 bit44 = (1ull<<43);
global const u64 bit45 = (1ull<<44);
global const u64 bit46 = (1ull<<45);
global const u64 bit47 = (1ull<<46);
global const u64 bit48 = (1ull<<47);
global const u64 bit49 = (1ull<<48);
global const u64 bit50 = (1ull<<49);
global const u64 bit51 = (1ull<<50);
global const u64 bit52 = (1ull<<51);
global const u64 bit53 = (1ull<<52);
global const u64 bit54 = (1ull<<53);
global const u64 bit55 = (1ull<<54);
global const u64 bit56 = (1ull<<55);
global const u64 bit57 = (1ull<<56);
global const u64 bit58 = (1ull<<57);
global const u64 bit59 = (1ull<<58);
global const u64 bit60 = (1ull<<59);
global const u64 bit61 = (1ull<<60);
global const u64 bit62 = (1ull<<61);
global const u64 bit63 = (1ull<<62);
global const u64 bit64 = (1ull<<63);

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



struct DateTime
{
  u16 micro_sec; // [0,999]
  u16 msec; // [0,999]
  u16 sec;  // [0,60]
  u16 min;  // [0,59]
  u16 hour; // [0,24]
  u16 day;  // [0,30]
  union
  {
    WeekDay week_day;
    u32 wday;
  };
  union
  {
    Month month;
    u32 mon;
  };
  u32 year; // 1 = 1 CE, 0 = 1 BC
};

typedef u64 DenseTime;

////////////////////////////////
//~ allen: Files

typedef u32 FilePropertyFlags;
enum
{
  FilePropertyFlag_IsFolder = (1 << 0),
};



struct FileProperties
{
  u64 size;
  DenseTime modified;
  DenseTime created;
  FilePropertyFlags flags;
};

////////////////////////////////
//~ rjf: Safe Casts

////////////////////////////////
//~ rjf: Large Base Type Functions

////////////////////////////////
//~ rjf: Bit Patterns

#if ARCH_LITTLE_ENDIAN
# define from_be_u16(x) bswap_u16(x)
# define from_be_u32(x) bswap_u32(x)
# define from_be_u64(x) bswap_u64(x)
#else
# define from_be_u16(x) (x)
# define from_be_u32(x) (x)
# define from_be_u64(x) (x)
#endif

////////////////////////////////
//~ rjf: Enum -> Sign

////////////////////////////////
//~ rjf: Memory Functions

////////////////////////////////
//~ rjf: Text 2D Coordinate/Range Functions

////////////////////////////////
//~ rjf: Toolchain/Environment Enum Functions

////////////////////////////////
//~ rjf: Time Functions

////////////////////////////////
//~ rjf: Non-Fancy Ring Buffer Reads/Writes

#define ring_write_struct(ring_base, ring_size, ring_pos, ptr) ring_write((ring_base), (ring_size), (ring_pos), (ptr), sizeof(*(ptr)))
#define ring_read_struct(ring_base, ring_size, ring_pos, ptr) ring_read((ring_base), (ring_size), (ring_pos), (ptr), sizeof(*(ptr)))

////////////////////////////////
//~ rjf: Sorts

#define quick_sort(ptr, count, element_size, cmp_function) qsort((ptr), (count), (element_size), (int (*)(const void *, const void *))(cmp_function))



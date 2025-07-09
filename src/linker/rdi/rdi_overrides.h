#pragma once

typedef u8  RDI_U8;
typedef u16 RDI_U16;
typedef u32 RDI_U32;
typedef u64 RDI_U64;
typedef i8  RDI_S8;
typedef i16 RDI_S16;
typedef i32 RDI_S32;
typedef i64 RDI_S64;

#define RDI_PROC internal
#define RDIM_MEMSET_OVERRIDE
#define rdim_memset MemorySet

#define RDIM_MEMCPY_OVERRIDE
#define rdim_memcpy MemoryCopy

#define rdim_vsnprintf raddbg_vsnprintf


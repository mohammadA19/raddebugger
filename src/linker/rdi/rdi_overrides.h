#pragma once

typedef uint8  RDI_U8;
typedef uint16 RDI_U16;
typedef uint32 RDI_U32;
typedef uint64 RDI_U64;
typedef int8  RDI_S8;
typedef int16 RDI_S16;
typedef int32 RDI_S32;
typedef int64 RDI_S64;

#define RDI_PROC internal
#define RDIM_MEMSET_OVERRIDE
#define rdim_memset MemorySet

#define RDIM_MEMCPY_OVERRIDE
#define rdim_memcpy MemoryCopy

#define rdim_vsnprintf raddbg_vsnprintf


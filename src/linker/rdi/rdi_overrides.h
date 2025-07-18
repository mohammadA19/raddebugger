#pragma once

typedef uint8  RDI_uint8;
typedef uint16 RDI_uint16;
typedef uint32 RDI_uint32;
typedef uint64 RDI_uint64;
typedef uint8  RDI_uint8;
typedef uint16 RDI_uint16;
typedef uint32 RDI_uint32;
typedef uint64 RDI_uint64;

#define RDI_PROC internal
#define RDIM_MEMSET_OVERRIDE
#define rdim_memset MemorySet

#define RDIM_MEMCPY_OVERRIDE
#define rdim_memcpy MemoryCopy

#define rdim_vsnprintf raddbg_vsnprintf


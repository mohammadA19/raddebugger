#pragma once

typedef byte  RDI_U8;
typedef ushort RDI_U16;
typedef uint RDI_U32;
typedef ulong RDI_U64;
typedef sbyte  RDI_S8;
typedef short RDI_S16;
typedef int RDI_S32;
typedef long RDI_S64;

#define RDI_PROC internal
#define RDIM_MEMSET_OVERRIDE
#define rdim_memset MemorySet

#define RDIM_MEMCPY_OVERRIDE
#define rdim_memcpy MemoryCopy

#define rdim_vsnprintf raddbg_vsnprintf


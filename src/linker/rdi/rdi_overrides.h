#pragma once

typedef U8  RDI_U8;
typedef U16 RDI_U16;
typedef U32 RDI_U32;
typedef U64 RDI_U64;
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


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Backend Constants

#define R_BACKEND_STUB 0
#define R_BACKEND_D3D11 1

////////////////////////////////
//~ rjf: Decide On Backend

#if !defined(R_BACKEND) && OS_WINDOWS
# define R_BACKEND R_BACKEND_D3D11
#endif

////////////////////////////////
//~ rjf: Main Includes

#include "render_core.h"

////////////////////////////////
//~ rjf: Backend Includes

#if R_BACKEND == R_BACKEND_STUB
# include "stub/render_stub.h"
#elif R_BACKEND == R_BACKEND_D3D11
# include "d3d11/render_d3d11.h"
#else
# error Renderer backend not specified.
#endif


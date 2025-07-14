// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "demon/demon_core.h"

#if OS_WINDOWS
# include "demon/win32/demon_core_win32.h"
#elif OS_LINUX
# include "demon/linux/demon_core_linux.h"
#else
# error Demon layer backend not defined for this operating system.
#endif


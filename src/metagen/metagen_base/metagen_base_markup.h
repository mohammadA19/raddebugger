// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

static void set_thread_name(String8 string);
static void set_thread_namef(char *fmt, ...);
#define ThreadNameF(...) (set_thread_namef(__VA_ARGS__))
#define ThreadName(str) (set_thread_name(str))


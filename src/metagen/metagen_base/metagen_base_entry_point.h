// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

static void main_thread_base_entry_point(int argc, char **argv);
static void supplement_thread_base_entry_point(void (*entry_point)(void *params), void *params);
static U64 update_tick_idx(void);
static B32 update(void);


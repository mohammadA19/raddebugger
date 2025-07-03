// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_ENTRY_POINT_H
#define BASE_ENTRY_POINT_H

static void main_thread_base_entry_point(int argc, char **argv);
static void supplement_thread_base_entry_point(void (*entry_point)(void *params), void *params);
static U64 update_tick_idx(void);
static B32 update(void);

#endif // BASE_ENTRY_POINT_H

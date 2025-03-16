// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_ENTRY_POINT_H
#define BASE_ENTRY_POINT_H

void main_thread_base_entry_point(void (*entry_point)(CmdLine* cmdline), char** arguments, U64 arguments_count);
void supplement_thread_base_entry_point(void (*entry_point)(void* params), void* params);

#endif // BASE_ENTRY_POINT_H

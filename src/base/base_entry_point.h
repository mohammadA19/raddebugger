// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


internal void main_thread_base_entry_point(int argc, char **argv);
internal void supplement_thread_base_entry_point(void (*entry_point)(void *params), void *params);
internal U64 update_tick_idx(void);
internal B32 update(void);


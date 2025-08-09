// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PE_MAKE_DEBUG_DIR_H
#define PE_MAKE_DEBUG_DIR_H

internal string pe_make_debug_directory_pdb_obj(Arena *arena, COFF_MachineType machine, Guid guid, U32 age, COFF_TimeStamp time_stamp, string path);
internal string pe_make_debug_directory_rdi_obj(Arena *arena, COFF_MachineType machine, Guid guid, U32 age, COFF_TimeStamp time_stamp, string path);

#endif // PE_MAKE_DEBUG_DIR_H


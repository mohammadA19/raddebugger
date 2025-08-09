// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


internal B32 dw_is_dwarf_present_coff_section_table(string raw_image, string string_table, U64 section_count, COFF_SectionHeader *section_table);
internal DW_Input dw_input_from_coff_section_table(Arena *arena, string raw_image, string string_table, U64 section_count, COFF_SectionHeader *section_table);



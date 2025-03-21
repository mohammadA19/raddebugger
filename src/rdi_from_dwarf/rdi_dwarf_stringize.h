// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_DWARF_STRINGIZE_H
#define RDI_DWARF_STRINGIZE_H

////////////////////////////////
//~ DWARF Stringize Functions

static void
dwarf_stringize_info(Arena *arena, String8List *out, DWARF_InfoUnit *unit, uint indent);

static void
dwarf_stringize_pubnames(Arena *arena, String8List *out, DWARF_PubNamesUnit *unit,
                         uint indent);

static void
dwarf_stringize_names(Arena *arena, String8List *out, DWARF_NamesUnit *unit, uint indent);

static void
dwarf_stringize_aranges(Arena *arena, String8List *out, DWARF_ArangesUnit *unit, uint indent);

static void
dwarf_stringize_addr(Arena *arena, String8List *out, DWARF_AddrUnit *unit, uint indent);



#endif //RDI_DWARF_STRINGIZE_H

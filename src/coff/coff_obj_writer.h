#ifndef COFF_OBJ_WRITER_H
#define COFF_OBJ_WRITER_H

enum
{
    COFF_SymbolLocation_Null,
    COFF_SymbolLocation_Section,
    COFF_SymbolLocation_Abs,
    COFF_SymbolLocation_Undef,
    COFF_SymbolLocation_Common,
} COFF_SymbolLocationType;

typedef struct COFF_SymbolLocation
{
    COFF_SymbolLocationType type;
    union {
        struct COFF_ObjSection *section;
    } u;
} COFF_SymbolLocation;

typedef struct COFF_ObjSymbolWeak
{
    struct COFF_ObjSymbol *tag;
    COFF_WeakExtType       characteristics;
} COFF_ObjSymbolWeak;

typedef struct COFF_ObjSymbolSecDef
{
    COFF_ComdatSelectType selection;
    struct COFF_ObjSection *associate;
} COFF_ObjSymbolSecDef;

typedef struct COFF_ObjSymbol
{
    StringView                 name;
    U32                     value;
    COFF_SymbolLocation     loc;
    COFF_SymbolType         type;
    COFF_SymStorageClass    storage_class;
    String8List             aux_symbols;
    U32                     idx;
} COFF_ObjSymbol;

typedef struct COFF_ObjSymbolNode
{
    struct COFF_ObjSymbolNode *next;
    COFF_ObjSymbol             v;
} COFF_ObjSymbolNode;

typedef struct COFF_ObjReloc
{
    U32             apply_off;
    COFF_ObjSymbol *symbol;
    COFF_RelocType  type;
} COFF_ObjReloc;

typedef struct COFF_ObjRelocNode
{
    struct COFF_ObjRelocNode *next;
    COFF_ObjReloc             v;
} COFF_ObjRelocNode;

typedef struct COFF_ObjSection
{
    StringView           name;
    String8List       data;
    COFF_SectionFlags flags;

    U64                reloc_count;
    COFF_ObjRelocNode *reloc_first;
    COFF_ObjRelocNode *reloc_last;

    U32 section_number;
} COFF_ObjSection;

typedef struct COFF_ObjSectionNode
{
    struct COFF_ObjSectionNode *next;
    COFF_ObjSection             v;
} COFF_ObjSectionNode;

typedef struct COFF_ObjWriter
{
    Arena               *arena;
    COFF_TimeStamp       time_stamp;
    COFF_MachineType     machine;
    U64                  symbol_count;
    COFF_ObjSymbolNode  *symbol_first;
    COFF_ObjSymbolNode  *symbol_last;
    U64                  sect_count;
    COFF_ObjSectionNode *sect_first;
    COFF_ObjSectionNode *sect_last;
    COFF_ObjSection     *drectve_sect;
} COFF_ObjWriter;

////////////////////////////////






#endif // COFF_OBJ_WRITER_H


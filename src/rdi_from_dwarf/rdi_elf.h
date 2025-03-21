// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RDI_ELF_H
#define RDI_ELF_H

// https://refspecs.linuxfoundation.org/elf/elf.pdf

////////////////////////////////
//~ Elf Format Types

// elf header

#define ELF_NIDENT 16

typedef struct ELF_Ehdr32{
  byte  e_ident[ELF_NIDENT];
  ushort e_type;
  ushort e_machine;
  uint e_version;
  uint e_entry;
  uint e_phoff;
  uint e_shoff;
  uint e_flags;
  ushort e_ehsize;
  ushort e_phentsize;
  ushort e_phnum;
  ushort e_shentsize;
  ushort e_shnum;
  ushort e_shstrndx;
} ELF_Ehdr32;

typedef struct ELF_Ehdr64{
  byte  e_ident[ELF_NIDENT];
  ushort e_type;
  ushort e_machine;
  uint e_version;
  ulong e_entry;
  ulong e_phoff;
  ulong e_shoff;
  uint e_flags;
  ushort e_ehsize;
  ushort e_phentsize;
  ushort e_phnum;
  ushort e_shentsize;
  ushort e_shnum;
  ushort e_shstrndx;
} ELF_Ehdr64;

typedef enum ELF_Type{
  ELF_Type_NONE   = 0,
  ELF_Type_REL    = 1,
  ELF_Type_EXEC   = 2,
  ELF_Type_DYN    = 3,
  ELF_Type_CORE   = 4,
  ELF_Type_LOOS   = 0xfe00,
  ELF_Type_HIOS   = 0xfeff,
  ELF_Type_LOPROC = 0xff00,
  ELF_Type_HIPROC = 0xffff,
} ELF_Type;

typedef enum ELF_Machine{
  ELF_Machine_NONE  = 0,
  ELF_Machine_M32   = 1,
  ELF_Machine_SPARC = 2,
  ELF_Machine_386   = 3,
  ELF_Machine_68K   = 4,
  ELF_Machine_88K   = 5,
  ELF_Machine_860   = 7,
  ELF_Machine_MIPS  = 8,
  ELF_Machine_S370  = 9,
  ELF_Machine_MIPS_RS3_LE = 10,
  ELF_Machine_PARISC = 15,
  ELF_Machine_VPP500 = 17,
  ELF_Machine_SPARC32PLUS = 18,
  ELF_Machine_960   = 19,
  ELF_Machine_PPC   = 20,
  ELF_Machine_PPC64 = 21,
  ELF_Machine_S390  = 22,
  ELF_Machine_V800  = 36,
  ELF_Machine_FR20  = 37,
  ELF_Machine_RH32  = 38,
  ELF_Machine_RCE   = 39,
  ELF_Machine_ARM   = 40,
  ELF_Machine_ALPHA = 41,
  ELF_Machine_SH    = 42,
  ELF_Machine_SPARCV9 = 43,
  ELF_Machine_TRICORE = 44,
  ELF_Machine_ARC   = 45,
  ELF_Machine_H8_300 = 46,
  ELF_Machine_H8_300H = 47,
  ELF_Machine_H8S    = 48,
  ELF_Machine_H8_500 = 49,
  ELF_Machine_IA_64  = 50,
  ELF_Machine_MIPS_X = 51,
  ELF_Machine_COLDFIRE = 52,
  ELF_Machine_68HC12 = 53,
  ELF_Machine_MMA    = 54,
  ELF_Machine_PCP    = 55,
  ELF_Machine_NCPU   = 56,
  ELF_Machine_NDR1   = 57,
  ELF_Machine_STARCORE = 58,
  ELF_Machine_ME16   = 59,
  ELF_Machine_ST100  = 60,
  ELF_Machine_TINYJ  = 61,
  ELF_Machine_X86_64 = 62,
  ELF_Machine_PDSP   = 63,
  ELF_Machine_PDP10  = 64,
  ELF_Machine_PDP11  = 65,
  ELF_Machine_FX66   = 66,
  ELF_Machine_ST9PLUS = 67,
  ELF_Machine_ST7    = 68,
  ELF_Machine_68HC16 = 69,
  ELF_Machine_68HC11 = 70,
  ELF_Machine_68HC08 = 71,
  ELF_Machine_68HC05 = 72,
  ELF_Machine_SVX    = 73,
  ELF_Machine_ST19   = 74,
  ELF_Machine_VAX    = 75,
  ELF_Machine_CRIS   = 76,
  ELF_Machine_JAVELIN  = 77,
  ELF_Machine_FIREPATH = 78,
  ELF_Machine_ZSP    = 79,
  ELF_Machine_MMIX   = 80,
  ELF_Machine_HUANY  = 81,
  ELF_Machine_PRISM  = 82,
  ELF_Machine_AVR    = 83,
  ELF_Machine_FR30   = 84,
  ELF_Machine_D10V   = 85,
  ELF_Machine_D30V   = 86,
  ELF_Machine_V850   = 87,
  ELF_Machine_M32R   = 88,
  ELF_Machine_MN10300  = 89,
  ELF_Machine_MN10200  = 90,
  ELF_Machine_PJ       = 91,
  ELF_Machine_OPENRISC = 92,
  ELF_Machine_ARC_A5   = 93,
  ELF_Machine_XTENSA   = 94,
  ELF_Machine_VIDEOCORE = 95,
  ELF_Machine_TMM_GPP   = 96,
  ELF_Machine_NS32K = 97,
  ELF_Machine_TPC   = 98,
  ELF_Machine_SNP1K = 99,
  ELF_Machine_ST200 = 100,
} ELF_Machine;

typedef enum ELF_Version{
  ELF_Version_NONE = 0,
  ELF_Version_CURRENT = 1,
} ELF_Version;

typedef enum ELF_Identification{
  ELF_Identification_MAG0 = 0,
  ELF_Identification_MAG1 = 1,
  ELF_Identification_MAG2 = 2,
  ELF_Identification_MAG3 = 3,
  ELF_Identification_CLASS = 4,
  ELF_Identification_DATA = 5,
  ELF_Identification_VERSION = 6,
  ELF_Identification_OSABI = 7,
  ELF_Identification_ABIVERSION = 8,
  ELF_Identification_PAD = 9,
} ELF_Identification;

read_only global byte elf_magic[] = {0x7F, 'E', 'L', 'F'};

typedef enum ELF_Class{
  ELF_Class_NONE = 0,
  ELF_Class_32 = 1,
  ELF_Class_64 = 2,
} ELF_Class;

typedef enum ELF_DataEncoding{
  ELF_DataEncoding_NONE = 0,
  ELF_DataEncoding_2LSB = 1,
  ELF_DataEncoding_2MSB = 2,
} ELF_DataEncoding;

typedef enum ELF_OsAbi{
  ELF_OsAbi_NONE = 0,
  ELF_OsAbi_HPUX = 1,
  ELF_OsAbi_NETBSD = 2,
  ELF_OsAbi_LINUX = 3,
  ELF_OsAbi_SOLARIS = 6,
  ELF_OsAbi_AIX = 7,
  ELF_OsAbi_IRIX = 8,
  ELF_OsAbi_FREEBSD = 9,
  ELF_OsAbi_TRU64 = 10,
  ELF_OsAbi_MODESTO = 11,
  ELF_OsAbi_OPENBSD = 12,
  ELF_OsAbi_OPENVMS = 13,
  ELF_OsAbi_NSK = 14,
} ELF_OsAbi;

// sections

typedef enum ELF_ReservedSectionIndex{
  ELF_ReservedSectionIndex_UNDEF = 0,
  ELF_ReservedSectionIndex_LORESERVE = 0xFF00,
  ELF_ReservedSectionIndex_LOPROC = 0xFF00,
  ELF_ReservedSectionIndex_HIPROC = 0xFF1F,
  ELF_ReservedSectionIndex_LOOS = 0xFF20,
  ELF_ReservedSectionIndex_HIOS = 0xFF3F,
  ELF_ReservedSectionIndex_ABS = 0xFFF1,
  ELF_ReservedSectionIndex_COMMON = 0xFFF2,
  ELF_ReservedSectionIndex_XINDEX = 0xFFFF,
  ELF_ReservedSectionIndex_HIRESERVE = 0xFFFF,
} ELF_ReservedSectionIndex;

typedef struct ELF_Shdr32{
  uint sh_name;
  uint sh_type;
  uint sh_flags;
  uint sh_addr;
  uint sh_offset;
  uint sh_size;
  uint sh_link;
  uint sh_info;
  uint sh_addralign;
  uint sh_entsize;
} ELF_Shdr32;

typedef struct ELF_Shdr64{
  uint sh_name;
  uint sh_type;
  ulong sh_flags;
  ulong sh_addr;
  ulong sh_offset;
  ulong sh_size;
  uint sh_link;
  uint sh_info;
  ulong sh_addralign;
  ulong sh_entsize;
} ELF_Shdr64;

//  X(name, code)
#define ELF_SectionTypeXList(X)\
X(NULL,           0)\
X(PROGBITS,       1)\
X(SYMTAB,         2)\
X(STRTAB,         3)\
X(RELA,           4)\
X(HASH,           5)\
X(DYNAMIC,        6)\
X(NOTE,           7)\
X(NOBITS,         8)\
X(REL,            9)\
X(SHLIB,         10)\
X(DYNSYM,        11)\
X(INIT_ARRAY,    14)\
X(FINI_ARRAY,    15)\
X(PREINIT_ARRAY, 16)\
X(GROUP,         17)\
X(SYMTAB_SHNDX,  18)\
X(LOOS,   0x60000000)\
X(HIOS,   0x6FFFFFFF)\
X(LOPROC, 0x70000000)\
X(HIPROC, 0x7FFFFFFF)\
X(LOUSER, 0x80000000)\
X(HIUSER, 0x8FFFFFFF)

typedef enum ELF_SectionType{
#define X(N,C) ELF_SectionType_##N = C,
  ELF_SectionTypeXList(X)
#undef X
} ELF_SectionType;

typedef enum ELF_SectionAttributeFlags{
  ELF_SectionAttributeFlag_WRITE      = 0x001,
  ELF_SectionAttributeFlag_ALLOC      = 0x002,
  ELF_SectionAttributeFlag_EXECINSTR  = 0x004,
  ELF_SectionAttributeFlag_MERGE      = 0x010,
  ELF_SectionAttributeFlag_STRINGS    = 0x020,
  ELF_SectionAttributeFlag_INFO_LINK  = 0x040,
  ELF_SectionAttributeFlag_LINK_ORDER = 0x080,
  ELF_SectionAttributeFlag_OS_NONCONFORMING = 0x100,
  ELF_SectionAttributeFlag_GROUP      = 0x200,
  ELF_SectionAttributeFlag_TLS        = 0x400,
  ELF_SectionAttributeFlag_MASKOS     = 0x0FF00000,
  ELF_SectionAttributeFlag_MASKPROC   = 0xF0000000,
} ELF_SectionAttributeFlags;

typedef enum ELF_SectionGroupFlags{
  ELF_SectionGroupFlag_COMDAT   = 0x1,
  ELF_SectionGroupFlag_MASKOS   = 0x0FF00000,
  ELF_SectionGroupFlag_MASKPROC = 0xF0000000,
} ELF_SectionGroupFlags;

typedef enum ELF_ReservedSymbolTableIndex{
  ELF_ReservedSymbolTableIndex_UNDEF = 0,
} ELF_ReservedSymbolTableIndex;

// symbol table

typedef struct ELF_Sym32{
  uint st_name;
  uint st_value;
  uint st_size;
  byte  st_info;
  byte  st_other;
  ushort st_shndx;
} ELF_Sym32;

typedef struct ELF_Sym64{
  uint st_name;
  byte  st_info;
  byte  st_other;
  ushort st_shndx;
  ulong st_value;
  ulong st_size;
} ELF_Sym64;

#define ELF_SymBindingFromInfo(x) (ELF_SymbolBinding)((x)>>4)
#define ELF_SymTypeFromInfo(x)    (ELF_SymbolType)((x)&0xF)
#define ELF_SymInfoFromBindingType(b,t) ((((b)<<4)&0xF)|((t)&0xF))

#define ELF_SymVisibilityFromOther(x) ((x)&0x3)
#define ELF_SymOtherFromVisibility(x) ((x)&0x3)

#define ELF_SymbolBindingXList(X)\
X(LOCAL,   0)\
X(GLOBAL,  1)\
X(WEAK,    2)\
X(LOOS,   10)\
X(HIOS,   12)\
X(LOPROC, 13)\
X(HIPROC, 15)\

typedef enum ELF_SymbolBinding{
#define X(N,C) ELF_SymbolBinding_##N = C,
  ELF_SymbolBindingXList(X)
#undef X
} ELF_SymbolBinding;

#define ELF_SymbolTypeXList(X)\
X(NOTYPE,  0)\
X(OBJECT,  1)\
X(FUNC,    2)\
X(SECTION, 3)\
X(FILE,    4)\
X(COMMON,  5)\
X(TLS,     6)\
X(LOOS,   10)\
X(HIOS,   12)\
X(LOPROC, 13)\
X(HIPROC, 15)

typedef enum ELF_SymbolType{
#define X(N,C) ELF_SymbolType_##N = C,
  ELF_SymbolTypeXList(X)
#undef X
} ELF_SymbolType;

#define ELF_SymbolVisibilityXList(X)\
X(DEFAULT,   0)\
X(INTERNAL,  1)\
X(HIDDEN,    2)\
X(PROTECTED, 3)

typedef enum ELF_SymbolVisibility{
#define X(N,C) ELF_SymbolVisibility_##N = C,
  ELF_SymbolVisibilityXList(X)
#undef X
} ELF_SymbolVisibility;

// relocation

typedef struct ELF_Rel32{
  uint r_offset;
  uint r_info;
} ELF_Rel32;

typedef struct ELF_Rela32{
  uint r_offset;
  uint r_info;
  int r_addend;
} ELF_Rela32;

typedef struct ELF_Rel64{
  ulong r_offset;
  ulong r_info;
} ELF_Rel64;

typedef struct ELF_Rela64{
  ulong r_offset;
  ulong r_info;
  long r_addend;
} ELF_Rela64;

#define ELF_RelSymIndexFromInfo32(x) ((x)>>8)
#define ELF_RelTypeFromInfo32(x)     ((x)&0xF)
#define ELF_RelInfoFromSymIndexType32(n,t) (((n)<<8)|((t)&0xF))

#define ELF_RelSymIndexFromInfo64(x) ((x)>>32)
#define ELF_RelTypeFromInfo64(x)     ((x)&0xFFFFFFFFL)
#define ELF_RelInfoFromSymIndexType64(n,t) (((n)<<8)|((t)&0xFFFFFFFFL))



// program header

typedef struct ELF_Phdr32{
  uint p_type;
  uint p_offset;
  uint p_vaddr;
  uint p_paddr;
  uint p_filesz;
  uint p_memsz;
  uint p_flags;
  uint p_align;
} ELF_Phdr32;

typedef struct ELF_Phdr64{
  uint p_type;
  uint p_flags;
  ulong p_offset;
  ulong p_vaddr;
  ulong p_paddr;
  ulong p_filesz;
  ulong p_memsz;
  ulong p_align;
} ELF_Phdr64;

typedef enum ELF_SegmentType{
  ELF_SegmentType_NULL    = 0,
  ELF_SegmentType_LOAD    = 1,
  ELF_SegmentType_DYNAMIC = 2,
  ELF_SegmentType_INTERP  = 3,
  ELF_SegmentType_NOTE    = 4,
  ELF_SegmentType_SHLIB   = 5,
  ELF_SegmentType_PHDR    = 6,
  ELF_SegmentType_TLS     = 7,
  ELF_SegmentType_LOOS    = 0x60000000,
  ELF_SegmentType_HIOS    = 0x6fffffff,
  ELF_SegmentType_LOPROC  = 0x70000000,
  ELF_SegmentType_HIPROC  = 0x7fffffff,
} ELF_SegmentType;

typedef enum ELF_SegmentFlags{
  ELF_SegmentFlag_X = 0x1,
  ELF_SegmentFlag_W = 0x2,
  ELF_SegmentFlag_R = 0x4,
  ELF_SegmentFlag_MASKOS   = 0x0FF00000,
  ELF_SegmentFlag_MASKPROC = 0xF0000000,
} ELF_SegmentFlags;

////////////////////////////////
//~ ELF Parser Types

// elf top level

typedef struct ELF_SectionArray{
  ELF_Shdr64 *sections;
  ulong count;
} ELF_SectionArray;

typedef struct ELF_SegmentArray{
  ELF_Phdr64 *segments;
  ulong count;
} ELF_SegmentArray;

typedef struct ELF_Parsed{
  String8 data;
  ELF_Class elf_class;
  Arch arch;
  
  ELF_Shdr64 *sections;
  String8 *section_names;
  ulong section_foff;
  ulong section_count;
  
  ELF_Phdr64 *segments;
  ulong segment_foff;
  ulong segment_count;
  
  ulong vbase;
  ulong entry_vaddr;
  ulong section_name_table_foff;
  ulong section_name_table_opl;
  
  ulong strtab_idx;
  ulong symtab_idx;
  ulong dynsym_idx;
} ELF_Parsed;

// elf symtab

typedef struct ELF_SymArray{
  ELF_Sym64 *symbols;
  ulong count;
} ELF_SymArray;

////////////////////////////////
//~ ELF Parser Functions

static ELF_Parsed* elf_parsed_from_data(Arena *arena, String8 elf_data);

static ELF_SectionArray elf_section_array_from_elf(ELF_Parsed *elf);
static String8Array     elf_section_name_array_from_elf(ELF_Parsed *elf);
static ELF_SegmentArray elf_segment_array_from_elf(ELF_Parsed *elf);

static String8 elf_section_name_from_name_offset(ELF_Parsed *elf, ulong offset);
static String8 elf_section_name_from_idx(ELF_Parsed *elf, uint idx);
static uint     elf_section_idx_from_name(ELF_Parsed *elf, String8 name);

static String8 elf_section_data_from_idx(ELF_Parsed *elf, uint idx);

static ELF_SymArray elf_sym_array_from_data(Arena *arena, ELF_Class elf_class, String8 data);

// string functions

static String8 elf_string_from_section_type(ELF_SectionType section_type);
static String8 elf_string_from_symbol_binding(ELF_SymbolBinding binding);
static String8 elf_string_from_symbol_type(ELF_SymbolType type);
static String8 elf_string_from_symbol_visibility(ELF_SymbolVisibility visibility);

#endif //RDI_ELF_H

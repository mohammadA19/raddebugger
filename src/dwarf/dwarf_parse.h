// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DWARF_PARSE_H
#define DWARF_PARSE_H

// NOTE(rjf): Some rules about the spaces of offsets and ranges:
//
// - Every stored/passed offset is relative to the base of its section.
// - Every stored/passed range has endpoints relative to the base of their section.
// - Upon calling a syms_based_range_* function, these offsets need to be
//   converted into range-relative.

////////////////////////////////
//~ rjf: Constants

#define DWARF_VOID_TYPE_ID 0xffffffffffffffffull

////////////////////////////////
//~ rjf: Files + External Debug References

typedef struct DW_ExtDebugRef DW_ExtDebugRef;
struct DW_ExtDebugRef
{
  // NOTE(rjf): .dwo => an external DWARF V5 .dwo file
  String8 dwo_path;
  ulong     dwo_id;
}

////////////////////////////////
//~ rjf: Abbrev Table

typedef struct DW_AbbrevTableEntry DW_AbbrevTableEntry;
struct DW_AbbrevTableEntry
{
  ulong id;
  ulong off;
}

typedef struct DW_AbbrevTable DW_AbbrevTable;
struct DW_AbbrevTable
{
  ulong                  count;
  DW_AbbrevTableEntry *entries;
}

////////////////////////////////
//~ Sections

typedef struct DW_Section DW_Section;
struct DW_Section
{
  String8 name;
  String8 data;
  DW_Mode mode;
  B32     is_dwo;
}

typedef struct DW_SectionArray DW_SectionArray;
struct DW_SectionArray
{
  DW_Section v[DW_Section_Count];
}

////////////////////////////////
//~ rjf: Basic Line Info

typedef struct DW_LineFile DW_LineFile;
struct DW_LineFile
{
  String8 file_name;
  ulong     dir_idx;
  ulong     modify_time;
  ulong     md5_digest[2];
  ulong     file_size;
}

typedef struct DW_LineVMFileNode DW_LineVMFileNode;
struct DW_LineVMFileNode
{
  DW_LineVMFileNode *next;
  DW_LineFile        file;
}

typedef struct DW_LineVMFileList DW_LineVMFileList;
struct DW_LineVMFileList
{
  ulong                node_count;
  DW_LineVMFileNode *first;
  DW_LineVMFileNode *last;
}

typedef struct DW_LineVMFileArray DW_LineVMFileArray;
struct DW_LineVMFileArray
{
  ulong          count;
  DW_LineFile *v;
}

////////////////////////////////
//~ rjf: Abbrevs

typedef enum DW_AbbrevKind
{
  DW_Abbrev_Null,
  DW_Abbrev_Tag,
  DW_Abbrev_Attrib,
  DW_Abbrev_AttribSequenceEnd,
  DW_Abbrev_DIEBegin,
  DW_Abbrev_DIEEnd,
}
DW_AbbrevKind;

typedef uint DW_AbbrevFlags;
enum{
  DW_AbbrevFlag_HasImplicitConst = (1<<0),
  DW_AbbrevFlag_HasChildren      = (1<<1),
}

typedef struct DW_Abbrev DW_Abbrev;
struct DW_Abbrev
{
  DW_AbbrevKind  kind;
  Rng1U64        abbrev_range;
  ulong            sub_kind;
  ulong            id;
  ulong            const_value;
  DW_AbbrevFlags flags;
}

////////////////////////////////
//~ rjf: Attribs

typedef struct DW_AttribValueResolveParams DW_AttribValueResolveParams;
struct DW_AttribValueResolveParams
{
  DW_Version  version;
  DW_Language language;
  ulong         addr_size;                // NOTE(rjf): size in bytes of containing compilation unit's addresses
  ulong         containing_unit_info_off; // NOTE(rjf): containing compilation unit's offset into the .debug_info section
  ulong         debug_addrs_base;         // NOTE(rjf): containing compilation unit's offset into the .debug_addrs section       (DWARF V5 ONLY)
  ulong         debug_rnglists_base;      // NOTE(rjf): containing compilation unit's offset into the .debug_rnglists section    (DWARF V5 ONLY)
  ulong         debug_str_offs_base;      // NOTE(rjf): containing compilation unit's offset into the .debug_str_offsets section (DWARF V5 ONLY)
  ulong         debug_loclists_base;      // NOTE(rjf): containing compilation unit's offset into the .debug_loclists section    (DWARF V5 ONLY)
}

typedef struct DW_AttribValue DW_AttribValue;
struct DW_AttribValue
{
  DW_SectionKind section;
  ulong            v[2];
}

typedef struct DW_Attrib DW_Attrib;
struct DW_Attrib
{
  ulong            info_off;
  ulong            abbrev_id;
  DW_AttribKind  attrib_kind;
  DW_FormKind    form_kind;
  DW_AttribClass value_class;
  DW_AttribValue form_value;
}

typedef struct DW_AttribArray DW_AttribArray;
struct DW_AttribArray
{
  DW_Attrib *v;
  ulong        count;
}

typedef struct DW_AttribNode DW_AttribNode;
struct DW_AttribNode
{
  DW_AttribNode *next;
  DW_Attrib      attrib;
}

typedef struct DW_AttribList DW_AttribList;
struct DW_AttribList
{
  DW_AttribNode *first;
  DW_AttribNode *last;
  ulong            count;
}

typedef struct DW_AttribListParseResult DW_AttribListParseResult;
struct DW_AttribListParseResult
{
  DW_AttribList attribs;
  ulong           max_info_off;
  ulong           max_abbrev_off;
}

////////////////////////////////
//~ rjf: Compilation Units + Accelerators

typedef struct DW_CompRoot DW_CompRoot;
struct DW_CompRoot
{
  // NOTE(rjf): Header Data
  ulong             size;
  DW_CompUnitKind kind;
  DW_Version      version;
  ulong             address_size;
  ulong             abbrev_off;
  ulong             info_off;
  Rng1U64         tags_info_range;
  DW_AbbrevTable  abbrev_table;
  
  // NOTE(rjf): [parsed from DWARF attributes] Offsets For More Info (DWARF V5 ONLY)
  ulong rnglist_base; // NOTE(rjf): Offset into the .debug_rnglists section where this comp unit's data is.
  ulong loclist_base; // NOTE(rjf): Offset into the .debug_loclists section where this comp unit's data is.
  ulong addrs_base;   // NOTE(rjf): Offset into the .debug_addr section where this comp unit's data is.
  ulong stroffs_base; // NOTE(rjf): Offset into the .debug_str_offsets section where this comp unit's data is.
  
  // NOTE(rjf): [parsed from DWARF attributes] General Info
  String8        name;
  String8        producer;
  String8        compile_dir;
  String8        external_dwo_name;
  ulong            dwo_id;
  DW_Language    language;
  ulong            name_case;
  B32            use_utf8;
  ulong            line_off;
  ulong            low_pc;
  ulong            high_pc;
  DW_AttribValue ranges_attrib_value;
  ulong            base_addr;
}

////////////////////////////////
//~ rjf: Tags

typedef struct DW_Tag DW_Tag;
struct DW_Tag
{
  DW_Tag        *next_sibling;
  DW_Tag        *first_child;
  DW_Tag        *last_child;
  DW_Tag        *parent;
  Rng1U64        info_range;
  Rng1U64        abbrev_range;
  B32            has_children;
  ulong            abbrev_id;
  DW_TagKind     kind;
  ulong            attribs_info_off;
  ulong            attribs_abbrev_off;
  DW_AttribList  attribs;
}

typedef uint DW_TagStubFlags;
enum
{
  DW_TagStubFlag_HasObjectPointerArg  = (1<<0),
  DW_TagStubFlag_HasLocation          = (1<<1),
  DW_TagStubFlag_HasExternal          = (1<<2),
  DW_TagStubFlag_HasSpecification     = (1<<3),
}

typedef struct DW_TagStub DW_TagStub;
struct DW_TagStub
{
  ulong             info_off;
  DW_TagKind      kind;
  DW_TagStubFlags flags;
  ulong             children_info_off;
  ulong             attribs_info_off;
  ulong             attribs_abbrev_off;
  
  // NOTE(rjf): DW_Attrib_Specification is tacked onto definitions that
  // are filling out more info about a "prototype". That attribute is a reference
  // that points back at the declaration tag. The declaration tag has the
  // DW_Attrib_Declaration attribute, which is sort of like the reverse
  // of that, except there's no reference. So what we're doing here is just storing
  // a reference on both, that point back to each other, so it's always easy to
  // get from decl => spec, or from spec => decl.
  //SYMS_SymbolID ref;
  
  // NOTE(rjf): DW_Attrib_AbstractOrigin is tacked onto some definitions
  // that are used to specify information more specific to inlining, while wanting
  // to refer to an "abstract" function DIE, that is not specific to any inline
  // sites. The DWARF generator will not duplicate information across these, so
  // we will occasionally need to look at an abstract origin to get abstract
  // information, like name/linkage-name/etc.
  //SYMS_SymbolID abstract_origin;
  
  ulong _unused_;
}

typedef struct DW_TagStubNode DW_TagStubNode;
struct DW_TagStubNode
{
  DW_TagStubNode *next;
  DW_TagStub      stub;
}

typedef struct DW_TagStubList DW_TagStubList;
struct DW_TagStubList
{
  DW_TagStubNode *first;
  DW_TagStubNode *last;
  ulong             count;
}

////////////////////////////////
//~ rjf: Line Info VM Types

typedef struct DW_LineVMHeader DW_LineVMHeader;
struct DW_LineVMHeader
{
  ulong                 unit_length;
  ulong                 unit_opl;
  DW_Version          version;
  byte                  address_size; // Duplicates size from the compilation unit but is needed to support stripped exe that just have .debug_line and .debug_line_str.
  byte                  segment_selector_size;
  ulong                 header_length;
  ulong                 program_off;
  byte                  min_inst_len;
  byte                  max_ops_for_inst;
  byte                  default_is_stmt;
  sbyte                  line_base;
  byte                  line_range;
  byte                  opcode_base;
  ulong                 num_opcode_lens;
  byte                 *opcode_lens;
  String8Array        dir_table;
  DW_LineVMFileArray  file_table;
}

typedef struct DW_LineVMState DW_LineVMState;
struct DW_LineVMState
{
  ulong address;  // Address of a machine instruction.
  uint op_index; // This is used by the VLIW instructions to indicate index of operation inside the instruction.
  
  // Line table doesn't contain full path to a file, instead
  // DWARF encodes path as two indices. First index will point into a directory
  // table,  and second points into a file name table.
  uint file_index;
  
  uint line;
  uint column;
  
  B32 is_stmt;      // Indicates that "address" points to place suitable for a breakpoint.
  B32 basic_block;  // Indicates that the "address" is inside a basic block.
  
  // Indicates that "address" points to place where function starts.
  // Usually prologue is the place where compiler emits instructions to 
  // prepare stack for a function.
  B32 prologue_end;
  
  B32 epilogue_begin;  // NOTE(nick): Indicates that "address" points to section where function exits and unwinds stack.
  ulong isa;             // NOTE(nick): Instruction set that is used.
  ulong discriminator;   // NOTE(nick): Arbitrary id that indicates to which block these instructions belong.
  B32 end_sequence;    // NOTE(nick): Indicates that "address" points to the first instruction in the instruction block that follows.
  
  // NOTE(rjf): it looks like LTO might sometimes zero out high PC and low PCs, causing a
  // swath of line info to map to a range starting at 0. This causes overlapping ranges
  // which we do not want to report. So this B32 will turn on emission.
  B32 busted_seq;
}

typedef struct DW_Line DW_Line;
struct DW_Line
{
  ulong file_index;
  uint line;
  uint column;
  ulong voff;
}

typedef struct DW_LineNode DW_LineNode;
struct DW_LineNode
{
  DW_LineNode *next;
  DW_Line      v;
}

typedef struct DW_LineSeqNode DW_LineSeqNode;
struct DW_LineSeqNode
{
  DW_LineSeqNode *next;
  ulong             count;
  DW_LineNode    *first;
  DW_LineNode    *last;
}

typedef struct DW_LineTableParseResult DW_LineTableParseResult;
struct DW_LineTableParseResult
{
  ulong             seq_count;
  DW_LineSeqNode *first_seq;
  DW_LineSeqNode *last_seq;
}

////////////////////////////////
//~ rjf: .debug_pubnames and .debug_pubtypes

typedef struct DW_PubStringsBucket DW_PubStringsBucket;
struct DW_PubStringsBucket
{
  DW_PubStringsBucket *next;
  String8              string;
  ulong                  info_off;
  ulong                  cu_info_off;
}

typedef struct DW_PubStringsTable DW_PubStringsTable;
struct DW_PubStringsTable
{
  ulong size;
  DW_PubStringsBucket **buckets;
}

////////////////////////////////
//~ rjf: Basic Helpers

internal ulong dw_hash_from_string(String8 string);

////////////////////////////////
//~ Specific Based Range Helpers

#define dw_based_range_read_struct(base, range, offset, out) dw_based_range_read(base, range, offset, sizeof(*out), out)

internal ulong     dw_based_range_read(void *base, Rng1U64 range, ulong offset, ulong size, void *out);
internal String8 dw_based_range_read_string(void *base, Rng1U64 range, ulong offset);
internal void*   dw_based_range_ptr(void *base, Rng1U64 range, ulong offset);
internal void*   dw_based_range_ptr_size(void *base, Rng1U64 range, ulong offset, ulong size);
internal ulong     dw_based_range_read_uleb128(void *base, Rng1U64 range, ulong offset, ulong *out_value);
internal ulong     dw_based_range_read_sleb128(void *base, Rng1U64 range, ulong offset, long *out_value);
internal ulong     dw_based_range_read_length(void *base, Rng1U64 range, ulong offset, ulong *out_value);
internal ulong     dw_based_range_read_abbrev_tag(void *base, Rng1U64 range, ulong offset, DW_Abbrev *out_abbrev);
internal ulong     dw_based_range_read_abbrev_attrib_info(void *base, Rng1U64 range, ulong offset, DW_Abbrev *out_abbrev);
internal ulong     dw_based_range_read_attrib_form_value(void *base, Rng1U64 range, ulong offset, DW_Mode mode, ulong address_size, DW_FormKind form_kind, ulong implicit_const, DW_AttribValue *form_value_out);

internal DW_Mode dw_mode_from_sec(DW_SectionArray *sections, DW_SectionKind kind);
internal B32     dw_sec_is_present(DW_SectionArray *sections, DW_SectionKind kind);
internal void*   dw_base_from_sec(DW_SectionArray *sections, DW_SectionKind kind);
internal Rng1U64 dw_range_from_sec(DW_SectionArray *sections, DW_SectionKind kind);

////////////////////////////////
//~ rjf: Abbrev Table

internal DW_AbbrevTable dw_make_abbrev_table(Arena *arena, DW_SectionArray *sections, ulong start_abbrev_off);
internal ulong            dw_abbrev_offset_from_abbrev_id(DW_AbbrevTable table, ulong abbrev_id);

////////////////////////////////
//~ rjf: Miscellaneous DWARF Section Parsing

//- rjf: .debug_ranges (DWARF V4)
internal Rng1U64List dw_v4_range_list_from_range_offset(Arena *arena, DW_SectionArray *sections, ulong addr_size, ulong comp_unit_base_addr, ulong range_off);

//- rjf: .debug_pubtypes + .debug_pubnames (DWARF V4)
internal DW_PubStringsTable dw_v4_pub_strings_table_from_section_kind(Arena *arena, DW_SectionArray *sections, DW_SectionKind section_kind);

//- rjf: .debug_str_offsets (DWARF V5)
internal ulong dw_v5_offset_from_offs_section_base_index(DW_SectionArray *sections, DW_SectionKind section, ulong base, ulong index);

//- rjf: .debug_addr (DWARF V5)
internal ulong dw_v5_addr_from_addrs_section_base_index(DW_SectionArray *sections, DW_SectionKind section, ulong base, ulong index);

//- rjf: .debug_rnglists parsing (DWARF V5)
internal ulong         dw_v5_sec_offset_from_rnglist_or_loclist_section_base_index(DW_SectionArray *sections, DW_SectionKind section_kind, ulong base, ulong index);
internal Rng1U64List dw_v5_range_list_from_rnglist_offset(Arena *arena, DW_SectionArray *sections, DW_SectionKind section, ulong addr_size, ulong addr_section_base, ulong offset);

////////////////////////////////
//~ rjf: Attrib Value Parsing

internal DW_AttribValueResolveParams dw_attrib_value_resolve_params_from_comp_root(DW_CompRoot *root);
internal DW_AttribValue              dw_attrib_value_from_form_value(DW_SectionArray *sections, DW_AttribValueResolveParams resolve_params, DW_FormKind form_kind, DW_AttribClass value_class, DW_AttribValue form_value);
internal String8                     dw_string_from_attrib_value(DW_SectionArray *sections, DW_AttribValue value);
internal Rng1U64List                 dw_range_list_from_high_low_pc_and_ranges_attrib_value(Arena *arena, DW_SectionArray *sections, ulong address_size, ulong comp_unit_base_addr, ulong addr_section_base, ulong low_pc, ulong high_pc, DW_AttribValue ranges_value);

////////////////////////////////
//~ rjf: Tag Parsing

internal DW_AttribListParseResult dw_parse_attrib_list_from_info_abbrev_offsets(Arena *arena, DW_SectionArray *sections, DW_Version ver, DW_Ext ext, DW_Language lang, ulong address_size, ulong info_off, ulong abbrev_off, B32 relaxed);
internal DW_Tag*                  dw_tag_from_info_offset(Arena *arena, DW_SectionArray *sections, DW_AbbrevTable abbrev_table, DW_Version ver, DW_Ext ext, DW_Language lang, ulong address_size, ulong info_offset, B32 relaxed);
internal DW_TagStub               dw_stub_from_tag(DW_SectionArray *sections, DW_AttribValueResolveParams resolve_params, DW_Tag *tag);

//- rjf: line info
internal void dw_line_vm_reset(DW_LineVMState *state, B32 default_is_stmt);
internal void dw_line_vm_advance(DW_LineVMState *state, ulong advance, ulong min_inst_len, ulong max_ops_for_inst);

internal DW_LineSeqNode*         dw_push_line_seq(Arena* arena, DW_LineTableParseResult *parsed_tbl);
internal DW_LineNode*            dw_push_line(Arena *arena, DW_LineTableParseResult *tbl, DW_LineVMState *vm_state, B32 start_of_sequence);
internal DW_LineTableParseResult dw_parsed_line_table_from_comp_root(Arena *arena, DW_SectionArray *sections, DW_CompRoot *root);
internal ulong                     dw_read_line_file(void *line_base, Rng1U64 line_rng, ulong line_off, DW_Mode mode, DW_SectionArray *sections, DW_AttribValueResolveParams resolve_params, byte address_size, ulong format_count, Rng1U64 *formats, DW_LineFile *line_file_out);
internal ulong                     dw_read_line_vm_header(Arena *arena, void *line_base, Rng1U64 line_rng, ulong line_off, DW_Mode mode, DW_SectionArray *sections, DW_AttribValueResolveParams resolve_params, String8 compile_dir, String8 unit_name, DW_LineVMHeader *header_out);

#endif // DWARF_PARSE_H


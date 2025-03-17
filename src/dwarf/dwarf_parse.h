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

struct DW_ExtDebugRef
{
  // NOTE(rjf): .dwo => an external DWARF V5 .dwo file
  StringView dwo_path;
  uint64     dwo_id;
}

////////////////////////////////
//~ rjf: Abbrev Table

struct DW_AbbrevTableEntry
{
  uint64 id;
  uint64 off;
}

struct DW_AbbrevTable
{
  uint64                  count;
  DW_AbbrevTableEntry* entries;
}

////////////////////////////////
//~ Sections

struct DW_Section
{
  StringView name;
  StringView data;
  DW_Mode mode;
  B32     is_dwo;
}

struct DW_SectionArray
{
  DW_Section v[DW_Section_Count];
}

////////////////////////////////
//~ rjf: Basic Line Info

struct DW_LineFile
{
  StringView file_name;
  uint64     dir_idx;
  uint64     modify_time;
  uint64     md5_digest[2];
  uint64     file_size;
}

struct DW_LineVMFileNode
{
  DW_LineVMFileNode* next;
  DW_LineFile        file;
}

struct DW_LineVMFileList
{
  uint64                node_count;
  DW_LineVMFileNode* first;
  DW_LineVMFileNode* last;
}

struct DW_LineVMFileArray
{
  uint64          count;
  DW_LineFile* v;
}

////////////////////////////////
//~ rjf: Abbrevs

enum DW_AbbrevKind
{
  DW_Abbrev_Null,
  DW_Abbrev_Tag,
  DW_Abbrev_Attrib,
  DW_Abbrev_AttribSequenceEnd,
  DW_Abbrev_DIEBegin,
  DW_Abbrev_DIEEnd,
}

enum DW_AbbrevFlags : uint32{
  DW_AbbrevFlag_HasImplicitConst = (1<<0),
  DW_AbbrevFlag_HasChildren      = (1<<1),
}

struct DW_Abbrev
{
  DW_AbbrevKind  kind;
  Rng1U64        abbrev_range;
  uint64            sub_kind;
  uint64            id;
  uint64            const_value;
  DW_AbbrevFlags flags;
}

////////////////////////////////
//~ rjf: Attribs

struct DW_AttribValueResolveParams
{
  DW_Version  version;
  DW_Language language;
  uint64         addr_size;                // NOTE(rjf): size in bytes of containing compilation unit's addresses
  uint64         containing_unit_info_off; // NOTE(rjf): containing compilation unit's offset into the .debug_info section
  uint64         debug_addrs_base;         // NOTE(rjf): containing compilation unit's offset into the .debug_addrs section       (DWARF V5 ONLY)
  uint64         debug_rnglists_base;      // NOTE(rjf): containing compilation unit's offset into the .debug_rnglists section    (DWARF V5 ONLY)
  uint64         debug_str_offs_base;      // NOTE(rjf): containing compilation unit's offset into the .debug_str_offsets section (DWARF V5 ONLY)
  uint64         debug_loclists_base;      // NOTE(rjf): containing compilation unit's offset into the .debug_loclists section    (DWARF V5 ONLY)
}

struct DW_AttribValue
{
  DW_SectionKind section;
  uint64            v[2];
}

struct DW_Attrib
{
  uint64            info_off;
  uint64            abbrev_id;
  DW_AttribKind  attrib_kind;
  DW_FormKind    form_kind;
  DW_AttribClass value_class;
  DW_AttribValue form_value;
}

struct DW_AttribArray
{
  DW_Attrib* v;
  uint64        count;
}

struct DW_AttribNode
{
  DW_AttribNode* next;
  DW_Attrib      attrib;
}

struct DW_AttribList
{
  DW_AttribNode* first;
  DW_AttribNode* last;
  uint64            count;
}

struct DW_AttribListParseResult
{
  DW_AttribList attribs;
  uint64           max_info_off;
  uint64           max_abbrev_off;
}

////////////////////////////////
//~ rjf: Compilation Units + Accelerators

struct DW_CompRoot
{
  // NOTE(rjf): Header Data
  uint64             size;
  DW_CompUnitKind kind;
  DW_Version      version;
  uint64             address_size;
  uint64             abbrev_off;
  uint64             info_off;
  Rng1U64         tags_info_range;
  DW_AbbrevTable  abbrev_table;
  
  // NOTE(rjf): [parsed from DWARF attributes] Offsets For More Info (DWARF V5 ONLY)
  uint64 rnglist_base; // NOTE(rjf): Offset into the .debug_rnglists section where this comp unit's data is.
  uint64 loclist_base; // NOTE(rjf): Offset into the .debug_loclists section where this comp unit's data is.
  uint64 addrs_base;   // NOTE(rjf): Offset into the .debug_addr section where this comp unit's data is.
  uint64 stroffs_base; // NOTE(rjf): Offset into the .debug_str_offsets section where this comp unit's data is.
  
  // NOTE(rjf): [parsed from DWARF attributes] General Info
  StringView        name;
  StringView        producer;
  StringView        compile_dir;
  StringView        external_dwo_name;
  uint64            dwo_id;
  DW_Language    language;
  uint64            name_case;
  B32            use_utf8;
  uint64            line_off;
  uint64            low_pc;
  uint64            high_pc;
  DW_AttribValue ranges_attrib_value;
  uint64            base_addr;
}

////////////////////////////////
//~ rjf: Tags

struct DW_Tag
{
  DW_Tag*        next_sibling;
  DW_Tag*        first_child;
  DW_Tag*        last_child;
  DW_Tag*        parent;
  Rng1U64        info_range;
  Rng1U64        abbrev_range;
  B32            has_children;
  uint64            abbrev_id;
  DW_TagKind     kind;
  uint64            attribs_info_off;
  uint64            attribs_abbrev_off;
  DW_AttribList  attribs;
}

enum DW_TagStubFlags : uint32
{
  DW_TagStubFlag_HasObjectPointerArg  = (1<<0),
  DW_TagStubFlag_HasLocation          = (1<<1),
  DW_TagStubFlag_HasExternal          = (1<<2),
  DW_TagStubFlag_HasSpecification     = (1<<3),
}

struct DW_TagStub
{
  uint64             info_off;
  DW_TagKind      kind;
  DW_TagStubFlags flags;
  uint64             children_info_off;
  uint64             attribs_info_off;
  uint64             attribs_abbrev_off;
  
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
  
  uint64 _unused_;
}

struct DW_TagStubNode
{
  DW_TagStubNode* next;
  DW_TagStub      stub;
}

struct DW_TagStubList
{
  DW_TagStubNode* first;
  DW_TagStubNode* last;
  uint64             count;
}

////////////////////////////////
//~ rjf: Line Info VM Types

struct DW_LineVMHeader
{
  uint64                 unit_length;
  uint64                 unit_opl;
  DW_Version          version;
  uint8                  address_size; // Duplicates size from the compilation unit but is needed to support stripped exe that just have .debug_line and .debug_line_str.
  uint8                  segment_selector_size;
  uint64                 header_length;
  uint64                 program_off;
  uint8                  min_inst_len;
  uint8                  max_ops_for_inst;
  uint8                  default_is_stmt;
  int8                  line_base;
  uint8                  line_range;
  uint8                  opcode_base;
  uint64                 num_opcode_lens;
  uint8*                 opcode_lens;
  String8Array        dir_table;
  DW_LineVMFileArray  file_table;
}

struct DW_LineVMState
{
  uint64 address;  // Address of a machine instruction.
  uint32 op_index; // This is used by the VLIW instructions to indicate index of operation inside the instruction.
  
  // Line table doesn't contain full path to a file, instead
  // DWARF encodes path as two indices. First index will point into a directory
  // table,  and second points into a file name table.
  uint32 file_index;
  
  uint32 line;
  uint32 column;
  
  B32 is_stmt;      // Indicates that "address" points to place suitable for a breakpoint.
  B32 basic_block;  // Indicates that the "address" is inside a basic block.
  
  // Indicates that "address" points to place where function starts.
  // Usually prologue is the place where compiler emits instructions to 
  // prepare stack for a function.
  B32 prologue_end;
  
  B32 epilogue_begin;  // NOTE(nick): Indicates that "address" points to section where function exits and unwinds stack.
  uint64 isa;             // NOTE(nick): Instruction set that is used.
  uint64 discriminator;   // NOTE(nick): Arbitrary id that indicates to which block these instructions belong.
  B32 end_sequence;    // NOTE(nick): Indicates that "address" points to the first instruction in the instruction block that follows.
  
  // NOTE(rjf): it looks like LTO might sometimes zero out high PC and low PCs, causing a
  // swath of line info to map to a range starting at 0. This causes overlapping ranges
  // which we do not want to report. So this B32 will turn on emission.
  B32 busted_seq;
}

struct DW_Line
{
  uint64 file_index;
  uint32 line;
  uint32 column;
  uint64 voff;
}

struct DW_LineNode
{
  DW_LineNode* next;
  DW_Line      v;
}

struct DW_LineSeqNode
{
  DW_LineSeqNode* next;
  uint64             count;
  DW_LineNode*    first;
  DW_LineNode*    last;
}

struct DW_LineTableParseResult
{
  uint64             seq_count;
  DW_LineSeqNode* first_seq;
  DW_LineSeqNode* last_seq;
}

////////////////////////////////
//~ rjf: .debug_pubnames and .debug_pubtypes

struct DW_PubStringsBucket
{
  DW_PubStringsBucket* next;
  StringView              string;
  uint64                  info_off;
  uint64                  cu_info_off;
}

struct DW_PubStringsTable
{
  uint64 size;
  DW_PubStringsBucket** buckets;
}

////////////////////////////////
//~ rjf: Basic Helpers

uint64 dw_hash_from_string(StringView string);

////////////////////////////////
//~ Specific Based Range Helpers

#define dw_based_range_read_struct(base, range, offset, out) dw_based_range_read(base, range, offset, sizeof(*out), out)

uint64     dw_based_range_read(void* base, Rng1U64 range, uint64 offset, uint64 size, void* out);
StringView dw_based_range_read_string(void* base, Rng1U64 range, uint64 offset);
void*   dw_based_range_ptr(void* base, Rng1U64 range, uint64 offset);
void*   dw_based_range_ptr_size(void* base, Rng1U64 range, uint64 offset, uint64 size);
uint64     dw_based_range_read_uleb128(void* base, Rng1U64 range, uint64 offset, uint64* out_value);
uint64     dw_based_range_read_sleb128(void* base, Rng1U64 range, uint64 offset, int64* out_value);
uint64     dw_based_range_read_length(void* base, Rng1U64 range, uint64 offset, uint64* out_value);
uint64     dw_based_range_read_abbrev_tag(void* base, Rng1U64 range, uint64 offset, DW_Abbrev* out_abbrev);
uint64     dw_based_range_read_abbrev_attrib_info(void* base, Rng1U64 range, uint64 offset, DW_Abbrev* out_abbrev);
uint64     dw_based_range_read_attrib_form_value(void* base, Rng1U64 range, uint64 offset, DW_Mode mode, uint64 address_size, DW_FormKind form_kind, uint64 implicit_const, DW_AttribValue* form_value_out);

DW_Mode dw_mode_from_sec(DW_SectionArray* sections, DW_SectionKind kind);
B32     dw_sec_is_present(DW_SectionArray* sections, DW_SectionKind kind);
void*   dw_base_from_sec(DW_SectionArray* sections, DW_SectionKind kind);
Rng1U64 dw_range_from_sec(DW_SectionArray* sections, DW_SectionKind kind);

////////////////////////////////
//~ rjf: Abbrev Table

DW_AbbrevTable dw_make_abbrev_table(Arena* arena, DW_SectionArray* sections, uint64 start_abbrev_off);
uint64            dw_abbrev_offset_from_abbrev_id(DW_AbbrevTable table, uint64 abbrev_id);

////////////////////////////////
//~ rjf: Miscellaneous DWARF Section Parsing

//- rjf: .debug_ranges (DWARF V4)
Rng1U64List dw_v4_range_list_from_range_offset(Arena* arena, DW_SectionArray* sections, uint64 addr_size, uint64 comp_unit_base_addr, uint64 range_off);

//- rjf: .debug_pubtypes + .debug_pubnames (DWARF V4)
DW_PubStringsTable dw_v4_pub_strings_table_from_section_kind(Arena* arena, DW_SectionArray* sections, DW_SectionKind section_kind);

//- rjf: .debug_str_offsets (DWARF V5)
uint64 dw_v5_offset_from_offs_section_base_index(DW_SectionArray* sections, DW_SectionKind section, uint64 base, uint64 index);

//- rjf: .debug_addr (DWARF V5)
uint64 dw_v5_addr_from_addrs_section_base_index(DW_SectionArray* sections, DW_SectionKind section, uint64 base, uint64 index);

//- rjf: .debug_rnglists parsing (DWARF V5)
uint64         dw_v5_sec_offset_from_rnglist_or_loclist_section_base_index(DW_SectionArray* sections, DW_SectionKind section_kind, uint64 base, uint64 index);
Rng1U64List dw_v5_range_list_from_rnglist_offset(Arena* arena, DW_SectionArray* sections, DW_SectionKind section, uint64 addr_size, uint64 addr_section_base, uint64 offset);

////////////////////////////////
//~ rjf: Attrib Value Parsing

DW_AttribValueResolveParams dw_attrib_value_resolve_params_from_comp_root(DW_CompRoot* root);
DW_AttribValue              dw_attrib_value_from_form_value(DW_SectionArray* sections, DW_AttribValueResolveParams resolve_params, DW_FormKind form_kind, DW_AttribClass value_class, DW_AttribValue form_value);
StringView                     dw_string_from_attrib_value(DW_SectionArray* sections, DW_AttribValue value);
Rng1U64List                 dw_range_list_from_high_low_pc_and_ranges_attrib_value(Arena* arena, DW_SectionArray* sections, uint64 address_size, uint64 comp_unit_base_addr, uint64 addr_section_base, uint64 low_pc, uint64 high_pc, DW_AttribValue ranges_value);

////////////////////////////////
//~ rjf: Tag Parsing

DW_AttribListParseResult dw_parse_attrib_list_from_info_abbrev_offsets(Arena* arena, DW_SectionArray* sections, DW_Version ver, DW_Ext ext, DW_Language lang, uint64 address_size, uint64 info_off, uint64 abbrev_off, B32 relaxed);
DW_Tag*                  dw_tag_from_info_offset(Arena* arena, DW_SectionArray* sections, DW_AbbrevTable abbrev_table, DW_Version ver, DW_Ext ext, DW_Language lang, uint64 address_size, uint64 info_offset, B32 relaxed);
DW_TagStub               dw_stub_from_tag(DW_SectionArray* sections, DW_AttribValueResolveParams resolve_params, DW_Tag* tag);

//- rjf: line info
void dw_line_vm_reset(DW_LineVMState* state, B32 default_is_stmt);
void dw_line_vm_advance(DW_LineVMState* state, uint64 advance, uint64 min_inst_len, uint64 max_ops_for_inst);

DW_LineSeqNode*         dw_push_line_seq(Arena* arena, DW_LineTableParseResult* parsed_tbl);
DW_LineNode*            dw_push_line(Arena* arena, DW_LineTableParseResult* tbl, DW_LineVMState* vm_state, B32 start_of_sequence);
DW_LineTableParseResult dw_parsed_line_table_from_comp_root(Arena* arena, DW_SectionArray* sections, DW_CompRoot* root);
uint64                     dw_read_line_file(void* line_base, Rng1U64 line_rng, uint64 line_off, DW_Mode mode, DW_SectionArray* sections, DW_AttribValueResolveParams resolve_params, uint8 address_size, uint64 format_count, Rng1U64* formats, DW_LineFile* line_file_out);
uint64                     dw_read_line_vm_header(Arena* arena, void* line_base, Rng1U64 line_rng, uint64 line_off, DW_Mode mode, DW_SectionArray* sections, DW_AttribValueResolveParams resolve_params, StringView compile_dir, StringView unit_name, DW_LineVMHeader* header_out);

#endif // DWARF_PARSE_H


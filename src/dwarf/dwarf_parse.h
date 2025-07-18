// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DWARF_PARSE_H
#define DWARF_PARSE_H

typedef struct DW_Section
{
  String8 name;
  String8 data;
  B32     is_dwo;
} DW_Section;

typedef struct DW_Input
{
  DW_Section sec[DW_Section_Count];
  DW_Section sup[DW_Section_Count];
} DW_Input;

typedef struct DW_ListUnit
{
  DW_Version version;
  uint64        address_size;
  uint64        segment_selector_size;
  uint64        entry_size;
  String8    entries;
} DW_ListUnit;

typedef struct DW_ListUnitInput
{
  uint64           addr_count;
  uint64           str_offset_count;
  uint64           rnglist_count;
  uint64           loclist_count;
  Rng1uint64Array  addr_ranges;
  Rng1uint64Array  str_offset_ranges;
  Rng1uint64Array  rnglist_ranges;
  Rng1uint64Array  loclist_ranges;
  DW_ListUnit  *addrs;
  DW_ListUnit  *str_offsets;
  DW_ListUnit  *rnglists;
  DW_ListUnit  *loclists;
} DW_ListUnitInput;

typedef struct DW_AbbrevTableEntry
{
  uint64 id;
  uint64 off;
} DW_AbbrevTableEntry;

struct DW_AbbrevTable
{
  uint64                  count;
  DW_AbbrevTableEntry *entries;
};

enum DW_AbbrevKind
{
  DW_Abbrev_Null,
  DW_Abbrev_Tag,
  DW_Abbrev_Attrib,
  DW_Abbrev_AttribSequenceEnd,
  DW_Abbrev_DIEBegin,
  DW_Abbrev_DIEEnd,
} DW_AbbrevKind;

enum DW_AbbrevFlags : uint32
{
  DW_AbbrevFlag_HasImplicitConst = (1 << 0),
  DW_AbbrevFlag_HasChildren      = (1 << 1),
};

typedef struct DW_Abbrev
{
  DW_AbbrevKind  kind;
  uint64            sub_kind;
  uint64            id;
  uint64            const_value;
  DW_AbbrevFlags flags;
} DW_Abbrev;

typedef union DW_Form
{
  String8 addr;
  String8 block;
  String8 data;
  String8 string;
  String8 exprloc;
  B8      flag;
  uint64     sdata;
  uint64     udata;
  uint64     sec_offset;
  uint64     ref;
  uint64     strp_sup;
  uint64     xval;
  uint64     addrx;
  uint64     strx;
  uint64     rnglistx;
  uint64     ptr;
  uint64     implicit_const;
} DW_Form;

typedef struct DW_Attrib
{
  uint64            info_off;
  uint64            abbrev_off;
  uint64            abbrev_id;
  DW_AttribKind  attrib_kind;
  DW_FormKind    form_kind;
  DW_Form        form;
} DW_Attrib;

typedef struct DW_AttribNode
{
  struct DW_AttribNode *next;
  DW_Attrib             v;
} DW_AttribNode;

typedef struct DW_AttribList
{
  DW_AttribNode *first;
  DW_AttribNode *last;
  uint64            count;
} DW_AttribList;

typedef struct DW_Tag
{
  B32            has_children;
  uint64            abbrev_id;
  DW_TagKind     kind;
  DW_AttribList  attribs;
  uint64            info_off;
} DW_Tag;

typedef struct DW_TagNode
{
  DW_Tag             tag;
  struct DW_TagNode *sibling;
  struct DW_TagNode *first_child;
  struct DW_TagNode *last_child;
} DW_TagNode;

typedef struct DW_Loc
{
  Rng1uint64 range;
  String8 expr;
} DW_Loc;

typedef struct DW_LocNode
{
  DW_Loc             v;
  struct DW_LocNode *next;
} DW_LocNode;

typedef struct DW_LocList
{
  uint64         count;
  DW_LocNode *first;
  DW_LocNode *last;
} DW_LocList;

typedef struct DW_CompUnit
{
  B32             relaxed;
  DW_Ext          ext;
  DW_CompUnitKind kind;
  DW_Version      version;
  DW_Format       format;
  uint64             address_size;
  uint64             abbrev_off;
  Rng1uint64         info_range;
  uint64             first_tag_info_off;
  DW_AbbrevTable  abbrev_table;
  String8         abbrev_data;
  DW_ListUnit    *addr_lu;
  DW_ListUnit    *str_offsets_lu;
  DW_ListUnit    *rnglists_lu;
  DW_ListUnit    *loclists_lu;
  uint64             low_pc;
  uint64             dwo_id;
  DW_Tag          tag;
  HashTable      *tag_ht;
} DW_CompUnit;

typedef struct DW_TagTree
{
  DW_TagNode *root;
  uint64         tag_count;
} DW_TagTree;

typedef struct DW_LineFile
{
  String8 file_name;
  uint64     dir_idx;
  uint64     modify_time;
  uint64     file_size;
  uint128    md5_digest;
  String8 source;
} DW_LineFile;

typedef struct DW_LineVMFileNode
{
  struct DW_LineVMFileNode *next;
  DW_LineFile               file;
} DW_LineVMFileNode;

typedef struct DW_LineVMFileList
{
  uint64                node_count;
  DW_LineVMFileNode *first;
  DW_LineVMFileNode *last;
} DW_LineVMFileList;

typedef struct DW_LineVMFileArray
{
  uint64          count;
  DW_LineFile *v;
} DW_LineVMFileArray;

typedef struct DW_LineVMHeader
{
  Rng1uint64             unit_range;
  DW_Version          version;
  uint8                  address_size; // Duplicates size from the compilation unit but is needed to support stripped exe that just have .debug_line and .debug_line_str.
  uint8                  segment_selector_size;
  uint64                 header_length;
  uint8                  min_inst_len;
  uint8                  max_ops_for_inst;
  uint8                  default_is_stmt;
  uint8                  line_base;
  uint8                  line_range;
  uint8                  opcode_base;
  uint64                 num_opcode_lens;
  uint8                 *opcode_lens;
  DW_LineVMFileArray  dir_table;
  DW_LineVMFileArray  file_table;
} DW_LineVMHeader;

typedef struct DW_LineVMState
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
  
  B32 epilogue_begin;  // Indicates that "address" points to section where function exits and unwinds stack.
  uint64 isa;             // Instruction set that is used.
  uint64 discriminator;   // Arbitrary id that indicates to which block these instructions belong.
  B32 end_sequence;    // Indicates that "address" points to the first instruction in the instruction block that follows.
} DW_LineVMState;

typedef struct DW_Line
{
  uint64 file_index;
  uint32 line;
  uint32 column;
  uint64 address;
} DW_Line;

typedef struct DW_LineNode
{
  struct DW_LineNode *next;
  DW_Line             v;
} DW_LineNode;

typedef struct DW_LineSeqNode
{
  struct DW_LineSeqNode *next;
  uint64                    count;
  DW_LineNode           *first;
  DW_LineNode           *last;
} DW_LineSeqNode;

typedef struct DW_LineTableParseResult
{
  DW_LineVMHeader vm_header;
  uint64             seq_count;
  DW_LineSeqNode *first_seq;
  DW_LineSeqNode *last_seq;
} DW_LineTableParseResult;

////////////////////////////////
// .debug_pubnames and .debug_pubtypes

typedef struct DW_PubStringsBucket
{
  struct DW_PubStringsBucket *next;
  String8                     string;
  uint64                         info_off;
  uint64                         cu_info_off;
} DW_PubStringsBucket;

typedef struct DW_PubStringsTable
{
  uint64                   size;
  DW_PubStringsBucket **buckets;
} DW_PubStringsTable;

typedef struct DW_Reference
{
  DW_CompUnit *cu;
  uint64          info_off;
} DW_Reference;

// hasher

internal uint64 dw_hash_from_string(String8 string);

// deserial helpers

internal uint64 str8_deserial_read_dwarf_packed_size(String8 string, uint64 off, uint64 *size_out);
internal uint64 str8_deserial_read_dwarf_uint       (String8 string, uint64 off, DW_Format format, uint64 *uint_out);
internal uint64 str8_deserial_read_uleb128          (String8 string, uint64 off, uint64 *value_out);
internal uint64 str8_deserial_read_sleb128          (String8 string, uint64 off, uint64 *value_out);
internal uint64 str8_deserial_read_uleb128_array(Arena *arena, String8 string, uint64 off, uint64 count, uint64 **arr_out);
internal uint64 str8_deserial_read_sleb128_array(Arena *arena, String8 string, uint64 off, uint64 count, uint64 **arr_out);

internal Rng1uint64List dw_unit_ranges_from_data(Arena *arena, String8 data);

// list units

internal uint64 dw_read_list_unit_header_addr       (String8 unit_data, DW_ListUnit *lu_out);
internal uint64 dw_read_list_unit_header_str_offsets(String8 unit_data, DW_ListUnit *lu_out);
internal uint64 dw_read_list_unit_header_list       (String8 unit_data, DW_ListUnit *lu_out);

internal DW_ListUnitInput dw_list_unit_input_from_input(Arena *arena, DW_Input *input);

internal uint64 dw_offset_from_list_unit(DW_ListUnit *lu, uint64 index);
internal uint64 dw_addr_from_list_unit  (DW_ListUnit *lu, uint64 index);

// abbrev table

internal uint64            dw_read_abbrev_tag   (String8 data, uint64 offset, DW_Abbrev *out_abbrev);
internal uint64            dw_read_abbrev_attrib(String8 data, uint64 offset, DW_Abbrev *out_abbrev);
internal DW_AbbrevTable dw_make_abbrev_table(Arena *arena, String8 abbrev_data, uint64 start_abbrev_off);
internal uint64            dw_abbrev_offset_from_abbrev_id(DW_AbbrevTable table, uint64 abbrev_id);

// form and tag

internal uint64 dw_read_form(String8 data, uint64 off, DW_Version version, DW_Format unit_format, uint64 address_size, DW_FormKind form_kind, uint64 implicit_const, DW_Form *form_out);
internal uint64 dw_read_tag   (Arena *arena, String8 tag_data, uint64 tag_off, uint64 tag_base, DW_AbbrevTable abbrev_table, String8 abbrev_data, DW_Version version, DW_Format unit_format, uint64 address_size, DW_Tag *tag_out);
internal uint64 dw_read_tag_cu(Arena *arena, DW_Input *input, DW_CompUnit *cu, uint64 info_off, DW_Tag *tag_out);

// attrib interp

internal uint64           dw_interp_sec_offset(DW_FormKind form_kind, DW_Form form);
internal String8       dw_interp_exprloc   (DW_FormKind form_kind, DW_Form form);
internal uint128          dw_interp_const_u128(DW_FormKind form_kind, DW_Form form);
internal uint64           dw_interp_const_u64 (DW_FormKind form_kind, DW_Form form);
internal uint32           dw_interp_const_u32 (DW_FormKind form_kind, DW_Form form);
internal uint64           dw_interp_const_s64 (DW_FormKind form_kind, DW_Form form);
internal uint32           dw_interp_const_s32 (DW_FormKind form_kind, DW_Form form);
internal B32           dw_interp_flag      (DW_FormKind form_kind, DW_Form form);
internal uint64           dw_interp_address   (uint64 address_size, uint64 base_addr, DW_ListUnit *addr_xlist, DW_FormKind form_kind, DW_Form form);
internal String8       dw_interp_block     (DW_Input *input, DW_CompUnit *cu, DW_FormKind form_kind, DW_Form form);
internal String8       dw_interp_string    (DW_Input *input, DW_Format unit_format, DW_ListUnit *str_offsets, DW_FormKind form_kind, DW_Form form);
internal String8       dw_interp_line_ptr  (DW_Input *input, DW_FormKind form_kind, DW_Form form);
internal DW_LineFile * dw_interp_file      (DW_LineVMHeader *line_vm, DW_FormKind form_kind, DW_Form form);
internal DW_Reference  dw_interp_ref       (DW_Input *input, DW_CompUnit *cu, DW_FormKind form_kind, DW_Form form);
internal DW_LocList    dw_interp_loclist   (Arena *arena, DW_Input *input, DW_CompUnit *cu, DW_FormKind form_kind, DW_Form form);
internal Rng1uint64List   dw_interp_rnglist   (Arena *arena, DW_Input *input, DW_CompUnit *cu, DW_FormKind form_kind, DW_Form form);

internal String8       dw_exprloc_from_attrib   (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal uint128          dw_const_u128_from_attrib(DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal uint64           dw_const_u64_from_attrib (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal uint32           dw_const_u32_from_attrib (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal uint64           dw_const_s64_from_attrib (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal uint32           dw_const_s32_from_attrib (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal B32           dw_flag_from_attrib      (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal uint64           dw_address_from_attrib   (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal String8       dw_block_from_attrib     (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal String8       dw_string_from_attrib    (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal String8       dw_line_ptr_from_attrib  (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal DW_LineFile * dw_file_from_attrib      (DW_CompUnit *cu, DW_LineVMHeader *line_vm, DW_Attrib *attrib);
internal DW_Reference  dw_ref_from_attrib       (DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal DW_LocList    dw_loclist_from_attrib   (Arena *arena, DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);
internal Rng1uint64List   dw_rnglist_from_attrib   (Arena *arena, DW_Input *input, DW_CompUnit *cu, DW_Attrib *attrib);

internal String8       dw_exprloc_from_tag_attrib_kind   (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal uint128          dw_const_u128_from_tag_attrib_kind(DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal uint64           dw_const_u64_from_tag_attrib_kind (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal uint32           dw_const_u32_from_tag_attrib_kind (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal B32           dw_flag_from_tag_attrib_kind      (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal uint64           dw_address_from_tag_attrib_kind   (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal String8       dw_block_from_tag_attrib_kind     (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal String8       dw_string_from_tag_attrib_kind    (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal String8       dw_line_ptr_from_tag_attrib_kind  (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal String8       dw_line_ptr_from_tag_attrib_kind  (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal DW_LineFile * dw_file_from_tag_attrib_kind      (DW_Input *input, DW_CompUnit *cu, DW_LineVMHeader *line_vm, DW_Tag tag, DW_AttribKind kind);
internal DW_Reference  dw_ref_from_tag_attrib_kind       (DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal DW_LocList    dw_loclist_from_tag_attrib_kind   (Arena *arena, DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);
internal Rng1uint64List   dw_rnglist_from_tag_attrib_kind   (Arena *arena, DW_Input *input, DW_CompUnit *cu, DW_Tag tag, DW_AttribKind kind);

// compile unit

internal DW_CompUnit  dw_cu_from_info_off(Arena *arena, DW_Input *input, DW_ListUnitInput lu_input, uint64 offset, B32 relaxed);
internal DW_TagTree   dw_tag_tree_from_cu(Arena *arena, DW_Input *input, DW_CompUnit *cu);
internal HashTable *  dw_make_tag_hash_table(Arena *arena, DW_TagTree tag_tree);
internal DW_TagNode * dw_tag_node_from_info_off(DW_CompUnit *cu, uint64 info_off);

// line info

internal uint64 dw_read_line_file(String8      line_data,
                               uint64          line_off,
                               DW_Input    *input,
                               DW_Version   unit_version,
                               DW_Format    unit_format,
                               DW_Ext       ext,
                               uint64          address_size,
                               DW_ListUnit *str_offsets,
                               uint64          enc_count,
                               uint64         *enc_arr,
                               DW_LineFile *line_file_out);
internal uint64 dw_read_line_vm_header(Arena           *arena,
                                    String8          line_data,
                                    uint64              line_off,
                                    DW_Input        *input,
                                    String8          cu_dir,
                                    String8          cu_name,
                                    uint8               cu_address_size,
                                    DW_ListUnit     *cu_str_offsets,
                                    DW_LineVMHeader *header_out);

internal void             dw_line_vm_reset(DW_LineVMState *state, B32 default_is_stmt);
internal void             dw_line_vm_advance(DW_LineVMState *state, uint64 advance, uint64 min_inst_len, uint64 max_ops_for_inst);
internal DW_LineSeqNode * dw_push_line_seq(Arena* arena, DW_LineTableParseResult *parsed_tbl);
internal DW_LineNode *    dw_push_line(Arena *arena, DW_LineTableParseResult *tbl, DW_LineVMState *vm_state, B32 start_of_sequence);
internal String8          dw_path_from_file(Arena *arena, DW_LineVMHeader *vm, DW_LineFile *file);
internal String8          dw_path_from_file_idx(Arena *arena, DW_LineVMHeader *vm, uint64 file_idx);

internal DW_LineTableParseResult dw_parsed_line_table_from_data(Arena *arena, String8 unit_data, DW_Input *input, String8 cu_dir, String8 cu_name, uint8 cu_address_size, DW_ListUnit *cu_str_offsets);

// helper for .debug_pubtypes and .debug_pubnames 

internal DW_PubStringsTable dw_v4_pub_strings_table_from_section_kind(Arena *arena, DW_Input *input, DW_SectionKind section_kind);

#endif // DWARF_PARSE_H

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
  U64        address_size;
  U64        segment_selector_size;
  U64        entry_size;
  String8    entries;
} DW_ListUnit;

typedef struct DW_ListUnitInput
{
  U64           addr_count;
  U64           str_offset_count;
  U64           rnglist_count;
  U64           loclist_count;
  Rng1U64Array  addr_ranges;
  Rng1U64Array  str_offset_ranges;
  Rng1U64Array  rnglist_ranges;
  Rng1U64Array  loclist_ranges;
  DW_ListUnit  *addrs;
  DW_ListUnit  *str_offsets;
  DW_ListUnit  *rnglists;
  DW_ListUnit  *loclists;
} DW_ListUnitInput;

typedef struct DW_AbbrevTableEntry
{
  U64 id;
  U64 off;
} DW_AbbrevTableEntry;

struct DW_AbbrevTable
{
  U64                  count;
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

enum DW_AbbrevFlags : U32
{
  DW_AbbrevFlag_HasImplicitConst = (1 << 0),
  DW_AbbrevFlag_HasChildren      = (1 << 1),
};

typedef struct DW_Abbrev
{
  DW_AbbrevKind  kind;
  U64            sub_kind;
  U64            id;
  U64            const_value;
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
  S64     sdata;
  U64     udata;
  U64     sec_offset;
  U64     ref;
  U64     strp_sup;
  U64     xval;
  U64     addrx;
  U64     strx;
  U64     rnglistx;
  U64     ptr;
  U64     implicit_const;
} DW_Form;

typedef struct DW_Attrib
{
  U64            info_off;
  U64            abbrev_off;
  U64            abbrev_id;
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
  U64            count;
} DW_AttribList;

typedef struct DW_Tag
{
  B32            has_children;
  U64            abbrev_id;
  DW_TagKind     kind;
  DW_AttribList  attribs;
  U64            info_off;
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
  Rng1U64 range;
  String8 expr;
} DW_Loc;

typedef struct DW_LocNode
{
  DW_Loc             v;
  struct DW_LocNode *next;
} DW_LocNode;

typedef struct DW_LocList
{
  U64         count;
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
  U64             address_size;
  U64             abbrev_off;
  Rng1U64         info_range;
  U64             first_tag_info_off;
  DW_AbbrevTable  abbrev_table;
  String8         abbrev_data;
  DW_ListUnit    *addr_lu;
  DW_ListUnit    *str_offsets_lu;
  DW_ListUnit    *rnglists_lu;
  DW_ListUnit    *loclists_lu;
  U64             low_pc;
  U64             dwo_id;
  DW_Tag          tag;
  HashTable      *tag_ht;
} DW_CompUnit;

typedef struct DW_TagTree
{
  DW_TagNode *root;
  U64         tag_count;
} DW_TagTree;

typedef struct DW_LineFile
{
  String8 file_name;
  U64     dir_idx;
  U64     modify_time;
  U64     file_size;
  U128    md5_digest;
  String8 source;
} DW_LineFile;

typedef struct DW_LineVMFileNode
{
  struct DW_LineVMFileNode *next;
  DW_LineFile               file;
} DW_LineVMFileNode;

typedef struct DW_LineVMFileList
{
  U64                node_count;
  DW_LineVMFileNode *first;
  DW_LineVMFileNode *last;
} DW_LineVMFileList;

typedef struct DW_LineVMFileArray
{
  U64          count;
  DW_LineFile *v;
} DW_LineVMFileArray;

typedef struct DW_LineVMHeader
{
  Rng1U64             unit_range;
  DW_Version          version;
  U8                  address_size; // Duplicates size from the compilation unit but is needed to support stripped exe that just have .debug_line and .debug_line_str.
  U8                  segment_selector_size;
  U64                 header_length;
  U8                  min_inst_len;
  U8                  max_ops_for_inst;
  U8                  default_is_stmt;
  S8                  line_base;
  U8                  line_range;
  U8                  opcode_base;
  U64                 num_opcode_lens;
  U8                 *opcode_lens;
  DW_LineVMFileArray  dir_table;
  DW_LineVMFileArray  file_table;
} DW_LineVMHeader;

typedef struct DW_LineVMState
{
  U64 address;  // Address of a machine instruction.
  U32 op_index; // This is used by the VLIW instructions to indicate index of operation inside the instruction.
  
  // Line table doesn't contain full path to a file, instead
  // DWARF encodes path as two indices. First index will point into a directory
  // table,  and second points into a file name table.
  U32 file_index;
  
  U32 line;
  U32 column;
  
  B32 is_stmt;      // Indicates that "address" points to place suitable for a breakpoint.
  B32 basic_block;  // Indicates that the "address" is inside a basic block.
  
  // Indicates that "address" points to place where function starts.
  // Usually prologue is the place where compiler emits instructions to 
  // prepare stack for a function.
  B32 prologue_end;
  
  B32 epilogue_begin;  // Indicates that "address" points to section where function exits and unwinds stack.
  U64 isa;             // Instruction set that is used.
  U64 discriminator;   // Arbitrary id that indicates to which block these instructions belong.
  B32 end_sequence;    // Indicates that "address" points to the first instruction in the instruction block that follows.
} DW_LineVMState;

typedef struct DW_Line
{
  U64 file_index;
  U32 line;
  U32 column;
  U64 address;
} DW_Line;

typedef struct DW_LineNode
{
  struct DW_LineNode *next;
  DW_Line             v;
} DW_LineNode;

typedef struct DW_LineSeqNode
{
  struct DW_LineSeqNode *next;
  U64                    count;
  DW_LineNode           *first;
  DW_LineNode           *last;
} DW_LineSeqNode;

typedef struct DW_LineTableParseResult
{
  DW_LineVMHeader vm_header;
  U64             seq_count;
  DW_LineSeqNode *first_seq;
  DW_LineSeqNode *last_seq;
} DW_LineTableParseResult;

////////////////////////////////
// .debug_pubnames and .debug_pubtypes

typedef struct DW_PubStringsBucket
{
  struct DW_PubStringsBucket *next;
  String8                     string;
  U64                         info_off;
  U64                         cu_info_off;
} DW_PubStringsBucket;

typedef struct DW_PubStringsTable
{
  U64                   size;
  DW_PubStringsBucket **buckets;
} DW_PubStringsTable;

typedef struct DW_Reference
{
  DW_CompUnit *cu;
  U64          info_off;
} DW_Reference;

// hasher


// deserial helpers



// list units




// abbrev table


// form and tag


// attrib interp




// compile unit


// line info

internal U64 dw_read_line_file(String8      line_data,
                               U64          line_off,
                               DW_Input    *input,
                               DW_Version   unit_version,
                               DW_Format    unit_format,
                               DW_Ext       ext,
                               U64          address_size,
                               DW_ListUnit *str_offsets,
                               U64          enc_count,
                               U64         *enc_arr,
                               DW_LineFile *line_file_out);
internal U64 dw_read_line_vm_header(Arena           *arena,
                                    String8          line_data,
                                    U64              line_off,
                                    DW_Input        *input,
                                    String8          cu_dir,
                                    String8          cu_name,
                                    U8               cu_address_size,
                                    DW_ListUnit     *cu_str_offsets,
                                    DW_LineVMHeader *header_out);



// helper for .debug_pubtypes and .debug_pubnames 


#endif // DWARF_PARSE_H

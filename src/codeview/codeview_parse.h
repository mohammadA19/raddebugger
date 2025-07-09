// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ CodeView Common Parser Types

// CV_Numeric layout
// x: u16
// buf: u8[]
// case (x < 0x8000):  kind=u16 val=x
// case (x >= 0x8000): kind=x   val=buf



struct CV_NumericParsed
{
  CV_NumericKind  kind;
  u8             *val;
  u64             encoded_size;
};



struct CV_RecRange
{
  u32          off;
  CV_RecHeader hdr;
};

#define CV_REC_RANGE_CHUNK_SIZE 511



struct CV_RecRangeChunk
{
  struct CV_RecRangeChunk *next;
  CV_RecRange              ranges[CV_REC_RANGE_CHUNK_SIZE];
};



struct CV_RecRangeStream
{
  CV_RecRangeChunk *first_chunk;
  CV_RecRangeChunk *last_chunk;
  u64               total_count;
};



struct CV_RecRangeArray
{
  CV_RecRange *ranges;
  u64          count;
};

////////////////////////////////
//~ CodeView Sym Parser Types



struct CV_SymTopLevelInfo
{
  CV_Arch     arch;
  CV_Language language;
  String8     compiler_name;
};



struct CV_SymParsed
{
  // source information
  String8 data;
  u64     sym_align;
  
  // sym index derived from source
  CV_RecRangeArray sym_ranges;
  
  // top-level info derived from the syms
  CV_SymTopLevelInfo info;
};

////////////////////////////////
//~ CodeView Leaf Parser Types



struct CV_LeafParsed
{
  // source information
  String8   data;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  
  // leaf index derived from source
  CV_RecRangeArray leaf_ranges;
};

////////////////////////////////
//~ CodeView C13 Info Parser Types



struct CV_C13InlineSiteDecoder
{
  u64                cursor;
  u64                parent_voff;
  CV_InlineRangeKind range_kind;
  u32                code_length;
  u32                code_offset;
  u32                file_off;
  i32                ln;
  i32                cn;
  u64                code_offset_lo;
  b32                code_offset_changed;
  b32                code_offset_lo_changed;
  b32                code_length_changed;
  b32                ln_changed;
  b32                file_off_changed;
  Rng1U64            last_range;
  u32                file_count;
  Rng1U64            file_last_range;
  u64                file_line_count;
  u64                file_last_ln;
};

enum CV_C13InlineSiteDecoderStepFlags : u32
{
  CV_C13InlineSiteDecoderStepFlag_EmitRange       = (1 << 0),
  CV_C13InlineSiteDecoderStepFlag_ExtendLastRange = (1 << 1),
  CV_C13InlineSiteDecoderStepFlag_EmitFile        = (1 << 2),
  CV_C13InlineSiteDecoderStepFlag_EmitLine        = (1 << 3),
};



struct CV_C13InlineSiteDecoderStep
{
  CV_C13InlineSiteDecoderStepFlags flags;
  Rng1U64                     range;
  u64                         line_voff;
  u64                         line_voff_end;
  u64                         ln;
  u64                         cn;
  u32                         file_off;
};



struct CV_C13LinesParsed
{
  // raw info
  u32 sec_idx;
  u32 file_off;
  u64 secrel_base_off;
  
  // parsed info
  String8  file_name;
  u64     *voffs;     // [line_count + 1]
  u32     *line_nums; // [line_count]
  u16     *col_nums;  // [2*line_count]
  u32      line_count;
};



struct CV_C13LinesParsedNode
{
  CV_C13LinesParsedNode *next;
  CV_C13LinesParsed      v;
};



struct CV_C13InlineeLinesParsed
{
  CV_ItemId  inlinee;
  u32        file_off;
  String8    file_name;
  u32        first_source_ln;
  u32        extra_file_count;
  u32       *extra_files;
};



struct CV_C13InlineeLinesParsedNode
{
  CV_C13InlineeLinesParsedNode *next;
  CV_C13InlineeLinesParsedNode *hash_next;
  CV_C13InlineeLinesParsed      v;
};



struct CV_C13SubSectionNode
{
  struct CV_C13SubSectionNode  *next;
  CV_C13SubSectionKind          kind;
  u32                           off;
  u32                           size;
  CV_C13LinesParsedNode        *lines_first;
  CV_C13LinesParsedNode        *lines_last;
  CV_C13InlineeLinesParsedNode *inlinee_lines_first;
  CV_C13InlineeLinesParsedNode *inlinee_lines_last;
};



struct CV_C13Parsed
{
  // rjf: source data
  String8 data;
  
  // rjf: full sub-section list
  CV_C13SubSectionNode *first_sub_section;
  CV_C13SubSectionNode *last_sub_section;
  u64                   sub_section_count;
  
  // rjf: fastpath to file checksums section
  CV_C13SubSectionNode *file_chksms_sub_section;
  
  // rjf: fastpath to map inlinee CV_ItemId -> CV_InlineeLinesParsed quickly
  CV_C13InlineeLinesParsedNode **inlinee_lines_parsed_slots;
  u64                            inlinee_lines_parsed_slots_count;
};



struct CV_UDTInfo
{
  String8      name;
  String8      unique_name;
  CV_TypeProps props;
};

////////////////////////////////
//~ CodeView Compound Types



struct CV_TypeIdArray
{
  CV_TypeId *itypes;
  u64        count;
};

////////////////////////////////

//- Hasher

//- Numeric Decoder

//- Inlinee Lines Binary Annot Decoder

//- Symbol/Leaf Helpers

//- rjf: record range stream parsing

//- rjf: sym stream parsing

//- rjf: leaf stream parsing




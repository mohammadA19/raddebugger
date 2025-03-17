// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef CODEVIEW_PARSE_H
#define CODEVIEW_PARSE_H

////////////////////////////////
//~ CodeView Common Parser Types

// CV_Numeric layout
// x: uint16
// buf: uint8[]
// case (x < 0x8000):  kind=uint16 val=x
// case (x >= 0x8000): kind=x   val=buf

struct CV_NumericParsed
{
  CV_NumericKind  kind;
  uint8*             val;
  uint64             encoded_size;
}

struct CV_RecRange
{
  uint32          off;
  CV_RecHeader hdr;
}

#define CV_REC_RANGE_CHUNK_SIZE 511

struct CV_RecRangeChunk
{
  struct CV_RecRangeChunk* next;
  CV_RecRange              ranges[CV_REC_RANGE_CHUNK_SIZE];
}

struct CV_RecRangeStream
{
  CV_RecRangeChunk* first_chunk;
  CV_RecRangeChunk* last_chunk;
  uint64               total_count;
}

struct CV_RecRangeArray
{
  CV_RecRange* ranges;
  uint64          count;
}

////////////////////////////////
//~ CodeView Sym Parser Types

struct CV_SymTopLevelInfo
{
  CV_Arch     arch;
  CV_Language language;
  StringView     compiler_name;
}

struct CV_SymParsed
{
  // source information
  StringView data;
  uint64     sym_align;
  
  // sym index derived from source
  CV_RecRangeArray sym_ranges;
  
  // top-level info derived from the syms
  CV_SymTopLevelInfo info;
}

////////////////////////////////
//~ CodeView Leaf Parser Types

struct CV_LeafParsed
{
  // source information
  StringView   data;
  CV_TypeId itype_first;
  CV_TypeId itype_opl;
  
  // leaf index derived from source
  CV_RecRangeArray leaf_ranges;
}

////////////////////////////////
//~ CodeView C13 Info Parser Types

struct CV_C13InlineSiteDecoder
{
  uint64                cursor;
  uint64                parent_voff;
  CV_InlineRangeKind range_kind;
  uint32                code_length;
  uint32                code_offset;
  uint32                file_off;
  int32                ln;
  int32                cn;
  uint64                code_offset_lo;
  B32                code_offset_changed;
  B32                code_offset_lo_changed;
  B32                code_length_changed;
  B32                ln_changed;
  B32                file_off_changed;
  Rng1U64            last_range;
  uint32                file_count;
  Rng1U64            file_last_range;
  uint64                file_line_count;
  uint64                file_last_ln;
}

enum CV_C13InlineSiteDecoderStepFlags : uint32
{
  CV_C13InlineSiteDecoderStepFlag_EmitRange       = (1 << 0),
  CV_C13InlineSiteDecoderStepFlag_ExtendLastRange = (1 << 1),
  CV_C13InlineSiteDecoderStepFlag_EmitFile        = (1 << 2),
  CV_C13InlineSiteDecoderStepFlag_EmitLine        = (1 << 3),
}

struct CV_C13InlineSiteDecoderStep
{
  CV_C13InlineSiteDecoderStepFlags flags;
  Rng1U64                     range;
  uint64                         line_voff;
  uint64                         line_voff_end;
  uint64                         ln;
  uint64                         cn;
  uint32                         file_off;
}

struct CV_C13LinesParsed
{
  // raw info
  uint32 sec_idx;
  uint32 file_off;
  uint64 secrel_base_off;
  
  // parsed info
  StringView  file_name;
  uint64*     voffs;     // [line_count + 1]
  uint32*     line_nums; // [line_count]
  uint16*     col_nums;  // [2*line_count]
  uint32      line_count;
}

struct CV_C13LinesParsedNode
{
  CV_C13LinesParsedNode* next;
  CV_C13LinesParsed      v;
}

struct CV_C13InlineeLinesParsed
{
  CV_ItemId  inlinee;
  uint32        file_off;
  StringView    file_name;
  uint32        first_source_ln;
  uint32        extra_file_count;
  uint32*       extra_files;
}

struct CV_C13InlineeLinesParsedNode
{
  CV_C13InlineeLinesParsedNode* next;
  CV_C13InlineeLinesParsedNode* hash_next;
  CV_C13InlineeLinesParsed      v;
}

struct CV_C13SubSectionNode
{
  struct CV_C13SubSectionNode*  next;
  CV_C13SubSectionKind          kind;
  uint32                           off;
  uint32                           size;
  CV_C13LinesParsedNode*        lines_first;
  CV_C13LinesParsedNode*        lines_last;
  CV_C13InlineeLinesParsedNode* inlinee_lines_first;
  CV_C13InlineeLinesParsedNode* inlinee_lines_last;
}

struct CV_C13Parsed
{
  // rjf: source data
  StringView data;
  
  // rjf: full sub-section list
  CV_C13SubSectionNode* first_sub_section;
  CV_C13SubSectionNode* last_sub_section;
  uint64                   sub_section_count;
  
  // rjf: fastpath to file checksums section
  CV_C13SubSectionNode* file_chksms_sub_section;
  
  // rjf: fastpath to map inlinee CV_ItemId . CV_InlineeLinesParsed quickly
  CV_C13InlineeLinesParsedNode** inlinee_lines_parsed_slots;
  uint64                            inlinee_lines_parsed_slots_count;
}

struct CV_UDTInfo
{
  StringView      name;
  StringView      unique_name;
  CV_TypeProps props;
}

////////////////////////////////
//~ CodeView Compound Types

struct CV_TypeIdArray
{
  CV_TypeId* itypes;
  uint64        count;
}

////////////////////////////////

//- Hasher

uint64 cv_hash_from_string(StringView str);
uint64 cv_hash_from_item_id(CV_ItemId item_id);

//- Numeric Decoder

CV_NumericParsed cv_numeric_from_data_range(uint8* first, uint8* opl);
uint64              cv_read_numeric(StringView data, uint64 offset, CV_NumericParsed* out);

B32 cv_numeric_fits_in_u64(CV_NumericParsed* num);
B32 cv_numeric_fits_in_s64(CV_NumericParsed* num);
B32 cv_numeric_fits_in_f64(CV_NumericParsed* num);

uint64 cv_u64_from_numeric(CV_NumericParsed* num);
int64 cv_s64_from_numeric(CV_NumericParsed* num);
double cv_f64_from_numeric(CV_NumericParsed* num);

//- Inlinee Lines Binary Annot Decoder

uint64 cv_decode_inline_annot_u32(StringView data, uint64 offset, uint32* out_value);
uint64 cv_decode_inline_annot_s32(StringView data, uint64 offset, int32* out_value);
int32 cv_inline_annot_signed_from_unsigned_operand(uint32 value);

CV_C13InlineSiteDecoder      cv_c13_inline_site_decoder_init(uint32 file_off, uint32 first_source_ln, uint32 parent_voff);
CV_C13InlineSiteDecoderStep cv_c13_inline_site_decoder_step(CV_C13InlineSiteDecoder* decoder, StringView binary_annots);

//- Symbol/Leaf Helpers

B32 cv_is_udt_name_anon(StringView name);

B32 cv_is_global_symbol(CV_SymKind kind);
B32 cv_is_typedef(CV_SymKind kind);
B32 cv_is_scope_symbol(CV_SymKind kind);
B32 cv_is_end_symbol(CV_SymKind kind);

B32                cv_is_udt(CV_LeafKind kind);
B32                cv_is_leaf_type_server(CV_LeafKind kind);
B32                cv_is_leaf_pch(CV_LeafKind kind);
CV_TypeIndexSource cv_type_index_source_from_leaf_kind(CV_LeafKind leaf_kind);

CV_TypeIndexInfoList cv_get_symbol_type_index_offsets(Arena* arena, CV_SymKind kind, StringView data);
CV_TypeIndexInfoList cv_get_leaf_type_index_offsets(Arena* arena, CV_LeafKind leaf_kind, StringView data);
CV_TypeIndexInfoList cv_get_inlinee_type_index_offsets(Arena* arena, StringView raw_data);
Span<StringView>         cv_get_data_around_type_indices(Arena* arena, CV_TypeIndexInfoList ti_list, StringView data);

uint64     cv_name_offset_from_symbol(CV_SymKind kind, StringView data);
StringView cv_name_from_symbol(CV_SymKind kind, StringView data);

CV_UDTInfo cv_get_udt_info(CV_LeafKind kind, StringView data);
StringView    cv_name_from_udt_info(CV_UDTInfo udt_info);

//- rjf: record range stream parsing
CV_RecRangeStream * cv_rec_range_stream_from_data(Arena* arena, StringView data, uint64 align);
CV_RecRangeArray    cv_rec_range_array_from_stream(Arena* arena, CV_RecRangeStream* stream);

//- rjf: sym stream parsing
CV_SymParsed * cv_sym_from_data(Arena* arena, StringView sym_data, uint64 sym_align);

//- rjf: leaf stream parsing
CV_LeafParsed * cv_leaf_from_data(Arena* arena, StringView leaf_data, CV_TypeId first);
CV_C13Parsed  * cv_c13_parsed_from_data(Arena* arena, StringView c13_data, StringView strtbl, COFF_SectionHeaderArray sections);

#endif // CODEVIEW_PARSE_H


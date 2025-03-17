// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADDBG_WIDGETS_H
#define RADDBG_WIDGETS_H

////////////////////////////////
//~ rjf: Line Edit Types

enum RD_LineEditFlags : uint32
{
  RD_LineEditFlag_Expander            = (1<<0),
  RD_LineEditFlag_ExpanderSpace       = (1<<1),
  RD_LineEditFlag_ExpanderPlaceholder = (1<<2),
  RD_LineEditFlag_DisableEdit         = (1<<3),
  RD_LineEditFlag_CodeContents        = (1<<4),
  RD_LineEditFlag_KeyboardClickable   = (1<<5),
  RD_LineEditFlag_Border              = (1<<6),
  RD_LineEditFlag_NoBackground        = (1<<7),
  RD_LineEditFlag_PreferDisplayString = (1<<8),
  RD_LineEditFlag_DisplayStringIsCode = (1<<9),
}

////////////////////////////////
//~ rjf: Code Slice Types

enum RD_CodeSliceFlags : uint32
{
  RD_CodeSliceFlag_Clickable         = (1<<0),
  RD_CodeSliceFlag_PriorityMargin    = (1<<1),
  RD_CodeSliceFlag_CatchallMargin    = (1<<2),
  RD_CodeSliceFlag_LineNums          = (1<<3),
}

struct RD_CodeSliceParams
{
  // rjf: content
  RD_CodeSliceFlags flags;
  Rng1S64 line_num_range;
  StringView* line_text;
  Rng1U64* line_ranges;
  TXT_TokenArray* line_tokens;
  RD_EntityList* line_bps;
  CTRL_EntityList* line_ips;
  RD_EntityList* line_pins;
  uint64* line_vaddrs;
  D_LineList* line_infos;
  DI_KeyList relevant_dbgi_keys;
  
  // rjf: visual parameters
  FNT_Tag font;
  float font_size;
  float tab_size;
  StringView search_query;
  float line_height_px;
  float priority_margin_width_px;
  float catchall_margin_width_px;
  float line_num_width_px;
  float line_text_max_width_px;
  float margin_float_off_px;
}

struct RD_CodeSliceSignal
{
  UI_Signal base;
  TxtPt mouse_pt;
  TxtRng mouse_expr_rng;
}

////////////////////////////////
//~ rjf: UI Building Helpers

#define RD_Palette(code) UI_Palette(rd_palette_from_code(code))
#define RD_Font(slot) UI_Font(rd_font_from_slot(slot)) UI_TextRasterFlags(rd_raster_flags_from_slot((slot)))

////////////////////////////////
//~ rjf: UI Widgets: Loading Overlay

void rd_loading_overlay(Rng2F32 rect, float loading_t, uint64 progress_v, uint64 progress_v_target);

////////////////////////////////
//~ rjf: UI Widgets: Fancy Buttons

void rd_cmd_binding_buttons(StringView name);
UI_Signal rd_menu_bar_button(StringView string);
UI_Signal rd_cmd_spec_button(StringView name);
void rd_cmd_list_menu_buttons(uint64 count, StringView* cmd_names, uint32* fastpath_codepoints);
UI_Signal rd_icon_button(RD_IconKind kind, FuzzyMatchRangeList* matches, StringView string);
UI_Signal rd_icon_buttonf(RD_IconKind kind, FuzzyMatchRangeList* matches, char* fmt, ...);

////////////////////////////////
//~ rjf: UI Widgets: Text View

UI_BOX_CUSTOM_DRAW(rd_thread_box_draw_extensions);
UI_BOX_CUSTOM_DRAW(rd_bp_box_draw_extensions);
RD_CodeSliceSignal rd_code_slice(RD_CodeSliceParams* params, TxtPt* cursor, TxtPt* mark, int64* preferred_column, StringView string);
RD_CodeSliceSignal rd_code_slicef(RD_CodeSliceParams* params, TxtPt* cursor, TxtPt* mark, int64* preferred_column, char* fmt, ...);

B32 rd_do_txt_controls(TXT_TextInfo* info, StringView data, uint64 line_count_per_page, TxtPt* cursor, TxtPt* mark, int64* preferred_column);

////////////////////////////////
//~ rjf: UI Widgets: Fancy Labels

UI_Signal rd_label(StringView string);
UI_Signal rd_error_label(StringView string);
B32 rd_help_label(StringView string);
DR_FancyStringList rd_fancy_string_list_from_code_string(Arena* arena, float alpha, B32 indirection_size_change, Vec4F32 base_color, StringView string);
UI_Box* rd_code_label(float alpha, B32 indirection_size_change, Vec4F32 base_color, StringView string);

////////////////////////////////
//~ rjf: UI Widgets: Line Edit

UI_Signal rd_line_edit(RD_LineEditFlags flags, int32 depth, FuzzyMatchRangeList* matches, TxtPt* cursor, TxtPt* mark, uint8* edit_buffer, uint64 edit_buffer_size, uint64* edit_string_size_out, B32* expanded_out, StringView pre_edit_value, StringView string);
UI_Signal rd_line_editf(RD_LineEditFlags flags, int32 depth, FuzzyMatchRangeList* matches, TxtPt* cursor, TxtPt* mark, uint8* edit_buffer, uint64 edit_buffer_size, uint64* edit_string_size_out, B32* expanded_out, StringView pre_edit_value, char* fmt, ...);

#endif // RADDBG_WIDGETS_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADDBG_WIDGETS_H
#define RADDBG_WIDGETS_H

////////////////////////////////
//~ rjf: Line Edit Types

enum RD_LineEditFlags : uint
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
};

////////////////////////////////
//~ rjf: Code Slice Types

enum RD_CodeSliceFlags : uint
{
  RD_CodeSliceFlag_Clickable         = (1<<0),
  RD_CodeSliceFlag_PriorityMargin    = (1<<1),
  RD_CodeSliceFlag_CatchallMargin    = (1<<2),
  RD_CodeSliceFlag_LineNums          = (1<<3),
};

struct RD_CodeSliceParams
{
  // rjf: content
  RD_CodeSliceFlags flags;
  Rng1S64 line_num_range;
  String8 *line_text;
  Rng1U64 *line_ranges;
  TXT_TokenArray *line_tokens;
  RD_EntityList *line_bps;
  CTRL_EntityList *line_ips;
  RD_EntityList *line_pins;
  ulong *line_vaddrs;
  D_LineList *line_infos;
  DI_KeyList relevant_dbgi_keys;
  
  // rjf: visual parameters
  FNT_Tag font;
  F32 font_size;
  F32 tab_size;
  String8 search_query;
  F32 line_height_px;
  F32 priority_margin_width_px;
  F32 catchall_margin_width_px;
  F32 line_num_width_px;
  F32 line_text_max_width_px;
  F32 margin_float_off_px;
};

struct RD_CodeSliceSignal
{
  UI_Signal base;
  TxtPt mouse_pt;
  TxtRng mouse_expr_rng;
};

////////////////////////////////
//~ rjf: UI Building Helpers

#define RD_Palette(code) UI_Palette(rd_palette_from_code(code))
#define RD_Font(slot) UI_Font(rd_font_from_slot(slot)) UI_TextRasterFlags(rd_raster_flags_from_slot((slot)))

////////////////////////////////
//~ rjf: UI Widgets: Loading Overlay

void rd_loading_overlay(Rng2F32 rect, F32 loading_t, ulong progress_v, ulong progress_v_target);

////////////////////////////////
//~ rjf: UI Widgets: Fancy Buttons

void rd_cmd_binding_buttons(String8 name);
UI_Signal rd_menu_bar_button(String8 string);
UI_Signal rd_cmd_spec_button(String8 name);
void rd_cmd_list_menu_buttons(ulong count, String8 *cmd_names, uint *fastpath_codepoints);
UI_Signal rd_icon_button(RD_IconKind kind, FuzzyMatchRangeList *matches, String8 string);
UI_Signal rd_icon_buttonf(RD_IconKind kind, FuzzyMatchRangeList *matches, char *fmt, ...);

////////////////////////////////
//~ rjf: UI Widgets: Text View

UI_BOX_CUSTOM_DRAW(rd_thread_box_draw_extensions);
UI_BOX_CUSTOM_DRAW(rd_bp_box_draw_extensions);
RD_CodeSliceSignal rd_code_slice(RD_CodeSliceParams *params, TxtPt *cursor, TxtPt *mark, long *preferred_column, String8 string);
RD_CodeSliceSignal rd_code_slicef(RD_CodeSliceParams *params, TxtPt *cursor, TxtPt *mark, long *preferred_column, char *fmt, ...);

B32 rd_do_txt_controls(TXT_TextInfo *info, String8 data, ulong line_count_per_page, TxtPt *cursor, TxtPt *mark, long *preferred_column);

////////////////////////////////
//~ rjf: UI Widgets: Fancy Labels

UI_Signal rd_label(String8 string);
UI_Signal rd_error_label(String8 string);
B32 rd_help_label(String8 string);
DR_FancyStringList rd_fancy_string_list_from_code_string(Arena *arena, F32 alpha, B32 indirection_size_change, Vec4F32 base_color, String8 string);
UI_Box *rd_code_label(F32 alpha, B32 indirection_size_change, Vec4F32 base_color, String8 string);

////////////////////////////////
//~ rjf: UI Widgets: Line Edit

UI_Signal rd_line_edit(RD_LineEditFlags flags, int depth, FuzzyMatchRangeList *matches, TxtPt *cursor, TxtPt *mark, byte *edit_buffer, ulong edit_buffer_size, ulong *edit_string_size_out, B32 *expanded_out, String8 pre_edit_value, String8 string);
UI_Signal rd_line_editf(RD_LineEditFlags flags, int depth, FuzzyMatchRangeList *matches, TxtPt *cursor, TxtPt *mark, byte *edit_buffer, ulong edit_buffer_size, ulong *edit_string_size_out, B32 *expanded_out, String8 pre_edit_value, char *fmt, ...);

#endif // RADDBG_WIDGETS_H

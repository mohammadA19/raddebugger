// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef UI_BASIC_WIDGETS_H
#define UI_BASIC_WIDGETS_H

////////////////////////////////
//~ rjf: Scroll List Types

enum UI_ScrollListFlags : uint32
{
  UI_ScrollListFlag_Nav  = (1<<0),
  UI_ScrollListFlag_Snap = (1<<1),
  UI_ScrollListFlag_All  = 0xffffffff,
};

struct UI_ScrollListRowBlock
{
  uint64 row_count;
  uint64 item_count;
};

struct UI_ScrollListRowBlockChunkNode
{
  UI_ScrollListRowBlockChunkNode *next;
  UI_ScrollListRowBlock *v;
  uint64 count;
  uint64 cap;
};

struct UI_ScrollListRowBlockChunkList
{
  UI_ScrollListRowBlockChunkNode *first;
  UI_ScrollListRowBlockChunkNode *last;
  uint64 chunk_count;
  uint64 total_count;
};

struct UI_ScrollListRowBlockArray
{
  UI_ScrollListRowBlock *v;
  uint64 count;
};

struct UI_ScrollListParams
{
  UI_ScrollListFlags flags;
  Vec2float dim_px;
  float row_height_px;
  UI_ScrollListRowBlockArray row_blocks;
  Rng2uint64 cursor_range;
  Rng1uint64 item_range;
  B32 cursor_min_is_empty_selection[Axis2_COUNT];
};

struct UI_ScrollListSignal
{
  B32 cursor_moved;
};

////////////////////////////////
//~ rjf: Basic Widgets

internal void ui_divider(UI_Size size);
internal UI_Signal ui_label(String8 string);
internal UI_Signal ui_labelf(char *fmt, ...);
internal void ui_label_multiline(float max, String8 string);
internal void ui_label_multilinef(float max, char *fmt, ...);
internal UI_Signal ui_button(String8 string);
internal UI_Signal ui_buttonf(char *fmt, ...);
internal UI_Signal ui_hover_label(String8 string);
internal UI_Signal ui_hover_labelf(char *fmt, ...);
internal UI_Signal ui_line_edit(TxtPt *cursor, TxtPt *mark, uint8 *edit_buffer, uint64 edit_buffer_size, uint64 *edit_string_size_out, String8 pre_edit_value, String8 string);
internal UI_Signal ui_line_editf(TxtPt *cursor, TxtPt *mark, uint8 *edit_buffer, uint64 edit_buffer_size, uint64 *edit_string_size_out, String8 pre_edit_value, char *fmt, ...);

////////////////////////////////
//~ rjf: Images

internal UI_Signal ui_image(R_Handle texture, R_Tex2DSampleKind sample_kind, Rng2float region, Vec4float tint, float blur, String8 string);
internal UI_Signal ui_imagef(R_Handle texture, R_Tex2DSampleKind sample_kind, Rng2float region, Vec4float tint, float blur, char *fmt, ...);

////////////////////////////////
//~ rjf: Special Buttons

internal UI_Signal ui_expander(B32 is_expanded, String8 string);
internal UI_Signal ui_expanderf(B32 is_expanded, char *fmt, ...);
internal UI_Signal ui_sort_header(B32 sorting, B32 ascending, String8 string);
internal UI_Signal ui_sort_headerf(B32 sorting, B32 ascending, char *fmt, ...);

////////////////////////////////
//~ rjf: Color Pickers

//- rjf: tooltips
internal void ui_do_color_tooltip_hsv(Vec3float hsv);
internal void ui_do_color_tooltip_hsva(Vec4float hsva);

//- rjf: saturation/value picker
internal UI_Signal ui_sat_val_picker(float hue, float *out_sat, float *out_val, String8 string);
internal UI_Signal ui_sat_val_pickerf(float hue, float *out_sat, float *out_val, char *fmt, ...);

//- rjf: hue picker
internal UI_Signal ui_hue_picker(float *out_hue, float sat, float val, String8 string);
internal UI_Signal ui_hue_pickerf(float *out_hue, float sat, float val, char *fmt, ...);

//- rjf: alpha picker
internal UI_Signal ui_alpha_picker(float *out_alpha, String8 string);
internal UI_Signal ui_alpha_pickerf(float *out_alpha, char *fmt, ...);

////////////////////////////////
//~ rjf: Simple Layout Widgets

internal UI_Box *ui_row_begin(void);
internal UI_Signal ui_row_end(void);
internal UI_Box *ui_column_begin(void);
internal UI_Signal ui_column_end(void);
internal UI_Box *ui_named_row_begin(String8 string);
internal UI_Signal ui_named_row_end(void);
internal UI_Box *ui_named_column_begin(String8 string);
internal UI_Signal ui_named_column_end(void);

////////////////////////////////
//~ rjf: Floating Panes

internal UI_Box *ui_pane_begin(Rng2float rect, String8 string);
internal UI_Box *ui_pane_beginf(Rng2float rect, char *fmt, ...);
internal UI_Signal ui_pane_end(void);

////////////////////////////////
//~ rjf: Tables

internal void ui_table_begin(uint64 column_pct_count, float **column_pcts, String8 string);
internal void ui_table_beginf(uint64 column_pct_count, float **column_pcts, char *fmt, ...);
internal void ui_table_end(void);
internal UI_Box *  ui_named_table_vector_begin(String8 string);
internal UI_Box *  ui_named_table_vector_beginf(char *fmt, ...);
internal UI_Box *  ui_table_vector_begin(void);
internal UI_Signal ui_table_vector_end(void);
internal UI_Box *  ui_table_cell_begin(void);
internal UI_Signal ui_table_cell_end(void);
internal UI_Box *  ui_table_cell_sized_begin(UI_Size size);

////////////////////////////////
//~ rjf: Scroll Regions

internal void ui_scroll_list_row_block_chunk_list_push(Arena *arena, UI_ScrollListRowBlockChunkList *list, uint64 cap, UI_ScrollListRowBlock *block);
internal UI_ScrollListRowBlockArray ui_scroll_list_row_block_array_from_chunk_list(Arena *arena, UI_ScrollListRowBlockChunkList *list);
internal uint64 ui_scroll_list_row_from_item(UI_ScrollListRowBlockArray *blocks, uint64 item);
internal uint64 ui_scroll_list_item_from_row(UI_ScrollListRowBlockArray *blocks, uint64 row);

internal UI_ScrollPt ui_scroll_bar(Axis2 axis, UI_Size off_axis_size, UI_ScrollPt pt, Rng1uint64 idx_range, uint64 view_num_indices);
internal void ui_scroll_list_begin(UI_ScrollListParams *params, UI_ScrollPt *scroll_pt_out, Vec2uint64 *cursor_out, Vec2uint64 *mark_out, Rng1uint64 *visible_row_range_out, UI_ScrollListSignal *signal_out);
internal void ui_scroll_list_end(void);

////////////////////////////////
//~ rjf: Macro Loop Wrappers

#define UI_Row DeferLoop(ui_row_begin(), ui_row_end())
#define UI_Column DeferLoop(ui_column_begin(), ui_column_end())
#define UI_NamedRow(s) DeferLoop(ui_named_row_begin(s), ui_named_row_end())
#define UI_NamedColumn(s) DeferLoop(ui_named_column_begin(s), ui_named_column_end())
#define UI_Pane(r, s) DeferLoop(ui_pane_begin(r, s), ui_pane_end())
#define UI_PaneF(r, ...) DeferLoop(ui_pane_beginf(r, __VA_ARGS__), ui_pane_end())
#define UI_Padding(size) DeferLoop(ui_spacer(size), ui_spacer(size))
#define UI_Center UI_Padding(ui_pct(1, 0))

#define UI_Table(col_pct_count, col_pcts, s) DeferLoop(ui_table_begin(col_pct_count, col_pcts, s), ui_table_end())
#define UI_TableF(col_pct_count, col_pcts, ...) DeferLoop(ui_table_beginf(col_pct_count, col_pcts, __VA_ARGS__), ui_table_end())
#define UI_NamedTableVector(s) DeferLoop(ui_named_table_vector_begin(s), ui_table_vector_end())
#define UI_NamedTableVectorF(...) DeferLoop(ui_named_table_vector_beginf(__VA_ARGS__), ui_table_vector_end())
#define UI_TableVector DeferLoop(ui_table_vector_begin(), ui_table_vector_end())
#define UI_TableCell DeferLoop(ui_table_cell_begin(), ui_table_cell_end())
#define UI_TableCellSized(size) DeferLoop(ui_table_cell_sized_begin(size), ui_table_cell_end())

#define UI_ScrollList(params, scroll_pt_out, cursor_out, mark_out, visible_row_range_out, signal_out) DeferLoop(ui_scroll_list_begin((params), (scroll_pt_out), (cursor_out), (mark_out), (visible_row_range_out), (signal_out)), ui_scroll_list_end())

#endif // UI_BASIC_WIDGETS_H

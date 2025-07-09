// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Scroll List Types

typedef U32 UI_ScrollListFlags;
enum
{
  UI_ScrollListFlag_Nav  = (1<<0),
  UI_ScrollListFlag_Snap = (1<<1),
  UI_ScrollListFlag_All  = 0xffffffff,
};



struct UI_ScrollListRowBlock
{
  U64 row_count;
  U64 item_count;
};



struct UI_ScrollListRowBlockChunkNode
{
  UI_ScrollListRowBlockChunkNode *next;
  UI_ScrollListRowBlock *v;
  U64 count;
  U64 cap;
};



struct UI_ScrollListRowBlockChunkList
{
  UI_ScrollListRowBlockChunkNode *first;
  UI_ScrollListRowBlockChunkNode *last;
  U64 chunk_count;
  U64 total_count;
};



struct UI_ScrollListRowBlockArray
{
  UI_ScrollListRowBlock *v;
  U64 count;
};



struct UI_ScrollListParams
{
  UI_ScrollListFlags flags;
  Vec2F32 dim_px;
  F32 row_height_px;
  UI_ScrollListRowBlockArray row_blocks;
  Rng2S64 cursor_range;
  Rng1S64 item_range;
  B32 cursor_min_is_empty_selection[Axis2_COUNT];
};



struct UI_ScrollListSignal
{
  B32 cursor_moved;
};

////////////////////////////////
//~ rjf: Basic Widgets













////////////////////////////////
//~ rjf: Images




////////////////////////////////
//~ rjf: Special Buttons






////////////////////////////////
//~ rjf: Color Pickers

//- rjf: tooltips



//- rjf: saturation/value picker



//- rjf: hue picker



//- rjf: alpha picker



////////////////////////////////
//~ rjf: Simple Layout Widgets










////////////////////////////////
//~ rjf: Floating Panes





////////////////////////////////
//~ rjf: Tables












////////////////////////////////
//~ rjf: Scroll Regions










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



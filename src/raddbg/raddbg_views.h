// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADDBG_VIEWS_H
#define RADDBG_VIEWS_H

////////////////////////////////
//~ rjf: Code View Types

enum RD_CodeViewBuildFlags : uint
{
  RD_CodeViewBuildFlag_Margins = (1<<0),
  RD_CodeViewBuildFlag_All     = 0xffffffff,
};

struct RD_CodeViewState
{
  // rjf: stable state
  B32 initialized;
  long preferred_column;
  B32 drifted_for_search;
  
  // rjf: per-frame command info
  long goto_line_num;
  B32 center_cursor;
  B32 contain_cursor;
  B32 watch_expr_at_mouse;
  Arena *find_text_arena;
  String8 find_text_fwd;
  String8 find_text_bwd;
};

struct RD_CodeViewBuildResult
{
  DI_KeyList dbgi_keys;
};

////////////////////////////////
//~ rjf: Watch View Types

enum RD_WatchViewFlags : uint
{
  RD_WatchViewFlag_NoHeader                   = (1<<0),
  RD_WatchViewFlag_PrettyNameMembers          = (1<<1),
  RD_WatchViewFlag_PrettyEntityRows           = (1<<2),
  RD_WatchViewFlag_DisableCacheLines          = (1<<3),
};

enum RD_WatchViewColumnKind
{
  RD_WatchViewColumnKind_Expr,
  RD_WatchViewColumnKind_Value,
  RD_WatchViewColumnKind_Type,
  RD_WatchViewColumnKind_ViewRule,
  RD_WatchViewColumnKind_Member,
  RD_WatchViewColumnKind_CallStackFrame,
  RD_WatchViewColumnKind_CallStackFrameSelection,
  RD_WatchViewColumnKind_Module,
  RD_WatchViewColumnKind_COUNT
};

struct RD_WatchViewColumnParams
{
  String8 string;
  String8 display_string;
  String8 view_rule;
  B32 is_non_code;
  B32 dequote_string;
  B32 rangify_braces;
};

struct RD_WatchViewColumn
{
  RD_WatchViewColumn *next;
  RD_WatchViewColumn *prev;
  RD_WatchViewColumnKind kind;
  F32 pct;
  byte string_buffer[1024];
  ulong string_size;
  byte display_string_buffer[1024];
  ulong display_string_size;
  byte view_rule_buffer[1024];
  ulong view_rule_size;
  B32 is_non_code;
  B32 dequote_string;
  B32 rangify_braces;
};

struct RD_WatchViewRowCtrl
{
  RD_EntityKind entity_kind;
  CTRL_EntityKind ctrl_entity_kind;
  RD_CmdKind kind;
};

enum RD_WatchViewRowKind
{
  RD_WatchViewRowKind_Normal,
  RD_WatchViewRowKind_Header,
  RD_WatchViewRowKind_Canvas,
  RD_WatchViewRowKind_PrettyEntityControls,
};

struct RD_WatchViewPoint
{
  long x;
  EV_Key parent_key;
  EV_Key key;
};

struct RD_WatchViewRowInfo
{
  RD_EntityKind collection_entity_kind;
  RD_Entity *collection_entity;
  CTRL_EntityKind collection_ctrl_entity_kind;
  CTRL_Entity *collection_ctrl_entity;
  CTRL_Entity *callstack_thread;
  ulong callstack_unwind_index;
  ulong callstack_inline_depth;
};

struct RD_WatchViewTextEditState
{
  RD_WatchViewTextEditState *pt_hash_next;
  RD_WatchViewPoint pt;
  TxtPt cursor;
  TxtPt mark;
  byte input_buffer[1024];
  ulong input_size;
  byte initial_buffer[1024];
  ulong initial_size;
};

struct RD_WatchViewState
{
  B32 initialized;
  
  // rjf: column state
  Arena *column_arena;
  RD_WatchViewColumn *first_column;
  RD_WatchViewColumn *last_column;
  RD_WatchViewColumn *free_column;
  ulong column_count;
  
  // rjf; table cursor state
  RD_WatchViewPoint cursor;
  RD_WatchViewPoint mark;
  RD_WatchViewPoint next_cursor;
  RD_WatchViewPoint next_mark;
  
  // rjf: text input state
  Arena *text_edit_arena;
  ulong text_edit_state_slots_count;
  RD_WatchViewTextEditState dummy_text_edit_state;
  RD_WatchViewTextEditState **text_edit_state_slots;
  B32 text_editing;
};

////////////////////////////////
//~ rjf: Code View Functions

void rd_code_view_init(RD_CodeViewState *cv);
RD_CodeViewBuildResult rd_code_view_build(Arena *arena, RD_CodeViewState *cv, RD_CodeViewBuildFlags flags, Rng2F32 rect, String8 text_data, TXT_TextInfo *text_info, DASM_LineArray *dasm_lines, Rng1U64 dasm_vaddr_range, DI_Key dasm_dbgi_key);

////////////////////////////////
//~ rjf: Watch View Functions

//- rjf: index -> column
RD_WatchViewColumn *rd_watch_view_column_from_x(RD_WatchViewState *wv, long index);

//- rjf: watch view points <-> table coordinates
B32 rd_watch_view_point_match(RD_WatchViewPoint a, RD_WatchViewPoint b);
RD_WatchViewPoint rd_watch_view_point_from_tbl(EV_BlockRangeList *block_ranges, Vec2S64 tbl);
Vec2S64 rd_tbl_from_watch_view_point(EV_BlockRangeList *block_ranges, RD_WatchViewPoint pt);

//- rjf: row -> context info
RD_WatchViewRowInfo rd_watch_view_row_info_from_row(EV_Row *row);

//- rjf: watch view flags & row & row info -> row kind
RD_WatchViewRowKind rd_watch_view_row_kind_from_flags_row_info(RD_WatchViewFlags flags, EV_Row *row, RD_WatchViewRowInfo *info);

//- rjf: row/column -> exprs / strings
E_Expr *rd_expr_from_watch_view_row_column(Arena *arena, EV_View *ev_view, EV_Row *row, RD_WatchViewColumn *col);
String8 rd_string_from_eval_viz_row_column(Arena *arena, EV_View *ev, EV_Row *row, RD_WatchViewColumn *col, EV_StringFlags string_flags, uint default_radix, FNT_Tag font, F32 font_size, F32 max_size_px);

//- rjf: table coordinates -> text edit state
RD_WatchViewTextEditState *rd_watch_view_text_edit_state_from_pt(RD_WatchViewState *wv, RD_WatchViewPoint pt);

//- rjf: watch view column state mutation
RD_WatchViewColumn *rd_watch_view_column_alloc_(RD_WatchViewState *wv, RD_WatchViewColumnKind kind, F32 pct, RD_WatchViewColumnParams *params);
#define rd_watch_view_column_alloc(wv, kind, pct, ...) rd_watch_view_column_alloc_((wv), (kind), (pct), &(RD_WatchViewColumnParams){.string = str8_zero(), __VA_ARGS__})
void rd_watch_view_column_release(RD_WatchViewState *wv, RD_WatchViewColumn *col);

//- rjf: watch view main hooks
void rd_watch_view_init(RD_WatchViewState *ewv);
void rd_watch_view_build(RD_WatchViewState *ewv, RD_WatchViewFlags flags, String8 root_expr, String8 root_view_rule, B32 modifiable, uint default_radix, Rng2F32 rect);

#endif // RADDBG_VIEWS_H

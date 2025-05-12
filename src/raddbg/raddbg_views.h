// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Code View Types

enum RD_CodeViewBuildFlags : U32
{
  RD_CodeViewBuildFlag_Margins     = (1<<0),
  RD_CodeViewBuildFlag_All         = 0xffffffff,
}

struct RD_CodeViewState
{
  // rjf: stable state
  B32 initialized;
  S64 preferred_column;
  B32 drifted_for_search;
  
  // rjf: per-frame command info
  S64 goto_line_num;
  B32 center_cursor;
  B32 contain_cursor;
  B32 watch_expr_at_mouse;
  Arena* find_text_arena;
  String8 find_text_fwd;
  String8 find_text_bwd;
}

struct RD_CodeViewBuildResult
{
  DI_KeyList dbgi_keys;
}

////////////////////////////////
//~ rjf: Watch View Types

enum RD_WatchCellKind
{
  RD_WatchCellKind_Eval,           // an evaluation cell
  RD_WatchCellKind_ViewUI,         // an arbitrary user interface, supplied by a hook
  RD_WatchCellKind_CallStackFrame, // a slot for a yellow arrow, to show call stack frame selection
}

enum RD_WatchCellFlags : U32
{
  RD_WatchCellFlag_Expr                    = (1<<0),
  RD_WatchCellFlag_NoEval                  = (1<<1),
  RD_WatchCellFlag_Button                  = (1<<2),
  RD_WatchCellFlag_Background              = (1<<3),
  RD_WatchCellFlag_ActivateWithSingleClick = (1<<4),
  RD_WatchCellFlag_IsNonCode               = (1<<5),
  RD_WatchCellFlag_CanEdit                 = (1<<6),
  RD_WatchCellFlag_IsErrored               = (1<<7),
  RD_WatchCellFlag_Indented                = (1<<8),
}

struct RD_WatchCell
{
  RD_WatchCell* next;
  RD_WatchCellKind kind;
  RD_WatchCellFlags flags;
  U64 index;
  E_Eval eval;
  F32 default_pct;
  F32 pct;
  F32 px;
}

struct RD_WatchCellList
{
  RD_WatchCell* first;
  RD_WatchCell* last;
  U64 count;
}

struct RD_WatchRowInfo
{
  CTRL_Entity* module;
  B32 can_expand;
  B32 expr_is_editable;
  String8 group_cfg_name;
  RD_Cfg* group_cfg_parent;
  RD_Cfg* group_cfg_child;
  CTRL_Entity* group_entity;
  CTRL_Entity* callstack_thread;
  U64 callstack_unwind_index;
  U64 callstack_inline_depth;
  U64 callstack_vaddr;
  String8 cell_style_key;
  RD_WatchCellList cells;
  RD_ViewUIRule* view_ui_rule;
}

struct RD_WatchRowCellInfo
{
  RD_WatchCellFlags flags;
  RD_Cfg* cfg;
  CTRL_Entity* entity;
  String8 cmd_name;
  String8 file_path;
  DR_FStrList expr_fstrs;
  DR_FStrList eval_fstrs;
  String8 description;
  String8 error_tooltip;
  String8 inheritance_tooltip;
  RD_ViewUIRule* view_ui_rule;
}

struct RD_WatchPt
{
  EV_Key parent_key;
  EV_Key key;
  U64 cell_id;
}

struct RD_WatchViewTextEditState
{
  RD_WatchViewTextEditState* pt_hash_next;
  RD_WatchPt pt;
  TxtPt cursor;
  TxtPt mark;
  U8[1024] input_buffer;
  U64 input_size;
  U8[1024] initial_buffer;
  U64 initial_size;
}

struct RD_WatchViewState
{
  B32 initialized;
  
  // rjf: filter history
  Arena* filter_arena;
  String8 last_filter;
  
  // rjf; table cursor state
  RD_WatchPt cursor;
  RD_WatchPt mark;
  RD_WatchPt next_cursor;
  RD_WatchPt next_mark;
  
  // rjf: text input state
  Arena* text_edit_arena;
  U64 text_edit_state_slots_count;
  RD_WatchViewTextEditState dummy_text_edit_state;
  RD_WatchViewTextEditState** text_edit_state_slots;
  B32 text_editing;
}

////////////////////////////////
//~ rjf: Watch View Functions

//- rjf: cell list building
#define rd_watch_cell_list_push_new(arena, list, kind_, eval_, ...) rd_watch_cell_list_push_new_((arena), (list), &(RD_WatchCell){.kind = (kind_), .eval = (eval_), __VA_ARGS__})
////////////////////////////////
//~ rjf: View Hooks

// TODO(rjf): eliminate once we are predeclaring these with metacode

RD_VIEW_UI_FUNCTION_DEF(null);

EV_EXPAND_RULE_INFO_FUNCTION_DEF(text);
EV_EXPAND_RULE_INFO_FUNCTION_DEF(disasm);
EV_EXPAND_RULE_INFO_FUNCTION_DEF(memory);
EV_EXPAND_RULE_INFO_FUNCTION_DEF(bitmap);
EV_EXPAND_RULE_INFO_FUNCTION_DEF(color);
EV_EXPAND_RULE_INFO_FUNCTION_DEF(geo3d);

RD_VIEW_UI_FUNCTION_DEF(text);
RD_VIEW_UI_FUNCTION_DEF(disasm);
RD_VIEW_UI_FUNCTION_DEF(memory);
RD_VIEW_UI_FUNCTION_DEF(bitmap);
RD_VIEW_UI_FUNCTION_DEF(color);
RD_VIEW_UI_FUNCTION_DEF(geo3d);


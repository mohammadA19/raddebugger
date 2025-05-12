// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Config IDs

typedef U64 RD_CfgID;

struct RD_CfgIDNode
{
  RD_CfgIDNode* next;
  RD_CfgID v;
}

struct RD_CfgIDList
{
  RD_CfgIDNode* first;
  RD_CfgIDNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Key Bindings

struct RD_Binding
{
  OS_Key key;
  OS_Modifiers modifiers;
}

struct RD_KeyMapNode
{
  RD_KeyMapNode* name_hash_next;
  RD_KeyMapNode* binding_hash_next;
  RD_CfgID cfg_id;
  String8 name;
  RD_Binding binding;
}

struct RD_KeyMapNodePtr
{
  RD_KeyMapNodePtr* next;
  RD_KeyMapNode* v;
}

struct RD_KeyMapNodePtrList
{
  RD_KeyMapNodePtr* first;
  RD_KeyMapNodePtr* last;
  U64 count;
}

struct RD_KeyMapSlot
{
  RD_KeyMapNode* first;
  RD_KeyMapNode* last;
}

struct RD_KeyMap
{
  U64 name_slots_count;
  RD_KeyMapSlot* name_slots;
  U64 binding_slots_count;
  RD_KeyMapSlot* binding_slots;
}

////////////////////////////////
//~ rjf: Evaluation Spaces

enum RD_EvalSpaceKind : U64
{
  RD_EvalSpaceKind_CtrlEntity = E_SpaceKind_FirstUserDefined,
  RD_EvalSpaceKind_MetaQuery,
  RD_EvalSpaceKind_MetaCfg,
  RD_EvalSpaceKind_MetaCmd,
  RD_EvalSpaceKind_MetaTheme,
  RD_EvalSpaceKind_MetaCtrlEntity,
  RD_EvalSpaceKind_MetaUnattachedProcess,
}

////////////////////////////////
//~ rjf: View UI Hook Types

#define RD_VIEW_UI_FUNCTION_SIG(name) void name(E_Eval eval, Rng2F32 rect)
#define RD_VIEW_UI_FUNCTION_NAME(name) rd_view_ui__##name
#define RD_VIEW_UI_FUNCTION_DEF(name) internal RD_VIEW_UI_FUNCTION_SIG(RD_VIEW_UI_FUNCTION_NAME(name))
typedef RD_VIEW_UI_FUNCTION_SIG(RD_ViewUIFunctionType);

struct RD_ViewUIRule
{
  String8 name;
  RD_ViewUIFunctionType* ui;
}

struct RD_ViewUIRuleNode
{
  RD_ViewUIRuleNode* next;
  RD_ViewUIRule v;
}

struct RD_ViewUIRuleSlot
{
  RD_ViewUIRuleNode* first;
  RD_ViewUIRuleNode* last;
}

struct RD_ViewUIRuleMap
{
  RD_ViewUIRuleSlot* slots;
  U64 slots_count;
}

////////////////////////////////
//~ rjf: Drag/Drop Types

enum RD_DragDropState
{
  RD_DragDropState_Null,
  RD_DragDropState_Dragging,
  RD_DragDropState_Dropping,
  RD_DragDropState_COUNT
}

////////////////////////////////
//~ rjf: Command Kind Types

enum RD_QueryFlags : U32
{
  RD_QueryFlag_AllowFiles       = (1<<0),
  RD_QueryFlag_AllowFolders     = (1<<1),
  RD_QueryFlag_CodeInput        = (1<<2),
  RD_QueryFlag_KeepOldInput     = (1<<3),
  RD_QueryFlag_SelectOldInput   = (1<<4),
  RD_QueryFlag_Floating         = (1<<5),
  RD_QueryFlag_Required         = (1<<6),
}

enum RD_CmdKindFlags : U32
{
  RD_CmdKindFlag_ListInUI      = (1<<0),
  RD_CmdKindFlag_ListInIPCDocs = (1<<1),
  RD_CmdKindFlag_ListInTab     = (1<<2),
  RD_CmdKindFlag_ListInTextPt  = (1<<3),
  RD_CmdKindFlag_ListInTextRng = (1<<4),
}

////////////////////////////////
//~ rjf: Autocompletion Cursor Info Type

struct RD_AutocompCursorInfo
{
  String8 list_expr;
  String8 filter;
  Rng1U64 replaced_range;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/raddbg.meta.h"

////////////////////////////////
//~ rjf: View State Types

struct RD_ArenaExt
{
  RD_ArenaExt* next;
  Arena* arena;
}

struct RD_ViewState
{
  // rjf: hash links & key
  RD_ViewState* hash_next;
  RD_ViewState* hash_prev;
  RD_CfgID cfg_id;
  
  // rjf: touch info
  U64 last_frame_index_touched;
  U64 last_frame_index_built;
  
  // rjf: loading indicator info
  F32 loading_t;
  F32 loading_t_target;
  U64 loading_progress_v;
  U64 loading_progress_v_target;
  
  // rjf: scroll position
  UI_ScrollPt2 scroll_pos;
  
  // rjf: eval visualization view state
  EV_View* ev_view;
  
  // rjf: view-lifetime allocation & user data extensions
  Arena* arena;
  U64 arena_reset_pos;
  RD_ArenaExt* first_arena_ext;
  RD_ArenaExt* last_arena_ext;
  void* user_data;
  
  // rjf: query state
  B32 query_is_selected;
  TxtPt query_cursor;
  TxtPt query_mark;
  U8[KB(1)] query_buffer;
  U64 query_string_size;
  
  // rjf: contents are focused (disables query focus)
  B32 contents_are_focused;
}

struct RD_ViewStateSlot
{
  RD_ViewState* first;
  RD_ViewState* last;
}

////////////////////////////////
//~ rjf: Vocabulary Map

struct RD_VocabInfoMapNode
{
  RD_VocabInfoMapNode* single_next;
  RD_VocabInfoMapNode* plural_next;
  RD_VocabInfo v;
}

struct RD_VocabInfoMapSlot
{
  RD_VocabInfoMapNode* first;
  RD_VocabInfoMapNode* last;
}

struct RD_VocabInfoMap
{
  U64 single_slots_count;
  RD_VocabInfoMapSlot* single_slots;
  U64 plural_slots_count;
  RD_VocabInfoMapSlot* plural_slots;
}

////////////////////////////////
//~ rjf: Config Tree

struct RD_Cfg
{
  RD_Cfg* first;
  RD_Cfg* last;
  RD_Cfg* next;
  RD_Cfg* prev;
  RD_Cfg* parent;
  RD_CfgID id;
  String8 string;
}

struct RD_CfgNode
{
  RD_CfgNode* next;
  RD_CfgNode* prev;
  RD_Cfg* v;
}

struct RD_CfgSlot
{
  RD_CfgNode* first;
  RD_CfgNode* last;
}

struct RD_CfgList
{
  RD_CfgNode* first;
  RD_CfgNode* last;
  U64 count;
}

struct RD_CfgArray
{
  RD_Cfg** v;
  U64 count;
}

struct RD_CfgRec
{
  RD_Cfg* next;
  S32 push_count;
  S32 pop_count;
}

////////////////////////////////
//~ rjf: Structured Locations, Parsed From Config Trees

struct RD_Location
{
  String8 file_path;
  TxtPt pt;
  String8 expr;
}

////////////////////////////////
//~ rjf: Structured Panel Trees, Parsed From Config Trees

struct RD_PanelNode
{
  // rjf: links data
  RD_PanelNode* first;
  RD_PanelNode* last;
  RD_PanelNode* next;
  RD_PanelNode* prev;
  RD_PanelNode* parent;
  U64 child_count;
  RD_Cfg* cfg;
  
  // rjf: split data
  Axis2 split_axis;
  F32 pct_of_parent;
  
  // rjf: tab params
  Side tab_side;
  
  // rjf: which tabs are attached
  RD_CfgList tabs;
  RD_Cfg* selected_tab;
}

struct RD_PanelTree
{
  RD_PanelNode* root;
  RD_PanelNode* focused;
}

struct RD_PanelNodeRec
{
  RD_PanelNode* next;
  S32 push_count;
  S32 pop_count;
}

////////////////////////////////
//~ rjf: Command Types

struct RD_Cmd
{
  String8 name;
  RD_Regs* regs;
}

struct RD_CmdNode
{
  RD_CmdNode* next;
  RD_CmdNode* prev;
  RD_Cmd cmd;
}

struct RD_CmdList
{
  RD_CmdNode* first;
  RD_CmdNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Context Register Types

struct RD_RegsNode
{
  RD_RegsNode* next;
  RD_Regs v;
}

////////////////////////////////
//~ rjf: Structured Theme Types, Parsed From Config

enum RD_FontSlot
{
  RD_FontSlot_Main,
  RD_FontSlot_Code,
  RD_FontSlot_Icons,
  RD_FontSlot_COUNT
}

////////////////////////////////
//~ rjf: Per-Window State

struct RD_WindowState
{
  // rjf: links & metadata
  RD_WindowState* order_next;
  RD_WindowState* order_prev;
  RD_WindowState* hash_next;
  RD_WindowState* hash_prev;
  RD_CfgID cfg_id;
  U64 frames_alive;
  U64 last_frame_index_touched;
  
  // rjf: top-level info & handles
  Arena* arena;
  OS_Handle os;
  R_Handle r;
  UI_State* ui;
  F32 last_dpi;
  B32 window_temporarily_focused_ipc;
  B32 window_layout_reset;
  Rng2F32 last_window_rect;
  
  // rjf: theme (recomputed each frame)
  UI_Theme* theme;
  Vec4F32[RD_CodeColorSlot_COUNT] theme_code_colors;
  
  // rjf: font raster flags (recomputed each frame)
  FNT_RasterFlags[RD_FontSlot_COUNT] font_slot_raster_flags;
  
  // rjf: dev interface state
  B32 dev_menu_is_open;
  
  // rjf: menu bar state
  B32 menu_bar_focused;
  B32 menu_bar_focused_on_press;
  B32 menu_bar_key_held;
  B32 menu_bar_focus_press_started;
  
  // rjf: drop-completion state
  Arena* drop_completion_arena;
  String8List drop_completion_paths;
  
  // rjf: query state
  B32 query_is_active;
  Arena* query_arena;
  RD_Regs* query_regs;
  RD_CfgID query_view_id;
  RD_CfgID query_last_view_id;
  
  // rjf: hover eval state
  B32 hover_eval_focused;
  Arena* hover_eval_arena;
  Vec2F32 hover_eval_spawn_pos;
  String8 hover_eval_string;
  U64 hover_eval_firstt_us;
  U64 hover_eval_lastt_us;
  
  // rjf: autocompletion state
  U64 autocomp_last_frame_index;
  Arena* autocomp_arena;
  RD_Regs* autocomp_regs;
  RD_AutocompCursorInfo autocomp_cursor_info;
  
  // rjf: error state
  U8[512] error_buffer;
  U64 error_string_size;
  F32 error_t;
  
  // rjf: per-frame ui events state
  UI_EventList ui_events;
  
  // rjf: per-frame drawing state
  DR_Bucket* draw_bucket;
}

struct RD_WindowStateSlot
{
  RD_WindowState* first;
  RD_WindowState* last;
}

////////////////////////////////
//~ rjf: Main Per-Process Graphical State

read_only global U64 rd_name_bucket_chunk_sizes[] =
{
  16,
  64,
  256,
  1024,
  4096,
  16384,
  65536,
  0xffffffffffffffffull,
}

struct RD_NameChunkNode
{
  RD_NameChunkNode* next;
  U64 size;
}

struct RD_AmbiguousPathNode
{
  RD_AmbiguousPathNode* next;
  String8 name;
  String8List paths;
}

struct RD_State
{
  // rjf: basics
  Arena* arena;
  B32 quit;
  B32 quit_after_success;
  S32 frame_depth;
  U64 frame_eval_memread_endt_us;
  
  // rjf: config bucket paths
  Arena* user_path_arena;
  String8 user_path;
  Arena* project_path_arena;
  String8 project_path;
  Arena* theme_path_arena;
  String8 theme_path;
  
  // rjf: unpacked settings (cached, because they need to be used
  // earlier than setting evaluation is legal in a frame)
  B32 alt_menu_bar_enabled;
  B32 use_default_stl_type_views;
  B32 use_default_ue_type_views;
  
  // rjf: animation rates
  F32 catchall_animation_rate;
  F32 menu_animation_rate;
  F32 menu_animation_rate__slow;
  F32 entity_alive_animation_rate;
  F32 rich_hover_animation_rate;
  F32 scrolling_animation_rate;
  F32 tooltip_animation_rate;
  
  // rjf: serialized config debug string keys
  U128 user_cfg_string_key;
  U128 project_cfg_string_key;
  U128 cmdln_cfg_string_key;
  U128 transient_cfg_string_key;
  
  // rjf: schema table
  MD_NodePtrList* schemas;
  
  // rjf: default theme table
  MD_Node *theme_preset_trees[RD_ThemePreset_COUNT];
  
  // rjf: vocab table
  RD_VocabInfoMap vocab_info_map;
  
  // rjf: log
  Log* log;
  String8 log_path;
  
  // rjf: frame history info
  U64 frame_index;
  Arena *frame_arenas[2];
  U64[64] frame_time_us_history;
  U64 num_frames_requested;
  F64 time_in_seconds;
  U64 time_in_us;
  
  // rjf: frame parameters
  F32 frame_dt;
  DI_Scope* frame_di_scope;
  
  // rjf: dbgi match store
  DI_MatchStore* match_store;
  
  // rjf: evaluation cache
  E_Cache* eval_cache;
  
  // rjf: ambiguous path table (constructed from-scratch each frame)
  U64 ambiguous_path_slots_count;
  RD_AmbiguousPathNode** ambiguous_path_slots;
  
  // rjf: key map (constructed from-scratch each frame)
  RD_KeyMap* key_map;
  
  // rjf: slot -> font tag map (constructed from-scratch each frame)
  FNT_Tag[RD_FontSlot_COUNT] font_slot_table;
  
  // rjf: meta name -> eval type key map (constructed from-scratch each frame)
  E_String2TypeKeyMap* meta_name2type_map;
  
  // rjf: name -> view ui map (constructed from-scratch each frame)
  RD_ViewUIRuleMap* view_ui_rule_map;
  
  // rjf: registers stack
  RD_RegsNode base_regs;
  RD_RegsNode* top_regs;
  
  // rjf: autosave state
  F32 seconds_until_autosave;
  
  // rjf: commands
  Arena *cmds_arenas[2];
  RD_CmdList[2] cmds;
  U64 cmds_gen;
  
  // rjf: popup state
  UI_Key popup_key;
  B32 popup_active;
  F32 popup_t;
  Arena* popup_arena;
  RD_CmdList popup_cmds;
  String8 popup_title;
  String8 popup_desc;
  
  // rjf: text editing mode state
  B32 text_edit_mode;
  
  // rjf: contextual hover info
  RD_Regs* hover_regs;
  RD_RegSlot hover_regs_slot;
  RD_Regs* next_hover_regs;
  RD_RegSlot next_hover_regs_slot;
  
  // rjf: icon texture
  R_Handle icon_texture;
  
  // rjf: fixed ui keys
  UI_Key drop_completion_key;
  UI_Key ctx_menu_key;
  
  // rjf: drag/drop state
  Arena* drag_drop_arena;
  RD_Regs* drag_drop_regs;
  RD_RegSlot drag_drop_regs_slot;
  RD_DragDropState drag_drop_state;
  
  // rjf: cfg state
  RD_NameChunkNode *free_name_chunks[ArrayCount(rd_name_bucket_chunk_sizes)];
  RD_Cfg* free_cfg;
  RD_Cfg* root_cfg;
  U64 cfg_id_slots_count;
  RD_CfgSlot* cfg_id_slots;
  RD_CfgNode* free_cfg_id_node;
  U64 cfg_id_gen;
  RD_CfgID cfg_last_accessed_id;
  RD_Cfg* cfg_last_accessed;
  U64 cfg_change_gen;
  
  // rjf: window state cache
  U64 window_state_slots_count;
  RD_WindowStateSlot* window_state_slots;
  RD_WindowState* free_window_state;
  RD_CfgID last_focused_window;
  RD_WindowState* first_window_state;
  RD_WindowState* last_window_state;
  RD_CfgID window_state_last_accessed_id;
  RD_WindowState* window_state_last_accessed;
  
  // rjf: view state cache
  U64 view_state_slots_count;
  RD_ViewStateSlot* view_state_slots;
  RD_ViewState* free_view_state;
  RD_CfgID view_state_last_accessed_id;
  RD_ViewState* view_state_last_accessed;
  
  // rjf: bind change
  Arena* bind_change_arena;
  B32 bind_change_active;
  RD_CfgID bind_change_binding_id;
  String8 bind_change_cmd_name;
}

////////////////////////////////
//~ rjf: Globals

read_only global RD_VocabInfo rd_nil_vocab_info = {0};

read_only global RD_Cfg rd_nil_cfg =
{
  &rd_nil_cfg,
  &rd_nil_cfg,
  &rd_nil_cfg,
  &rd_nil_cfg,
  &rd_nil_cfg,
}

read_only global RD_PanelNode rd_nil_panel_node =
{
  &rd_nil_panel_node,
  &rd_nil_panel_node,
  &rd_nil_panel_node,
  &rd_nil_panel_node,
  &rd_nil_panel_node,
  0,
  &rd_nil_cfg,
  .selected_tab = &rd_nil_cfg,
}

read_only global RD_CmdKindInfo rd_nil_cmd_kind_info = {0};

RD_VIEW_UI_FUNCTION_DEF(null);
read_only global RD_ViewUIRule rd_nil_view_ui_rule =
{
  {0},
  RD_VIEW_UI_FUNCTION_NAME(null),
}

read_only global RD_ViewState rd_nil_view_state =
{
  &rd_nil_view_state,
  &rd_nil_view_state,
}

read_only global RD_WindowState rd_nil_window_state =
{
  &rd_nil_window_state,
  &rd_nil_window_state,
  &rd_nil_window_state,
  &rd_nil_window_state,
}

global RD_State *rd_state = 0;
global RD_CfgID rd_last_drag_drop_panel = 0;
global RD_CfgID rd_last_drag_drop_prev_tab = 0;

////////////////////////////////
//~ rjf: Config Tree Functions

#define rd_cfg_list_first(list) ((list)->count ? (list)->first->v : &rd_nil_cfg)
#define rd_cfg_list_last(list)  ((list)->count ? (list)->last->v  : &rd_nil_cfg)

#define rd_panel_node_rec__depth_first_pre(root, p)     rd_panel_node_rec__depth_first((root), (p), OffsetOf(RD_PanelNode, next), OffsetOf(RD_PanelNode, first))
#define rd_panel_node_rec__depth_first_pre_rev(root, p) rd_panel_node_rec__depth_first((root), (p), OffsetOf(RD_PanelNode, prev), OffsetOf(RD_PanelNode, last))








////////////////////////////////
//~ rjf: View Building API
//- rjf: pushing/attaching view resources
#define rd_view_state(T) (T *)rd_view_state_by_size(sizeof(T))

//- rjf: storing view-attached state
#define rd_store_view_param_f32(key, f32) rd_store_view_paramf((key), "%ff", (f32))
#define rd_store_view_param_s64(key, s64) rd_store_view_paramf((key), "%I64d", (s64))
#define rd_store_view_param_u64(key, u64) rd_store_view_paramf((key), "0x%I64x", (u64))

////////////////////////////////
//~ rjf: Autocompletion Lister

#define rd_set_autocomp_regs(dst_eval, ...) rd_set_autocomp_regs_((dst_eval), &(RD_Regs){rd_regs_lit_init_top __VA_ARGS__})

////////////////////////////////
//~ rjf: Vocab Info Lookups

#define rd_plural_from_code_name(code_name) (rd_vocab_info_from_code_name(code_name)->code_name_plural)
#define rd_display_from_code_name(code_name) (rd_vocab_info_from_code_name(code_name)->display_name)
#define rd_display_plural_from_code_name(code_name) (rd_vocab_info_from_code_name(code_name)->display_name_plural)
#define rd_icon_kind_from_code_name(code_name) (rd_vocab_info_from_code_name(code_name)->icon_kind)
#define rd_singular_from_code_name_plural(code_name_plural) (rd_vocab_info_from_code_name_plural(code_name_plural)->code_name)

////////////////////////////////
//~ rjf: Registers

#define rd_regs() (&rd_state->top_regs->v)
#define rd_base_regs() (&rd_state->base_regs.v)
#define rd_push_regs(...) rd_push_regs_(&(RD_Regs){rd_regs_lit_init_top __VA_ARGS__})
#define RD_RegsScope(...) DeferLoop(rd_push_regs(__VA_ARGS__), rd_pop_regs())

////////////////////////////////
//~ rjf: Commands
//- rjf: pushing
#define rd_cmd(kind, ...) rd_push_cmd(rd_cmd_kind_info_table[kind].string, &(RD_Regs){rd_regs_lit_init_top __VA_ARGS__})

////////////////////////////////
//~ rjf: Main Layer Top-Level Calls



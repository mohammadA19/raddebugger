// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADDBG_CORE_H
#define RADDBG_CORE_H

////////////////////////////////
//~ rjf: Handles

struct RD_Handle
{
  ulong u64[2];
};

struct RD_HandleNode
{
  RD_HandleNode *next;
  RD_HandleNode *prev;
  RD_Handle handle;
};

struct RD_HandleList
{
  RD_HandleNode *first;
  RD_HandleNode *last;
  ulong count;
};

////////////////////////////////
//~ rjf: Evaluation Spaces

enum RD_EvalSpaceKind : ulong
{
  RD_EvalSpaceKind_CtrlEntity = E_SpaceKind_FirstUserDefined,
  RD_EvalSpaceKind_MetaEntity,
  RD_EvalSpaceKind_MetaCtrlEntity,
  RD_EvalSpaceKind_MetaCollection,
};

////////////////////////////////
//~ rjf: Entity Kind Flags

enum RD_EntityKindFlags : uint
{
  //- rjf: allowed operations
  RD_EntityKindFlag_CanDelete                = (1<<0),
  RD_EntityKindFlag_CanFreeze                = (1<<1),
  RD_EntityKindFlag_CanEdit                  = (1<<2),
  RD_EntityKindFlag_CanRename                = (1<<3),
  RD_EntityKindFlag_CanEnable                = (1<<4),
  RD_EntityKindFlag_CanCondition             = (1<<5),
  RD_EntityKindFlag_CanDuplicate             = (1<<6),
  
  //- rjf: name categorization
  RD_EntityKindFlag_NameIsCode               = (1<<7),
  RD_EntityKindFlag_NameIsPath               = (1<<8),
  
  //- rjf: lifetime categorization
  RD_EntityKindFlag_UserDefinedLifetime      = (1<<9),
  
  //- rjf: serialization
  RD_EntityKindFlag_IsSerializedToConfig     = (1<<10),
};

////////////////////////////////
//~ rjf: Entity Flags

enum RD_EntityFlags : uint
{
  //- rjf: allocationless, simple equipment
  RD_EntityFlag_HasTextPoint      = (1<<0),
  RD_EntityFlag_HasEntityHandle   = (1<<2),
  RD_EntityFlag_HasU64            = (1<<4),
  RD_EntityFlag_HasColor          = (1<<6),
  RD_EntityFlag_HasVAddr          = (1<<15),
  
  //- rjf: deletion
  RD_EntityFlag_MarkedForDeletion = (1<<31),
};

////////////////////////////////
//~ rjf: Binding Types

struct RD_Binding
{
  OS_Key key;
  OS_Modifiers modifiers;
};

struct RD_BindingNode
{
  RD_BindingNode *next;
  RD_Binding binding;
};

struct RD_BindingList
{
  RD_BindingNode *first;
  RD_BindingNode *last;
  ulong count;
};

struct RD_StringBindingPair
{
  String8 string;
  RD_Binding binding;
};

////////////////////////////////
//~ rjf: Key Map Types

struct RD_KeyMapNode
{
  RD_KeyMapNode *hash_next;
  RD_KeyMapNode *hash_prev;
  String8 name;
  RD_Binding binding;
};

struct RD_KeyMapSlot
{
  RD_KeyMapNode *first;
  RD_KeyMapNode *last;
};

////////////////////////////////
//~ rjf: Setting Types

struct RD_SettingVal
{
  B32 set;
  int s32;
};

////////////////////////////////
//~ rjf: View Rule Info Types

enum RD_ViewRuleInfoFlags : uint
{
  RD_ViewRuleInfoFlag_ShowInDocs                 = (1<<0),
  RD_ViewRuleInfoFlag_CanFilter                  = (1<<1),
  RD_ViewRuleInfoFlag_FilterIsCode               = (1<<2),
  RD_ViewRuleInfoFlag_TypingAutomaticallyFilters = (1<<3),
  RD_ViewRuleInfoFlag_CanUseInWatchTable         = (1<<4),
  RD_ViewRuleInfoFlag_CanFillValueCell           = (1<<5),
  RD_ViewRuleInfoFlag_CanExpand                  = (1<<6),
  RD_ViewRuleInfoFlag_ProjectFiltered            = (1<<7),
};

#define RD_VIEW_RULE_UI_FUNCTION_SIG(name) void name(String8 string, MD_Node *params, Rng2F32 rect)
#define RD_VIEW_RULE_UI_FUNCTION_NAME(name) rd_view_rule_ui_##name
#define RD_VIEW_RULE_UI_FUNCTION_DEF(name) internal RD_VIEW_RULE_UI_FUNCTION_SIG(RD_VIEW_RULE_UI_FUNCTION_NAME(name))
typedef RD_VIEW_RULE_UI_FUNCTION_SIG(RD_ViewRuleUIFunctionType);

////////////////////////////////
//~ rjf: View Types


struct RD_ArenaExt
{
  RD_ArenaExt *next;
  Arena *arena;
};

struct RD_TransientViewNode
{
  RD_TransientViewNode *next;
  RD_TransientViewNode *prev;
  EV_Key key;
  RD_View *view;
  Arena *initial_params_arena;
  MD_Node *initial_params;
  ulong first_frame_index_touched;
  ulong last_frame_index_touched;
};

struct RD_TransientViewSlot
{
  RD_TransientViewNode *first;
  RD_TransientViewNode *last;
};

struct RD_View
{
  // rjf: allocation links (for iterating all views)
  RD_View *alloc_next;
  RD_View *alloc_prev;
  
  // rjf: ownership links ('owners' can have lists of views)
  RD_View *order_next;
  RD_View *order_prev;
  
  // rjf: transient view children
  RD_View *first_transient;
  RD_View *last_transient;
  
  // rjf: view specification info
  struct RD_ViewRuleInfo *spec;
  
  // rjf: allocation info
  ulong generation;
  
  // rjf: loading animation state
  float loading_t;
  float loading_t_target;
  ulong loading_progress_v;
  ulong loading_progress_v_target;
  
  // rjf: view project (for project-specific/filtered views)
  Arena *project_path_arena;
  String8 project_path;
  
  // rjf: view state
  UI_ScrollPt2 scroll_pos;
  
  // rjf: view-lifetime allocation & user data extensions
  Arena *arena;
  RD_ArenaExt *first_arena_ext;
  RD_ArenaExt *last_arena_ext;
  ulong transient_view_slots_count;
  RD_TransientViewSlot *transient_view_slots;
  RD_TransientViewNode *free_transient_view_node;
  void *user_data;
  
  // rjf: filter mode
  B32 is_filtering;
  float is_filtering_t;
  
  // rjf: params tree state
  Arena *params_arenas[2];
  MD_Node *params_roots[2];
  ulong params_write_gen;
  ulong params_read_gen;
  
  // rjf: text query state
  TxtPt query_cursor;
  TxtPt query_mark;
  ulong query_string_size;
  byte query_buffer[KB(4)];
};

////////////////////////////////
//~ rjf: Panel Types

struct RD_Panel
{
  // rjf: tree links/data
  RD_Panel *first;
  RD_Panel *last;
  RD_Panel *next;
  RD_Panel *prev;
  RD_Panel *parent;
  ulong child_count;
  
  // rjf: allocation data
  ulong generation;
  
  // rjf: split data
  Axis2 split_axis;
  float pct_of_parent;
  
  // rjf: animated rectangle data
  Rng2F32 animated_rect_pct;
  
  // rjf: tab params
  Side tab_side;
  
  // rjf: stable views (tabs)
  RD_View *first_tab_view;
  RD_View *last_tab_view;
  ulong tab_view_count;
  RD_Handle selected_tab_view;
};

struct RD_PanelRec
{
  RD_Panel *next;
  int push_count;
  int pop_count;
};

////////////////////////////////
//~ rjf: Drag/Drop Types

enum RD_DragDropState
{
  RD_DragDropState_Null,
  RD_DragDropState_Dragging,
  RD_DragDropState_Dropping,
  RD_DragDropState_COUNT
};

////////////////////////////////
//~ rjf: Command Kind Types

enum RD_QueryFlags : uint
{
  RD_QueryFlag_AllowFiles       = (1<<0),
  RD_QueryFlag_AllowFolders     = (1<<1),
  RD_QueryFlag_CodeInput        = (1<<2),
  RD_QueryFlag_KeepOldInput     = (1<<3),
  RD_QueryFlag_SelectOldInput   = (1<<4),
  RD_QueryFlag_Required         = (1<<5),
};

enum RD_CmdKindFlags : uint
{
  RD_CmdKindFlag_ListInUI      = (1<<0),
  RD_CmdKindFlag_ListInIPCDocs = (1<<1),
};

////////////////////////////////
//~ rjf: Generated Code

#include "generated/raddbg.meta.h"

////////////////////////////////
//~ rjf: Config Types

struct RD_CfgTree
{
  RD_CfgTree *next;
  RD_CfgSrc source;
  MD_Node *root;
};

struct RD_CfgVal
{
  RD_CfgVal *hash_next;
  RD_CfgVal *linear_next;
  RD_CfgTree *first;
  RD_CfgTree *last;
  ulong insertion_stamp;
  String8 string;
};

struct RD_CfgSlot
{
  RD_CfgVal *first;
};

struct RD_CfgTable
{
  ulong slot_count;
  RD_CfgSlot *slots;
  ulong insertion_stamp_counter;
  RD_CfgVal *first_val;
  RD_CfgVal *last_val;
};

////////////////////////////////
//~ rjf: New Config/Entity Data Structure

struct RD_Cfg
{
  RD_Cfg *first;
  RD_Cfg *last;
  RD_Cfg *next;
  RD_Cfg *prev;
  RD_Cfg *parent;
  ulong gen;
  String8 string;
};

struct RD_CfgNode
{
  RD_CfgNode *next;
  RD_Cfg *v;
};

struct RD_CfgList
{
  RD_CfgNode *first;
  RD_CfgNode *last;
  ulong count;
};

struct RD_CfgRec
{
  RD_Cfg *next;
  int push_count;
  int pop_count;
};

////////////////////////////////
//~ rjf: Entity Types

typedef ulong RD_EntityID;

struct RD_Entity
{
  // rjf: tree links
  RD_Entity *first;
  RD_Entity *last;
  RD_Entity *next;
  RD_Entity *prev;
  RD_Entity *parent;
  
  // rjf: metadata
  RD_EntityKind kind;
  RD_EntityFlags flags;
  RD_EntityID id;
  ulong gen;
  ulong alloc_time_us;
  
  // rjf: basic equipment
  TxtPt text_point;
  B32 disabled;
  B32 debug_subprocesses;
  ulong u64;
  ulong vaddr;
  Vec4F32 color_hsva;
  RD_CfgSrc cfg_src;
  ulong timestamp;
  
  // rjf: string equipment
  String8 string;
};

struct RD_EntityNode
{
  RD_EntityNode *next;
  RD_Entity *entity;
};

struct RD_EntityList
{
  RD_EntityNode *first;
  RD_EntityNode *last;
  ulong count;
};

struct RD_EntityArray
{
  RD_Entity **v;
  ulong count;
};

struct RD_EntityRec
{
  RD_Entity *next;
  int push_count;
  int pop_count;
};

////////////////////////////////
//~ rjf: Command Types

struct RD_Cmd
{
  String8 name;
  RD_Regs *regs;
};

struct RD_CmdNode
{
  RD_CmdNode *next;
  RD_CmdNode *prev;
  RD_Cmd cmd;
};

struct RD_CmdList
{
  RD_CmdNode *first;
  RD_CmdNode *last;
  ulong count;
};

////////////////////////////////
//~ rjf: Context Register Types

struct RD_RegsNode
{
  RD_RegsNode *next;
  RD_Regs v;
};

////////////////////////////////
//~ rjf: Theme Types

struct RD_Theme
{
  Vec4F32 colors[RD_ThemeColor_COUNT];
};

enum RD_FontSlot
{
  RD_FontSlot_Main,
  RD_FontSlot_Code,
  RD_FontSlot_Icons,
  RD_FontSlot_COUNT
};

enum RD_PaletteCode
{
  RD_PaletteCode_Base,
  RD_PaletteCode_MenuBar,
  RD_PaletteCode_Floating,
  RD_PaletteCode_ImplicitButton,
  RD_PaletteCode_PlainButton,
  RD_PaletteCode_PositivePopButton,
  RD_PaletteCode_NegativePopButton,
  RD_PaletteCode_NeutralPopButton,
  RD_PaletteCode_ScrollBarButton,
  RD_PaletteCode_Tab,
  RD_PaletteCode_TabInactive,
  RD_PaletteCode_DropSiteOverlay,
  RD_PaletteCode_COUNT
};

////////////////////////////////
//~ rjf: Auto-Complete Lister Types

enum RD_AutoCompListerFlags : uint
{
  RD_AutoCompListerFlag_Locals        = (1<<0),
  RD_AutoCompListerFlag_Registers     = (1<<1),
  RD_AutoCompListerFlag_ViewRules     = (1<<2),
  RD_AutoCompListerFlag_ViewRuleParams= (1<<3),
  RD_AutoCompListerFlag_Members       = (1<<4),
  RD_AutoCompListerFlag_Globals       = (1<<5),
  RD_AutoCompListerFlag_ThreadLocals  = (1<<6),
  RD_AutoCompListerFlag_Procedures    = (1<<7),
  RD_AutoCompListerFlag_Types         = (1<<8),
  RD_AutoCompListerFlag_Languages     = (1<<9),
  RD_AutoCompListerFlag_Architectures = (1<<10),
  RD_AutoCompListerFlag_Tex2DFormats  = (1<<11),
  RD_AutoCompListerFlag_Files         = (1<<12),
};

struct RD_AutoCompListerItem
{
  String8 string;
  String8 kind_string;
  FuzzyMatchRangeList matches;
  ulong group;
  B32 is_non_code;
};

struct RD_AutoCompListerItemChunkNode
{
  RD_AutoCompListerItemChunkNode *next;
  RD_AutoCompListerItem *v;
  ulong count;
  ulong cap;
};

struct RD_AutoCompListerItemChunkList
{
  RD_AutoCompListerItemChunkNode *first;
  RD_AutoCompListerItemChunkNode *last;
  ulong chunk_count;
  ulong total_count;
};

struct RD_AutoCompListerItemArray
{
  RD_AutoCompListerItem *v;
  ulong count;
};

struct RD_AutoCompListerParams
{
  RD_AutoCompListerFlags flags;
  String8List strings;
};

////////////////////////////////
//~ rjf: Per-Window State

struct RD_Window
{
  // rjf: links & metadata
  RD_Window *next;
  RD_Window *prev;
  ulong gen;
  ulong frames_alive;
  RD_CfgSrc cfg_src;
  
  // rjf: top-level info & handles
  Arena *arena;
  OS_Handle os;
  R_Handle r;
  UI_State *ui;
  float last_dpi;
  B32 window_temporarily_focused_ipc;
  
  // rjf: config/settings
  RD_SettingVal setting_vals[RD_SettingCode_COUNT];
  UI_Palette cfg_palettes[RD_PaletteCode_COUNT]; // derivative from theme
  
  // rjf: dev interface state
  B32 dev_menu_is_open;
  
  // rjf: menu bar state
  B32 menu_bar_focused;
  B32 menu_bar_focused_on_press;
  B32 menu_bar_key_held;
  B32 menu_bar_focus_press_started;
  
  // rjf: context menu state
  Arena *ctx_menu_arena;
  RD_Regs *ctx_menu_regs;
  RD_RegSlot ctx_menu_regs_slot;
  byte *ctx_menu_input_buffer;
  ulong ctx_menu_input_buffer_size;
  ulong ctx_menu_input_string_size;
  TxtPt ctx_menu_input_cursor;
  TxtPt ctx_menu_input_mark;
  
  // rjf: drop-completion state
  Arena *drop_completion_arena;
  String8List drop_completion_paths;
  
  // rjf: autocomplete lister state
  ulong autocomp_last_frame_idx;
  B32 autocomp_input_dirty;
  UI_Key autocomp_root_key;
  Arena *autocomp_lister_params_arena;
  RD_AutoCompListerParams autocomp_lister_params;
  ulong autocomp_cursor_off;
  byte autocomp_lister_input_buffer[1024];
  ulong autocomp_lister_input_size;
  float autocomp_open_t;
  float autocomp_num_visible_rows_t;
  long autocomp_cursor_num;
  
  // rjf: query view stack
  Arena *query_cmd_arena;
  String8 query_cmd_name;
  RD_Regs *query_cmd_regs;
  ulong query_cmd_regs_mask[(RD_RegSlot_COUNT + 63) / 64];
  RD_View *query_view_stack_top;
  B32 query_view_selected;
  float query_view_selected_t;
  float query_view_t;
  
  // rjf: hover eval state
  B32 hover_eval_focused;
  TxtPt hover_eval_txt_cursor;
  TxtPt hover_eval_txt_mark;
  byte hover_eval_txt_buffer[1024];
  ulong hover_eval_txt_size;
  Arena *hover_eval_arena;
  Vec2F32 hover_eval_spawn_pos;
  String8 hover_eval_string;
  ulong hover_eval_first_frame_idx;
  ulong hover_eval_last_frame_idx;
  String8 hover_eval_file_path;
  TxtPt hover_eval_file_pt;
  ulong hover_eval_vaddr;
  float hover_eval_open_t;
  float hover_eval_num_visible_rows_t;
  
  // rjf: error state
  byte error_buffer[512];
  ulong error_string_size;
  float error_t;
  
  // rjf: panel state
  RD_Panel *root_panel;
  RD_Panel *free_panel;
  RD_Panel *focused_panel;
  
  // rjf: per-frame ui events state
  UI_EventList ui_events;
  
  // rjf: per-frame drawing state
  DR_Bucket *draw_bucket;
};

////////////////////////////////
//~ rjf: Eval Visualization View Cache Types

struct RD_EvalVizViewCacheNode
{
  RD_EvalVizViewCacheNode *next;
  RD_EvalVizViewCacheNode *prev;
  ulong key;
  EV_View *v;
};

struct RD_EvalVizViewCacheSlot
{
  RD_EvalVizViewCacheNode *first;
  RD_EvalVizViewCacheNode *last;
};

////////////////////////////////
//~ rjf: Meta Evaluation Cache Types

struct RD_CtrlEntityMetaEvalCacheNode
{
  RD_CtrlEntityMetaEvalCacheNode *next;
  CTRL_Handle handle;
  CTRL_MetaEval *meval;
  Rng1U64 range;
};

struct RD_CtrlEntityMetaEvalCacheSlot
{
  RD_CtrlEntityMetaEvalCacheNode *first;
  RD_CtrlEntityMetaEvalCacheNode *last;
};

////////////////////////////////
//~ rjf: Main Per-Process Graphical State

struct RD_NameChunkNode
{
  RD_NameChunkNode *next;
  ulong size;
};

struct RD_EntityListCache
{
  Arena *arena;
  ulong alloc_gen;
  RD_EntityList list;
};

struct RD_AmbiguousPathNode
{
  RD_AmbiguousPathNode *next;
  String8 name;
  String8List paths;
};

struct RD_State
{
  // rjf: basics
  Arena *arena;
  B32 quit;
  B32 quit_after_success;
  
  // rjf: log
  Log *log;
  String8 log_path;
  
  // rjf: frame history info
  ulong frame_index;
  Arena *frame_arenas[2];
  ulong frame_time_us_history[64];
  ulong num_frames_requested;
  double time_in_seconds;
  
  // rjf: frame parameters
  float frame_dt;
  DI_Scope *frame_di_scope;
  
  // rjf: dbgi match store
  DI_MatchStore *match_store;
  
  // rjf: ambiguous path table
  ulong ambiguous_path_slots_count;
  RD_AmbiguousPathNode **ambiguous_path_slots;
  
  // rjf: registers stack
  RD_RegsNode base_regs;
  RD_RegsNode *top_regs;
  
  // rjf: autosave state
  float seconds_until_autosave;
  
  // rjf: commands
  Arena *cmds_arenas[2];
  RD_CmdList cmds[2];
  ulong cmds_gen;
  
  // rjf: popup state
  UI_Key popup_key;
  B32 popup_active;
  float popup_t;
  Arena *popup_arena;
  RD_CmdList popup_cmds;
  String8 popup_title;
  String8 popup_desc;
  
  // rjf: text editing mode state
  B32 text_edit_mode;
  
  // rjf: string search state
  Arena *string_search_arena;
  String8 string_search_string;
  
  // rjf: eval visualization view cache
  ulong eval_viz_view_cache_slots_count;
  RD_EvalVizViewCacheSlot *eval_viz_view_cache_slots;
  RD_EvalVizViewCacheNode *eval_viz_view_cache_node_free;
  
  // rjf: ctrl entity meta eval cache
  ulong ctrl_entity_meval_cache_slots_count;
  RD_CtrlEntityMetaEvalCacheSlot *ctrl_entity_meval_cache_slots;
  
  // rjf: contextual hover info
  RD_Regs *hover_regs;
  RD_RegSlot hover_regs_slot;
  RD_Regs *next_hover_regs;
  RD_RegSlot next_hover_regs_slot;
  
  // rjf: icon texture
  R_Handle icon_texture;
  
  // rjf: current path
  Arena *current_path_arena;
  String8 current_path;
  
  // rjf: fixed ui keys
  UI_Key drop_completion_key;
  UI_Key ctx_menu_key;
  
  // rjf: drag/drop state
  Arena *drag_drop_arena;
  RD_Regs *drag_drop_regs;
  RD_RegSlot drag_drop_regs_slot;
  RD_DragDropState drag_drop_state;
  
  // rjf: cfg state
  RD_NameChunkNode *free_name_chunks[8];
  RD_Cfg *free_cfg;
  RD_Cfg *root_cfg;
  
  //-
  // TODO(rjf): TO BE ELIMINATED OR REPLACED VVVVVVVVVVVVVVVV
  //-
  
  // rjf: entity state
  Arena *entities_arena;
  RD_Entity *entities_base;
  ulong entities_count;
  ulong entities_id_gen;
  RD_Entity *entities_root;
  RD_Entity *entities_free[2]; // [0] -> normal lifetime, not user defined; [1] -> user defined lifetime (& thus undoable)
  ulong entities_free_count;
  ulong entities_active_count;
  
  // rjf: entity query caches
  ulong kind_alloc_gens[RD_EntityKind_COUNT];
  RD_EntityListCache kind_caches[RD_EntityKind_COUNT];
  
  // rjf: key map table
  Arena *key_map_arena;
  ulong key_map_table_size;
  RD_KeyMapSlot *key_map_table;
  RD_KeyMapNode *free_key_map_node;
  ulong key_map_total_count;
  
  // rjf: bind change
  Arena *bind_change_arena;
  B32 bind_change_active;
  String8 bind_change_cmd_name;
  RD_Binding bind_change_binding;
  
  // rjf: windows
  RD_Window *first_window;
  RD_Window *last_window;
  RD_Window *free_window;
  ulong window_count;
  B32 last_window_queued_save;
  RD_Handle last_focused_window;
  
  // rjf: view state
  RD_View *first_view;
  RD_View *last_view;
  RD_View *free_view;
  ulong free_view_count;
  ulong allocated_view_count;
  
  // rjf: config reading state
  Arena *cfg_path_arenas[RD_CfgSrc_COUNT];
  String8 cfg_paths[RD_CfgSrc_COUNT];
  ulong cfg_cached_timestamp[RD_CfgSrc_COUNT];
  Arena *cfg_arena;
  RD_CfgTable cfg_table;
  ulong ctrl_exception_code_filters[(CTRL_ExceptionCodeKind_COUNT+63)/64];
  
  // rjf: running theme state
  RD_Theme cfg_theme_target;
  RD_Theme cfg_theme;
  Arena *cfg_main_font_path_arena;
  Arena *cfg_code_font_path_arena;
  String8 cfg_main_font_path;
  String8 cfg_code_font_path;
  FNT_Tag cfg_font_tags[RD_FontSlot_COUNT]; // derivative from font paths
  
  // rjf: static settings
  RD_SettingVal cfg_setting_vals[RD_CfgSrc_COUNT][RD_SettingCode_COUNT];
  
  //-
  // TODO(rjf): TO BE ELIMINATED OR REPLACED ^^^^^^^^^^^^^^^^^^
  //-
};

////////////////////////////////
//~ rjf: Globals

read_only static RD_CfgTree d_nil_cfg_tree = {&d_nil_cfg_tree, RD_CfgSrc_User, &md_nil_node};
read_only static RD_CfgVal d_nil_cfg_val = {&d_nil_cfg_val, &d_nil_cfg_val, &d_nil_cfg_tree, &d_nil_cfg_tree};

read_only static RD_Cfg rd_nil_cfg =
{
  &rd_nil_cfg,
  &rd_nil_cfg,
  &rd_nil_cfg,
  &rd_nil_cfg,
  &rd_nil_cfg,
};

read_only static RD_Entity rd_nil_entity =
{
  &rd_nil_entity,
  &rd_nil_entity,
  &rd_nil_entity,
  &rd_nil_entity,
  &rd_nil_entity,
};

read_only static RD_CmdKindInfo rd_nil_cmd_kind_info = {0};

read_only static RD_ViewRuleInfo rd_nil_view_rule_info =
{
  {0},
  {0},
  {0},
  {0},
  RD_IconKind_Null,
  0,
  EV_VIEW_RULE_EXPR_EXPAND_INFO_FUNCTION_NAME(nil),
  RD_VIEW_RULE_UI_FUNCTION_NAME(null)
};

read_only static RD_View rd_nil_view =
{
  &rd_nil_view,
  &rd_nil_view,
  &rd_nil_view,
  &rd_nil_view,
  &rd_nil_view,
  &rd_nil_view,
  &rd_nil_view_rule_info,
};

read_only static RD_Panel rd_nil_panel =
{
  &rd_nil_panel,
  &rd_nil_panel,
  &rd_nil_panel,
  &rd_nil_panel,
  &rd_nil_panel,
};

static RD_State *rd_state = 0;
static RD_Handle rd_last_drag_drop_panel = {0};
static RD_Handle rd_last_drag_drop_prev_tab = {0};

////////////////////////////////
//~ rjf: Handle Type Pure Functions

RD_Handle rd_handle_zero();
B32 rd_handle_match(RD_Handle a, RD_Handle b);
void rd_handle_list_push_node(RD_HandleList *list, RD_HandleNode *node);
void rd_handle_list_push(Arena *arena, RD_HandleList *list, RD_Handle handle);
RD_HandleList rd_handle_list_copy(Arena *arena, RD_HandleList list);

////////////////////////////////
//~ rjf: Config Type Pure Functions

void rd_cfg_table_push_unparsed_string(Arena *arena, RD_CfgTable *table, String8 string, RD_CfgSrc source);
RD_CfgVal *rd_cfg_val_from_string(RD_CfgTable *table, String8 string);

////////////////////////////////
//~ rjf: Registers Type Functions

void rd_regs_copy_contents(Arena *arena, RD_Regs *dst, RD_Regs *src);
RD_Regs *rd_regs_copy(Arena *arena, RD_Regs *src);

////////////////////////////////
//~ rjf: Commands Type Functions

void rd_cmd_list_push_new(Arena *arena, RD_CmdList *cmds, String8 name, RD_Regs *regs);

////////////////////////////////
//~ rjf: Entity Type Pure Functions

//- rjf: nil
B32 rd_entity_is_nil(RD_Entity *entity);
#define rd_require_entity_nonnil(entity, if_nil_stmts) do{if(rd_entity_is_nil(entity)){if_nil_stmts;}}while(0)

//- rjf: handle <-> entity conversions
ulong rd_index_from_entity(RD_Entity *entity);
RD_Handle rd_handle_from_entity(RD_Entity *entity);
RD_Entity *rd_entity_from_handle(RD_Handle handle);

//- rjf: entity recursion iterators
RD_EntityRec rd_entity_rec_depth_first(RD_Entity *entity, RD_Entity *subtree_root, ulong sib_off, ulong child_off);
#define rd_entity_rec_depth_first_pre(entity, subtree_root)  rd_entity_rec_depth_first((entity), (subtree_root), OffsetOf(RD_Entity, next), OffsetOf(RD_Entity, first))
#define rd_entity_rec_depth_first_post(entity, subtree_root) rd_entity_rec_depth_first((entity), (subtree_root), OffsetOf(RD_Entity, prev), OffsetOf(RD_Entity, last))

//- rjf: ancestor/child introspection
RD_Entity *rd_entity_child_from_kind(RD_Entity *entity, RD_EntityKind kind);

//- rjf: entity list building
void rd_entity_list_push(Arena *arena, RD_EntityList *list, RD_Entity *entity);
RD_EntityArray rd_entity_array_from_list(Arena *arena, RD_EntityList *list);
#define rd_first_entity_from_list(list) ((list)->first != 0 ? (list)->first->entity : &rd_nil_entity)

//- rjf: entity -> color operations
Vec4F32 rd_hsva_from_entity(RD_Entity *entity);
Vec4F32 rd_rgba_from_entity(RD_Entity *entity);

//- rjf: entity -> expansion tree keys
EV_Key rd_ev_key_from_entity(RD_Entity *entity);
EV_Key rd_parent_ev_key_from_entity(RD_Entity *entity);

////////////////////////////////
//~ rjf: View Type Functions

B32 rd_view_is_nil(RD_View *view);
B32 rd_view_is_project_filtered(RD_View *view);
RD_Handle rd_handle_from_view(RD_View *view);
RD_View *rd_view_from_handle(RD_Handle handle);

////////////////////////////////
//~ rjf: View Spec Type Functions

RD_ViewRuleKind rd_view_rule_kind_from_string(String8 string);
RD_ViewRuleInfo *rd_view_rule_info_from_kind(RD_ViewRuleKind kind);
RD_ViewRuleInfo *rd_view_rule_info_from_string(String8 string);

////////////////////////////////
//~ rjf: Panel Type Functions

//- rjf: basic type functions
B32 rd_panel_is_nil(RD_Panel *panel);
RD_Handle rd_handle_from_panel(RD_Panel *panel);
RD_Panel *rd_panel_from_handle(RD_Handle handle);
UI_Key rd_ui_key_from_panel(RD_Panel *panel);

//- rjf: tree construction
void rd_panel_insert(RD_Panel *parent, RD_Panel *prev_child, RD_Panel *new_child);
void rd_panel_remove(RD_Panel *parent, RD_Panel *child);

//- rjf: tree walk
RD_PanelRec rd_panel_rec_depth_first(RD_Panel *panel, ulong sib_off, ulong child_off);
#define rd_panel_rec_depth_first_pre(panel) rd_panel_rec_depth_first(panel, OffsetOf(RD_Panel, next), OffsetOf(RD_Panel, first))
#define rd_panel_rec_depth_first_pre_rev(panel) rd_panel_rec_depth_first(panel, OffsetOf(RD_Panel, prev), OffsetOf(RD_Panel, last))

//- rjf: panel -> rect calculations
Rng2F32 rd_target_rect_from_panel_child(Rng2F32 parent_rect, RD_Panel *parent, RD_Panel *panel);
Rng2F32 rd_target_rect_from_panel(Rng2F32 root_rect, RD_Panel *root, RD_Panel *panel);

//- rjf: view ownership insertion/removal
void rd_panel_insert_tab_view(RD_Panel *panel, RD_View *prev_view, RD_View *view);
void rd_panel_remove_tab_view(RD_Panel *panel, RD_View *view);
RD_View *rd_selected_tab_from_panel(RD_Panel *panel);

//- rjf: icons & display strings
RD_IconKind rd_icon_kind_from_view(RD_View *view);
DR_FancyStringList rd_title_fstrs_from_view(Arena *arena, RD_View *view, Vec4F32 primary_color, Vec4F32 secondary_color, float size);

////////////////////////////////
//~ rjf: Window Type Functions

RD_Handle rd_handle_from_window(RD_Window *window);
RD_Window *rd_window_from_handle(RD_Handle handle);

////////////////////////////////
//~ rjf: Command Parameters From Context

B32 rd_prefer_dasm_from_window(RD_Window *window);

////////////////////////////////
//~ rjf: Global Cross-Window UI Interaction State Functions

B32 rd_drag_is_active();
void rd_drag_begin(RD_RegSlot slot);
B32 rd_drag_drop();
void rd_drag_kill();

void rd_set_hover_regs(RD_RegSlot slot);
RD_Regs *rd_get_hover_regs();

void rd_open_ctx_menu(UI_Key anchor_box_key, Vec2F32 anchor_box_off, RD_RegSlot slot);

////////////////////////////////
//~ rjf: Name Allocation

ulong rd_name_bucket_idx_from_string_size(ulong size);
String8 rd_name_alloc(String8 string);
void rd_name_release(String8 string);

////////////////////////////////
//~ rjf: New Config/Entity Data Structure Functions

RD_Cfg *rd_cfg_alloc();
void rd_cfg_release(RD_Cfg *cfg);
RD_Cfg *rd_cfg_new(RD_Cfg *parent, String8 string);
RD_Cfg *rd_cfg_newf(RD_Cfg *parent, char *fmt, ...);
void rd_cfg_equip_string(RD_Cfg *cfg, String8 string);
void rd_cfg_insert_child(RD_Cfg *parent, RD_Cfg *prev_child, RD_Cfg *new_child);
void rd_cfg_unhook(RD_Cfg *parent, RD_Cfg *child);
RD_Cfg *rd_cfg_child_from_string(RD_Cfg *parent, String8 string);
RD_CfgList rd_cfg_child_list_from_string(Arena *arena, RD_Cfg *parent, String8 string);
RD_CfgList rd_cfg_top_level_list_from_string(Arena *arena, String8 string);
RD_CfgList rd_cfg_tree_list_from_string(Arena *arena, String8 string);
String8 rd_string_from_cfg_tree(Arena *arena, RD_Cfg *cfg);
RD_CfgRec rd_cfg_rec__depth_first(RD_Cfg *root, RD_Cfg *cfg);
void rd_cfg_list_push(Arena *arena, RD_CfgList *list, RD_Cfg *cfg);

////////////////////////////////
//~ rjf: Entity Stateful Functions

//- rjf: entity allocation + tree forming
RD_Entity *rd_entity_alloc(RD_Entity *parent, RD_EntityKind kind);
void rd_entity_mark_for_deletion(RD_Entity *entity);
void rd_entity_release(RD_Entity *entity);
void rd_entity_change_parent(RD_Entity *entity, RD_Entity *old_parent, RD_Entity *new_parent, RD_Entity *prev_child);
RD_Entity *rd_entity_child_from_kind_or_alloc(RD_Entity *entity, RD_EntityKind kind);

//- rjf: entity simple equipment
void rd_entity_equip_txt_pt(RD_Entity *entity, TxtPt point);
void rd_entity_equip_disabled(RD_Entity *entity, B32 b32);
void rd_entity_equip_u64(RD_Entity *entity, ulong u64);
void rd_entity_equip_color_rgba(RD_Entity *entity, Vec4F32 rgba);
void rd_entity_equip_color_hsva(RD_Entity *entity, Vec4F32 hsva);
void rd_entity_equip_cfg_src(RD_Entity *entity, RD_CfgSrc cfg_src);
void rd_entity_equip_timestamp(RD_Entity *entity, ulong timestamp);

//- rjf: control layer correllation equipment
void rd_entity_equip_vaddr(RD_Entity *entity, ulong vaddr);

//- rjf: name equipment
void rd_entity_equip_name(RD_Entity *entity, String8 name);

//- rjf: file path map override lookups
String8 rd_mapped_from_file_path(Arena *arena, String8 file_path);
String8List rd_possible_overrides_from_file_path(Arena *arena, String8 file_path);

//- rjf: top-level state queries
RD_Entity *rd_entity_root();
RD_EntityList rd_push_entity_list_with_kind(Arena *arena, RD_EntityKind kind);
RD_Entity *rd_entity_from_id(RD_EntityID id);
RD_Entity *rd_entity_from_name_and_kind(String8 string, RD_EntityKind kind);

////////////////////////////////
//~ rjf: Frontend Entity Info Extraction

D_Target rd_d_target_from_entity(RD_Entity *entity);
DR_FancyStringList rd_title_fstrs_from_entity(Arena *arena, RD_Entity *entity, Vec4F32 secondary_color, float size);

////////////////////////////////
//~ rjf: Control Entity Info Extraction

Vec4F32 rd_rgba_from_ctrl_entity(CTRL_Entity *entity);
String8 rd_name_from_ctrl_entity(Arena *arena, CTRL_Entity *entity);
DR_FancyStringList rd_title_fstrs_from_ctrl_entity(Arena *arena, CTRL_Entity *entity, Vec4F32 secondary_color, float size, B32 include_extras);

////////////////////////////////
//~ rjf: Evaluation Spaces

//- rjf: entity <-> eval space
RD_Entity *rd_entity_from_eval_space(E_Space space);
E_Space rd_eval_space_from_entity(RD_Entity *entity);

//- rjf: ctrl entity <-> eval space
CTRL_Entity *rd_ctrl_entity_from_eval_space(E_Space space);
E_Space rd_eval_space_from_ctrl_entity(CTRL_Entity *entity, E_SpaceKind kind);

//- rjf: entity -> meta eval
CTRL_MetaEval *rd_ctrl_meta_eval_from_entity(Arena *arena, RD_Entity *entity);
CTRL_MetaEval *rd_ctrl_meta_eval_from_ctrl_entity(Arena *arena, CTRL_Entity *entity);

//- rjf: eval space reads/writes
B32 rd_eval_space_read(void *u, E_Space space, void *out, Rng1U64 range);
B32 rd_eval_space_write(void *u, E_Space space, void *in, Rng1U64 range);

//- rjf: asynchronous streamed reads -> hashes from spaces
U128 rd_key_from_eval_space_range(E_Space space, Rng1U64 range, B32 zero_terminated);

//- rjf: space -> entire range
Rng1U64 rd_whole_range_from_eval_space(E_Space space);

////////////////////////////////
//~ rjf: Evaluation Visualization

//- rjf: writing values back to child processes
B32 rd_commit_eval_value_string(E_Eval dst_eval, String8 string, B32 string_needs_unescaping);

//- rjf: eval / view rule params tree info extraction
ulong rd_base_offset_from_eval(E_Eval eval);
E_Value rd_value_from_params_key(MD_Node *params, String8 key);
Rng1U64 rd_range_from_eval_params(E_Eval eval, MD_Node *params);
TXT_LangKind rd_lang_kind_from_eval_params(E_Eval eval, MD_Node *params);
Arch rd_arch_from_eval_params(E_Eval eval, MD_Node *params);
Vec2S32 rd_dim2s32_from_eval_params(E_Eval eval, MD_Node *params);
R_Tex2DFormat rd_tex2dformat_from_eval_params(E_Eval eval, MD_Node *params);

//- rjf: eval <-> file path
String8 rd_file_path_from_eval_string(Arena *arena, String8 string);
String8 rd_eval_string_from_file_path(Arena *arena, String8 string);

////////////////////////////////
//~ rjf: View State Functions

//- rjf: allocation/releasing
RD_View *rd_view_alloc();
void rd_view_release(RD_View *view);

//- rjf: equipment
void rd_view_equip_spec(RD_View *view, RD_ViewRuleInfo *spec, String8 query, MD_Node *params);
void rd_view_equip_query(RD_View *view, String8 query);
void rd_view_equip_loading_info(RD_View *view, B32 is_loading, ulong progress_v, ulong progress_target);

//- rjf: user state extensions
void *rd_view_get_or_push_user_state(RD_View *view, ulong size);
Arena *rd_view_push_arena_ext(RD_View *view);
#define rd_view_user_state(view, type) (type *)rd_view_get_or_push_user_state((view), sizeof(type))

//- rjf: param saving
void rd_view_store_param(RD_View *view, String8 key, String8 value);
void rd_view_store_paramf(RD_View *view, String8 key, char *fmt, ...);
#define rd_view_store_param_f32(view, key, f32) rd_view_store_paramf((view), (key), "%ff", (f32))
#define rd_view_store_param_s64(view, key, s64) rd_view_store_paramf((view), (key), "%I64d", (s64))
#define rd_view_store_param_u64(view, key, u64) rd_view_store_paramf((view), (key), "0x%I64x", (u64))

////////////////////////////////
//~ rjf: View Building API

//- rjf: view info extraction
Arena *rd_view_arena();
UI_ScrollPt2 rd_view_scroll_pos();
String8 rd_view_expr_string();
String8 rd_view_filter();

//- rjf: pushing/attaching view resources
void *rd_view_state_by_size(ulong size);
#define rd_view_state(T) (T *)rd_view_state_by_size(sizeof(T))
Arena *rd_push_view_arena();

//- rjf: storing view-attached state
void rd_store_view_expr_string(String8 string);
void rd_store_view_filter(String8 string);
void rd_store_view_loading_info(B32 is_loading, ulong progress_u64, ulong progress_u64_target);
void rd_store_view_scroll_pos(UI_ScrollPt2 pos);
void rd_store_view_param(String8 key, String8 value);
void rd_store_view_paramf(String8 key, char *fmt, ...);
#define rd_store_view_param_f32(key, f32) rd_store_view_paramf((key), "%ff", (f32))
#define rd_store_view_param_s64(key, s64) rd_store_view_paramf((key), "%I64d", (s64))
#define rd_store_view_param_u64(key, u64) rd_store_view_paramf((key), "0x%I64x", (u64))

////////////////////////////////
//~ rjf: Expand-Keyed Transient View Functions

RD_TransientViewNode *rd_transient_view_node_from_ev_key(RD_View *owner_view, EV_Key key);

////////////////////////////////
//~ rjf: Panel State Functions

RD_Panel *rd_panel_alloc(RD_Window *ws);
void rd_panel_release(RD_Window *ws, RD_Panel *panel);
void rd_panel_release_all_views(RD_Panel *panel);

////////////////////////////////
//~ rjf: Window State Functions

RD_Window *rd_window_open(Vec2F32 size, OS_Handle preferred_monitor, RD_CfgSrc cfg_src);

RD_Window *rd_window_from_os_handle(OS_Handle os);

void rd_window_frame(RD_Window *ws);

////////////////////////////////
//~ rjf: Eval Visualization

EV_ExpandInfo      rd_ev_view_rule_expr_expand_info__meta_entities(Arena *arena, EV_View *view, String8 filter, E_Expr *expr, MD_Node *params, RD_EntityKind kind);
EV_ExpandRangeInfo rd_ev_view_rule_expr_expand_range_info__meta_entities(Arena *arena, EV_View *view, String8 filter, E_Expr *expr, MD_Node *params, Rng1U64 idx_range, void *user_data, RD_EntityKind kind, B32 add_new_at_top);
ulong                rd_ev_view_rule_expr_id_from_num__meta_entities(ulong num, void *user_data, RD_EntityKind kind, B32 add_new_at_top);
ulong                rd_ev_view_rule_expr_num_from_id__meta_entities(ulong id, void *user_data, RD_EntityKind kind, B32 add_new_at_top);

EV_ExpandInfo      rd_ev_view_rule_expr_expand_info__meta_ctrl_entities(Arena *arena, EV_View *view, String8 filter, E_Expr *expr, MD_Node *params, CTRL_EntityKind kind);
EV_ExpandRangeInfo rd_ev_view_rule_expr_expand_range_info__meta_ctrl_entities(Arena *arena, EV_View *view, String8 filter, E_Expr *expr, MD_Node *params, Rng1U64 idx_range, void *user_data, CTRL_EntityKind kind);
ulong                rd_ev_view_rule_expr_id_from_num__meta_ctrl_entities(ulong num, void *user_data, CTRL_EntityKind kind);
ulong                rd_ev_view_rule_expr_num_from_id__meta_ctrl_entities(ulong id, void *user_data, CTRL_EntityKind kind);

EV_ExpandInfo      rd_ev_view_rule_expr_expand_info__debug_info_tables(Arena *arena, EV_View *view, String8 filter, E_Expr *expr, MD_Node *params, RDI_SectionKind section);
EV_ExpandRangeInfo rd_ev_view_rule_expr_expand_range_info__debug_info_tables(Arena *arena, EV_View *view, String8 filter, E_Expr *expr, MD_Node *params, Rng1U64 idx_range, void *user_data, RDI_SectionKind section);
ulong                rd_ev_view_rule_expr_id_from_num__debug_info_tables(ulong num, void *user_data, RDI_SectionKind section);
ulong                rd_ev_view_rule_expr_num_from_id__debug_info_tables(ulong id, void *user_data, RDI_SectionKind section);

EV_View *rd_ev_view_from_key(ulong key);
float rd_append_value_strings_from_eval(Arena *arena, EV_StringFlags flags, uint default_radix, FNT_Tag font, float font_size, float max_size, int depth, E_Eval eval, E_Member *member, EV_ViewRuleList *view_rules, String8List *out);
String8 rd_value_string_from_eval(Arena *arena, EV_StringFlags flags, uint default_radix, FNT_Tag font, float font_size, float max_size, E_Eval eval, E_Member *member, EV_ViewRuleList *view_rules);

////////////////////////////////
//~ rjf: Hover Eval

void rd_set_hover_eval(Vec2F32 pos, String8 file_path, TxtPt pt, ulong vaddr, String8 string);

////////////////////////////////
//~ rjf: Auto-Complete Lister

void rd_autocomp_lister_item_chunk_list_push(Arena *arena, RD_AutoCompListerItemChunkList *list, ulong cap, RD_AutoCompListerItem *item);
RD_AutoCompListerItemArray rd_autocomp_lister_item_array_from_chunk_list(Arena *arena, RD_AutoCompListerItemChunkList *list);
int rd_autocomp_lister_item_qsort_compare(RD_AutoCompListerItem *a, RD_AutoCompListerItem *b);
void rd_autocomp_lister_item_array_sort__in_place(RD_AutoCompListerItemArray *array);

String8 rd_autocomp_query_word_from_input_string_off(String8 input, ulong cursor_off);
String8 rd_autocomp_query_path_from_input_string_off(String8 input, ulong cursor_off);
RD_AutoCompListerParams rd_view_rule_autocomp_lister_params_from_input_cursor(Arena *arena, String8 string, ulong cursor_off);
void rd_set_autocomp_lister_query(UI_Key root_key, RD_AutoCompListerParams *params, String8 input, ulong cursor_off);

////////////////////////////////
//~ rjf: Search Strings

void rd_set_search_string(String8 string);
String8 rd_push_search_string(Arena *arena);

////////////////////////////////
//~ rjf: Colors, Fonts, Config

//- rjf: keybindings
OS_Key rd_os_key_from_cfg_string(String8 string);
void rd_clear_bindings();
RD_BindingList rd_bindings_from_name(Arena *arena, String8 name);
void rd_bind_name(String8 name, RD_Binding binding);
void rd_unbind_name(String8 name, RD_Binding binding);
String8List rd_cmd_name_list_from_binding(Arena *arena, RD_Binding binding);

//- rjf: colors
Vec4F32 rd_rgba_from_theme_color(RD_ThemeColor color);
RD_ThemeColor rd_theme_color_from_txt_token_kind(TXT_TokenKind kind);
RD_ThemeColor rd_theme_color_from_txt_token_kind_lookup_string(TXT_TokenKind kind, String8 string);

//- rjf: code -> palette
UI_Palette *rd_palette_from_code(RD_PaletteCode code);

//- rjf: fonts/sizes
FNT_Tag rd_font_from_slot(RD_FontSlot slot);
float rd_font_size_from_slot(RD_FontSlot slot);
FNT_RasterFlags rd_raster_flags_from_slot(RD_FontSlot slot);

//- rjf: settings
RD_SettingVal rd_setting_val_from_code(RD_SettingCode code);

//- rjf: config serialization
int rd_qsort_compare__cfg_string_bindings(RD_StringBindingPair *a, RD_StringBindingPair *b);
String8List rd_cfg_strings_from_gfx(Arena *arena, String8 root_path, RD_CfgSrc source);

////////////////////////////////
//~ rjf: Process Control Info Stringification

String8 rd_string_from_exception_code(uint code);
DR_FancyStringList rd_stop_explanation_fstrs_from_ctrl_event(Arena *arena, CTRL_Event *event);

////////////////////////////////
//~ rjf: Continuous Frame Requests

void rd_request_frame();

////////////////////////////////
//~ rjf: Main State Accessors

//- rjf: per-frame arena
Arena *rd_frame_arena();

//- rjf: config paths
String8 rd_cfg_path_from_src(RD_CfgSrc src);

//- rjf: entity cache queries
RD_EntityList rd_query_cached_entity_list_with_kind(RD_EntityKind kind);
RD_EntityList rd_push_active_target_list(Arena *arena);
RD_Entity *rd_entity_from_ev_key_and_kind(EV_Key key, RD_EntityKind kind);

//- rjf: config state
RD_CfgTable *rd_cfg_table();

////////////////////////////////
//~ rjf: Registers

RD_Regs *rd_regs();
RD_Regs *rd_base_regs();
RD_Regs *rd_push_regs_(RD_Regs *regs);
#define rd_push_regs(...) rd_push_regs_(&(RD_Regs){rd_regs_lit_init_top __VA_ARGS__})
RD_Regs *rd_pop_regs();
#define RD_RegsScope(...) DeferLoop(rd_push_regs(__VA_ARGS__), rd_pop_regs())
void rd_regs_fill_slot_from_string(RD_RegSlot slot, String8 string);

////////////////////////////////
//~ rjf: Commands

//- rjf: name -> info
RD_CmdKind rd_cmd_kind_from_string(String8 string);
RD_CmdKindInfo *rd_cmd_kind_info_from_string(String8 string);

//- rjf: pushing
void rd_push_cmd(String8 name, RD_Regs *regs);
#define rd_cmd(kind, ...) rd_push_cmd(rd_cmd_kind_info_table[kind].string, &(RD_Regs){rd_regs_lit_init_top __VA_ARGS__})

//- rjf: iterating
B32 rd_next_cmd(RD_Cmd **cmd);

////////////////////////////////
//~ rjf: Main Layer Top-Level Calls

void rd_init(CmdLine *cmdln);
void rd_frame();

#endif // RADDBG_CORE_H

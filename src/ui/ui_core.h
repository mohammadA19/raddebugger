// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Icon Info

enum UI_IconKind
{
  UI_IconKind_Null,
  UI_IconKind_RightArrow,
  UI_IconKind_DownArrow,
  UI_IconKind_LeftArrow,
  UI_IconKind_UpArrow,
  UI_IconKind_RightCaret,
  UI_IconKind_DownCaret,
  UI_IconKind_LeftCaret,
  UI_IconKind_UpCaret,
  UI_IconKind_CheckHollow,
  UI_IconKind_CheckFilled,
  UI_IconKind_COUNT
}

struct UI_IconInfo
{
  FNT_Tag icon_font;
  String8 icon_kind_text_map[UI_IconKind_COUNT];
}

////////////////////////////////
//~ rjf: Mouse Button Kinds

enum UI_MouseButtonKind
{
  UI_MouseButtonKind_Left,
  UI_MouseButtonKind_Middle,
  UI_MouseButtonKind_Right,
  UI_MouseButtonKind_COUNT
}

////////////////////////////////
//~ rjf: Codepath Permissions

enum UI_PermissionFlags : U32
{
  UI_PermissionFlag_ClicksLeft        = (1<<0),
  UI_PermissionFlag_ClicksMiddle      = (1<<1),
  UI_PermissionFlag_ClicksRight       = (1<<2),
  UI_PermissionFlag_ScrollX           = (1<<3),
  UI_PermissionFlag_ScrollY           = (1<<4),
  UI_PermissionFlag_KeyboardPrimary   = (1<<5),
  UI_PermissionFlag_KeyboardSecondary = (1<<6),
  UI_PermissionFlag_Text              = (1<<7),
  
  //- rjf bundles
  UI_PermissionFlag_Keyboard = (UI_PermissionFlag_KeyboardPrimary|UI_PermissionFlag_KeyboardSecondary),
  UI_PermissionFlag_Clicks = (UI_PermissionFlag_ClicksLeft|UI_PermissionFlag_ClicksMiddle|UI_PermissionFlag_ClicksRight),
  UI_PermissionFlag_All = 0xffffffff,
}

////////////////////////////////
//~ rjf: Focus Types

enum UI_FocusKind
{
  UI_FocusKind_Null,
  UI_FocusKind_Off,
  UI_FocusKind_On,
  UI_FocusKind_Root,
  UI_FocusKind_COUNT
}

////////////////////////////////
//~ rjf: Events

// TODO(rjf): clean all this up

enum UI_EventKind
{
  UI_EventKind_Null,
  UI_EventKind_Press,
  UI_EventKind_Release,
  UI_EventKind_Text,
  UI_EventKind_Navigate,
  UI_EventKind_Edit,
  UI_EventKind_MouseMove,
  UI_EventKind_Scroll,
  UI_EventKind_FileDrop,
  UI_EventKind_COUNT
}

enum UI_EventActionSlot
{
  UI_EventActionSlot_Null,
  UI_EventActionSlot_Accept,
  UI_EventActionSlot_Cancel,
  UI_EventActionSlot_Edit,
  UI_EventActionSlot_COUNT
}

enum UI_EventFlags : U32
{
  UI_EventFlag_KeepMark            = (1<<0),
  UI_EventFlag_Delete              = (1<<1),
  UI_EventFlag_Copy                = (1<<2),
  UI_EventFlag_Paste               = (1<<3),
  UI_EventFlag_ZeroDeltaOnSelect   = (1<<4),
  UI_EventFlag_PickSelectSide      = (1<<5),
  UI_EventFlag_CapAtLine           = (1<<6),
  UI_EventFlag_ExplicitDirectional = (1<<7),
  UI_EventFlag_Reorder             = (1<<8),
  UI_EventFlag_Secondary           = (1<<9),
}

enum UI_EventDeltaUnit
{
  UI_EventDeltaUnit_Null,
  UI_EventDeltaUnit_Char,
  UI_EventDeltaUnit_Word,
  UI_EventDeltaUnit_Line,
  UI_EventDeltaUnit_Page,
  UI_EventDeltaUnit_Whole,
  UI_EventDeltaUnit_COUNT
}

struct UI_Event
{
  UI_EventKind kind;
  UI_EventActionSlot slot;
  UI_EventFlags flags;
  UI_EventDeltaUnit delta_unit;
  OS_Key key;
  OS_Modifiers modifiers;
  String8 string;
  String8List paths;
  Vec2F32 pos;
  Vec2F32 delta_2f32;
  Vec2S32 delta_2s32;
  U64 timestamp_us;
}

struct UI_EventNode
{
  UI_EventNode *next;
  UI_EventNode *prev;
  UI_Event v;
}

struct UI_EventList
{
  UI_EventNode *first;
  UI_EventNode *last;
  U64 count;
}

////////////////////////////////
//~ rjf: Textual Operations

enum UI_TxtOpFlags : U32
{
  UI_TxtOpFlag_Invalid = (1<<0),
  UI_TxtOpFlag_Copy    = (1<<1),
}

struct UI_TxtOp
{
  UI_TxtOpFlags flags;
  String8 replace;
  String8 copy;
  TxtRng range;
  TxtPt cursor;
  TxtPt mark;
}

////////////////////////////////
//~ rjf: Keys

struct UI_Key
{
  U64 u64[1];
}

////////////////////////////////
//~ rjf: Sizes

enum UI_SizeKind
{
  UI_SizeKind_Null,
  UI_SizeKind_Pixels,      // size is computed via a preferred pixel value
  UI_SizeKind_TextContent, // size is computed via the dimensions of box's rendered string
  UI_SizeKind_ParentPct,   // size is computed via a well-determined parent or grandparent size
  UI_SizeKind_ChildrenSum, // size is computed via summing well-determined sizes of children
}

struct UI_Size
{
  UI_SizeKind kind;
  F32 value;
  F32 strictness;
}

////////////////////////////////
//~ rjf: Themes

struct UI_ThemePattern
{
  String8Array tags;
  Vec4F32 linear;
}

struct UI_Theme
{
  UI_ThemePattern *patterns;
  U64 patterns_count;
}

////////////////////////////////
//~ rjf: Animation Info

struct UI_AnimationInfo
{
  F32 hot_animation_rate;
  F32 active_animation_rate;
  F32 focus_animation_rate;
  F32 tooltip_animation_rate;
  F32 menu_animation_rate;
  F32 scroll_animation_rate;
}

////////////////////////////////
//~ rjf: Scroll Positions

struct UI_ScrollPt
{
  S64 idx;
  F32 off;
}

typedef union UI_ScrollPt2 UI_ScrollPt2;
union UI_ScrollPt2
{
  UI_ScrollPt v[2];
  struct
  {
    UI_ScrollPt x;
    UI_ScrollPt y;
  };
}

////////////////////////////////
//~ rjf: Box Types

enum UI_TextAlign
{
  UI_TextAlign_Left,
  UI_TextAlign_Center,
  UI_TextAlign_Right,
  UI_TextAlign_COUNT
}

#define UI_BOX_CUSTOM_DRAW(name) void name(struct UI_Box *box, void *user_data)
typedef UI_BOX_CUSTOM_DRAW(UI_BoxCustomDrawFunctionType);

enum UI_BoxFlags : U64
{
  //- rjf: interaction
  UI_BoxFlag_MouseClickable            = (1 << 0),
  UI_BoxFlag_KeyboardClickable         = (1 << 1),
  UI_BoxFlag_DropSite                  = (1 << 2),
  UI_BoxFlag_ClickToFocus              = (1 << 3),
  UI_BoxFlag_Scroll                    = (1 << 4),
  UI_BoxFlag_ViewScrollX               = (1 << 5),
  UI_BoxFlag_ViewScrollY               = (1 << 6),
  UI_BoxFlag_ViewClampX                = (1 << 7),
  UI_BoxFlag_ViewClampY                = (1 << 8),
  UI_BoxFlag_FocusHot                  = (1 << 9),
  UI_BoxFlag_FocusActive               = (1 << 10),
  UI_BoxFlag_FocusHotDisabled          = (1 << 11),
  UI_BoxFlag_FocusActiveDisabled       = (1 << 12),
  UI_BoxFlag_DefaultFocusNavX          = (1 << 13),
  UI_BoxFlag_DefaultFocusNavY          = (1 << 14),
  UI_BoxFlag_DefaultFocusEdit          = (1 << 15),
  UI_BoxFlag_FocusNavSkip              = (1 << 16),
  UI_BoxFlag_DisableTruncatedHover     = (1 << 17),
  UI_BoxFlag_Disabled                  = (1 << 18),

  //- rjf: layout
  UI_BoxFlag_FloatingX                 = (1 << 19),
  UI_BoxFlag_FloatingY                 = (1 << 20),
  UI_BoxFlag_FixedWidth                = (1 << 21),
  UI_BoxFlag_FixedHeight               = (1 << 22),
  UI_BoxFlag_AllowOverflowX            = (1 << 23),
  UI_BoxFlag_AllowOverflowY            = (1 << 24),
  UI_BoxFlag_SkipViewOffX              = (1 << 25),
  UI_BoxFlag_SkipViewOffY              = (1 << 26),

  //- rjf: appearance / animation
  UI_BoxFlag_DrawDropShadow            = (1 << 27),
  UI_BoxFlag_DrawBackgroundBlur        = (1 << 28),
  UI_BoxFlag_DrawBackground            = (1 << 29),
  UI_BoxFlag_DrawBorder                = (1 << 30),
  UI_BoxFlag_DrawSideTop               = (1 << 31),
  UI_BoxFlag_DrawSideBottom            = (1 << 32),
  UI_BoxFlag_DrawSideLeft              = (1 << 33),
  UI_BoxFlag_DrawSideRight             = (1 << 34),
  UI_BoxFlag_DrawText                  = (1 << 35),
  UI_BoxFlag_DrawTextFastpathCodepoint = (1 << 36),
  UI_BoxFlag_DrawTextWeak              = (1 << 37),
  UI_BoxFlag_DrawHotEffects            = (1 << 38),
  UI_BoxFlag_DrawActiveEffects         = (1 << 39),
  UI_BoxFlag_DrawOverlay               = (1 << 40),
  UI_BoxFlag_DrawBucket                = (1 << 41),
  UI_BoxFlag_Clip                      = (1 << 42),
  UI_BoxFlag_AnimatePosX               = (1 << 43),
  UI_BoxFlag_AnimatePosY               = (1 << 44),
  UI_BoxFlag_DisableTextTrunc          = (1 << 45),
  UI_BoxFlag_DisableIDString           = (1 << 46),
  UI_BoxFlag_DisableFocusBorder        = (1 << 47),
  UI_BoxFlag_DisableFocusOverlay       = (1 << 48),
  UI_BoxFlag_HasDisplayString          = (1 << 49),
  UI_BoxFlag_HasFuzzyMatchRanges       = (1 << 50),
  UI_BoxFlag_RoundChildrenByParent     = (1 << 51),
  UI_BoxFlag_SquishAnchored            = (1 << 52),

  //- rjf: debug
  UI_BoxFlag_Debug                     = (1 << 53),

  //- rjf: bundles
  UI_BoxFlag_Clickable           = (UI_BoxFlag_MouseClickable|UI_BoxFlag_KeyboardClickable),
  UI_BoxFlag_DefaultFocusNav     = (UI_BoxFlag_DefaultFocusNavX|UI_BoxFlag_DefaultFocusNavY|UI_BoxFlag_DefaultFocusEdit),
  UI_BoxFlag_Floating            = (UI_BoxFlag_FloatingX|UI_BoxFlag_FloatingY),
  UI_BoxFlag_FixedSize           = (UI_BoxFlag_FixedWidth|UI_BoxFlag_FixedHeight),
  UI_BoxFlag_AllowOverflow       = (UI_BoxFlag_AllowOverflowX|UI_BoxFlag_AllowOverflowY),
  UI_BoxFlag_AnimatePos          = (UI_BoxFlag_AnimatePosX|UI_BoxFlag_AnimatePosY),
  UI_BoxFlag_ViewScroll          = (UI_BoxFlag_ViewScrollX|UI_BoxFlag_ViewScrollY),
  UI_BoxFlag_ViewClamp           = (UI_BoxFlag_ViewClampX|UI_BoxFlag_ViewClampY),
  UI_BoxFlag_DisableFocusEffects = (UI_BoxFlag_DisableFocusBorder|UI_BoxFlag_DisableFocusOverlay),
}

struct UI_Box
{
  //- rjf: persistent links
  UI_Box *hash_next;
  UI_Box *hash_prev;
  
  //- rjf: per-build links/data
  UI_Box *first;
  UI_Box *last;
  UI_Box *next;
  UI_Box *prev;
  UI_Box *parent;
  U64 child_count;
  
  //- rjf: per-build equipment
  UI_Key key;
  UI_BoxFlags flags;
  UI_Key tags_key;
  String8 string;
  UI_TextAlign text_align;
  Vec2F32 fixed_position;
  Vec2F32 fixed_size;
  Vec2F32 min_size;
  UI_Size pref_size[Axis2_COUNT];
  Axis2 child_layout_axis;
  OS_Cursor hover_cursor;
  U32 fastpath_codepoint;
  UI_Key group_key;
  DR_Bucket *draw_bucket;
  UI_BoxCustomDrawFunctionType *custom_draw;
  void *custom_draw_user_data;
  Vec4F32 background_color;
  Vec4F32 text_color;
  Vec4F32 border_color;
  FNT_Tag font;
  F32 font_size;
  F32 tab_size;
  FNT_RasterFlags text_raster_flags;
  F32 corner_radii[Corner_COUNT];
  F32 blur_size;
  F32 transparency;
  F32 squish;
  F32 text_padding;
  
  //- rjf: per-build artifacts
  DR_FStrList display_fstrs;
  DR_FRunList display_fruns;
  Rng2F32 rect;
  Vec2F32 fixed_position_animated;
  Vec2F32 position_delta;
  FuzzyMatchRangeList fuzzy_match_ranges;
  
  //- rjf: persistent data
  U64 first_touched_build_index;
  U64 last_touched_build_index;
  U64 first_disabled_build_index;
  F32 hot_t;
  F32 active_t;
  F32 disabled_t;
  F32 focus_hot_t;
  F32 focus_active_t;
  F32 focus_active_disabled_t;
  Vec2F32 view_off;
  Vec2F32 view_off_target;
  Vec2F32 view_bounds;
  UI_Key default_nav_focus_hot_key;
  UI_Key default_nav_focus_active_key;
  UI_Key default_nav_focus_next_hot_key;
  UI_Key default_nav_focus_next_active_key;
}

struct UI_BoxRec
{
  UI_Box *next;
  S32 push_count;
  S32 pop_count;
}

struct UI_BoxNode
{
  UI_BoxNode *next;
  UI_Box *box;
}

struct UI_BoxList
{
  UI_BoxNode *first;
  UI_BoxNode *last;
  U64 count;
}

enum UI_SignalFlags : U32
{
  // rjf: mouse press -> box was pressed while hovering
  UI_SignalFlag_LeftPressed         = (1<<0),
  UI_SignalFlag_MiddlePressed       = (1<<1),
  UI_SignalFlag_RightPressed        = (1<<2),
  
  // rjf: dragging -> box was previously pressed, user is still holding button
  UI_SignalFlag_LeftDragging        = (1<<3),
  UI_SignalFlag_MiddleDragging      = (1<<4),
  UI_SignalFlag_RightDragging       = (1<<5),
  
  // rjf: double-dragging -> box was previously double-clicked, user is still holding button
  UI_SignalFlag_LeftDoubleDragging  = (1<<6),
  UI_SignalFlag_MiddleDoubleDragging= (1<<7),
  UI_SignalFlag_RightDoubleDragging = (1<<8),
  
  // rjf: triple-dragging -> box was previously triple-clicked, user is still holding button
  UI_SignalFlag_LeftTripleDragging  = (1<<9),
  UI_SignalFlag_MiddleTripleDragging= (1<<10),
  UI_SignalFlag_RightTripleDragging = (1<<11),
  
  // rjf: released -> box was previously pressed & user released, in or out of bounds
  UI_SignalFlag_LeftReleased        = (1<<12),
  UI_SignalFlag_MiddleReleased      = (1<<13),
  UI_SignalFlag_RightReleased       = (1<<14),
  
  // rjf: clicked -> box was previously pressed & user released, in bounds
  UI_SignalFlag_LeftClicked         = (1<<15),
  UI_SignalFlag_MiddleClicked       = (1<<16),
  UI_SignalFlag_RightClicked        = (1<<17),
  
  // rjf: double clicked -> box was previously clicked, pressed again
  UI_SignalFlag_LeftDoubleClicked   = (1<<18),
  UI_SignalFlag_MiddleDoubleClicked = (1<<19),
  UI_SignalFlag_RightDoubleClicked  = (1<<20),
  
  // rjf: triple clicked -> box was previously clicked twice, pressed again
  UI_SignalFlag_LeftTripleClicked   = (1<<21),
  UI_SignalFlag_MiddleTripleClicked = (1<<22),
  UI_SignalFlag_RightTripleClicked  = (1<<23),
  
  // rjf: keyboard pressed -> box had focus, user activated via their keyboard
  UI_SignalFlag_KeyboardPressed     = (1<<24),
  
  // rjf: passive mouse info
  UI_SignalFlag_Hovering            = (1<<25), // hovering specifically this box
  UI_SignalFlag_MouseOver           = (1<<26), // mouse is over, but may be occluded
  
  // rjf: committing state changes via user interaction
  UI_SignalFlag_Commit              = (1<<27),
  
  // rjf: high-level combos
  UI_SignalFlag_Pressed = UI_SignalFlag_LeftPressed|UI_SignalFlag_KeyboardPressed,
  UI_SignalFlag_Released = UI_SignalFlag_LeftReleased,
  UI_SignalFlag_Clicked = UI_SignalFlag_LeftClicked|UI_SignalFlag_KeyboardPressed,
  UI_SignalFlag_DoubleClicked = UI_SignalFlag_LeftDoubleClicked,
  UI_SignalFlag_TripleClicked = UI_SignalFlag_LeftTripleClicked,
  UI_SignalFlag_Dragging = UI_SignalFlag_LeftDragging,
}

struct UI_Signal
{
  UI_Box *box;
  OS_Modifiers event_flags;
  Vec2S16 scroll;
  UI_SignalFlags f;
}

#define ui_pressed(s)        !!((s).f&UI_SignalFlag_Pressed)
#define ui_clicked(s)        !!((s).f&UI_SignalFlag_Clicked)
#define ui_released(s)       !!((s).f&UI_SignalFlag_Released)
#define ui_double_clicked(s) !!((s).f&UI_SignalFlag_DoubleClicked)
#define ui_triple_clicked(s) !!((s).f&UI_SignalFlag_TripleClicked)
#define ui_middle_clicked(s) !!((s).f&UI_SignalFlag_MiddleClicked)
#define ui_right_clicked(s)  !!((s).f&UI_SignalFlag_RightClicked)
#define ui_dragging(s)       !!((s).f&UI_SignalFlag_Dragging)
#define ui_hovering(s)       !!((s).f&UI_SignalFlag_Hovering)
#define ui_mouse_over(s)     !!((s).f&UI_SignalFlag_MouseOver)
#define ui_committed(s)      !!((s).f&UI_SignalFlag_Commit)

struct UI_Nav
{
  B32 moved;
  Vec2S64 new_p;
}

////////////////////////////////
//~ rjf: Animation State Types

struct UI_AnimParams
{
  F32 initial;
  F32 target;
  F32 rate;
  F32 epsilon;
  B32 reset;
}

struct UI_AnimNode
{
  UI_AnimNode *slot_next;
  UI_AnimNode *slot_prev;
  UI_AnimNode *lru_next;
  UI_AnimNode *lru_prev;
  U64 first_touched_build_index;
  U64 last_touched_build_index;
  UI_Key key;
  UI_AnimParams params;
  F32 current;
}

struct UI_AnimSlot
{
  UI_AnimNode *first;
  UI_AnimNode *last;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/ui.meta.h"

////////////////////////////////
//~ rjf: State Types

//- rjf: cache for mapping 64-bit key -> array of tags

struct UI_TagsCacheNode
{
  UI_TagsCacheNode *next;
  UI_Key key;
  String8Array tags;
}

struct UI_TagsCacheSlot
{
  UI_TagsCacheNode *first;
  UI_TagsCacheNode *last;
}

struct UI_TagsKeyStackNode
{
  UI_TagsKeyStackNode *next;
  UI_Key key;
}

//- rjf: cache for mapping 64-bit key -> theme pattern

struct UI_ThemePatternCacheNode
{
  UI_ThemePatternCacheNode *slot_next;
  UI_ThemePatternCacheNode *slot_prev;
  UI_ThemePatternCacheNode *lru_next;
  UI_ThemePatternCacheNode *lru_prev;
  U64 last_build_index_accessed;
  UI_Key key;
  Vec4F32 target_rgba;
  Vec4F32 current_rgba;
}

struct UI_ThemePatternCacheSlot
{
  UI_ThemePatternCacheNode *first;
  UI_ThemePatternCacheNode *last;
}

//- rjf: cache for mapping 64-bit key -> box

struct UI_BoxHashSlot
{
  UI_Box *hash_first;
  UI_Box *hash_last;
}

//- rjf: main state bundle

struct UI_State
{
  //- rjf: main arena
  Arena *arena;
  
  //- rjf: fixed keys
  UI_Key external_key;
  
  //- rjf: build arenas
  Arena *build_arenas[2];
  U64 build_index;
  
  //- rjf: box cache
  UI_Box *first_free_box;
  U64 box_table_size;
  UI_BoxHashSlot *box_table;
  
  //- rjf: anim cache
  UI_AnimNode *free_anim_node;
  UI_AnimNode *lru_anim_node;
  UI_AnimNode *mru_anim_node;
  U64 anim_slots_count;
  UI_AnimSlot *anim_slots;
  
  //- rjf: build state machine state
  B32 is_in_open_ctx_menu;
  String8 autocomplete_string;
  B32 tooltip_can_overflow_window;
  UI_Key tooltip_anchor_key;
  String8Array current_gen_tags;
  U64 current_gen_tags_gen;
  UI_TagsKeyStackNode *tags_key_stack_top;
  UI_TagsKeyStackNode *tags_key_stack_free;
  U64 tags_cache_slots_count;
  UI_TagsCacheSlot *tags_cache_slots;
  
  //- rjf: theme pattern cache
  U64 theme_pattern_cache_slots_count;
  UI_ThemePatternCacheSlot *theme_pattern_cache_slots;
  UI_ThemePatternCacheNode *theme_pattern_cache_node_free;
  UI_ThemePatternCacheNode *lru_theme_pattern_cache_node;
  UI_ThemePatternCacheNode *mru_theme_pattern_cache_node;
  
  //- rjf: build phase output
  UI_Box *root;
  UI_Box *tooltip_root;
  UI_Box *ctx_menu_root;
  UI_Key default_nav_root_key;
  U64 build_box_count;
  U64 last_build_box_count;
  B32 ctx_menu_touched_this_frame;
  B32 is_animating;
  
  //- rjf: build parameters
  UI_IconInfo icon_info;
  UI_Theme *theme;
  UI_AnimationInfo animation_info;
  OS_Handle window;
  UI_EventList *events;
  Vec2F32 mouse;
  F32 animation_dt;
  F32 default_animation_rate;
  
  //- rjf: user interaction state
  UI_Key hot_box_key;
  UI_Key active_box_key[UI_MouseButtonKind_COUNT];
  UI_Key drop_hot_box_key;
  UI_Key clipboard_copy_key;
  U64 press_timestamp_history_us[UI_MouseButtonKind_COUNT][3];
  UI_Key press_key_history[UI_MouseButtonKind_COUNT][3];
  Vec2F32 press_pos_history[UI_MouseButtonKind_COUNT][3];
  Vec2F32 drag_start_mouse;
  Arena *drag_state_arena;
  String8 drag_state_data;
  Arena *string_hover_arena;
  String8 string_hover_string;
  F32 string_hover_size;
  DR_FStrList string_hover_fstrs;
  U64 string_hover_begin_us;
  U64 string_hover_build_index;
  U64 last_time_mousemoved_us;
  
  //- rjf: tooltip state
  F32 tooltip_open_t;
  B32 tooltip_open;
  
  //- rjf: context menu state
  UI_Key ctx_menu_anchor_key;
  UI_Key next_ctx_menu_anchor_key;
  Vec2F32 ctx_menu_anchor_box_last_pos;
  Vec2F32 ctx_menu_anchor_off;
  B32 ctx_menu_open;
  B32 next_ctx_menu_open;
  F32 ctx_menu_open_t;
  UI_Key ctx_menu_key;
  B32 ctx_menu_changed;
  
  //- rjf: build phase stacks
  UI_DeclStackNils;
  UI_DeclStacks;
}

////////////////////////////////
//~ rjf: Size Type Functions

#define ui_px(value, strictness)         ui_size(UI_SizeKind_Pixels, value, strictness)
#define ui_em(value, strictness)         ui_size(UI_SizeKind_Pixels, (value) * ui_top_font_size(), strictness)
#define ui_text_dim(padding, strictness) ui_size(UI_SizeKind_TextContent, padding, strictness)
#define ui_pct(value, strictness)        ui_size(UI_SizeKind_ParentPct, value, strictness)
#define ui_children_sum(strictness)      ui_size(UI_SizeKind_ChildrenSum, 0.f, strictness)

////////////////////////////////
//~ rjf: Box Type Functions

read_only global UI_Box ui_nil_box =
{
  &ui_nil_box,
  &ui_nil_box,
  &ui_nil_box,
  &ui_nil_box,
  &ui_nil_box,
  &ui_nil_box,
  &ui_nil_box,
}
#define ui_box_rec_df_pre(box, root) ui_box_rec_df(box, root, OffsetOf(UI_Box, next), OffsetOf(UI_Box, first))
#define ui_box_rec_df_post(box, root) ui_box_rec_df(box, root, OffsetOf(UI_Box, prev), OffsetOf(UI_Box, last))

////////////////////////////////
//~ rjf: Implicit State Accessors/Mutators

//- rjf: drag data
#define ui_store_drag_struct(ptr) ui_store_drag_data(str8_struct(ptr))
#define ui_get_drag_struct(type) ((type *)ui_get_drag_data(sizeof(type)).str)

////////////////////////////////
//~ rjf: Animation Cache Interaction API

read_only global UI_AnimNode ui_nil_anim_node =
{
  &ui_nil_anim_node,
  &ui_nil_anim_node,
}

#define ui_anim(key, target_val, ...) ui_anim_((key), &(UI_AnimParams){.target = (target_val), .rate = (ui_state->default_animation_rate), __VA_ARGS__})

////////////////////////////////
//~ rjf: Macro Loop Wrappers

//- rjf: stacks (base)
#define UI_Parent(v) DeferLoop(ui_push_parent(v), ui_pop_parent())
#define UI_ChildLayoutAxis(v) DeferLoop(ui_push_child_layout_axis(v), ui_pop_child_layout_axis())
#define UI_FixedX(v) DeferLoop(ui_push_fixed_x(v), ui_pop_fixed_x())
#define UI_FixedY(v) DeferLoop(ui_push_fixed_y(v), ui_pop_fixed_y())
#define UI_FixedWidth(v) DeferLoop(ui_push_fixed_width(v), ui_pop_fixed_width())
#define UI_FixedHeight(v) DeferLoop(ui_push_fixed_height(v), ui_pop_fixed_height())
#define UI_PrefWidth(v) DeferLoop(ui_push_pref_width(v), ui_pop_pref_width())
#define UI_PrefHeight(v) DeferLoop(ui_push_pref_height(v), ui_pop_pref_height())
#define UI_MinWidth(v) DeferLoop(ui_push_min_width(v), ui_pop_min_width())
#define UI_MinHeight(v) DeferLoop(ui_push_min_height(v), ui_pop_min_height())
#define UI_PermissionFlags(v) DeferLoop(ui_push_permission_flags(v), ui_pop_permission_flags())
#define UI_Flags(v) DeferLoop(ui_push_flags(v), ui_pop_flags())
#define UI_OmitFlags(v) DeferLoop(ui_push_omit_flags(v), ui_pop_omit_flags())
#define UI_FocusHot(v) DeferLoop(ui_push_focus_hot(v), ui_pop_focus_hot())
#define UI_FocusActive(v) DeferLoop(ui_push_focus_active(v), ui_pop_focus_active())
#define UI_FastpathCodepoint(v) DeferLoop(ui_push_fastpath_codepoint(v), ui_pop_fastpath_codepoint())
#define UI_GroupKey(v) DeferLoop(ui_push_group_key(v), ui_pop_group_key())
#define UI_Transparency(v) DeferLoop(ui_push_transparency(v), ui_pop_transparency())
#define UI_Tag(v) DeferLoop(ui_push_tag(v), ui_pop_tag())
#define UI_BackgroundColor(v) DeferLoop(ui_push_background_color(v), ui_pop_background_color())
#define UI_TextColor(v) DeferLoop(ui_push_text_color(v), ui_pop_text_color())
#define UI_Squish(v) DeferLoop(ui_push_squish(v), ui_pop_squish())
#define UI_HoverCursor(v) DeferLoop(ui_push_hover_cursor(v), ui_pop_hover_cursor())
#define UI_Font(v) DeferLoop(ui_push_font(v), ui_pop_font())
#define UI_FontSize(v) DeferLoop(ui_push_font_size(v), ui_pop_font_size())
#define UI_TextRasterFlags(v) DeferLoop(ui_push_text_raster_flags(v), ui_pop_text_raster_flags())
#define UI_TabSize(v) DeferLoop(ui_push_tab_size(v), ui_pop_tab_size())
#define UI_CornerRadius00(v) DeferLoop(ui_push_corner_radius_00(v), ui_pop_corner_radius_00())
#define UI_CornerRadius01(v) DeferLoop(ui_push_corner_radius_01(v), ui_pop_corner_radius_01())
#define UI_CornerRadius10(v) DeferLoop(ui_push_corner_radius_10(v), ui_pop_corner_radius_10())
#define UI_CornerRadius11(v) DeferLoop(ui_push_corner_radius_11(v), ui_pop_corner_radius_11())
#define UI_BlurSize(v) DeferLoop(ui_push_blur_size(v), ui_pop_blur_size())
#define UI_TextPadding(v) DeferLoop(ui_push_text_padding(v), ui_pop_text_padding())
#define UI_TextAlignment(v) DeferLoop(ui_push_text_alignment(v), ui_pop_text_alignment())

//- rjf: stacks (compositions)
#define UI_FixedPos(v)       DeferLoop((ui_push_fixed_x((v).x), ui_push_fixed_y((v).y)), (ui_pop_fixed_x(), ui_pop_fixed_y()))
#define UI_FixedSize(v)      DeferLoop((ui_push_fixed_width((v).x), ui_push_fixed_height((v).y)), (ui_pop_fixed_width(), ui_pop_fixed_height()))
#define UI_WidthFill         UI_PrefWidth(ui_pct(1.f, 0.f))
#define UI_HeightFill        UI_PrefHeight(ui_pct(1.f, 0.f))
#define UI_Rect(r)           DeferLoop(ui_push_rect(r), ui_pop_rect())
#define UI_PrefSize(axis, v) DeferLoop(ui_push_pref_size((axis), (v)), ui_pop_pref_size(axis))
#define UI_CornerRadius(v)   DeferLoop(ui_push_corner_radius(v), ui_pop_corner_radius())
#define UI_Focus(kind)       DeferLoop((ui_push_focus_hot(kind), ui_push_focus_active(kind)), (ui_pop_focus_hot(), ui_pop_focus_active()))
#define UI_FlagsAdd(v)       DeferLoop(ui_push_flags(ui_top_flags()|(v)), ui_pop_flags())
#define UI_TagF(...)         DeferLoop(ui_push_tagf(__VA_ARGS__), ui_pop_tag())

//- rjf: tooltip
#define UI_TooltipBase DeferLoop(ui_tooltip_begin_base(), ui_tooltip_end_base())
#define UI_Tooltip DeferLoop(ui_tooltip_begin(), ui_tooltip_end())

//- rjf: context menu
#define UI_CtxMenu(key) DeferLoopChecked(ui_begin_ctx_menu(key), ui_end_ctx_menu())

//- rjf: debug
#define UI_Debug UI_FlagsAdd(UI_BoxFlag_Debug)

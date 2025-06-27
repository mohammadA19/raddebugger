// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Icon Info

public enum UI_IconKind
{
  Null,
  RightArrow,
  DownArrow,
  LeftArrow,
  UpArrow,
  RightCaret,
  DownCaret,
  LeftCaret,
  UpCaret,
  CheckHollow,
  CheckFilled,
  COUNT
}

public struct UI_IconInfo
{
  FNT_Tag icon_font;
  String8[UI_IconKind.COUNT] icon_kind_text_map;
}

////////////////////////////////
//~ rjf: Mouse Button Kinds

public enum UI_MouseButtonKind
{
  Left,
  Middle,
  Right,
  COUNT
}

////////////////////////////////
//~ rjf: Codepath Permissions

public enum UI_PermissionFlags : U32
{
  ClicksLeft        = (1<<0),
  ClicksMiddle      = (1<<1),
  ClicksRight       = (1<<2),
  ScrollX           = (1<<3),
  ScrollY           = (1<<4),
  KeyboardPrimary   = (1<<5),
  KeyboardSecondary = (1<<6),
  Text              = (1<<7),
  
  //- rjf bundles
  Keyboard = (KeyboardPrimary | KeyboardSecondary),
  Clicks = (ClicksLeft | ClicksMiddle | ClicksRight),
  All = 0xffffffff,
}

////////////////////////////////
//~ rjf: Focus Types

public enum UI_FocusKind
{
  Null,
  Off,
  On,
  Root,
  COUNT
}

////////////////////////////////
//~ rjf: Events

// TODO(rjf): clean all this up

public enum UI_EventKind
{
  Null,
  Press,
  Release,
  Text,
  Navigate,
  Edit,
  MouseMove,
  Scroll,
  FileDrop,
  COUNT
}

public enum UI_EventActionSlot
{
  Null,
  Accept,
  Cancel,
  Edit,
  COUNT
}

public enum UI_EventFlags : U32
{
  KeepMark            = (1<<0),
  Delete              = (1<<1),
  Copy                = (1<<2),
  Paste               = (1<<3),
  ZeroDeltaOnSelect   = (1<<4),
  PickSelectSide      = (1<<5),
  CapAtLine           = (1<<6),
  ExplicitDirectional = (1<<7),
  Reorder             = (1<<8),
  Secondary           = (1<<9),
}

public enum UI_EventDeltaUnit
{
  Null,
  Char,
  Word,
  Line,
  Page,
  Whole,
  COUNT
}

public struct UI_Event
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

public struct UI_EventNode
{
  UI_EventNode* next;
  UI_EventNode* prev;
  UI_Event v;
}

public struct UI_EventList
{
  UI_EventNode* first;
  UI_EventNode* last;
  U64 count;
}

////////////////////////////////
//~ rjf: Textual Operations

public enum UI_TxtOpFlags : U32
{
  UI_TxtOpFlag_Invalid = (1<<0),
  UI_TxtOpFlag_Copy    = (1<<1),
}

public struct UI_TxtOp
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

public struct UI_Key
{
  U64[1] u64;
}

////////////////////////////////
//~ rjf: Sizes

public enum UI_SizeKind
{
  Null,
  Pixels,      // size is computed via a preferred pixel value
  TextContent, // size is computed via the dimensions of box's rendered string
  ParentPct,   // size is computed via a well-determined parent or grandparent size
  ChildrenSum, // size is computed via summing well-determined sizes of children
}

public struct UI_Size
{
  UI_SizeKind kind;
  F32 value;
  F32 strictness;
}

////////////////////////////////
//~ rjf: Themes

public struct UI_ThemePattern
{
  String8Array tags;
  Vec4F32 linear;
}

public struct UI_Theme
{
  UI_ThemePattern* patterns;
  U64 patterns_count;
}

////////////////////////////////
//~ rjf: Animation Info

public struct UI_AnimationInfo
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

public struct UI_ScrollPt
{
  S64 idx;
  F32 off;
}

[Union]
public struct UI_ScrollPt2
{
  UI_ScrollPt[2] v;
  struct
  {
    UI_ScrollPt x;
    UI_ScrollPt y;
  };
}

////////////////////////////////
//~ rjf: Box Types

public enum UI_TextAlign
{
  Left,
  Center,
  Right,
  COUNT
}

#define UI_BOX_CUSTOM_DRAW(name) void name(struct UI_Box *box, void *user_data)
typedef UI_BOX_CUSTOM_DRAW(UI_BoxCustomDrawFunctionType);

public enum UI_BoxFlags : U64
{
  //- rjf: interaction
  MouseClickable            = (1 << 0),
  KeyboardClickable         = (1 << 1),
  DropSite                  = (1 << 2),
  ClickToFocus              = (1 << 3),
  Scroll                    = (1 << 4),
  ViewScrollX               = (1 << 5),
  ViewScrollY               = (1 << 6),
  ViewClampX                = (1 << 7),
  ViewClampY                = (1 << 8),
  FocusHot                  = (1 << 9),
  FocusActive               = (1 << 10),
  FocusHotDisabled          = (1 << 11),
  FocusActiveDisabled       = (1 << 12),
  DefaultFocusNavX          = (1 << 13),
  DefaultFocusNavY          = (1 << 14),
  DefaultFocusEdit          = (1 << 15),
  FocusNavSkip              = (1 << 16),
  DisableTruncatedHover     = (1 << 17),
  Disabled                  = (1 << 18),

  //- rjf: layout
  FloatingX                 = (1 << 19),
  FloatingY                 = (1 << 20),
  FixedWidth                = (1 << 21),
  FixedHeight               = (1 << 22),
  AllowOverflowX            = (1 << 23),
  AllowOverflowY            = (1 << 24),
  SkipViewOffX              = (1 << 25),
  SkipViewOffY              = (1 << 26),

  //- rjf: appearance / animation
  DrawDropShadow            = (1 << 27),
  DrawBackgroundBlur        = (1 << 28),
  DrawBackground            = (1 << 29),
  DrawBorder                = (1 << 30),
  DrawSideTop               = (1 << 31),
  DrawSideBottom            = (1 << 32),
  DrawSideLeft              = (1 << 33),
  DrawSideRight             = (1 << 34),
  DrawText                  = (1 << 35),
  DrawTextFastpathCodepoint = (1 << 36),
  DrawTextWeak              = (1 << 37),
  DrawHotEffects            = (1 << 38),
  DrawActiveEffects         = (1 << 39),
  DrawOverlay               = (1 << 40),
  DrawBucket                = (1 << 41),
  Clip                      = (1 << 42),
  AnimatePosX               = (1 << 43),
  AnimatePosY               = (1 << 44),
  DisableTextTrunc          = (1 << 45),
  DisableIDString           = (1 << 46),
  DisableFocusBorder        = (1 << 47),
  DisableFocusOverlay       = (1 << 48),
  HasDisplayString          = (1 << 49),
  HasFuzzyMatchRanges       = (1 << 50),
  RoundChildrenByParent     = (1 << 51),
  SquishAnchored            = (1 << 52),

  //- rjf: debug
  Debug                     = (1 << 53),

  //- rjf: bundles
  Clickable           = (MouseClickable | KeyboardClickable),
  DefaultFocusNav     = (DefaultFocusNavX | DefaultFocusNavY | DefaultFocusEdit),
  Floating            = (FloatingX | FloatingY),
  FixedSize           = (FixedWidth | FixedHeight),
  AllowOverflow       = (AllowOverflowX | AllowOverflowY),
  AnimatePos          = (AnimatePosX | AnimatePosY),
  ViewScroll          = (ViewScrollX | ViewScrollY),
  ViewClamp           = (ViewClampX | ViewClampY),
  DisableFocusEffects = (DisableFocusBorder | DisableFocusOverlay),
}

public struct UI_Box
{
  //- rjf: persistent links
  UI_Box* hash_next;
  UI_Box* hash_prev;
  
  //- rjf: per-build links/data
  UI_Box* first;
  UI_Box* last;
  UI_Box* next;
  UI_Box* prev;
  UI_Box* parent;
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
  UI_Size[Axis2_COUNT] pref_size;
  Axis2 child_layout_axis;
  OS_Cursor hover_cursor;
  U32 fastpath_codepoint;
  UI_Key group_key;
  DR_Bucket* draw_bucket;
  UI_BoxCustomDrawFunctionType* custom_draw;
  void* custom_draw_user_data;
  Vec4F32 background_color;
  Vec4F32 text_color;
  Vec4F32 border_color;
  FNT_Tag font;
  F32 font_size;
  F32 tab_size;
  FNT_RasterFlags text_raster_flags;
  F32[Corner_COUNT] corner_radii;
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

public struct UI_BoxRec
{
  UI_Box* next;
  S32 push_count;
  S32 pop_count;
}

public struct UI_BoxNode
{
  UI_BoxNode* next;
  UI_Box* box;
}

public struct UI_BoxList
{
  UI_BoxNode* first;
  UI_BoxNode* last;
  U64 count;
}

public enum UI_SignalFlags : U32
{
  // rjf: mouse press -> box was pressed while hovering
  LeftPressed         = (1<<0),
  MiddlePressed       = (1<<1),
  RightPressed        = (1<<2),
  
  // rjf: dragging -> box was previously pressed, user is still holding button
  LeftDragging        = (1<<3),
  MiddleDragging      = (1<<4),
  RightDragging       = (1<<5),
  
  // rjf: double-dragging -> box was previously double-clicked, user is still holding button
  LeftDoubleDragging  = (1<<6),
  MiddleDoubleDragging= (1<<7),
  RightDoubleDragging = (1<<8),
  
  // rjf: triple-dragging -> box was previously triple-clicked, user is still holding button
  LeftTripleDragging  = (1<<9),
  MiddleTripleDragging= (1<<10),
  RightTripleDragging = (1<<11),
  
  // rjf: released -> box was previously pressed & user released, in or out of bounds
  LeftReleased        = (1<<12),
  MiddleReleased      = (1<<13),
  RightReleased       = (1<<14),
  
  // rjf: clicked -> box was previously pressed & user released, in bounds
  LeftClicked         = (1<<15),
  MiddleClicked       = (1<<16),
  RightClicked        = (1<<17),
  
  // rjf: double clicked -> box was previously clicked, pressed again
  LeftDoubleClicked   = (1<<18),
  MiddleDoubleClicked = (1<<19),
  RightDoubleClicked  = (1<<20),
  
  // rjf: triple clicked -> box was previously clicked twice, pressed again
  LeftTripleClicked   = (1<<21),
  MiddleTripleClicked = (1<<22),
  RightTripleClicked  = (1<<23),
  
  // rjf: keyboard pressed -> box had focus, user activated via their keyboard
  KeyboardPressed     = (1<<24),
  
  // rjf: passive mouse info
  Hovering            = (1<<25), // hovering specifically this box
  MouseOver           = (1<<26), // mouse is over, but may be occluded
  
  // rjf: committing state changes via user interaction
  Commit              = (1<<27),
  
  // rjf: high-level combos
  Pressed = LeftPressed | KeyboardPressed,
  Released = LeftReleased,
  Clicked = LeftClicked | KeyboardPressed,
  DoubleClicked = LeftDoubleClicked,
  TripleClicked = LeftTripleClicked,
  Dragging = LeftDragging,
}

public struct UI_Signal
{
  UI_Box* box;
  OS_Modifiers event_flags;
  Vec2S16 scroll;
  UI_SignalFlags f;
}

#define ui_pressed(s)        !!((s).f&UI_SignalFlags.Pressed)
#define ui_clicked(s)        !!((s).f&UI_SignalFlags.Clicked)
#define ui_released(s)       !!((s).f&UI_SignalFlags.Released)
#define ui_double_clicked(s) !!((s).f&UI_SignalFlags.DoubleClicked)
#define ui_triple_clicked(s) !!((s).f&UI_SignalFlags.TripleClicked)
#define ui_middle_clicked(s) !!((s).f&UI_SignalFlags.MiddleClicked)
#define ui_right_clicked(s)  !!((s).f&UI_SignalFlags.RightClicked)
#define ui_dragging(s)       !!((s).f&UI_SignalFlags.Dragging)
#define ui_hovering(s)       !!((s).f&UI_SignalFlags.Hovering)
#define ui_mouse_over(s)     !!((s).f&UI_SignalFlags.MouseOver)
#define ui_committed(s)      !!((s).f&UI_SignalFlags.Commit)

public struct UI_Nav
{
  B32 moved;
  Vec2S64 new_p;
}

////////////////////////////////
//~ rjf: Animation State Types

public struct UI_AnimParams
{
  F32 initial;
  F32 target;
  F32 rate;
  F32 epsilon;
  B32 reset;
}

public struct UI_AnimNode
{
  UI_AnimNode* slot_next;
  UI_AnimNode* slot_prev;
  UI_AnimNode* lru_next;
  UI_AnimNode* lru_prev;
  U64 first_touched_build_index;
  U64 last_touched_build_index;
  UI_Key key;
  UI_AnimParams params;
  F32 current;
}

public struct UI_AnimSlot
{
  UI_AnimNode* first;
  UI_AnimNode* last;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/ui.meta.h"

////////////////////////////////
//~ rjf: State Types

//- rjf: cache for mapping 64-bit key -> array of tags

public struct UI_TagsCacheNode
{
  UI_TagsCacheNode* next;
  UI_Key key;
  String8Array tags;
}

public struct UI_TagsCacheSlot
{
  UI_TagsCacheNode* first;
  UI_TagsCacheNode* last;
}

public struct UI_TagsKeyStackNode
{
  UI_TagsKeyStackNode* next;
  UI_Key key;
}

//- rjf: cache for mapping 64-bit key -> theme pattern

public struct UI_ThemePatternCacheNode
{
  UI_ThemePatternCacheNode* slot_next;
  UI_ThemePatternCacheNode* slot_prev;
  UI_ThemePatternCacheNode* lru_next;
  UI_ThemePatternCacheNode* lru_prev;
  U64 last_build_index_accessed;
  UI_Key key;
  Vec4F32 target_rgba;
  Vec4F32 current_rgba;
}

public struct UI_ThemePatternCacheSlot
{
  UI_ThemePatternCacheNode* first;
  UI_ThemePatternCacheNode* last;
}

//- rjf: cache for mapping 64-bit key -> box

public struct UI_BoxHashSlot
{
  UI_Box* hash_first;
  UI_Box* hash_last;
}

//- rjf: main state bundle

public struct UI_State
{
  //- rjf: main arena
  Arena* arena;
  
  //- rjf: fixed keys
  UI_Key external_key;
  
  //- rjf: build arenas
  Arena*[2] build_arenas;
  U64 build_index;
  
  //- rjf: box cache
  UI_Box* first_free_box;
  U64 box_table_size;
  UI_BoxHashSlot* box_table;
  
  //- rjf: anim cache
  UI_AnimNode* free_anim_node;
  UI_AnimNode* lru_anim_node;
  UI_AnimNode* mru_anim_node;
  U64 anim_slots_count;
  UI_AnimSlot* anim_slots;
  
  //- rjf: build state machine state
  B32 is_in_open_ctx_menu;
  String8 autocomplete_string;
  B32 tooltip_can_overflow_window;
  UI_Key tooltip_anchor_key;
  String8Array current_gen_tags;
  U64 current_gen_tags_gen;
  UI_TagsKeyStackNode* tags_key_stack_top;
  UI_TagsKeyStackNode* tags_key_stack_free;
  U64 tags_cache_slots_count;
  UI_TagsCacheSlot* tags_cache_slots;
  
  //- rjf: theme pattern cache
  U64 theme_pattern_cache_slots_count;
  UI_ThemePatternCacheSlot* theme_pattern_cache_slots;
  UI_ThemePatternCacheNode* theme_pattern_cache_node_free;
  UI_ThemePatternCacheNode* lru_theme_pattern_cache_node;
  UI_ThemePatternCacheNode* mru_theme_pattern_cache_node;
  
  //- rjf: build phase output
  UI_Box* root;
  UI_Box* tooltip_root;
  UI_Box* ctx_menu_root;
  UI_Key default_nav_root_key;
  U64 build_box_count;
  U64 last_build_box_count;
  B32 ctx_menu_touched_this_frame;
  B32 is_animating;
  
  //- rjf: build parameters
  UI_IconInfo icon_info;
  UI_Theme* theme;
  UI_AnimationInfo animation_info;
  OS_Handle window;
  UI_EventList* events;
  Vec2F32 mouse;
  F32 animation_dt;
  F32 default_animation_rate;
  
  //- rjf: user interaction state
  UI_Key hot_box_key;
  UI_Key[UI_MouseButtonKind.COUNT] active_box_key;
  UI_Key drop_hot_box_key;
  UI_Key clipboard_copy_key;
  U64[UI_MouseButtonKind.COUNT][3] press_timestamp_history_us;
  UI_Key[UI_MouseButtonKind.COUNT][3] press_key_history;
  Vec2F32[UI_MouseButtonKind.COUNT][3] press_pos_history;
  Vec2F32 drag_start_mouse;
  Arena* drag_state_arena;
  String8 drag_state_data;
  Arena* string_hover_arena;
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

#define ui_px(value, strictness)         ui_size(UI_SizeKind.Pixels, value, strictness)
#define ui_em(value, strictness)         ui_size(UI_SizeKind.Pixels, (value) * ui_top_font_size(), strictness)
#define ui_text_dim(padding, strictness) ui_size(UI_SizeKind.TextContent, padding, strictness)
#define ui_pct(value, strictness)        ui_size(UI_SizeKind.ParentPct, value, strictness)
#define ui_children_sum(strictness)      ui_size(UI_SizeKind.ChildrenSum, 0.f, strictness)

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
#define UI_Debug UI_FlagsAdd(UI_BoxFlags.Debug)

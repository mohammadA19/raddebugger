// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef UI_H
#define UI_H

////////////////////////////////
//~ rjf: Icon Info

enum UI_IconKind
{
  NULL,
  RIGHT_ARROW,
  DOWN_ARROW,
  LEFT_ARROW,
  UP_ARROW,
  RIGHT_CARET,
  DOWN_CARET,
  LEFT_CARET,
  UP_CARET,
  CHECK_HOLLOW,
  CHECK_FILLED,
  COUNT
}

struct UI_IconInfo
{
  FNT_Tag icon_font;
  String8[UI_IconKind.COUNT] icon_kind_text_map;
}

////////////////////////////////
//~ rjf: Mouse Button Kinds

enum UI_MouseButtonKind
{
  LEFT,
  MIDDLE,
  RIGHT,
  COUNT
}

////////////////////////////////
//~ rjf: Codepath Permissions

typedef uint UI_PermissionFlags;
enum
{
  UI_PermissionFlag_ClicksLeft   = (1<<0),
  UI_PermissionFlag_ClicksMiddle = (1<<1),
  UI_PermissionFlag_ClicksRight  = (1<<2),
  UI_PermissionFlag_ScrollX      = (1<<3),
  UI_PermissionFlag_ScrollY      = (1<<4),
  UI_PermissionFlag_Keyboard     = (1<<5),
  UI_PermissionFlag_Text         = (1<<6),
  
  //- rjf bundles
  UI_PermissionFlag_Clicks = (UI_PermissionFlag_ClicksLeft|UI_PermissionFlag_ClicksMiddle|UI_PermissionFlag_ClicksRight),
  UI_PermissionFlag_All = 0xffffffff,
};

////////////////////////////////
//~ rjf: Focus Types

enum UI_FocusKind
{
  NULL,
  OFF,
  ON,
  ROOT,
  COUNT
}

////////////////////////////////
//~ rjf: Events

// TODO(rjf): clean all this up

enum UI_EventKind
{
  NULL,
  PRESS,
  RELEASE,
  TEXT,
  NAVIGATE,
  EDIT,
  MOUSEMOVE,
  SCROLL,
  AUTOCOMPLETE_HINT,
  FILEDROP,
  COUNT
}

enum UI_EventActionSlot
{
  NULL,
  ACCEPT,
  CANCEL,
  EDIT,
  COUNT
}

typedef uint UI_EventFlags;
enum
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
};

enum UI_EventDeltaUnit
{
  NULL,
  CHAR,
  WORD,
  LINE,
  PAGE,
  WHOLE,
  COUNT
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
  ulong timestamp_us;
}

struct UI_EventNode
{
  UI_EventNode* next;
  UI_EventNode* prev;
  UI_Event v;
}

struct UI_EventList
{
  UI_EventNode* first;
  UI_EventNode* last;
  ulong count;
}

////////////////////////////////
//~ rjf: Textual Operations

typedef uint UI_TxtOpFlags;
enum
{
  UI_TxtOpFlag_Invalid = (1<<0),
  UI_TxtOpFlag_Copy    = (1<<1),
};

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
  ulong[1] u64;
}

////////////////////////////////
//~ rjf: Sizes

enum UI_SizeKind
{
  NULL,
  PIXELS,      // size is computed via a preferred pixel value
  TEXT_CONTENT, // size is computed via the dimensions of box's rendered string
  PARENT_PCT,   // size is computed via a well-determined parent or grandparent size
  CHILDREN_SUM, // size is computed via summing well-determined sizes of children
}

struct UI_Size
{
  UI_SizeKind kind;
  float value;
  float strictness;
}

////////////////////////////////
//~ rjf: Palettes

enum UI_ColorCode
{
  NULL,
  BACKGROUND,
  TEXT,
  TEXT_WEAK,
  BORDER,
  OVERLAY,
  CURSOR,
  SELECTION,
  COUNT
}

struct UI_Palette
{
  union
  {
    Vec4F32[UI_ColorCode.COUNT] colors;
    struct
    {
      Vec4F32 null;
      Vec4F32 background;
      Vec4F32 text;
      Vec4F32 text_weak;
      Vec4F32 border;
      Vec4F32 overlay;
      Vec4F32 cursor;
      Vec4F32 selection;
    }
  }
}

struct UI_WidgetPaletteInfo
{
  UI_Palette* tooltip_palette;
  UI_Palette* ctx_menu_palette;
  UI_Palette* scrollbar_palette;
}

////////////////////////////////
//~ rjf: Animation Info

typedef uint UI_AnimationInfoFlags;
enum
{
  UI_AnimationInfoFlag_HotAnimations          = (1<<0),
  UI_AnimationInfoFlag_ActiveAnimations       = (1<<1),
  UI_AnimationInfoFlag_FocusAnimations        = (1<<2),
  UI_AnimationInfoFlag_TooltipAnimations      = (1<<3),
  UI_AnimationInfoFlag_ContextMenuAnimations  = (1<<4),
  UI_AnimationInfoFlag_ScrollingAnimations    = (1<<5),
  UI_AnimationInfoFlag_All = 0xffffffff,
};

struct UI_AnimationInfo
{
  UI_AnimationInfoFlags flags;
}

////////////////////////////////
//~ rjf: Scroll Positions

struct UI_ScrollPt
{
  long idx;
  float off;
}

union UI_ScrollPt2
{
  UI_ScrollPt v[2];
  struct
  {
    UI_ScrollPt x;
    UI_ScrollPt y;
  }
}

////////////////////////////////
//~ rjf: Box Types

enum UI_TextAlign
{
  LEFT,
  CENTER,
  RIGHT,
  COUNT
}

struct UI_Box;
#define UI_BOX_CUSTOM_DRAW(name) void name(struct UI_Box *box, void *user_data)
typedef UI_BOX_CUSTOM_DRAW(UI_BoxCustomDrawFunctionType);

enum UI_BoxFlags : ulong
{
//- rjf: interaction
  MOUSECLICKABLE            = 1 << 0,
  KEYBOARDCLICKABLE         = 1 << 1,
  DROPSITE                  = 1 << 2,
  CLICKTOFOCUS              = 1 << 3,
  SCROLL                    = 1 << 4,
  VIEWSCROLLX               = 1 << 5,
  VIEWSCROLLY               = 1 << 6,
  VIEWCLAMPX                = 1 << 7,
  VIEWCLAMPY                = 1 << 8,
  FOCUSHOT                  = 1 << 9,
  FOCUSACTIVE               = 1 << 10,
  FOCUSHOTDISABLED          = 1 << 11,
  FOCUSACTIVEDISABLED       = 1 << 12,
  DEFAULTFOCUSNAVX          = 1 << 13,
  DEFAULTFOCUSNAVY          = 1 << 14,
  DEFAULTFOCUSEDIT          = 1 << 15,
  FOCUSNAVSKIP              = 1 << 16,
  DISABLETRUNCATEDHOVER     = 1 << 17,
  DISABLED                  = 1 << 18,

//- rjf: layout
  FLOATINGX                 = 1 << 19,
  FLOATINGY                 = 1 << 20,
  FIXEDWIDTH                = 1 << 21,
  FIXEDHEIGHT               = 1 << 22,
  ALLOWOVERFLOWX            = 1 << 23,
  ALLOWOVERFLOWY            = 1 << 24,
  SKIPVIEWOFFX              = 1 << 25,
  SKIPVIEWOFFY              = 1 << 26,

//- rjf: appearance / animation
  DRAWDROPSHADOW            = 1 << 27,
  DRAWBACKGROUNDBLUR        = 1 << 28,
  DRAWBACKGROUND            = 1 << 29,
  DRAWBORDER                = 1 << 30,
  DRAWSIDETOP               = 1 << 31,
  DRAWSIDEBOTTOM            = 1 << 32,
  DRAWSIDELEFT              = 1 << 33,
  DRAWSIDERIGHT             = 1 << 34,
  DRAWTEXT                  = 1 << 35,
  DRAWTEXTFASTPATHCODEPOINT = 1 << 36,
  DRAWTEXTWEAK              = 1 << 37,
  DRAWHOTEFFECTS            = 1 << 38,
  DRAWACTIVEEFFECTS         = 1 << 39,
  DRAWOVERLAY               = 1 << 40,
  DRAWBUCKET                = 1 << 41,
  CLIP                      = 1 << 42,
  ANIMATEPOSX               = 1 << 43,
  ANIMATEPOSY               = 1 << 44,
  DISABLETEXTTRUNC          = 1 << 45,
  DISABLEIDSTRING           = 1 << 46,
  DISABLEFOCUSBORDER        = 1 << 47,
  DISABLEFOCUSOVERLAY       = 1 << 48,
  HASDISPLAYSTRING          = 1 << 49,
  HASFUZZYMATCHRANGES       = 1 << 50,
  ROUNDCHILDRENBYPARENT     = 1 << 51,

//- rjf: bundles
  CLICKABLE           = MOUSECLICKABLE | KEYBOARDCLICKABLE,
  DEFAULTFOCUSNAV     = DEFAULTFOCUSNAVX | DEFAULTFOCUSNAVY | DEFAULTFOCUSEDIT,
  FLOATING            = FLOATINGX | FLOATINGY,
  FIXEDSIZE           = FIXEDWIDTH | FIXEDHEIGHT,
  ALLOWOVERFLOW       = ALLOWOVERFLOWX | ALLOWOVERFLOWY,
  ANIMATEPOS          = ANIMATEPOSX | ANIMATEPOSY,
  VIEWSCROLL          = VIEWSCROLLX | VIEWSCROLLY,
  VIEWCLAMP           = VIEWCLAMPX | VIEWCLAMPY,
  DISABLEFOCUSEFFECTS = DISABLEFOCUSBORDER | DISABLEFOCUSOVERLAY,
}

struct UI_Box
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
  ulong child_count;
  
  //- rjf: per-build equipment
  UI_Key key;
  UI_BoxFlags flags;
  String8 string;
  UI_TextAlign text_align;
  Vec2F32 fixed_position;
  Vec2F32 fixed_size;
  UI_Size pref_size[Axis2_COUNT];
  Axis2 child_layout_axis;
  OS_Cursor hover_cursor;
  uint fastpath_codepoint;
  UI_Key group_key;
  DR_Bucket* draw_bucket;
  UI_BoxCustomDrawFunctionType *custom_draw;
  void* custom_draw_user_data;
  UI_Palette* palette;
  FNT_Tag font;
  float font_size;
  float tab_size;
  FNT_RasterFlags text_raster_flags;
  float[Corner_COUNT] corner_radii;
  float blur_size;
  float transparency;
  float squish;
  float text_padding;
  
  //- rjf: per-build artifacts
  DR_FancyRunList display_string_runs;
  Rng2F32 rect;
  Vec2F32 fixed_position_animated;
  Vec2F32 position_delta;
  FuzzyMatchRangeList fuzzy_match_ranges;
  
  //- rjf: persistent data
  ulong first_touched_build_index;
  ulong last_touched_build_index;
  ulong first_disabled_build_index;
  float hot_t;
  float active_t;
  float disabled_t;
  float focus_hot_t;
  float focus_active_t;
  float focus_active_disabled_t;
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
  UI_Box* next;
  int push_count;
  int pop_count;
}

struct UI_BoxNode
{
  UI_BoxNode* next;
  UI_Box* box;
}

struct UI_BoxList
{
  UI_BoxNode* first;
  UI_BoxNode* last;
  ulong count;
}

enum UI_SignalFlags : uint
{
  // rjf: mouse press -> box was pressed while hovering
  LEFT_PRESSED         = (1<<0),
  MIDDLE_PRESSED       = (1<<1),
  RIGHT_PRESSED        = (1<<2),
  
  // rjf: dragging -> box was previously pressed, user is still holding button
  LEFT_DRAGGING        = (1<<3),
  MIDDLE_DRAGGING      = (1<<4),
  RIGHT_DRAGGING       = (1<<5),
  
  // rjf: double-dragging -> box was previously double-clicked, user is still holding button
  LEFT_DOUBLE_DRAGGING  = (1<<6),
  MIDDLE_DOUBLE_DRAGGING= (1<<7),
  RIGHT_DOUBLE_DRAGGING = (1<<8),
  
  // rjf: triple-dragging -> box was previously triple-clicked, user is still holding button
  LEFT_TRIPLE_DRAGGING  = (1<<9),
  MIDDLE_TRIPLE_DRAGGING= (1<<10),
  RIGHT_TRIPLE_DRAGGING = (1<<11),
  
  // rjf: released -> box was previously pressed & user released, in or out of bounds
  LEFT_RELEASED        = (1<<12),
  MIDDLE_RELEASED      = (1<<13),
  RIGHT_RELEASED       = (1<<14),
  
  // rjf: clicked -> box was previously pressed & user released, in bounds
  LEFT_CLICKED         = (1<<15),
  MIDDLE_CLICKED       = (1<<16),
  RIGHT_CLICKED        = (1<<17),

  // rjf: double clicked -> box was previously clicked, pressed again
  LEFT_DOUBLE_CLICKED   = (1<<18),
  MIDDLE_DOUBLE_CLICKED = (1<<19),
  RIGHT_DOUBLE_CLICKED  = (1<<20),
  
  // rjf: triple clicked -> box was previously clicked twice, pressed again
  LEFT_TRIPLE_CLICKED   = (1<<21),
  MIDDLE_TRIPLE_CLICKED = (1<<22),
  RIGHT_TRIPLE_CLICKED  = (1<<23),
  
  // rjf: keyboard pressed -> box had focus, user activated via their keyboard
  KEYBOARD_PRESSED     = (1<<24),
  
  // rjf: passive mouse info
  HOVERING            = (1<<25), // hovering specifically this box
  MOUSE_OVER           = (1<<26), // mouse is over, but may be occluded
  
  // rjf: committing state changes via user interaction
  COMMIT              = (1<<27),
  
  // rjf: high-level combos
  PRESSED = LEFT_PRESSED | KEYBOARD_PRESSED,
  RELEASED = LEFT_RELEASED,
  CLICKED = LEFT_CLICKED | KEYBOARD_PRESSED,
  DOUBLE_CLICKED = LEFT_DOUBLE_CLICKED,
  TRIPLE_CLICKED = LEFT_TRIPLE_CLICKED,
  DRAGGING = LEFT_DRAGGING,
}

struct UI_Signal
{
  UI_Box* box;
  OS_Modifiers event_flags;
  Vec2S16 scroll;
  UI_SignalFlags f;
}

#define ui_pressed(s)        !!((s).f&UI_SignalFlags.PRESSED)
#define ui_clicked(s)        !!((s).f&UI_SignalFlags.CLICKED)
#define ui_released(s)       !!((s).f&UI_SignalFlags.RELEASED)
#define ui_double_clicked(s) !!((s).f&UI_SignalFlags.DOUBLE_CLICKED)
#define ui_triple_clicked(s) !!((s).f&UI_SignalFlags.TRIPLE_CLICKED)
#define ui_middle_clicked(s) !!((s).f&UI_SignalFlags.MIDDLE_CLICKED)
#define ui_right_clicked(s)  !!((s).f&UI_SignalFlags.RIGHT_CLICKED)
#define ui_dragging(s)       !!((s).f&UI_SignalFlags.DRAGGING)
#define ui_hovering(s)       !!((s).f&UI_SignalFlags.HOVERING)
#define ui_mouse_over(s)     !!((s).f&UI_SignalFlags.MOUSE_OVER)
#define ui_committed(s)      !!((s).f&UI_SignalFlags.COMMIT)

struct UI_Nav
{
  B32 moved;
  Vec2S64 new_p;
}

////////////////////////////////
//~ rjf: Animation State Types

struct UI_AnimParams
{
  float initial;
  float target;
  float rate;
  float epsilon;
}

struct UI_AnimNode
{
  UI_AnimNode* slot_next;
  UI_AnimNode* slot_prev;
  UI_AnimNode* lru_next;
  UI_AnimNode* lru_prev;
  ulong first_touched_build_index;
  ulong last_touched_build_index;
  UI_Key key;
  UI_AnimParams params;
  float current;
}

struct UI_AnimSlot
{
  UI_AnimNode* first;
  UI_AnimNode* last;
};

////////////////////////////////
//~ rjf: Generated Code

#include "generated/ui.meta.h"

////////////////////////////////
//~ rjf: State Types

struct UI_BoxHashSlot
{
  UI_Box* hash_first;
  UI_Box* hash_last;
}

struct UI_State
{
  //- rjf: main arena
  Arena* arena;
  
  //- rjf: fixed keys
  UI_Key external_key;
  
  //- rjf: build arenas
  Arena*[2] build_arenas;
  ulong build_index;
  
  //- rjf: box cache
  UI_Box* first_free_box;
  ulong box_table_size;
  UI_BoxHashSlot *box_table;
  
  //- rjf: anim cache
  UI_AnimNode* free_anim_node;
  UI_AnimNode* lru_anim_node;
  UI_AnimNode* mru_anim_node;
  ulong anim_slots_count;
  UI_AnimSlot* anim_slots;
  
  //- rjf: build state machine state
  B32 is_in_open_ctx_menu;
  
  //- rjf: build phase output
  UI_Box* root;
  UI_Box* tooltip_root;
  UI_Box* ctx_menu_root;
  UI_Key default_nav_root_key;
  ulong build_box_count;
  ulong last_build_box_count;
  B32 ctx_menu_touched_this_frame;
  B32 is_animating;
  
  //- rjf: build parameters
  UI_IconInfo icon_info;
  UI_WidgetPaletteInfo widget_palette_info;
  UI_AnimationInfo animation_info;
  OS_Handle window;
  UI_EventList* events;
  Vec2F32 mouse;
  float animation_dt;
  float default_animation_rate;
  
  //- rjf: user interaction state
  UI_Key hot_box_key;
  UI_Key[UI_MouseButtonKind.COUNT] active_box_key;
  UI_Key drop_hot_box_key;
  UI_Key clipboard_copy_key;
  ulong[UI_MouseButtonKind.COUNT][3] press_timestamp_history_us;
  UI_Key[UI_MouseButtonKind.COUNT][3] press_key_history;
  Vec2F32[UI_MouseButtonKind.COUNT][3] press_pos_history;
  Vec2F32 drag_start_mouse;
  Arena* drag_state_arena;
  String8 drag_state_data;
  Arena* string_hover_arena;
  String8 string_hover_string;
  DR_FancyRunList string_hover_fancy_runs;
  ulong string_hover_begin_us;
  ulong string_hover_build_index;
  ulong last_time_mousemoved_us;
  
  //- rjf: tooltip state
  float tooltip_open_t;
  B32 tooltip_open;
  
  //- rjf: context menu state
  UI_Key ctx_menu_anchor_key;
  UI_Key next_ctx_menu_anchor_key;
  Vec2F32 ctx_menu_anchor_box_last_pos;
  Vec2F32 ctx_menu_anchor_off;
  B32 ctx_menu_open;
  B32 next_ctx_menu_open;
  float ctx_menu_open_t;
  UI_Key ctx_menu_key;
  B32 ctx_menu_changed;
  
  //- rjf: build phase stacks
  UI_DeclStackNils;
  UI_DeclStacks;
}

////////////////////////////////
//~ rjf: Size Type Functions

internal UI_Size ui_size(UI_SizeKind kind, float value, float strictness);
#define ui_px(value, strictness)         ui_size(UI_SizeKind.PIXELS, value, strictness)
#define ui_em(value, strictness)         ui_size(UI_SizeKind.PIXELS, (value) * ui_top_font_size(), strictness)
#define ui_text_dim(padding, strictness) ui_size(UI_SizeKind.TEXT_CONTENT, padding, strictness)
#define ui_pct(value, strictness)        ui_size(UI_SizeKind.PARENT_PCT, value, strictness)
#define ui_children_sum(strictness)      ui_size(UI_SizeKind.CHILDREN_SUM, 0.f, strictness)

////////////////////////////////
//~ rjf: Color Scheme Type Functions

read_only global UI_Palette ui_g_nil_palette = {0};

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
};
#define ui_box_rec_df_pre(box, root) ui_box_rec_df(box, root, OffsetOf(UI_Box, next), OffsetOf(UI_Box, first))
#define ui_box_rec_df_post(box, root) ui_box_rec_df(box, root, OffsetOf(UI_Box, prev), OffsetOf(UI_Box, last))

////////////////////////////////
//~ rjf: Implicit State Accessors/Mutators

//- rjf: drag data
#define ui_store_drag_struct(ptr) ui_store_drag_data(str8_struct(ptr))
#define ui_get_drag_struct(type) ((type *)ui_get_drag_data(sizeof(type)).str)

////////////////////////////////
//~ rjf: Box Tree Building API

//- rjf: palette forming
#define ui_build_palette(base, ...) ui_build_palette_((base), &(UI_Palette){.text = v4f32(0, 0, 0, 0), __VA_ARGS__})

////////////////////////////////
//~ rjf: Animation Cache Interaction API

read_only global UI_AnimNode ui_nil_anim_node =
{
  &ui_nil_anim_node,
  &ui_nil_anim_node,
};

internal float ui_anim_(UI_Key key, UI_AnimParams *params);
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
#define UI_PermissionFlags(v) DeferLoop(ui_push_permission_flags(v), ui_pop_permission_flags())
#define UI_Flags(v) DeferLoop(ui_push_flags(v), ui_pop_flags())
#define UI_FocusHot(v) DeferLoop(ui_push_focus_hot(v), ui_pop_focus_hot())
#define UI_FocusActive(v) DeferLoop(ui_push_focus_active(v), ui_pop_focus_active())
#define UI_FastpathCodepoint(v) DeferLoop(ui_push_fastpath_codepoint(v), ui_pop_fastpath_codepoint())
#define UI_GroupKey(v) DeferLoop(ui_push_group_key(v), ui_pop_group_key())
#define UI_Transparency(v) DeferLoop(ui_push_transparency(v), ui_pop_transparency())
#define UI_Palette(v) DeferLoop(ui_push_palette(v), ui_pop_palette())
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

//- rjf: tooltip
#define UI_TooltipBase DeferLoop(ui_tooltip_begin_base(), ui_tooltip_end_base())
#define UI_Tooltip DeferLoop(ui_tooltip_begin(), ui_tooltip_end())

//- rjf: context menu
#define UI_CtxMenu(key) DeferLoopChecked(ui_begin_ctx_menu(key), ui_end_ctx_menu())

#endif // UI_H

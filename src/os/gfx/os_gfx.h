// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef OS_GFX_H
#define OS_GFX_H

////////////////////////////////
//~ rjf: Graphics System Info

typedef struct OS_GfxInfo OS_GfxInfo;
struct OS_GfxInfo
{
  F32 double_click_time;
  F32 caret_blink_time;
  F32 default_refresh_rate;
};

////////////////////////////////
//~ rjf: Window Types

typedef U32 OS_WindowFlags;
enum
{
  OS_WindowFlag_CustomBorder       = (1<<0),
  OS_WindowFlag_UseDefaultPosition = (1<<1),
};

////////////////////////////////
//~ rjf: Cursor Types

typedef enum OS_Cursor
{
  OS_Cursor_Pointer,
  OS_Cursor_IBar,
  OS_Cursor_LeftRight,
  OS_Cursor_UpDown,
  OS_Cursor_DownRight,
  OS_Cursor_UpRight,
  OS_Cursor_UpDownLeftRight,
  OS_Cursor_HandPoint,
  OS_Cursor_Disabled,
  OS_Cursor_COUNT,
}
OS_Cursor;

////////////////////////////////
//~ rjf: Generated Code

#include "os/gfx/generated/os_gfx.meta.h"

////////////////////////////////
//~ rjf: Event Types

typedef enum OS_EventKind
{
  OS_EventKind_Null,
  OS_EventKind_Press,
  OS_EventKind_Release,
  OS_EventKind_MouseMove,
  OS_EventKind_Text,
  OS_EventKind_Scroll,
  OS_EventKind_WindowLoseFocus,
  OS_EventKind_WindowClose,
  OS_EventKind_FileDrop,
  OS_EventKind_Wakeup,
  OS_EventKind_COUNT
}
OS_EventKind;

typedef U32 OS_Modifiers;
enum
{
  OS_Modifier_Ctrl  = (1<<0),
  OS_Modifier_Shift = (1<<1),
  OS_Modifier_Alt   = (1<<2),
};

typedef struct OS_Event OS_Event;
struct OS_Event
{
  OS_Event *next;
  OS_Event *prev;
  U64 timestamp_us;
  OS_Handle window;
  OS_EventKind kind;
  OS_Modifiers modifiers;
  OS_Key key;
  B32 is_repeat;
  B32 right_sided;
  U32 character;
  U32 repeat_count;
  Vec2F32 pos;
  Vec2F32 delta;
  String8List strings;
};

typedef struct OS_EventList OS_EventList;
struct OS_EventList
{
  U64 count;
  OS_Event *first;
  OS_Event *last;
};

////////////////////////////////
//~ rjf: Application-Defined Frame Hook Forward Declaration

static B32 frame(void);

////////////////////////////////
//~ rjf: Event Functions (Helpers, Implemented Once)

static String8 os_string_from_event_kind(OS_EventKind kind);
static String8List os_string_list_from_modifiers(Arena *arena, OS_Modifiers flags);
static String8 os_string_from_modifiers_key(Arena *arena, OS_Modifiers modifiers, OS_Key key);
static U32 os_codepoint_from_modifiers_and_key(OS_Modifiers flags, OS_Key key);
static void os_eat_event(OS_EventList *events, OS_Event *event);
static B32  os_key_press(OS_EventList *events, OS_Handle window, OS_Modifiers modifiers, OS_Key key);
static B32  os_key_release(OS_EventList *events, OS_Handle window, OS_Modifiers modifiers, OS_Key key);
static B32  os_text(OS_EventList *events, OS_Handle window, U32 character);
static OS_EventList os_event_list_copy(Arena *arena, OS_EventList *src);
static void os_event_list_concat_in_place(OS_EventList *dst, OS_EventList *to_push);
static OS_Event *os_event_list_push_new(Arena *arena, OS_EventList *evts, OS_EventKind kind);

////////////////////////////////
//~ rjf: @os_hooks Main Initialization API (Implemented Per-OS)

static void os_gfx_init(void);

////////////////////////////////
//~ rjf: @os_hooks Graphics System Info (Implemented Per-OS)

static OS_GfxInfo *os_get_gfx_info(void);

////////////////////////////////
//~ rjf: @os_hooks Clipboards (Implemented Per-OS)

static void    os_set_clipboard_text(String8 string);
static String8 os_get_clipboard_text(Arena *arena);

////////////////////////////////
//~ rjf: @os_hooks Windows (Implemented Per-OS)

static OS_Handle      os_window_open(Rng2F32 rect, OS_WindowFlags flags, String8 title);
static void           os_window_close(OS_Handle window);
static void           os_window_set_title(OS_Handle window, String8 title);
static void           os_window_first_paint(OS_Handle window);
static void           os_window_focus(OS_Handle window);
static B32            os_window_is_focused(OS_Handle window);
static B32            os_window_is_fullscreen(OS_Handle window);
static void           os_window_set_fullscreen(OS_Handle window, B32 fullscreen);
static B32            os_window_is_maximized(OS_Handle window);
static void           os_window_set_maximized(OS_Handle window, B32 maximized);
static B32            os_window_is_minimized(OS_Handle window);
static void           os_window_set_minimized(OS_Handle window, B32 minimized);
static void           os_window_bring_to_front(OS_Handle window);
static void           os_window_set_monitor(OS_Handle window, OS_Handle monitor);
static void           os_window_clear_custom_border_data(OS_Handle handle);
static void           os_window_push_custom_title_bar(OS_Handle handle, F32 thickness);
static void           os_window_push_custom_edges(OS_Handle handle, F32 thickness);
static void           os_window_push_custom_title_bar_client_area(OS_Handle handle, Rng2F32 rect);
static Rng2F32        os_rect_from_window(OS_Handle window);
static Rng2F32        os_client_rect_from_window(OS_Handle window);
static F32            os_dpi_from_window(OS_Handle window);

////////////////////////////////
//~ rjf: @os_hooks Monitors (Implemented Per-OS)

static OS_HandleArray os_push_monitors_array(Arena *arena);
static OS_Handle      os_primary_monitor(void);
static OS_Handle      os_monitor_from_window(OS_Handle window);
static String8        os_name_from_monitor(Arena *arena, OS_Handle monitor);
static Vec2F32        os_dim_from_monitor(OS_Handle monitor);
static F32            os_dpi_from_monitor(OS_Handle monitor);

////////////////////////////////
//~ rjf: @os_hooks Events (Implemented Per-OS)

static void           os_send_wakeup_event(void);
static OS_EventList   os_get_events(Arena *arena, B32 wait);
static OS_Modifiers   os_get_modifiers(void);
static B32            os_key_is_down(OS_Key key);
static Vec2F32        os_mouse_from_window(OS_Handle window);

////////////////////////////////
//~ rjf: @os_hooks Cursors (Implemented Per-OS)

static void           os_set_cursor(OS_Cursor cursor);

////////////////////////////////
//~ rjf: @os_hooks Native User-Facing Graphical Messages (Implemented Per-OS)

static void           os_graphical_message(B32 error, String8 title, String8 message);
static String8        os_graphical_pick_file(Arena *arena, String8 initial_path);

////////////////////////////////
//~ rjf: @os_hooks Shell Operations

static void           os_show_in_filesystem_ui(String8 path);
static void           os_open_in_browser(String8 url);

#endif // OS_GFX_H

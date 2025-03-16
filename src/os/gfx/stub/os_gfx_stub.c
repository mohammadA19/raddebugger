////////////////////////////////
//~ rjf: @os_hooks Main Initialization API (Implemented Per-OS)

void
os_gfx_init()
{
}

////////////////////////////////
//~ rjf: @os_hooks Graphics System Info (Implemented Per-OS)

OS_GfxInfo *
os_get_gfx_info()
{
  static OS_GfxInfo g = {0};
  return &g;
}

////////////////////////////////
//~ rjf: @os_hooks Clipboards (Implemented Per-OS)

void
os_set_clipboard_text(String8 string)
{
}

String8
os_get_clipboard_text(Arena* arena)
{
  return str8_zero();
}

////////////////////////////////
//~ rjf: @os_hooks Windows (Implemented Per-OS)

OS_Handle
os_window_open(Vec2F32 resolution, OS_WindowFlags flags, String8 title)
{
  OS_Handle handle = {1};
  return handle;
}

void
os_window_close(OS_Handle window)
{
}

void
os_window_first_paint(OS_Handle window)
{
}

void
os_window_focus(OS_Handle window)
{
}

B32
os_window_is_focused(OS_Handle window)
{
  return 0;
}

B32
os_window_is_fullscreen(OS_Handle window)
{
  return 0;
}

void
os_window_set_fullscreen(OS_Handle window, B32 fullscreen)
{
}

B32
os_window_is_maximized(OS_Handle window)
{
  return 0;
}

void
os_window_set_maximized(OS_Handle window, B32 maximized)
{
}

B32
os_window_is_minimized(OS_Handle window)
{
  return 0;
}

void
os_window_set_minimized(OS_Handle window, B32 minimized)
{
}

void
os_window_bring_to_front(OS_Handle window)
{
}

void
os_window_set_monitor(OS_Handle window, OS_Handle monitor)
{
}

void
os_window_clear_custom_border_data(OS_Handle handle)
{
}

void
os_window_push_custom_title_bar(OS_Handle handle, F32 thickness)
{
}

void
os_window_push_custom_edges(OS_Handle handle, F32 thickness)
{
}

void
os_window_push_custom_title_bar_client_area(OS_Handle handle, Rng2F32 rect)
{
}

Rng2F32
os_rect_from_window(OS_Handle window)
{
  Rng2F32 rect = r2f32(v2f32(0, 0), v2f32(500, 500));
  return rect;
}

Rng2F32
os_client_rect_from_window(OS_Handle window)
{
  Rng2F32 rect = r2f32(v2f32(0, 0), v2f32(500, 500));
  return rect;
}

F32
os_dpi_from_window(OS_Handle window)
{
  return 96.f;
}

////////////////////////////////
//~ rjf: @os_hooks Monitors (Implemented Per-OS)

OS_HandleArray
os_push_monitors_array(Arena* arena)
{
  OS_HandleArray arr = {0};
  return arr;
}

OS_Handle
os_primary_monitor()
{
  OS_Handle handle = {1};
  return handle;
}

OS_Handle
os_monitor_from_window(OS_Handle window)
{
  OS_Handle handle = {1};
  return handle;
}

String8
os_name_from_monitor(Arena* arena, OS_Handle monitor)
{
  return str8_zero();
}

Vec2F32
os_dim_from_monitor(OS_Handle monitor)
{
  Vec2F32 v = v2f32(1000, 1000);
  return v;
}

////////////////////////////////
//~ rjf: @os_hooks Events (Implemented Per-OS)

void
os_send_wakeup_event()
{
}

OS_EventList
os_get_events(Arena* arena, B32 wait)
{
  OS_EventList evts = {0};
  return evts;
}

OS_Modifiers
os_get_modifiers()
{
  OS_Modifiers f = 0;
  return f;
}

B32
os_key_is_down(OS_Key key)
{
  return 0;
}

Vec2F32
os_mouse_from_window(OS_Handle window)
{
  return v2f32(0, 0);
}

////////////////////////////////
//~ rjf: @os_hooks Cursors (Implemented Per-OS)

void
os_set_cursor(OS_Cursor cursor)
{
}

////////////////////////////////
//~ rjf: @os_hooks Native User-Facing Graphical Messages (Implemented Per-OS)

void
os_graphical_message(B32 error, String8 title, String8 message)
{
}

////////////////////////////////
//~ rjf: @os_hooks Shell Operations

void
os_show_in_filesystem_ui(String8 path)
{
}

void
os_open_in_browser(String8 url)
{
}

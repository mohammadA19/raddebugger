////////////////////////////////
//~ rjf: @os_hooks Main Initialization API (Implemented Per-OS)

static void
os_gfx_init(void)
{
}

////////////////////////////////
//~ rjf: @os_hooks Graphics System Info (Implemented Per-OS)

static OS_GfxInfo *
os_get_gfx_info(void)
{
  local_persist OS_GfxInfo g = {0};
  return &g;
}

////////////////////////////////
//~ rjf: @os_hooks Clipboards (Implemented Per-OS)

static void
os_set_clipboard_text(String8 string)
{
}

static String8
os_get_clipboard_text(Arena *arena)
{
  return str8_zero();
}

////////////////////////////////
//~ rjf: @os_hooks Windows (Implemented Per-OS)

static OS_Handle
os_window_open(Rng2F32 rect, OS_WindowFlags flags, String8 title)
{
  OS_Handle handle = {1};
  return handle;
}

static void
os_window_close(OS_Handle window)
{
}

static void
os_window_set_title(OS_Handle window, String8 title)
{
}

static void
os_window_first_paint(OS_Handle window)
{
}

static void
os_window_focus(OS_Handle window)
{
}

static B32
os_window_is_focused(OS_Handle window)
{
  return 0;
}

static B32
os_window_is_fullscreen(OS_Handle window)
{
  return 0;
}

static void
os_window_set_fullscreen(OS_Handle window, B32 fullscreen)
{
}

static B32
os_window_is_maximized(OS_Handle window)
{
  return 0;
}

static void
os_window_set_maximized(OS_Handle window, B32 maximized)
{
}

static B32
os_window_is_minimized(OS_Handle window)
{
  return 0;
}

static void
os_window_set_minimized(OS_Handle window, B32 minimized)
{
}

static void
os_window_bring_to_front(OS_Handle window)
{
}

static void
os_window_set_monitor(OS_Handle window, OS_Handle monitor)
{
}

static void
os_window_clear_custom_border_data(OS_Handle handle)
{
}

static void
os_window_push_custom_title_bar(OS_Handle handle, F32 thickness)
{
}

static void
os_window_push_custom_edges(OS_Handle handle, F32 thickness)
{
}

static void
os_window_push_custom_title_bar_client_area(OS_Handle handle, Rng2F32 rect)
{
}

static Rng2F32
os_rect_from_window(OS_Handle window)
{
  Rng2F32 rect = r2f32(v2f32(0, 0), v2f32(500, 500));
  return rect;
}

static Rng2F32
os_client_rect_from_window(OS_Handle window)
{
  Rng2F32 rect = r2f32(v2f32(0, 0), v2f32(500, 500));
  return rect;
}

static F32
os_dpi_from_window(OS_Handle window)
{
  return 96.f;
}

////////////////////////////////
//~ rjf: @os_hooks Monitors (Implemented Per-OS)

static OS_HandleArray
os_push_monitors_array(Arena *arena)
{
  OS_HandleArray arr = {0};
  return arr;
}

static OS_Handle
os_primary_monitor(void)
{
  OS_Handle handle = {1};
  return handle;
}

static OS_Handle
os_monitor_from_window(OS_Handle window)
{
  OS_Handle handle = {1};
  return handle;
}

static String8
os_name_from_monitor(Arena *arena, OS_Handle monitor)
{
  return str8_zero();
}

static Vec2F32
os_dim_from_monitor(OS_Handle monitor)
{
  Vec2F32 v = v2f32(1000, 1000);
  return v;
}

static F32
os_dpi_from_monitor(OS_Handle monitor)
{
  return 96.f;
}

////////////////////////////////
//~ rjf: @os_hooks Events (Implemented Per-OS)

static void
os_send_wakeup_event(void)
{
}

static OS_EventList
os_get_events(Arena *arena, B32 wait)
{
  OS_EventList evts = {0};
  return evts;
}

static OS_Modifiers
os_get_modifiers(void)
{
  OS_Modifiers f = 0;
  return f;
}

static B32
os_key_is_down(OS_Key key)
{
  return 0;
}

static Vec2F32
os_mouse_from_window(OS_Handle window)
{
  return v2f32(0, 0);
}

////////////////////////////////
//~ rjf: @os_hooks Cursors (Implemented Per-OS)

static void
os_set_cursor(OS_Cursor cursor)
{
}

////////////////////////////////
//~ rjf: @os_hooks Native User-Facing Graphical Messages (Implemented Per-OS)

static void
os_graphical_message(B32 error, String8 title, String8 message)
{
}

static String8
os_graphical_pick_file(Arena *arena, String8 initial_path)
{
  return str8_zero();
}

////////////////////////////////
//~ rjf: @os_hooks Shell Operations

static void
os_show_in_filesystem_ui(String8 path)
{
}

static void
os_open_in_browser(String8 url)
{
}

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Includes / Libraries

#include <uxtheme.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#pragma comment(lib, "gdi32")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "UxTheme")
#pragma comment(lib, "ole32")
#pragma comment(lib, "user32")
#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION (0x00AE)
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME (0x00AF)
#endif

////////////////////////////////
//~ rjf: Windows

struct OS_W32_TitleBarClientArea
{
  OS_W32_TitleBarClientArea* next;
  Rng2F32 rect;
}

struct OS_W32_Window
{
  OS_W32_Window* next;
  OS_W32_Window* prev;
  HWND hwnd;
  WINDOWPLACEMENT last_window_placement;
  F32 dpi;
  B32 first_paint_done;
  B32 maximized;
  B32 custom_border;
  F32 custom_border_title_thickness;
  F32 custom_border_edge_thickness;
  B32 custom_border_composition_enabled;
  Arena* paint_arena;
  OS_W32_TitleBarClientArea* first_title_bar_client_area;
  OS_W32_TitleBarClientArea* last_title_bar_client_area;
}

////////////////////////////////
//~ rjf: Monitor Gathering Bundle

struct OS_W32_MonitorGatherBundle
{
  Arena* arena;
  OS_HandleList* list;
}

////////////////////////////////
//~ rjf: Global State

struct OS_W32_GfxState
{
  Arena* arena;
  U32 gfx_thread_tid;
  HINSTANCE hInstance;
  HCURSOR hCursor;
  OS_GfxInfo gfx_info;
  OS_W32_Window* first_window;
  OS_W32_Window* last_window;
  OS_W32_Window* free_window;
  OS_Key[256] key_from_vkey_table;
}

////////////////////////////////
//~ rjf: Globals

global OS_W32_GfxState *os_w32_gfx_state = 0;
global OS_EventList os_w32_event_list = {0};
global Arena *os_w32_event_arena = 0;
global B32 os_w32_resizing = 0;
global B32 os_w32_new_window_custom_border = 0;

////////////////////////////////
//~ rjf: Monitors



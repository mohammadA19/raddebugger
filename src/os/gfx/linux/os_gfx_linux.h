// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef OS_GFX_LINUX_H
#define OS_GFX_LINUX_H

////////////////////////////////
//~ rjf: Includes

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/sync.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

////////////////////////////////
//~ rjf: Window State

struct OS_LNX_Window
{
  OS_LNX_Window *next;
  OS_LNX_Window *prev;
  Window window;
  XID counter_xid;
  ulong counter_value;
};

////////////////////////////////
//~ rjf: State Bundle

struct OS_LNX_GfxState
{
  Arena *arena;
  Display *display;
  OS_LNX_Window *first_window;
  OS_LNX_Window *last_window;
  OS_LNX_Window *free_window;
  Atom wm_delete_window_atom;
  Atom wm_sync_request_atom;
  Atom wm_sync_request_counter_atom;
  OS_GfxInfo gfx_info;
};

////////////////////////////////
//~ rjf: Globals

static OS_LNX_GfxState *os_lnx_gfx_state = 0;

////////////////////////////////
//~ rjf: Helpers

OS_LNX_Window *os_lnx_window_from_x11window(Window window);

#endif // OS_GFX_LINUX_H

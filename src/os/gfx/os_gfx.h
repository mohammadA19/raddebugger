// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef OS_GFX_H
#define OS_GFX_H

////////////////////////////////
//~ rjf: Graphics System Info

struct OS_GfxInfo
{
  float double_click_time;
  float caret_blink_time;
  float default_refresh_rate;
};

////////////////////////////////
//~ rjf: Window Types

enum OS_WindowFlags : uint32
{
  OS_WindowFlag_CustomBorder       = (1<<0),
  OS_WindowFlag_UseDefaultPosition = (1<<1),
};

////////////////////////////////
//~ rjf: Cursor Types

enum OS_Cursor
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

////////////////////////////////
//~ rjf: Generated Code

#include "os/gfx/generated/os_gfx.meta.h"

////////////////////////////////
//~ rjf: Event Types

enum OS_EventKind
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

enum OS_Modifiers : uint32
{
  OS_Modifier_Ctrl  = (1<<0),
  OS_Modifier_Shift = (1<<1),
  OS_Modifier_Alt   = (1<<2),
};

struct OS_Event
{
  OS_Event *next;
  OS_Event *prev;
  uint64 timestamp_us;
  OS_Handle window;
  OS_EventKind kind;
  OS_Modifiers modifiers;
  OS_Key key;
  B32 is_repeat;
  B32 right_sided;
  uint32 character;
  uint32 repeat_count;
  Vec2<float> pos;
  Vec2<float> delta;
  String8List strings;
};

struct OS_EventList
{
  uint64 count;
  OS_Event *first;
  OS_Event *last;
};

////////////////////////////////
//~ rjf: Application-Defined Frame Hook Forward Declaration


////////////////////////////////
//~ rjf: Event Functions (Helpers, Implemented Once)


////////////////////////////////
//~ rjf: @os_hooks Main Initialization API (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Graphics System Info (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Clipboards (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Windows (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Monitors (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Events (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Cursors (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Native User-Facing Graphical Messages (Implemented Per-OS)


////////////////////////////////
//~ rjf: @os_hooks Shell Operations


#endif // OS_GFX_H

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


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



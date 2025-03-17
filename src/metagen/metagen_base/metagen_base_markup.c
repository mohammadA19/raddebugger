// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

void
set_thread_name(StringView str)
{
  ProfThreadName("%.*s", str8_varg(str));
  os_set_thread_name(str);
}

void
set_thread_namef(char* fmt, ...)
{
  Temp scratch = scratch_begin(0, 0);
  va_list args;
  va_start(args, fmt);
  StringView str = push_str8fv(scratch.arena, fmt, args);
  set_thread_name(str);
  va_end(args);
  scratch_end(scratch);
}

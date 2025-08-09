// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal void
set_thread_name(string string)
{
  ProfThreadName("%.*s", str8_varg(string));
  os_set_thread_name(string);
}

internal void
set_thread_namef(char* fmt, ...)
{
  Temp scratch = scratch_begin(0, 0);
  va_list args;
  va_start(args, fmt);
  string string = push_str8fv(scratch.arena, fmt, args);
  set_thread_name(string);
  va_end(args);
  scratch_end(scratch);
}

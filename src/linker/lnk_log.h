// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

enum
{
  LNK_Log_Null,
  LNK_Log_Debug,
  LNK_Log_InputObj,
  LNK_Log_InputLib,
  LNK_Log_IO_Read,
  LNK_Log_IO_Write,
  LNK_Log_SizeBreakdown,
  LNK_Log_LinkStats,
  LNK_Log_Timers,
  LNK_Log_Count
}

void set_log_level(LNK_LogType type, B32 is_enabled);
B32  lnk_get_log_status(LNK_LogType type);
void lnk_log(LNK_LogType type, char* fmt, ...);

LNK_LogType lnk_log_type_from_string(StringView string);


// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

LNK_TimerType :: enum {
  LNK_Timer_Image,
  LNK_Timer_Pdb,
  LNK_Timer_Rdi,
  LNK_Timer_Lib,
  LNK_Timer_Debug,
  LNK_Timer_Count
}

srtuct LNK_Timer
{
  U64 begin;
  U64 end;
}

void lnk_timer_begin(LNK_TimerType timer);
void lnk_timer_end(LNK_TimerType timer);


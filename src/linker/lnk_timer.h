// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

enum LNK_TimerType
{
  LNK_Timer_Image,
  LNK_Timer_Pdb,
  LNK_Timer_Rdi,
  LNK_Timer_Lib,
  LNK_Timer_Debug,
  LNK_Timer_Count
} LNK_TimerType;

typedef struct LNK_Timer
{
  U64 begin;
  U64 end;
} LNK_Timer;



// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- GENERATED CODE


typedef enum D_CmdKind
{
D_CmdKind_Null,
D_CmdKind_LaunchAndRun,
D_CmdKind_LaunchAndStepInto,
D_CmdKind_Kill,
D_CmdKind_KillAll,
D_CmdKind_Detach,
D_CmdKind_Continue,
D_CmdKind_StepIntoInst,
D_CmdKind_StepOverInst,
D_CmdKind_StepIntoLine,
D_CmdKind_StepOverLine,
D_CmdKind_StepOut,
D_CmdKind_Halt,
D_CmdKind_SoftHaltRefresh,
D_CmdKind_SetThreadIP,
D_CmdKind_RunToLine,
D_CmdKind_Run,
D_CmdKind_Restart,
D_CmdKind_StepInto,
D_CmdKind_StepOver,
D_CmdKind_FreezeThread,
D_CmdKind_ThawThread,
D_CmdKind_FreezeProcess,
D_CmdKind_ThawProcess,
D_CmdKind_FreezeMachine,
D_CmdKind_ThawMachine,
D_CmdKind_FreezeLocalMachine,
D_CmdKind_ThawLocalMachine,
D_CmdKind_FreezeEntity,
D_CmdKind_ThawEntity,
D_CmdKind_SetEntityColor,
D_CmdKind_SetEntityName,
D_CmdKind_Attach,
D_CmdKind_COUNT,
} D_CmdKind;

global B32 DEV_always_refresh = 0;
global B32 DEV_simulate_lag = 0;
global B32 DEV_draw_ui_text_pos = 0;
global B32 DEV_draw_ui_focus_debug = 0;
global B32 DEV_draw_ui_box_heatmap = 0;
global B32 DEV_eval_compiler_tooltips = 0;
global B32 DEV_eval_watch_key_tooltips = 0;
global B32 DEV_cmd_context_tooltips = 0;
global B32 DEV_updating_indicator = 0;
struct {B32 *value_ptr; string name;} DEV_toggle_table[] =
{
{&DEV_always_refresh, ("always_refresh")},
{&DEV_simulate_lag, ("simulate_lag")},
{&DEV_draw_ui_text_pos, ("draw_ui_text_pos")},
{&DEV_draw_ui_focus_debug, ("draw_ui_focus_debug")},
{&DEV_draw_ui_box_heatmap, ("draw_ui_box_heatmap")},
{&DEV_eval_compiler_tooltips, ("eval_compiler_tooltips")},
{&DEV_eval_watch_key_tooltips, ("eval_watch_key_tooltips")},
{&DEV_cmd_context_tooltips, ("cmd_context_tooltips")},
{&DEV_updating_indicator, ("updating_indicator")},
};

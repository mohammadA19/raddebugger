// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Fancy String Types

public struct DR_FancyString
{
  FNT_Tag font;
  String8 string;
  Vec4F32 color;
  float size;
  float underline_thickness;
  float strikethrough_thickness;
}

public struct DR_FancyStringNode
{
  DR_FancyStringNode* next;
  DR_FancyString v;
}

public struct DR_FancyStringList
{
  DR_FancyStringNode* first;
  DR_FancyStringNode* last;
  uint64 node_count;
  uint64 total_size;
}

public struct DR_FancyRun
{
  FNT_Run run;
  Vec4F32 color;
  float underline_thickness;
  float strikethrough_thickness;
}

public struct DR_FancyRunNode
{
  DR_FancyRunNode* next;
  DR_FancyRun v;
}

public struct DR_FancyRunList
{
  DR_FancyRunNode* first;
  DR_FancyRunNode* last;
  uint64 node_count;
  Vec2F32 dim;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/draw.meta.h"

////////////////////////////////
//~ rjf: Draw Bucket Types

public struct DR_Bucket
{
  R_PassList passes;
  uint64 stack_gen;
  uint64 last_cmd_stack_gen;
  DR_BucketStackDecls;
}

////////////////////////////////
//~ rjf: Thread Context

public struct DR_BucketSelectionNode
{
  DR_BucketSelectionNode* next;
  DR_Bucket* bucket;
}

public struct DR_ThreadCtx
{
  Arena* arena;
  uint64 arena_frame_start_pos;
  DR_BucketSelectionNode* top_bucket;
  DR_BucketSelectionNode* free_bucket_selection;
}

////////////////////////////////
//~ rjf: Globals

[ThreadStatic]
public static DR_ThreadCtx* dr_thread_ctx = null;

////////////////////////////////
//~ rjf: Fancy String Type Functions

#define dr_fancy_string_list_push_new(arena, list, font_, size_, color_, string_, ...) dr_fancy_string_list_push((arena), (list), &(DR_FancyString){.font = (font_), .string = (string_), .color = (color_), .size = (size_), __VA_ARGS__})

////////////////////////////////
//~ rjf: Bucket Construction & Selection API
//
// (Bucket: Handle to sequence of many render passes, constructed by this layer)

#define DR_BucketScope(b) DeferLoop(dr_push_bucket(b), dr_pop_bucket())

////////////////////////////////
//~ rjf: Bucket Stacks
//
// (Pushing/popping implicit draw parameters)

#define DR_Tex2DSampleKindScope(v)   DeferLoop(dr_push_tex2d_sample_kind(v), dr_pop_tex2d_sample_kind())
#define DR_XForm2DScope(v)           DeferLoop(dr_push_xform2d(v), dr_pop_xform2d())
#define DR_ClipScope(v)              DeferLoop(dr_push_clip(v), dr_pop_clip())
#define DR_TransparencyScope(v)      DeferLoop(dr_push_transparency(v), dr_pop_transparency())

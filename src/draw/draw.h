// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DRAW_H
#define DRAW_H

////////////////////////////////
//~ rjf: Fancy String Types

struct DR_FStrParams
{
  FNT_Tag font;
  FNT_RasterFlags raster_flags;
  Vec4F32 color;
  F32 size;
  F32 underline_thickness;
  F32 strikethrough_thickness;
}

struct DR_FStr
{
  String8 string;
  DR_FStrParams params;
}

struct DR_FStrNode
{
  DR_FStrNode *next;
  DR_FStr v;
}

struct DR_FStrList
{
  DR_FStrNode *first;
  DR_FStrNode *last;
  U64 node_count;
  U64 total_size;
}

struct DR_FRun
{
  FNT_Run run;
  Vec4F32 color;
  F32 underline_thickness;
  F32 strikethrough_thickness;
  B32 icon;
}

struct DR_FRunNode
{
  DR_FRunNode *next;
  DR_FRun v;
}

struct DR_FRunList
{
  DR_FRunNode *first;
  DR_FRunNode *last;
  U64 node_count;
  Vec2F32 dim;
}

////////////////////////////////
//~ rjf: Generated Code

#include "generated/draw.meta.h"

////////////////////////////////
//~ rjf: Draw Bucket Types

struct DR_Bucket
{
  R_PassList passes;
  U64 stack_gen;
  U64 last_cmd_stack_gen;
  DR_BucketStackDecls;
}

////////////////////////////////
//~ rjf: Thread Context

struct DR_BucketSelectionNode
{
  DR_BucketSelectionNode *next;
  DR_Bucket *bucket;
}

struct DR_ThreadCtx
{
  Arena *arena;
  U64 arena_frame_start_pos;
  FNT_Tag icon_font;
  DR_BucketSelectionNode *top_bucket;
  DR_BucketSelectionNode *free_bucket_selection;
}

////////////////////////////////
//~ rjf: Globals

thread_static DR_ThreadCtx *dr_thread_ctx = 0;

////////////////////////////////
//~ rjf: Fancy String Type Functions

#define dr_fstrs_push_new(arena, list, params, string, ...) dr_fstrs_push_new_((arena), (list), (params), &(DR_FStrParams){.size = 0, __VA_ARGS__}, (string))

////////////////////////////////
//~ rjf: Top-Level API
//
// (Frame boundaries & bucket submission)


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

////////////////////////////////
//~ rjf: Core Draw Calls
//
// (Apply to the calling thread's currently selected bucket)
////////////////////////////////
//~ rjf: Draw Call Helpers

//- rjf: text

#endif // DRAW_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DRAW_H
#define DRAW_H

////////////////////////////////
//~ rjf: Fancy String Types

struct DR_FancyString
{
  FNT_Tag font;
  String8 string;
  Vec4F32 color;
  float size;
  float underline_thickness;
  float strikethrough_thickness;
}

struct DR_FancyStringNode
{
  DR_FancyStringNode* next;
  DR_FancyString v;
}

struct DR_FancyStringList
{
  DR_FancyStringNode* first;
  DR_FancyStringNode* last;
  uint64 node_count;
  uint64 total_size;
}

struct DR_FancyRun
{
  FNT_Run run;
  Vec4F32 color;
  float underline_thickness;
  float strikethrough_thickness;
}

struct DR_FancyRunNode
{
  DR_FancyRunNode* next;
  DR_FancyRun v;
}

struct DR_FancyRunList
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

struct DR_Bucket
{
  R_PassList passes;
  uint64 stack_gen;
  uint64 last_cmd_stack_gen;
  DR_BucketStackDecls;
}

////////////////////////////////
//~ rjf: Thread Context

struct DR_BucketSelectionNode
{
  DR_BucketSelectionNode* next;
  DR_Bucket* bucket;
}

struct DR_ThreadCtx
{
  Arena* arena;
  uint64 arena_frame_start_pos;
  DR_BucketSelectionNode* top_bucket;
  DR_BucketSelectionNode* free_bucket_selection;
}

////////////////////////////////
//~ rjf: Globals

thread_static DR_ThreadCtx* dr_thread_ctx = 0;

////////////////////////////////
//~ rjf: Basic Helpers

uint64 dr_hash_from_string(String8 string);

////////////////////////////////
//~ rjf: Fancy String Type Functions

void dr_fancy_string_list_push(Arena* arena, DR_FancyStringList* list, DR_FancyString* str);
#define dr_fancy_string_list_push_new(arena, list, font_, size_, color_, string_, ...) dr_fancy_string_list_push((arena), (list), &(DR_FancyString){.font = (font_), .string = (string_), .color = (color_), .size = (size_), __VA_ARGS__})
void dr_fancy_string_list_concat_in_place(DR_FancyStringList* dst, DR_FancyStringList* to_push);
String8 dr_string_from_fancy_string_list(Arena* arena, DR_FancyStringList* list);
DR_FancyRunList dr_fancy_run_list_from_fancy_string_list(Arena* arena, float tab_size_px, FNT_RasterFlags flags, DR_FancyStringList* strs);
DR_FancyRunList dr_fancy_run_list_copy(Arena* arena, DR_FancyRunList* src);

////////////////////////////////
//~ rjf: Top-Level API
//
// (Frame boundaries & bucket submission)

void dr_begin_frame();
void dr_submit_bucket(OS_Handle os_window, R_Handle r_window, DR_Bucket* bucket);

////////////////////////////////
//~ rjf: Bucket Construction & Selection API
//
// (Bucket: Handle to sequence of many render passes, constructed by this layer)

DR_Bucket* dr_bucket_make();
void dr_push_bucket(DR_Bucket* bucket);
void dr_pop_bucket();
DR_Bucket* dr_top_bucket();
#define DR_BucketScope(b) DeferLoop(dr_push_bucket(b), dr_pop_bucket())

////////////////////////////////
//~ rjf: Bucket Stacks
//
// (Pushing/popping implicit draw parameters)

R_Tex2DSampleKind          dr_push_tex2d_sample_kind(R_Tex2DSampleKind v);
Mat3x3F32                  dr_push_xform2d(Mat3x3F32 v);
Rng2F32                    dr_push_clip(Rng2F32 v);
float                        dr_push_transparency(float v);
R_Tex2DSampleKind          dr_pop_tex2d_sample_kind();
Mat3x3F32                  dr_pop_xform2d();
Rng2F32                    dr_pop_clip();
float                        dr_pop_transparency();
R_Tex2DSampleKind          dr_top_tex2d_sample_kind();
Mat3x3F32                  dr_top_xform2d();
Rng2F32                    dr_top_clip();
float                        dr_top_transparency();

#define DR_Tex2DSampleKindScope(v)   DeferLoop(dr_push_tex2d_sample_kind(v), dr_pop_tex2d_sample_kind())
#define DR_XForm2DScope(v)           DeferLoop(dr_push_xform2d(v), dr_pop_xform2d())
#define DR_ClipScope(v)              DeferLoop(dr_push_clip(v), dr_pop_clip())
#define DR_TransparencyScope(v)      DeferLoop(dr_push_transparency(v), dr_pop_transparency())

////////////////////////////////
//~ rjf: Core Draw Calls
//
// (Apply to the calling thread's currently selected bucket)

//- rjf: rectangles
inline R_Rect2DInst* dr_rect(Rng2F32 dst, Vec4F32 color, float corner_radius, float border_thickness, float edge_softness);

//- rjf: images
inline R_Rect2DInst* dr_img(Rng2F32 dst, Rng2F32 src, R_Handle texture, Vec4F32 color, float corner_radius, float border_thickness, float edge_softness);

//- rjf: blurs
R_PassParams_Blur* dr_blur(Rng2F32 rect, float blur_size, float corner_radius);

//- rjf: 3d rendering pass params
R_PassParams_Geo3D* dr_geo3d_begin(Rng2F32 viewport, Mat4x4F32 view, Mat4x4F32 projection);

//- rjf: meshes
R_Mesh3DInst* dr_mesh(R_Handle mesh_vertices, R_Handle mesh_indices, R_GeoTopologyKind mesh_geo_topology, R_GeoVertexFlags mesh_geo_vertex_flags, R_Handle albedo_tex, Mat4x4F32 inst_xform);

//- rjf: collating one pre-prepped bucket into parent bucket
void dr_sub_bucket(DR_Bucket* bucket);

////////////////////////////////
//~ rjf: Draw Call Helpers

//- rjf: text
void dr_truncated_fancy_run_list(Vec2F32 p, DR_FancyRunList* list, float max_x, FNT_Run trailer_run);
void dr_truncated_fancy_run_fuzzy_matches(Vec2F32 p, DR_FancyRunList* list, float max_x, FuzzyMatchRangeList* ranges, Vec4F32 color);
void dr_text_run(Vec2F32 p, Vec4F32 color, FNT_Run run);
void dr_text(FNT_Tag font, float size, float base_align_px, float tab_size_px, FNT_RasterFlags flags, Vec2F32 p, Vec4F32 color, String8 string);

#endif // DRAW_H

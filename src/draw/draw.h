// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DRAW_H
#define DRAW_H

////////////////////////////////
//~ rjf: Fancy String Types

typedef struct DR_FStrParams DR_FStrParams;
struct DR_FStrParams
{
  FNT_Tag font;
  FNT_RasterFlags raster_flags;
  Vec4float color;
  float size;
  float underline_thickness;
  float strikethrough_thickness;
};

typedef struct DR_FStr DR_FStr;
struct DR_FStr
{
  String8 string;
  DR_FStrParams params;
};

typedef struct DR_FStrNode DR_FStrNode;
struct DR_FStrNode
{
  DR_FStrNode *next;
  DR_FStr v;
};

typedef struct DR_FStrList DR_FStrList;
struct DR_FStrList
{
  DR_FStrNode *first;
  DR_FStrNode *last;
  uint64 node_count;
  uint64 total_size;
};

typedef struct DR_FRun DR_FRun;
struct DR_FRun
{
  FNT_Run run;
  Vec4float color;
  float underline_thickness;
  float strikethrough_thickness;
  B32 icon;
};

typedef struct DR_FRunNode DR_FRunNode;
struct DR_FRunNode
{
  DR_FRunNode *next;
  DR_FRun v;
};

typedef struct DR_FRunList DR_FRunList;
struct DR_FRunList
{
  DR_FRunNode *first;
  DR_FRunNode *last;
  uint64 node_count;
  Vec2float dim;
};

////////////////////////////////
//~ rjf: Generated Code

#include "generated/draw.meta.h"

////////////////////////////////
//~ rjf: Draw Bucket Types

typedef struct DR_Bucket DR_Bucket;
struct DR_Bucket
{
  R_PassList passes;
  uint64 stack_gen;
  uint64 last_cmd_stack_gen;
  DR_BucketStackDecls;
};

////////////////////////////////
//~ rjf: Thread Context

typedef struct DR_BucketSelectionNode DR_BucketSelectionNode;
struct DR_BucketSelectionNode
{
  DR_BucketSelectionNode *next;
  DR_Bucket *bucket;
};

typedef struct DR_ThreadCtx DR_ThreadCtx;
struct DR_ThreadCtx
{
  Arena *arena;
  uint64 arena_frame_start_pos;
  FNT_Tag icon_font;
  DR_BucketSelectionNode *top_bucket;
  DR_BucketSelectionNode *free_bucket_selection;
};

////////////////////////////////
//~ rjf: Globals

thread_static DR_ThreadCtx *dr_thread_ctx = 0;

////////////////////////////////
//~ rjf: Basic Helpers

internal uint64 dr_hash_from_string(String8 string);

////////////////////////////////
//~ rjf: Fancy String Type Functions

internal void dr_fstrs_push(Arena *arena, DR_FStrList *list, DR_FStr *str);
internal void dr_fstrs_push_new_(Arena *arena, DR_FStrList *list, DR_FStrParams *params, DR_FStrParams *overrides, String8 string);
#define dr_fstrs_push_new(arena, list, params, string, ...) dr_fstrs_push_new_((arena), (list), (params), &(DR_FStrParams){.size = 0, __VA_ARGS__}, (string))
internal void dr_fstrs_concat_in_place(DR_FStrList *dst, DR_FStrList *to_push);
internal DR_FStrList dr_fstrs_copy(Arena *arena, DR_FStrList *src);
internal String8 dr_string_from_fstrs(Arena *arena, DR_FStrList *list);
internal FuzzyMatchRangeList dr_fuzzy_match_find_from_fstrs(Arena *arena, DR_FStrList *fstrs, String8 needle);
internal DR_FRunList dr_fruns_from_fstrs(Arena *arena, float tab_size_px, DR_FStrList *strs);
internal Vec2float dr_dim_from_fstrs(float tab_size_px, DR_FStrList *fstrs);

////////////////////////////////
//~ rjf: Top-Level API
//
// (Frame boundaries & bucket submission)

internal void dr_begin_frame(FNT_Tag icon_font);
internal void dr_submit_bucket(OS_Handle os_window, R_Handle r_window, DR_Bucket *bucket);

////////////////////////////////
//~ rjf: Bucket Construction & Selection API
//
// (Bucket: Handle to sequence of many render passes, constructed by this layer)

internal DR_Bucket *dr_bucket_make(void);
internal void dr_push_bucket(DR_Bucket *bucket);
internal void dr_pop_bucket(void);
internal DR_Bucket *dr_top_bucket(void);
#define DR_BucketScope(b) DeferLoop(dr_push_bucket(b), dr_pop_bucket())

////////////////////////////////
//~ rjf: Bucket Stacks
//
// (Pushing/popping implicit draw parameters)

internal R_Tex2DSampleKind          dr_push_tex2d_sample_kind(R_Tex2DSampleKind v);
internal Mat3x3float                  dr_push_xform2d(Mat3x3float v);
internal Rng2float                    dr_push_clip(Rng2float v);
internal float                        dr_push_transparency(float v);
internal R_Tex2DSampleKind          dr_pop_tex2d_sample_kind(void);
internal Mat3x3float                  dr_pop_xform2d(void);
internal Rng2float                    dr_pop_clip(void);
internal float                        dr_pop_transparency(void);
internal R_Tex2DSampleKind          dr_top_tex2d_sample_kind(void);
internal Mat3x3float                  dr_top_xform2d(void);
internal Rng2float                    dr_top_clip(void);
internal float                        dr_top_transparency(void);

#define DR_Tex2DSampleKindScope(v)   DeferLoop(dr_push_tex2d_sample_kind(v), dr_pop_tex2d_sample_kind())
#define DR_XForm2DScope(v)           DeferLoop(dr_push_xform2d(v), dr_pop_xform2d())
#define DR_ClipScope(v)              DeferLoop(dr_push_clip(v), dr_pop_clip())
#define DR_TransparencyScope(v)      DeferLoop(dr_push_transparency(v), dr_pop_transparency())

////////////////////////////////
//~ rjf: Core Draw Calls
//
// (Apply to the calling thread's currently selected bucket)

//- rjf: rectangles
internal inline R_Rect2DInst *dr_rect(Rng2float dst, Vec4float color, float corner_radius, float border_thickness, float edge_softness);

//- rjf: images
internal inline R_Rect2DInst *dr_img(Rng2float dst, Rng2float src, R_Handle texture, Vec4float color, float corner_radius, float border_thickness, float edge_softness);

//- rjf: blurs
internal R_PassParams_Blur *dr_blur(Rng2float rect, float blur_size, float corner_radius);

//- rjf: 3d rendering pass params
internal R_PassParams_Geo3D *dr_geo3d_begin(Rng2float viewport, Mat4x4float view, Mat4x4float projection);

//- rjf: meshes
internal R_Mesh3DInst *dr_mesh(R_Handle mesh_vertices, R_Handle mesh_indices, R_GeoTopologyKind mesh_geo_topology, R_GeoVertexFlags mesh_geo_vertex_flags, R_Handle albedo_tex, Mat4x4float inst_xform);

//- rjf: collating one pre-prepped bucket into parent bucket
internal void dr_sub_bucket(DR_Bucket *bucket);

////////////////////////////////
//~ rjf: Draw Call Helpers

//- rjf: text
internal void dr_truncated_fancy_run_list(Vec2float p, DR_FRunList *list, float max_x, FNT_Run trailer_run);
internal void dr_truncated_fancy_run_fuzzy_matches(Vec2float p, DR_FRunList *list, float max_x, FuzzyMatchRangeList *ranges, Vec4float color);
internal void dr_text_run(Vec2float p, Vec4float color, FNT_Run run);
internal void dr_text(FNT_Tag font, float size, float base_align_px, float tab_size_px, FNT_RasterFlags flags, Vec2float p, Vec4float color, String8 string);

#endif // DRAW_H

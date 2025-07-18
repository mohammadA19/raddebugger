// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_MATH_H
#define BASE_MATH_H

////////////////////////////////
//~ rjf: Vector Types

//- rjf: 2-vectors

union Vec2float
{
  struct
  {
    float x;
    float y;
  };
  float v[2];
};

union Vec2uint64
{
  struct
  {
    uint64 x;
    uint64 y;
  };
  uint64 v[2];
};

union Vec2uint32
{
  struct
  {
    uint32 x;
    uint32 y;
  };
  uint32 v[2];
};

union Vec2uint16
{
  struct
  {
    uint16 x;
    uint16 y;
  };
  uint16 v[2];
};

//- rjf: 3-vectors

union Vec3float
{
  struct
  {
    float x;
    float y;
    float z;
  };
  struct
  {
    Vec2float xy;
    float _z0;
  };
  struct
  {
    float _x0;
    Vec2float yz;
  };
  float v[3];
};

union Vec3uint32
{
  struct
  {
    uint32 x;
    uint32 y;
    uint32 z;
  };
  struct
  {
    Vec2uint32 xy;
    uint32 _z0;
  };
  struct
  {
    uint32 _x0;
    Vec2uint32 yz;
  };
  uint32 v[3];
};

//- rjf: 4-vectors

union Vec4float
{
  struct
  {
    float x;
    float y;
    float z;
    float w;
  };
  struct
  {
    Vec2float xy;
    Vec2float zw;
  };
  struct
  {
    Vec3float xyz;
    float _z0;
  };
  struct
  {
    float _x0;
    Vec3float yzw;
  };
  float v[4];
};

union Vec4uint32
{
  struct
  {
    uint32 x;
    uint32 y;
    uint32 z;
    uint32 w;
  };
  struct
  {
    Vec2uint32 xy;
    Vec2uint32 zw;
  };
  struct
  {
    Vec3uint32 xyz;
    uint32 _z0;
  };
  struct
  {
    uint32 _x0;
    Vec3uint32 yzw;
  };
  uint32 v[4];
};

////////////////////////////////
//~ rjf: Matrix Types

struct Mat3x3float
{
  float v[3][3];
};

struct Mat4x4float
{
  float v[4][4];
};

////////////////////////////////
//~ rjf: Range Types

//- rjf: 1-range

union Rng1uint32
{
  struct
  {
    uint32 min;
    uint32 max;
  };
  uint32 v[2];
};

union Rng1uint32
{
  struct
  {
    uint32 min;
    uint32 max;
  };
  uint32 v[2];
};

union Rng1uint64
{
  struct
  {
    uint64 min;
    uint64 max;
  };
  uint64 v[2];
};

union Rng1uint64
{
  struct
  {
    uint64 min;
    uint64 max;
  };
  uint64 v[2];
};

union Rng1float
{
  struct
  {
    float min;
    float max;
  };
  float v[2];
};

//- rjf: 2-range (rectangles)

union Rng2uint16
{
  struct
  {
    Vec2uint16 min;
    Vec2uint16 max;
  };
  struct
  {
    Vec2uint16 p0;
    Vec2uint16 p1;
  };
  struct
  {
    uint16 x0;
    uint16 y0;
    uint16 x1;
    uint16 y1;
  };
  Vec2uint16 v[2];
};

union Rng2uint32
{
  struct
  {
    Vec2uint32 min;
    Vec2uint32 max;
  };
  struct
  {
    Vec2uint32 p0;
    Vec2uint32 p1;
  };
  struct
  {
    uint32 x0;
    uint32 y0;
    uint32 x1;
    uint32 y1;
  };
  Vec2uint32 v[2];
};

union Rng2float
{
  struct
  {
    Vec2float min;
    Vec2float max;
  };
  struct
  {
    Vec2float p0;
    Vec2float p1;
  };
  struct
  {
    float x0;
    float y0;
    float x1;
    float y1;
  };
  Vec2float v[2];
};

union Rng2uint64
{
  struct
  {
    Vec2uint64 min;
    Vec2uint64 max;
  };
  struct
  {
    Vec2uint64 p0;
    Vec2uint64 p1;
  };
  struct
  {
    uint64 x0;
    uint64 y0;
    uint64 x1;
    uint64 y1;
  };
  Vec2uint64 v[2];
};

////////////////////////////////
//~ rjf: List Types

struct Rng1uint64Node
{
  Rng1uint64Node *next;
  Rng1uint64 v;
};

struct Rng1uint64List
{
  uint64 count;
  Rng1uint64Node *first;
  Rng1uint64Node *last;
};

struct Rng1uint64Array
{
  Rng1uint64 *v;
  uint64 count;
};

struct Rng1uint64Node
{
  Rng1uint64Node *next;
  Rng1uint64 v;
};

struct Rng1uint64List
{
  Rng1uint64Node *first;
  Rng1uint64Node *last;
  uint64 count;
};

struct Rng1uint64Array
{
  Rng1uint64 *v;
  uint64 count;
};

////////////////////////////////
//~ rjf: Scalar Ops

#define abs_s64(v) (uint64)llabs(v)

#define sqrt_f32(v)   sqrtf(v)
#define cbrt_f32(v)   cbrtf(v)
#define mod_f32(a, b) fmodf((a), (b))
#define pow_f32(b, e) powf((b), (e))
#define ceil_f32(v)   ceilf(v)
#define floor_f32(v)  floorf(v)
#define round_f32(v)  roundf(v)
#define abs_f32(v)    fabsf(v)
#define radians_from_turns_f32(v) ((v)*(2*3.1415926535897f))
#define turns_from_radians_f32(v) ((v)/(2*3.1415926535897f))
#define degrees_from_turns_f32(v) ((v)*360.f)
#define turns_from_degrees_f32(v) ((v)/360.f)
#define degrees_from_radians_f32(v) (degrees_from_turns_f32(turns_from_radians_f32(v)))
#define radians_from_degrees_f32(v) (radians_from_turns_f32(turns_from_degrees_f32(v)))
#define sin_f32(v)    sinf(radians_from_turns_f32(v))
#define cos_f32(v)    cosf(radians_from_turns_f32(v))
#define tan_f32(v)    tanf(radians_from_turns_f32(v))

#define sqrt_f64(v)   sqrt(v)
#define cbrt_f64(v)   cbrt(v)
#define mod_f64(a, b) fmod((a), (b))
#define pow_f64(b, e) pow((b), (e))
#define ceil_f64(v)   ceil(v)
#define floor_f64(v)  floor(v)
#define round_f64(v)  round(v)
#define abs_f64(v)    fabs(v)
#define radians_from_turns_f64(v) ((v)*(2*3.1415926535897))
#define turns_from_radians_f64(v) ((v)/(2*3.1415926535897))
#define degrees_from_turns_f64(v) ((v)*360.0)
#define turns_from_degrees_f64(v) ((v)/360.0)
#define degrees_from_radians_f64(v) (degrees_from_turns_f64(turns_from_radians_f64(v)))
#define radians_from_degrees_f64(v) (radians_from_turns_f64(turns_from_degrees_f64(v)))
#define sin_f64(v)    sin(radians_from_turns_f64(v))
#define cos_f64(v)    cos(radians_from_turns_f64(v))
#define tan_f64(v)    tan(radians_from_turns_f64(v))

internal float mix_1f32(float a, float b, float t);
internal double mix_1f64(double a, double b, double t);

////////////////////////////////
//~ rjf: Vector Ops

#define v2f32(x, y) vec_2f32((x), (y))
internal Vec2float vec_2f32(float x, float y);
internal Vec2float add_2f32(Vec2float a, Vec2float b);
internal Vec2float sub_2f32(Vec2float a, Vec2float b);
internal Vec2float mul_2f32(Vec2float a, Vec2float b);
internal Vec2float div_2f32(Vec2float a, Vec2float b);
internal Vec2float scale_2f32(Vec2float v, float s);
internal float dot_2f32(Vec2float a, Vec2float b);
internal float length_squared_2f32(Vec2float v);
internal float length_2f32(Vec2float v);
internal Vec2float normalize_2f32(Vec2float v);
internal Vec2float mix_2f32(Vec2float a, Vec2float b, float t);

#define v2s64(x, y) vec_2s64((x), (y))
internal Vec2uint64 vec_2s64(uint64 x, uint64 y);
internal Vec2uint64 add_2s64(Vec2uint64 a, Vec2uint64 b);
internal Vec2uint64 sub_2s64(Vec2uint64 a, Vec2uint64 b);
internal Vec2uint64 mul_2s64(Vec2uint64 a, Vec2uint64 b);
internal Vec2uint64 div_2s64(Vec2uint64 a, Vec2uint64 b);
internal Vec2uint64 scale_2s64(Vec2uint64 v, uint64 s);
internal uint64 dot_2s64(Vec2uint64 a, Vec2uint64 b);
internal uint64 length_squared_2s64(Vec2uint64 v);
internal uint64 length_2s64(Vec2uint64 v);
internal Vec2uint64 normalize_2s64(Vec2uint64 v);
internal Vec2uint64 mix_2s64(Vec2uint64 a, Vec2uint64 b, float t);

#define v2s32(x, y) vec_2s32((x), (y))
internal Vec2uint32 vec_2s32(uint32 x, uint32 y);
internal Vec2uint32 add_2s32(Vec2uint32 a, Vec2uint32 b);
internal Vec2uint32 sub_2s32(Vec2uint32 a, Vec2uint32 b);
internal Vec2uint32 mul_2s32(Vec2uint32 a, Vec2uint32 b);
internal Vec2uint32 div_2s32(Vec2uint32 a, Vec2uint32 b);
internal Vec2uint32 scale_2s32(Vec2uint32 v, uint32 s);
internal uint32 dot_2s32(Vec2uint32 a, Vec2uint32 b);
internal uint32 length_squared_2s32(Vec2uint32 v);
internal uint32 length_2s32(Vec2uint32 v);
internal Vec2uint32 normalize_2s32(Vec2uint32 v);
internal Vec2uint32 mix_2s32(Vec2uint32 a, Vec2uint32 b, float t);

#define v2s16(x, y) vec_2s16((x), (y))
internal Vec2uint16 vec_2s16(uint16 x, uint16 y);
internal Vec2uint16 add_2s16(Vec2uint16 a, Vec2uint16 b);
internal Vec2uint16 sub_2s16(Vec2uint16 a, Vec2uint16 b);
internal Vec2uint16 mul_2s16(Vec2uint16 a, Vec2uint16 b);
internal Vec2uint16 div_2s16(Vec2uint16 a, Vec2uint16 b);
internal Vec2uint16 scale_2s16(Vec2uint16 v, uint16 s);
internal uint16 dot_2s16(Vec2uint16 a, Vec2uint16 b);
internal uint16 length_squared_2s16(Vec2uint16 v);
internal uint16 length_2s16(Vec2uint16 v);
internal Vec2uint16 normalize_2s16(Vec2uint16 v);
internal Vec2uint16 mix_2s16(Vec2uint16 a, Vec2uint16 b, float t);

#define v3f32(x, y, z) vec_3f32((x), (y), (z))
internal Vec3float vec_3f32(float x, float y, float z);
internal Vec3float add_3f32(Vec3float a, Vec3float b);
internal Vec3float sub_3f32(Vec3float a, Vec3float b);
internal Vec3float mul_3f32(Vec3float a, Vec3float b);
internal Vec3float div_3f32(Vec3float a, Vec3float b);
internal Vec3float scale_3f32(Vec3float v, float s);
internal float dot_3f32(Vec3float a, Vec3float b);
internal float length_squared_3f32(Vec3float v);
internal float length_3f32(Vec3float v);
internal Vec3float normalize_3f32(Vec3float v);
internal Vec3float mix_3f32(Vec3float a, Vec3float b, float t);
internal Vec3float cross_3f32(Vec3float a, Vec3float b);

#define v3s32(x, y, z) vec_3s32((x), (y), (z))
internal Vec3uint32 vec_3s32(uint32 x, uint32 y, uint32 z);
internal Vec3uint32 add_3s32(Vec3uint32 a, Vec3uint32 b);
internal Vec3uint32 sub_3s32(Vec3uint32 a, Vec3uint32 b);
internal Vec3uint32 mul_3s32(Vec3uint32 a, Vec3uint32 b);
internal Vec3uint32 div_3s32(Vec3uint32 a, Vec3uint32 b);
internal Vec3uint32 scale_3s32(Vec3uint32 v, uint32 s);
internal uint32 dot_3s32(Vec3uint32 a, Vec3uint32 b);
internal uint32 length_squared_3s32(Vec3uint32 v);
internal uint32 length_3s32(Vec3uint32 v);
internal Vec3uint32 normalize_3s32(Vec3uint32 v);
internal Vec3uint32 mix_3s32(Vec3uint32 a, Vec3uint32 b, float t);
internal Vec3uint32 cross_3s32(Vec3uint32 a, Vec3uint32 b);

#define v4f32(x, y, z, w) vec_4f32((x), (y), (z), (w))
internal Vec4float vec_4f32(float x, float y, float z, float w);
internal Vec4float add_4f32(Vec4float a, Vec4float b);
internal Vec4float sub_4f32(Vec4float a, Vec4float b);
internal Vec4float mul_4f32(Vec4float a, Vec4float b);
internal Vec4float div_4f32(Vec4float a, Vec4float b);
internal Vec4float scale_4f32(Vec4float v, float s);
internal float dot_4f32(Vec4float a, Vec4float b);
internal float length_squared_4f32(Vec4float v);
internal float length_4f32(Vec4float v);
internal Vec4float normalize_4f32(Vec4float v);
internal Vec4float mix_4f32(Vec4float a, Vec4float b, float t);

#define v4s32(x, y, z, w) vec_4s32((x), (y), (z), (w))
internal Vec4uint32 vec_4s32(uint32 x, uint32 y, uint32 z, uint32 w);
internal Vec4uint32 add_4s32(Vec4uint32 a, Vec4uint32 b);
internal Vec4uint32 sub_4s32(Vec4uint32 a, Vec4uint32 b);
internal Vec4uint32 mul_4s32(Vec4uint32 a, Vec4uint32 b);
internal Vec4uint32 div_4s32(Vec4uint32 a, Vec4uint32 b);
internal Vec4uint32 scale_4s32(Vec4uint32 v, uint32 s);
internal uint32 dot_4s32(Vec4uint32 a, Vec4uint32 b);
internal uint32 length_squared_4s32(Vec4uint32 v);
internal uint32 length_4s32(Vec4uint32 v);
internal Vec4uint32 normalize_4s32(Vec4uint32 v);
internal Vec4uint32 mix_4s32(Vec4uint32 a, Vec4uint32 b, float t);

////////////////////////////////
//~ rjf: Matrix Ops

internal Mat3x3float mat_3x3f32(float diagonal);
internal Mat3x3float make_translate_3x3f32(Vec2float delta);
internal Mat3x3float make_scale_3x3f32(Vec2float scale);
internal Mat3x3float mul_3x3f32(Mat3x3float a, Mat3x3float b);

internal Mat4x4float mat_4x4f32(float diagonal);
internal Mat4x4float make_translate_4x4f32(Vec3float delta);
internal Mat4x4float make_scale_4x4f32(Vec3float scale);
internal Mat4x4float make_perspective_4x4f32(float fov, float aspect_ratio, float near_z, float far_z);
internal Mat4x4float make_orthographic_4x4f32(float left, float right, float bottom, float top, float near_z, float far_z);
internal Mat4x4float make_look_at_4x4f32(Vec3float eye, Vec3float center, Vec3float up);
internal Mat4x4float make_rotate_4x4f32(Vec3float axis, float turns);
internal Mat4x4float mul_4x4f32(Mat4x4float a, Mat4x4float b);
internal Mat4x4float scale_4x4f32(Mat4x4float m, float scale);
internal Mat4x4float inverse_4x4f32(Mat4x4float m);
internal Mat4x4float derotate_4x4f32(Mat4x4float mat);
internal Mat4x4float transpose_4x4f32(Mat4x4float mat);

////////////////////////////////
//~ rjf: Range Ops

#define r1u32(min, max) rng_1u32((min), (max))
internal Rng1uint32 rng_1u32(uint32 min, uint32 max);
internal Rng1uint32 shift_1u32(Rng1uint32 r, uint32 x);
internal Rng1uint32 pad_1u32(Rng1uint32 r, uint32 x);
internal uint32 center_1u32(Rng1uint32 r);
internal B32 contains_1u32(Rng1uint32 r, uint32 x);
internal uint32 dim_1u32(Rng1uint32 r);
internal Rng1uint32 union_1u32(Rng1uint32 a, Rng1uint32 b);
internal Rng1uint32 intersect_1u32(Rng1uint32 a, Rng1uint32 b);
internal uint32 clamp_1u32(Rng1uint32 r, uint32 v);

#define r1s32(min, max) rng_1s32((min), (max))
internal Rng1uint32 rng_1s32(uint32 min, uint32 max);
internal Rng1uint32 shift_1s32(Rng1uint32 r, uint32 x);
internal Rng1uint32 pad_1s32(Rng1uint32 r, uint32 x);
internal uint32 center_1s32(Rng1uint32 r);
internal B32 contains_1s32(Rng1uint32 r, uint32 x);
internal uint32 dim_1s32(Rng1uint32 r);
internal Rng1uint32 union_1s32(Rng1uint32 a, Rng1uint32 b);
internal Rng1uint32 intersect_1s32(Rng1uint32 a, Rng1uint32 b);
internal uint32 clamp_1s32(Rng1uint32 r, uint32 v);

#define r1u64(min, max) rng_1u64((min), (max))
internal Rng1uint64 rng_1u64(uint64 min, uint64 max);
internal Rng1uint64 shift_1u64(Rng1uint64 r, uint64 x);
internal Rng1uint64 pad_1u64(Rng1uint64 r, uint64 x);
internal uint64 center_1u64(Rng1uint64 r);
internal B32 contains_1u64(Rng1uint64 r, uint64 x);
internal uint64 dim_1u64(Rng1uint64 r);
internal Rng1uint64 union_1u64(Rng1uint64 a, Rng1uint64 b);
internal Rng1uint64 intersect_1u64(Rng1uint64 a, Rng1uint64 b);
internal uint64 clamp_1u64(Rng1uint64 r, uint64 v);

#define r1s64(min, max) rng_1s64((min), (max))
internal Rng1uint64 rng_1s64(uint64 min, uint64 max);
internal Rng1uint64 shift_1s64(Rng1uint64 r, uint64 x);
internal Rng1uint64 pad_1s64(Rng1uint64 r, uint64 x);
internal uint64 center_1s64(Rng1uint64 r);
internal B32 contains_1s64(Rng1uint64 r, uint64 x);
internal uint64 dim_1s64(Rng1uint64 r);
internal Rng1uint64 union_1s64(Rng1uint64 a, Rng1uint64 b);
internal Rng1uint64 intersect_1s64(Rng1uint64 a, Rng1uint64 b);
internal uint64 clamp_1s64(Rng1uint64 r, uint64 v);

#define r1f32(min, max) rng_1f32((min), (max))
internal Rng1float rng_1f32(float min, float max);
internal Rng1float shift_1f32(Rng1float r, float x);
internal Rng1float pad_1f32(Rng1float r, float x);
internal float center_1f32(Rng1float r);
internal B32 contains_1f32(Rng1float r, float x);
internal float dim_1f32(Rng1float r);
internal Rng1float union_1f32(Rng1float a, Rng1float b);
internal Rng1float intersect_1f32(Rng1float a, Rng1float b);
internal float clamp_1f32(Rng1float r, float v);

#define r2s16(min, max) rng_2s16((min), (max))
#define r2s16p(x, y, z, w) r2s16(v2s16((x), (y)), v2s16((z), (w)))
internal Rng2uint16 rng_2s16(Vec2uint16 min, Vec2uint16 max);
internal Rng2uint16 shift_2s16(Rng2uint16 r, Vec2uint16 x);
internal Rng2uint16 pad_2s16(Rng2uint16 r, uint16 x);
internal Vec2uint16 center_2s16(Rng2uint16 r);
internal B32 contains_2s16(Rng2uint16 r, Vec2uint16 x);
internal Vec2uint16 dim_2s16(Rng2uint16 r);
internal Rng2uint16 union_2s16(Rng2uint16 a, Rng2uint16 b);
internal Rng2uint16 intersect_2s16(Rng2uint16 a, Rng2uint16 b);
internal Vec2uint16 clamp_2s16(Rng2uint16 r, Vec2uint16 v);

#define r2s32(min, max) rng_2s32((min), (max))
#define r2s32p(x, y, z, w) r2s32(v2s32((x), (y)), v2s32((z), (w)))
internal Rng2uint32 rng_2s32(Vec2uint32 min, Vec2uint32 max);
internal Rng2uint32 shift_2s32(Rng2uint32 r, Vec2uint32 x);
internal Rng2uint32 pad_2s32(Rng2uint32 r, uint32 x);
internal Vec2uint32 center_2s32(Rng2uint32 r);
internal B32 contains_2s32(Rng2uint32 r, Vec2uint32 x);
internal Vec2uint32 dim_2s32(Rng2uint32 r);
internal Rng2uint32 union_2s32(Rng2uint32 a, Rng2uint32 b);
internal Rng2uint32 intersect_2s32(Rng2uint32 a, Rng2uint32 b);
internal Vec2uint32 clamp_2s32(Rng2uint32 r, Vec2uint32 v);

#define r2s64(min, max) rng_2s64((min), (max))
#define r2s64p(x, y, z, w) r2s64(v2s64((x), (y)), v2s64((z), (w)))
internal Rng2uint64 rng_2s64(Vec2uint64 min, Vec2uint64 max);
internal Rng2uint64 shift_2s64(Rng2uint64 r, Vec2uint64 x);
internal Rng2uint64 pad_2s64(Rng2uint64 r, uint64 x);
internal Vec2uint64 center_2s64(Rng2uint64 r);
internal B32 contains_2s64(Rng2uint64 r, Vec2uint64 x);
internal Vec2uint64 dim_2s64(Rng2uint64 r);
internal Rng2uint64 union_2s64(Rng2uint64 a, Rng2uint64 b);
internal Rng2uint64 intersect_2s64(Rng2uint64 a, Rng2uint64 b);
internal Vec2uint64 clamp_2s64(Rng2uint64 r, Vec2uint64 v);

#define r2f32(min, max) rng_2f32((min), (max))
#define r2f32p(x, y, z, w) r2f32(v2f32((x), (y)), v2f32((z), (w)))
internal Rng2float rng_2f32(Vec2float min, Vec2float max);
internal Rng2float shift_2f32(Rng2float r, Vec2float x);
internal Rng2float pad_2f32(Rng2float r, float x);
internal Vec2float center_2f32(Rng2float r);
internal B32 contains_2f32(Rng2float r, Vec2float x);
internal Vec2float dim_2f32(Rng2float r);
internal Rng2float union_2f32(Rng2float a, Rng2float b);
internal Rng2float intersect_2f32(Rng2float a, Rng2float b);
internal Vec2float clamp_2f32(Rng2float r, Vec2float v);

////////////////////////////////
//~ rjf: Color Operations

//- rjf: hsv <-> rgb
internal Vec3float hsv_from_rgb(Vec3float rgb);
internal Vec3float rgb_from_hsv(Vec3float hsv);
internal Vec4float hsva_from_rgba(Vec4float rgba);
internal Vec4float rgba_from_hsva(Vec4float hsva);

//- rjf: srgb <-> linear
internal Vec3float linear_from_srgb(Vec3float srgb);
internal Vec3float srgb_from_linear(Vec3float linear);
internal Vec4float linear_from_srgba(Vec4float srgba);
internal Vec4float srgba_from_linear(Vec4float linear);

//- rjf: oklab <-> linear
internal Vec3float oklab_from_linear(Vec3float linear);
internal Vec3float linear_from_oklab(Vec3float oklab);
internal Vec4float oklab_from_lineara(Vec4float lineara);
internal Vec4float lineara_from_oklab(Vec4float oklab);

//- rjf: rgba <-> u32
internal uint32 u32_from_rgba(Vec4float rgba);
internal Vec4float rgba_from_u32(uint32 hex);
#define rgba_from_u32_lit_comp(h) { (((h)&0xff000000)>>24)/255.f, (((h)&0x00ff0000)>>16)/255.f, (((h)&0x0000ff00)>> 8)/255.f, (((h)&0x000000ff)>> 0)/255.f }

////////////////////////////////
//~ rjf: List Type Functions

internal void rng1u64_list_push(Arena *arena, Rng1uint64List *list, Rng1uint64 rng);
internal void rng1u64_list_concat(Rng1uint64List *list, Rng1uint64List *to_concat);
internal Rng1uint64Array rng1u64_array_from_list(Arena *arena, Rng1uint64List *list);
internal uint64 rng_1u64_array_bsearch(Rng1uint64Array arr, uint64 value);

internal void rng1s64_list_push(Arena *arena, Rng1uint64List *list, Rng1uint64 rng);
internal Rng1uint64Array rng1s64_array_from_list(Arena *arena, Rng1uint64List *list);

#endif // BASE_MATH_H

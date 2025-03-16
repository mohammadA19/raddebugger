// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_MATH_H
#define BASE_MATH_H

////////////////////////////////
//~ rjf: Vector Types

//- rjf: 2-vectors

union Vec2F32
{
  struct
  {
    float x;
    float y;
  };
  float v[2];
}

union Vec2S64
{
  struct
  {
    int64 x;
    int64 y;
  };
  int64 v[2];
}

union Vec2S32
{
  struct
  {
    int32 x;
    int32 y;
  };
  int32 v[2];
}

union Vec2S16
{
  struct
  {
    int16 x;
    int16 y;
  };
  int16 v[2];
}

//- rjf: 3-vectors

union Vec3F32
{
  struct
  {
    float x;
    float y;
    float z;
  };
  struct
  {
    Vec2F32 xy;
    float _z0;
  };
  struct
  {
    float _x0;
    Vec2F32 yz;
  };
  float v[3];
}

union Vec3S32
{
  struct
  {
    int32 x;
    int32 y;
    int32 z;
  };
  struct
  {
    Vec2S32 xy;
    int32 _z0;
  };
  struct
  {
    int32 _x0;
    Vec2S32 yz;
  };
  int32 v[3];
}

//- rjf: 4-vectors

union Vec4F32
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
    Vec2F32 xy;
    Vec2F32 zw;
  };
  struct
  {
    Vec3F32 xyz;
    float _z0;
  };
  struct
  {
    float _x0;
    Vec3F32 yzw;
  };
  float v[4];
}

union Vec4S32
{
  struct
  {
    int32 x;
    int32 y;
    int32 z;
    int32 w;
  };
  struct
  {
    Vec2S32 xy;
    Vec2S32 zw;
  };
  struct
  {
    Vec3S32 xyz;
    int32 _z0;
  };
  struct
  {
    int32 _x0;
    Vec3S32 yzw;
  };
  int32 v[4];
}

////////////////////////////////
//~ rjf: Matrix Types

struct Mat3x3F32
{
  float v[3][3];
}

struct Mat4x4F32
{
  float v[4][4];
}

////////////////////////////////
//~ rjf: Range Types

//- rjf: 1-range

union Rng1U32
{
  struct
  {
    uint32 min;
    uint32 max;
  };
  uint32 v[2];
}

union Rng1S32
{
  struct
  {
    int32 min;
    int32 max;
  };
  int32 v[2];
}

union Rng1U64
{
  struct
  {
    uint64 min;
    uint64 max;
  };
  uint64 v[2];
}

union Rng1S64
{
  struct
  {
    int64 min;
    int64 max;
  };
  int64 v[2];
}

union Rng1F32
{
  struct
  {
    float min;
    float max;
  };
  float v[2];
}

//- rjf: 2-range (rectangles)

union Rng2S16
{
  struct
  {
    Vec2S16 min;
    Vec2S16 max;
  };
  struct
  {
    Vec2S16 p0;
    Vec2S16 p1;
  };
  struct
  {
    int16 x0;
    int16 y0;
    int16 x1;
    int16 y1;
  };
  Vec2S16 v[2];
}

union Rng2S32
{
  struct
  {
    Vec2S32 min;
    Vec2S32 max;
  };
  struct
  {
    Vec2S32 p0;
    Vec2S32 p1;
  };
  struct
  {
    int32 x0;
    int32 y0;
    int32 x1;
    int32 y1;
  };
  Vec2S32 v[2];
}

union Rng2F32
{
  struct
  {
    Vec2F32 min;
    Vec2F32 max;
  };
  struct
  {
    Vec2F32 p0;
    Vec2F32 p1;
  };
  struct
  {
    float x0;
    float y0;
    float x1;
    float y1;
  };
  Vec2F32 v[2];
}

union Rng2S64
{
  struct
  {
    Vec2S64 min;
    Vec2S64 max;
  };
  struct
  {
    Vec2S64 p0;
    Vec2S64 p1;
  };
  struct
  {
    int64 x0;
    int64 y0;
    int64 x1;
    int64 y1;
  };
  Vec2S64 v[2];
}

////////////////////////////////
//~ rjf: List Types

struct Rng1U64Node
{
  Rng1U64Node* next;
  Rng1U64 v;
}

struct Rng1U64List
{
  uint64 count;
  Rng1U64Node* first;
  Rng1U64Node* last;
}

struct Rng1U64Array
{
  Rng1U64* v;
  uint64 count;
}

struct Rng1S64Node
{
  Rng1S64Node* next;
  Rng1S64 v;
}

struct Rng1S64List
{
  Rng1S64Node* first;
  Rng1S64Node* last;
  uint64 count;
}

struct Rng1S64Array
{
  Rng1S64* v;
  uint64 count;
}

////////////////////////////////
//~ rjf: Scalar Ops

#define abs_s64(v) (int64)llabs(v)

#define sqrt_f32(v)   sqrtf(v)
#define mod_f32(a, b) fmodf((a), (b))
#define pow_f32(b, e) powf((b), (e))
#define ceil_f32(v)   ceilf(v)
#define floor_f32(v)  floorf(v)
#define round_f32(v)  roundf(v)
#define abs_f32(v)    fabsf(v)
#define radians_from_turns_f32(v) ((v)*2*3.1415926535897f)
#define turns_from_radians_f32(v) ((v)/2*3.1415926535897f)
#define degrees_from_turns_f32(v) ((v)*360.f)
#define turns_from_degrees_f32(v) ((v)/360.f)
#define degrees_from_radians_f32(v) (degrees_from_turns_f32(turns_from_radians_f32(v)))
#define radians_from_degrees_f32(v) (radians_from_turns_f32(turns_from_degrees_f32(v)))
#define sin_f32(v)    sinf(radians_from_turns_f32(v))
#define cos_f32(v)    cosf(radians_from_turns_f32(v))
#define tan_f32(v)    tanf(radians_from_turns_f32(v))

#define sqrt_f64(v)   sqrt(v)
#define mod_f64(a, b) fmod((a), (b))
#define pow_f64(b, e) pow((b), (e))
#define ceil_f64(v)   ceil(v)
#define floor_f64(v)  floor(v)
#define round_f64(v)  round(v)
#define abs_f64(v)    fabs(v)
#define radians_from_turns_f64(v) ((v)*2*3.1415926535897)
#define turns_from_radians_f64(v) ((v)/2*3.1415926535897)
#define degrees_from_turns_f64(v) ((v)*360.0)
#define turns_from_degrees_f64(v) ((v)/360.0)
#define degrees_from_radians_f64(v) (degrees_from_turns_f64(turns_from_radians_f64(v)))
#define radians_from_degrees_f64(v) (radians_from_turns_f64(turns_from_degrees_f64(v)))
#define sin_f64(v)    sin(radians_from_turns_f64(v))
#define cos_f64(v)    cos(radians_from_turns_f64(v))
#define tan_f64(v)    tan(radians_from_turns_f64(v))

float mix_1f32(float a, float b, float t);
double mix_1f64(double a, double b, double t);

////////////////////////////////
//~ rjf: Vector Ops

#define v2f32(x, y) vec_2f32((x), (y))
Vec2F32 vec_2f32(float x, float y);
Vec2F32 add_2f32(Vec2F32 a, Vec2F32 b);
Vec2F32 sub_2f32(Vec2F32 a, Vec2F32 b);
Vec2F32 mul_2f32(Vec2F32 a, Vec2F32 b);
Vec2F32 div_2f32(Vec2F32 a, Vec2F32 b);
Vec2F32 scale_2f32(Vec2F32 v, float s);
float dot_2f32(Vec2F32 a, Vec2F32 b);
float length_squared_2f32(Vec2F32 v);
float length_2f32(Vec2F32 v);
Vec2F32 normalize_2f32(Vec2F32 v);
Vec2F32 mix_2f32(Vec2F32 a, Vec2F32 b, float t);

#define v2s64(x, y) vec_2s64((x), (y))
Vec2S64 vec_2s64(int64 x, int64 y);
Vec2S64 add_2s64(Vec2S64 a, Vec2S64 b);
Vec2S64 sub_2s64(Vec2S64 a, Vec2S64 b);
Vec2S64 mul_2s64(Vec2S64 a, Vec2S64 b);
Vec2S64 div_2s64(Vec2S64 a, Vec2S64 b);
Vec2S64 scale_2s64(Vec2S64 v, int64 s);
int64 dot_2s64(Vec2S64 a, Vec2S64 b);
int64 length_squared_2s64(Vec2S64 v);
int64 length_2s64(Vec2S64 v);
Vec2S64 normalize_2s64(Vec2S64 v);
Vec2S64 mix_2s64(Vec2S64 a, Vec2S64 b, float t);

#define v2s32(x, y) vec_2s32((x), (y))
Vec2S32 vec_2s32(int32 x, int32 y);
Vec2S32 add_2s32(Vec2S32 a, Vec2S32 b);
Vec2S32 sub_2s32(Vec2S32 a, Vec2S32 b);
Vec2S32 mul_2s32(Vec2S32 a, Vec2S32 b);
Vec2S32 div_2s32(Vec2S32 a, Vec2S32 b);
Vec2S32 scale_2s32(Vec2S32 v, int32 s);
int32 dot_2s32(Vec2S32 a, Vec2S32 b);
int32 length_squared_2s32(Vec2S32 v);
int32 length_2s32(Vec2S32 v);
Vec2S32 normalize_2s32(Vec2S32 v);
Vec2S32 mix_2s32(Vec2S32 a, Vec2S32 b, float t);

#define v2s16(x, y) vec_2s16((x), (y))
Vec2S16 vec_2s16(int16 x, int16 y);
Vec2S16 add_2s16(Vec2S16 a, Vec2S16 b);
Vec2S16 sub_2s16(Vec2S16 a, Vec2S16 b);
Vec2S16 mul_2s16(Vec2S16 a, Vec2S16 b);
Vec2S16 div_2s16(Vec2S16 a, Vec2S16 b);
Vec2S16 scale_2s16(Vec2S16 v, int16 s);
int16 dot_2s16(Vec2S16 a, Vec2S16 b);
int16 length_squared_2s16(Vec2S16 v);
int16 length_2s16(Vec2S16 v);
Vec2S16 normalize_2s16(Vec2S16 v);
Vec2S16 mix_2s16(Vec2S16 a, Vec2S16 b, float t);

#define v3f32(x, y, z) vec_3f32((x), (y), (z))
Vec3F32 vec_3f32(float x, float y, float z);
Vec3F32 add_3f32(Vec3F32 a, Vec3F32 b);
Vec3F32 sub_3f32(Vec3F32 a, Vec3F32 b);
Vec3F32 mul_3f32(Vec3F32 a, Vec3F32 b);
Vec3F32 div_3f32(Vec3F32 a, Vec3F32 b);
Vec3F32 scale_3f32(Vec3F32 v, float s);
float dot_3f32(Vec3F32 a, Vec3F32 b);
float length_squared_3f32(Vec3F32 v);
float length_3f32(Vec3F32 v);
Vec3F32 normalize_3f32(Vec3F32 v);
Vec3F32 mix_3f32(Vec3F32 a, Vec3F32 b, float t);
Vec3F32 cross_3f32(Vec3F32 a, Vec3F32 b);

#define v3s32(x, y, z) vec_3s32((x), (y), (z))
Vec3S32 vec_3s32(int32 x, int32 y, int32 z);
Vec3S32 add_3s32(Vec3S32 a, Vec3S32 b);
Vec3S32 sub_3s32(Vec3S32 a, Vec3S32 b);
Vec3S32 mul_3s32(Vec3S32 a, Vec3S32 b);
Vec3S32 div_3s32(Vec3S32 a, Vec3S32 b);
Vec3S32 scale_3s32(Vec3S32 v, int32 s);
int32 dot_3s32(Vec3S32 a, Vec3S32 b);
int32 length_squared_3s32(Vec3S32 v);
int32 length_3s32(Vec3S32 v);
Vec3S32 normalize_3s32(Vec3S32 v);
Vec3S32 mix_3s32(Vec3S32 a, Vec3S32 b, float t);
Vec3S32 cross_3s32(Vec3S32 a, Vec3S32 b);

#define v4f32(x, y, z, w) vec_4f32((x), (y), (z), (w))
Vec4F32 vec_4f32(float x, float y, float z, float w);
Vec4F32 add_4f32(Vec4F32 a, Vec4F32 b);
Vec4F32 sub_4f32(Vec4F32 a, Vec4F32 b);
Vec4F32 mul_4f32(Vec4F32 a, Vec4F32 b);
Vec4F32 div_4f32(Vec4F32 a, Vec4F32 b);
Vec4F32 scale_4f32(Vec4F32 v, float s);
float dot_4f32(Vec4F32 a, Vec4F32 b);
float length_squared_4f32(Vec4F32 v);
float length_4f32(Vec4F32 v);
Vec4F32 normalize_4f32(Vec4F32 v);
Vec4F32 mix_4f32(Vec4F32 a, Vec4F32 b, float t);

#define v4s32(x, y, z, w) vec_4s32((x), (y), (z), (w))
Vec4S32 vec_4s32(int32 x, int32 y, int32 z, int32 w);
Vec4S32 add_4s32(Vec4S32 a, Vec4S32 b);
Vec4S32 sub_4s32(Vec4S32 a, Vec4S32 b);
Vec4S32 mul_4s32(Vec4S32 a, Vec4S32 b);
Vec4S32 div_4s32(Vec4S32 a, Vec4S32 b);
Vec4S32 scale_4s32(Vec4S32 v, int32 s);
int32 dot_4s32(Vec4S32 a, Vec4S32 b);
int32 length_squared_4s32(Vec4S32 v);
int32 length_4s32(Vec4S32 v);
Vec4S32 normalize_4s32(Vec4S32 v);
Vec4S32 mix_4s32(Vec4S32 a, Vec4S32 b, float t);

////////////////////////////////
//~ rjf: Matrix Ops

Mat3x3F32 mat_3x3f32(float diagonal);
Mat3x3F32 make_translate_3x3f32(Vec2F32 delta);
Mat3x3F32 make_scale_3x3f32(Vec2F32 scale);
Mat3x3F32 mul_3x3f32(Mat3x3F32 a, Mat3x3F32 b);

Mat4x4F32 mat_4x4f32(float diagonal);
Mat4x4F32 make_translate_4x4f32(Vec3F32 delta);
Mat4x4F32 make_scale_4x4f32(Vec3F32 scale);
Mat4x4F32 make_perspective_4x4f32(float fov, float aspect_ratio, float near_z, float far_z);
Mat4x4F32 make_orthographic_4x4f32(float left, float right, float bottom, float top, float near_z, float far_z);
Mat4x4F32 make_look_at_4x4f32(Vec3F32 eye, Vec3F32 center, Vec3F32 up);
Mat4x4F32 make_rotate_4x4f32(Vec3F32 axis, float turns);
Mat4x4F32 mul_4x4f32(Mat4x4F32 a, Mat4x4F32 b);
Mat4x4F32 scale_4x4f32(Mat4x4F32 m, float scale);
Mat4x4F32 inverse_4x4f32(Mat4x4F32 m);
Mat4x4F32 derotate_4x4f32(Mat4x4F32 mat);

////////////////////////////////
//~ rjf: Range Ops

#define r1u32(min, max) rng_1u32((min), (max))
Rng1U32 rng_1u32(uint32 min, uint32 max);
Rng1U32 shift_1u32(Rng1U32 r, uint32 x);
Rng1U32 pad_1u32(Rng1U32 r, uint32 x);
uint32 center_1u32(Rng1U32 r);
B32 contains_1u32(Rng1U32 r, uint32 x);
uint32 dim_1u32(Rng1U32 r);
Rng1U32 union_1u32(Rng1U32 a, Rng1U32 b);
Rng1U32 intersect_1u32(Rng1U32 a, Rng1U32 b);
uint32 clamp_1u32(Rng1U32 r, uint32 v);

#define r1s32(min, max) rng_1s32((min), (max))
Rng1S32 rng_1s32(int32 min, int32 max);
Rng1S32 shift_1s32(Rng1S32 r, int32 x);
Rng1S32 pad_1s32(Rng1S32 r, int32 x);
int32 center_1s32(Rng1S32 r);
B32 contains_1s32(Rng1S32 r, int32 x);
int32 dim_1s32(Rng1S32 r);
Rng1S32 union_1s32(Rng1S32 a, Rng1S32 b);
Rng1S32 intersect_1s32(Rng1S32 a, Rng1S32 b);
int32 clamp_1s32(Rng1S32 r, int32 v);

#define r1u64(min, max) rng_1u64((min), (max))
Rng1U64 rng_1u64(uint64 min, uint64 max);
Rng1U64 shift_1u64(Rng1U64 r, uint64 x);
Rng1U64 pad_1u64(Rng1U64 r, uint64 x);
uint64 center_1u64(Rng1U64 r);
B32 contains_1u64(Rng1U64 r, uint64 x);
uint64 dim_1u64(Rng1U64 r);
Rng1U64 union_1u64(Rng1U64 a, Rng1U64 b);
Rng1U64 intersect_1u64(Rng1U64 a, Rng1U64 b);
uint64 clamp_1u64(Rng1U64 r, uint64 v);

#define r1s64(min, max) rng_1s64((min), (max))
Rng1S64 rng_1s64(int64 min, int64 max);
Rng1S64 shift_1s64(Rng1S64 r, int64 x);
Rng1S64 pad_1s64(Rng1S64 r, int64 x);
int64 center_1s64(Rng1S64 r);
B32 contains_1s64(Rng1S64 r, int64 x);
int64 dim_1s64(Rng1S64 r);
Rng1S64 union_1s64(Rng1S64 a, Rng1S64 b);
Rng1S64 intersect_1s64(Rng1S64 a, Rng1S64 b);
int64 clamp_1s64(Rng1S64 r, int64 v);

#define r1f32(min, max) rng_1f32((min), (max))
Rng1F32 rng_1f32(float min, float max);
Rng1F32 shift_1f32(Rng1F32 r, float x);
Rng1F32 pad_1f32(Rng1F32 r, float x);
float center_1f32(Rng1F32 r);
B32 contains_1f32(Rng1F32 r, float x);
float dim_1f32(Rng1F32 r);
Rng1F32 union_1f32(Rng1F32 a, Rng1F32 b);
Rng1F32 intersect_1f32(Rng1F32 a, Rng1F32 b);
float clamp_1f32(Rng1F32 r, float v);

#define r2s16(min, max) rng_2s16((min), (max))
#define r2s16p(x, y, z, w) r2s16(v2s16((x), (y)), v2s16((z), (w)))
Rng2S16 rng_2s16(Vec2S16 min, Vec2S16 max);
Rng2S16 shift_2s16(Rng2S16 r, Vec2S16 x);
Rng2S16 pad_2s16(Rng2S16 r, int16 x);
Vec2S16 center_2s16(Rng2S16 r);
B32 contains_2s16(Rng2S16 r, Vec2S16 x);
Vec2S16 dim_2s16(Rng2S16 r);
Rng2S16 union_2s16(Rng2S16 a, Rng2S16 b);
Rng2S16 intersect_2s16(Rng2S16 a, Rng2S16 b);
Vec2S16 clamp_2s16(Rng2S16 r, Vec2S16 v);

#define r2s32(min, max) rng_2s32((min), (max))
#define r2s32p(x, y, z, w) r2s32(v2s32((x), (y)), v2s32((z), (w)))
Rng2S32 rng_2s32(Vec2S32 min, Vec2S32 max);
Rng2S32 shift_2s32(Rng2S32 r, Vec2S32 x);
Rng2S32 pad_2s32(Rng2S32 r, int32 x);
Vec2S32 center_2s32(Rng2S32 r);
B32 contains_2s32(Rng2S32 r, Vec2S32 x);
Vec2S32 dim_2s32(Rng2S32 r);
Rng2S32 union_2s32(Rng2S32 a, Rng2S32 b);
Rng2S32 intersect_2s32(Rng2S32 a, Rng2S32 b);
Vec2S32 clamp_2s32(Rng2S32 r, Vec2S32 v);

#define r2s64(min, max) rng_2s64((min), (max))
#define r2s64p(x, y, z, w) r2s64(v2s64((x), (y)), v2s64((z), (w)))
Rng2S64 rng_2s64(Vec2S64 min, Vec2S64 max);
Rng2S64 shift_2s64(Rng2S64 r, Vec2S64 x);
Rng2S64 pad_2s64(Rng2S64 r, int64 x);
Vec2S64 center_2s64(Rng2S64 r);
B32 contains_2s64(Rng2S64 r, Vec2S64 x);
Vec2S64 dim_2s64(Rng2S64 r);
Rng2S64 union_2s64(Rng2S64 a, Rng2S64 b);
Rng2S64 intersect_2s64(Rng2S64 a, Rng2S64 b);
Vec2S64 clamp_2s64(Rng2S64 r, Vec2S64 v);

#define r2f32(min, max) rng_2f32((min), (max))
#define r2f32p(x, y, z, w) r2f32(v2f32((x), (y)), v2f32((z), (w)))
Rng2F32 rng_2f32(Vec2F32 min, Vec2F32 max);
Rng2F32 shift_2f32(Rng2F32 r, Vec2F32 x);
Rng2F32 pad_2f32(Rng2F32 r, float x);
Vec2F32 center_2f32(Rng2F32 r);
B32 contains_2f32(Rng2F32 r, Vec2F32 x);
Vec2F32 dim_2f32(Rng2F32 r);
Rng2F32 union_2f32(Rng2F32 a, Rng2F32 b);
Rng2F32 intersect_2f32(Rng2F32 a, Rng2F32 b);
Vec2F32 clamp_2f32(Rng2F32 r, Vec2F32 v);

////////////////////////////////
//~ rjf: Miscellaneous Ops

Vec3F32 hsv_from_rgb(Vec3F32 rgb);
Vec3F32 rgb_from_hsv(Vec3F32 hsv);
Vec4F32 hsva_from_rgba(Vec4F32 rgba);
Vec4F32 rgba_from_hsva(Vec4F32 hsva);
Vec4F32 rgba_from_u32(uint32 hex);
uint32 u32_from_rgba(Vec4F32 rgba);

#define rgba_from_u32_lit_comp(h) { (((h)&0xff000000)>>24)/255.f, (((h)&0x00ff0000)>>16)/255.f, (((h)&0x0000ff00)>> 8)/255.f, (((h)&0x000000ff)>> 0)/255.f }

////////////////////////////////
//~ rjf: List Type Functions

void rng1u64_list_push(Arena* arena, Rng1U64List* list, Rng1U64 rng);
void rng1u64_list_concat(Rng1U64List* list, Rng1U64List* to_concat);
Rng1U64Array rng1u64_array_from_list(Arena* arena, Rng1U64List* list);

void rng1s64_list_push(Arena* arena, Rng1S64List* list, Rng1S64 rng);
Rng1S64Array rng1s64_array_from_list(Arena* arena, Rng1S64List* list);

#endif // BASE_MATH_H

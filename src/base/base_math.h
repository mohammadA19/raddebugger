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


////////////////////////////////
//~ rjf: Vector Ops

#define v2f32(x, y) vec_2f32((x), (y))

#define v2s64(x, y) vec_2s64((x), (y))

#define v2s32(x, y) vec_2s32((x), (y))

#define v2s16(x, y) vec_2s16((x), (y))

#define v3f32(x, y, z) vec_3f32((x), (y), (z))

#define v3s32(x, y, z) vec_3s32((x), (y), (z))

#define v4f32(x, y, z, w) vec_4f32((x), (y), (z), (w))

#define v4s32(x, y, z, w) vec_4s32((x), (y), (z), (w))

////////////////////////////////
//~ rjf: Matrix Ops



////////////////////////////////
//~ rjf: Range Ops

#define r1u32(min, max) rng_1u32((min), (max))

#define r1s32(min, max) rng_1s32((min), (max))

#define r1u64(min, max) rng_1u64((min), (max))

#define r1s64(min, max) rng_1s64((min), (max))

#define r1f32(min, max) rng_1f32((min), (max))

#define r2s16(min, max) rng_2s16((min), (max))
#define r2s16p(x, y, z, w) r2s16(v2s16((x), (y)), v2s16((z), (w)))

#define r2s32(min, max) rng_2s32((min), (max))
#define r2s32p(x, y, z, w) r2s32(v2s32((x), (y)), v2s32((z), (w)))

#define r2s64(min, max) rng_2s64((min), (max))
#define r2s64p(x, y, z, w) r2s64(v2s64((x), (y)), v2s64((z), (w)))

#define r2f32(min, max) rng_2f32((min), (max))
#define r2f32p(x, y, z, w) r2f32(v2f32((x), (y)), v2f32((z), (w)))

////////////////////////////////
//~ rjf: Color Operations

//- rjf: hsv <-> rgb

//- rjf: srgb <-> linear

//- rjf: oklab <-> linear

//- rjf: rgba <-> u32
#define rgba_from_u32_lit_comp(h) { (((h)&0xff000000)>>24)/255.f, (((h)&0x00ff0000)>>16)/255.f, (((h)&0x0000ff00)>> 8)/255.f, (((h)&0x000000ff)>> 0)/255.f }

////////////////////////////////
//~ rjf: List Type Functions



#endif // BASE_MATH_H

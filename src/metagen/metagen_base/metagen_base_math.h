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
    F32 x;
    F32 y;
  }
  F32 v[2];
}

union Vec2S64
{
  struct
  {
    S64 x;
    S64 y;
  }
  S64 v[2];
}

union Vec2S32
{
  struct
  {
    S32 x;
    S32 y;
  }
  S32 v[2];
}

union Vec2S16
{
  struct
  {
    S16 x;
    S16 y;
  }
  S16 v[2];
}

//- rjf: 3-vectors

union Vec3F32
{
  struct
  {
    F32 x;
    F32 y;
    F32 z;
  }
  struct
  {
    Vec2F32 xy;
    F32 _z0;
  }
  struct
  {
    F32 _x0;
    Vec2F32 yz;
  }
  F32 v[3];
}

union Vec3S32
{
  struct
  {
    S32 x;
    S32 y;
    S32 z;
  }
  struct
  {
    Vec2S32 xy;
    S32 _z0;
  }
  struct
  {
    S32 _x0;
    Vec2S32 yz;
  }
  S32 v[3];
}

//- rjf: 4-vectors

union Vec4F32
{
  struct
  {
    F32 x;
    F32 y;
    F32 z;
    F32 w;
  }
  struct
  {
    Vec2F32 xy;
    Vec2F32 zw;
  }
  struct
  {
    Vec3F32 xyz;
    F32 _z0;
  }
  struct
  {
    F32 _x0;
    Vec3F32 yzw;
  }
  F32 v[4];
}

union Vec4S32
{
  struct
  {
    S32 x;
    S32 y;
    S32 z;
    S32 w;
  }
  struct
  {
    Vec2S32 xy;
    Vec2S32 zw;
  }
  struct
  {
    Vec3S32 xyz;
    S32 _z0;
  }
  struct
  {
    S32 _x0;
    Vec3S32 yzw;
  }
  S32 v[4];
}

////////////////////////////////
//~ rjf: Matrix Types

struct Mat3x3F32
{
  F32 v[3][3];
}

struct Mat4x4F32
{
  F32 v[4][4];
}

////////////////////////////////
//~ rjf: Range Types

//- rjf: 1-range

union Rng1U32
{
  struct
  {
    U32 min;
    U32 max;
  }
  U32 v[2];
}

union Rng1S32
{
  struct
  {
    S32 min;
    S32 max;
  }
  S32 v[2];
}

union Rng1U64
{
  struct
  {
    U64 min;
    U64 max;
  }
  U64 v[2];
}

union Rng1S64
{
  struct
  {
    S64 min;
    S64 max;
  }
  S64 v[2];
}

union Rng1F32
{
  struct
  {
    F32 min;
    F32 max;
  }
  F32 v[2];
}

//- rjf: 2-range (rectangles)

union Rng2S16
{
  struct
  {
    Vec2S16 min;
    Vec2S16 max;
  }
  struct
  {
    Vec2S16 p0;
    Vec2S16 p1;
  }
  struct
  {
    S16 x0;
    S16 y0;
    S16 x1;
    S16 y1;
  }
  Vec2S16 v[2];
}

union Rng2S32
{
  struct
  {
    Vec2S32 min;
    Vec2S32 max;
  }
  struct
  {
    Vec2S32 p0;
    Vec2S32 p1;
  }
  struct
  {
    S32 x0;
    S32 y0;
    S32 x1;
    S32 y1;
  }
  Vec2S32 v[2];
}

union Rng2F32
{
  struct
  {
    Vec2F32 min;
    Vec2F32 max;
  }
  struct
  {
    Vec2F32 p0;
    Vec2F32 p1;
  }
  struct
  {
    F32 x0;
    F32 y0;
    F32 x1;
    F32 y1;
  }
  Vec2F32 v[2];
}

union Rng2S64
{
  struct
  {
    Vec2S64 min;
    Vec2S64 max;
  }
  struct
  {
    Vec2S64 p0;
    Vec2S64 p1;
  }
  struct
  {
    S64 x0;
    S64 y0;
    S64 x1;
    S64 y1;
  }
  Vec2S64 v[2];
}

////////////////////////////////
//~ rjf: List Types

struct Rng1U64Node
{
  Rng1U64Node *next;
  Rng1U64 v;
}

struct Rng1U64List
{
  U64 count;
  Rng1U64Node *first;
  Rng1U64Node *last;
}

struct Rng1U64Array
{
  Rng1U64 *v;
  U64 count;
}

struct Rng1S64Node
{
  Rng1S64Node *next;
  Rng1S64 v;
}

struct Rng1S64List
{
  Rng1S64Node *first;
  Rng1S64Node *last;
  U64 count;
}

struct Rng1S64Array
{
  Rng1S64 *v;
  U64 count;
}

////////////////////////////////
//~ rjf: Scalar Ops

#define abs_s64(v) (S64)llabs(v)

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
//~ rjf: Miscellaneous Ops


#define rgba_from_u32_lit_comp(h) { (((h)&0xff000000)>>24)/255.f, (((h)&0x00ff0000)>>16)/255.f, (((h)&0x0000ff00)>> 8)/255.f, (((h)&0x000000ff)>> 0)/255.f }

////////////////////////////////
//~ rjf: List Type Functions



#endif // BASE_MATH_H

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_MATH_H
#define BASE_MATH_H

////////////////////////////////
//~ rjf: Vector Types

//- rjf: 2-vectors

[Union]
struct Vec2<T>
{
  [CRepr] struct
  {
    T x;
    T y;
  };
  T[2] v;
}

//- rjf: 3-vectors

[Union]
struct Vec3<T>
{
  [CRepr] struct
  {
    T x;
    T y;
    T z;
  };

  [CRepr] struct
  {
    Vec2<T> xy;
    T _z0;
  };

  [CRepr] struct
  {
    float _x0;
    Vec2<T> yz;
  }

  T[3] v;
}

//- rjf: 4-vectors

[Union]
struct Vec4<T>
{
  [CRepr]
  struct
  {
    T x;
    T y;
    T z;
    T w;
  };

  [CRepr]
  struct
  {
    Vec2<T> xy;
    Vec2<T> zw;
  };

  [CRepr]
  struct
  {
    Vec3<T> xyz;
    T _z0;
  };

  [CRepr]
  struct
  {
    T _x0;
    Vec3<T> yzw;
  };
  T[4] v;
}

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

[Union]
struct Rng1<T>
{
  [CRepr]
  struct
  {
    T min;
    T max;
  };

  T[2] v;
}

//- rjf: 2-range (rectangles)

[Union]
struct Rng2<T>
{
  [CRepr]
  struct
  {
    Vec<T> min;
    Vec<T> max;
  };

  [CRepr]
  struct
  {
    Vec<T> p0;
    Vec<T> p1;
  };

  [CRepr]
  struct
  {
    T x0;
    T y0;
    T x1;
    T y1;
  };

  Vec2<T>[2] v;
}

////////////////////////////////
//~ rjf: List Types

struct Node<T>
{
  Node<T>* next;
  T v;
}

struct List<T>
{
  Node<T>* first;
  Node<T>* last;
  uint64 count;
}


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

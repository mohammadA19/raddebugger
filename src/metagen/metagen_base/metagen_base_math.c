// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Scalar Ops

internal float
mix_1f32(float a, float b, float t)
{
  float c = (a + (b-a) * Clamp(0.f, t, 1.f));
  return c;
}

internal double
mix_1f64(double a, double b, double t)
{
  double c = (a + (b-a) * Clamp(0.0, t, 1.0));
  return c;
}

////////////////////////////////
//~ rjf: Vector Ops

internal Vec2float vec_2f32(float x, float y)                         {Vec2float v = {x, y}; return v;}
internal Vec2float add_2f32(Vec2float a, Vec2float b)                 {Vec2float c = {a.x+b.x, a.y+b.y}; return c;}
internal Vec2float sub_2f32(Vec2float a, Vec2float b)                 {Vec2float c = {a.x-b.x, a.y-b.y}; return c;}
internal Vec2float mul_2f32(Vec2float a, Vec2float b)                 {Vec2float c = {a.x*b.x, a.y*b.y}; return c;}
internal Vec2float div_2f32(Vec2float a, Vec2float b)                 {Vec2float c = {a.x/b.x, a.y/b.y}; return c;}
internal Vec2float scale_2f32(Vec2float v, float s)                   {Vec2float c = {v.x*s, v.y*s}; return c;}
internal float dot_2f32(Vec2float a, Vec2float b)                     {float c = a.x*b.x + a.y*b.y; return c;}
internal float length_squared_2f32(Vec2float v)                     {float c = v.x*v.x + v.y*v.y; return c;}
internal float length_2f32(Vec2float v)                             {float c = sqrt_f32(v.x*v.x + v.y*v.y); return c;}
internal Vec2float normalize_2f32(Vec2float v)                      {v = scale_2f32(v, 1.f/length_2f32(v)); return v;}
internal Vec2float mix_2f32(Vec2float a, Vec2float b, float t)          {Vec2float c = {mix_1f32(a.x, b.x, t), mix_1f32(a.y, b.y, t)}; return c;}

internal Vec2uint64 vec_2s64(uint64 x, uint64 y)                         {Vec2uint64 v = {x, y}; return v;}
internal Vec2uint64 add_2s64(Vec2uint64 a, Vec2uint64 b)                 {Vec2uint64 c = {a.x+b.x, a.y+b.y}; return c;}
internal Vec2uint64 sub_2s64(Vec2uint64 a, Vec2uint64 b)                 {Vec2uint64 c = {a.x-b.x, a.y-b.y}; return c;}
internal Vec2uint64 mul_2s64(Vec2uint64 a, Vec2uint64 b)                 {Vec2uint64 c = {a.x*b.x, a.y*b.y}; return c;}
internal Vec2uint64 div_2s64(Vec2uint64 a, Vec2uint64 b)                 {Vec2uint64 c = {a.x/b.x, a.y/b.y}; return c;}
internal Vec2uint64 scale_2s64(Vec2uint64 v, uint64 s)                   {Vec2uint64 c = {v.x*s, v.y*s}; return c;}
internal uint64 dot_2s64(Vec2uint64 a, Vec2uint64 b)                     {uint64 c = a.x*b.x + a.y*b.y; return c;}
internal uint64 length_squared_2s64(Vec2uint64 v)                     {uint64 c = v.x*v.x + v.y*v.y; return c;}
internal uint64 length_2s64(Vec2uint64 v)                             {uint64 c = (uint64)sqrt_f64((double)(v.x*v.x + v.y*v.y)); return c;}
internal Vec2uint64 normalize_2s64(Vec2uint64 v)                      {v = scale_2s64(v, (uint64)(1.f/length_2s64(v))); return v;}
internal Vec2uint64 mix_2s64(Vec2uint64 a, Vec2uint64 b, float t)          {Vec2uint64 c = {(uint64)mix_1f32((float)a.x, (float)b.x, t), (uint64)mix_1f32((float)a.y, (float)b.y, t)}; return c;}

internal Vec2uint32 vec_2s32(uint32 x, uint32 y)                         {Vec2uint32 v = {x, y}; return v;}
internal Vec2uint32 add_2s32(Vec2uint32 a, Vec2uint32 b)                 {Vec2uint32 c = {a.x+b.x, a.y+b.y}; return c;}
internal Vec2uint32 sub_2s32(Vec2uint32 a, Vec2uint32 b)                 {Vec2uint32 c = {a.x-b.x, a.y-b.y}; return c;}
internal Vec2uint32 mul_2s32(Vec2uint32 a, Vec2uint32 b)                 {Vec2uint32 c = {a.x*b.x, a.y*b.y}; return c;}
internal Vec2uint32 div_2s32(Vec2uint32 a, Vec2uint32 b)                 {Vec2uint32 c = {a.x/b.x, a.y/b.y}; return c;}
internal Vec2uint32 scale_2s32(Vec2uint32 v, uint32 s)                   {Vec2uint32 c = {v.x*s, v.y*s}; return c;}
internal uint32 dot_2s32(Vec2uint32 a, Vec2uint32 b)                     {uint32 c = a.x*b.x + a.y*b.y; return c;}
internal uint32 length_squared_2s32(Vec2uint32 v)                     {uint32 c = v.x*v.x + v.y*v.y; return c;}
internal uint32 length_2s32(Vec2uint32 v)                             {uint32 c = (uint32)sqrt_f32((float)v.x*(float)v.x + (float)v.y*(float)v.y); return c;}
internal Vec2uint32 normalize_2s32(Vec2uint32 v)                      {v = scale_2s32(v, (uint32)(1.f/length_2s32(v))); return v;}
internal Vec2uint32 mix_2s32(Vec2uint32 a, Vec2uint32 b, float t)          {Vec2uint32 c = {(uint32)mix_1f32((float)a.x, (float)b.x, t), (uint32)mix_1f32((float)a.y, (float)b.y, t)}; return c;}

internal Vec2uint16 vec_2s16(uint16 x, uint16 y)                         {Vec2uint16 v = {x, y}; return v;}
internal Vec2uint16 add_2s16(Vec2uint16 a, Vec2uint16 b)                 {Vec2uint16 c = {(uint16)(a.x+b.x), (uint16)(a.y+b.y)}; return c;}
internal Vec2uint16 sub_2s16(Vec2uint16 a, Vec2uint16 b)                 {Vec2uint16 c = {(uint16)(a.x-b.x), (uint16)(a.y-b.y)}; return c;}
internal Vec2uint16 mul_2s16(Vec2uint16 a, Vec2uint16 b)                 {Vec2uint16 c = {(uint16)(a.x*b.x), (uint16)(a.y*b.y)}; return c;}
internal Vec2uint16 div_2s16(Vec2uint16 a, Vec2uint16 b)                 {Vec2uint16 c = {(uint16)(a.x/b.x), (uint16)(a.y/b.y)}; return c;}
internal Vec2uint16 scale_2s16(Vec2uint16 v, uint16 s)                   {Vec2uint16 c = {(uint16)(v.x*s), (uint16)(v.y*s)}; return c;}
internal uint16 dot_2s16(Vec2uint16 a, Vec2uint16 b)                     {uint16 c = a.x*b.x + a.y*b.y; return c;}
internal uint16 length_squared_2s16(Vec2uint16 v)                     {uint16 c = v.x*v.x + v.y*v.y; return c;}
internal uint16 length_2s16(Vec2uint16 v)                             {uint16 c = (uint16)sqrt_f32((float)(v.x*v.x + v.y*v.y)); return c;}
internal Vec2uint16 normalize_2s16(Vec2uint16 v)                      {v = scale_2s16(v, (uint16)(1.f/length_2s16(v))); return v;}
internal Vec2uint16 mix_2s16(Vec2uint16 a, Vec2uint16 b, float t)          {Vec2uint16 c = {(uint16)mix_1f32((float)a.x, (float)b.x, t), (uint16)mix_1f32((float)a.y, (float)b.y, t)}; return c;}

internal Vec3float vec_3f32(float x, float y, float z)                  {Vec3float v = {x, y, z}; return v;}
internal Vec3float add_3f32(Vec3float a, Vec3float b)                 {Vec3float c = {a.x+b.x, a.y+b.y, a.z+b.z}; return c;}
internal Vec3float sub_3f32(Vec3float a, Vec3float b)                 {Vec3float c = {a.x-b.x, a.y-b.y, a.z-b.z}; return c;}
internal Vec3float mul_3f32(Vec3float a, Vec3float b)                 {Vec3float c = {a.x*b.x, a.y*b.y, a.z*b.z}; return c;}
internal Vec3float div_3f32(Vec3float a, Vec3float b)                 {Vec3float c = {a.x/b.x, a.y/b.y, a.z/b.z}; return c;}
internal Vec3float scale_3f32(Vec3float v, float s)                   {Vec3float c = {v.x*s, v.y*s, v.z*s}; return c;}
internal float dot_3f32(Vec3float a, Vec3float b)                     {float c = a.x*b.x + a.y*b.y + a.z*b.z; return c;}
internal float length_squared_3f32(Vec3float v)                     {float c = v.x*v.x + v.y*v.y + v.z*v.z; return c;}
internal float length_3f32(Vec3float v)                             {float c = sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z); return c;}
internal Vec3float normalize_3f32(Vec3float v)                      {v = scale_3f32(v, 1.f/length_3f32(v)); return v;}
internal Vec3float mix_3f32(Vec3float a, Vec3float b, float t)          {Vec3float c = {mix_1f32(a.x, b.x, t), mix_1f32(a.y, b.y, t), mix_1f32(a.z, b.z, t)}; return c;}
internal Vec3float cross_3f32(Vec3float a, Vec3float b)               {Vec3float c = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; return c;}

internal Vec3uint32 vec_3s32(uint32 x, uint32 y, uint32 z)                  {Vec3uint32 v = {x, y, z}; return v;}
internal Vec3uint32 add_3s32(Vec3uint32 a, Vec3uint32 b)                 {Vec3uint32 c = {a.x+b.x, a.y+b.y, a.z+b.z}; return c;}
internal Vec3uint32 sub_3s32(Vec3uint32 a, Vec3uint32 b)                 {Vec3uint32 c = {a.x-b.x, a.y-b.y, a.z-b.z}; return c;}
internal Vec3uint32 mul_3s32(Vec3uint32 a, Vec3uint32 b)                 {Vec3uint32 c = {a.x*b.x, a.y*b.y, a.z*b.z}; return c;}
internal Vec3uint32 div_3s32(Vec3uint32 a, Vec3uint32 b)                 {Vec3uint32 c = {a.x/b.x, a.y/b.y, a.z/b.z}; return c;}
internal Vec3uint32 scale_3s32(Vec3uint32 v, uint32 s)                   {Vec3uint32 c = {v.x*s, v.y*s, v.z*s}; return c;}
internal uint32 dot_3s32(Vec3uint32 a, Vec3uint32 b)                     {uint32 c = a.x*b.x + a.y*b.y + a.z*b.z; return c;}
internal uint32 length_squared_3s32(Vec3uint32 v)                     {uint32 c = v.x*v.x + v.y*v.y + v.z*v.z; return c;}
internal uint32 length_3s32(Vec3uint32 v)                             {uint32 c = (uint32)sqrt_f32((float)(v.x*v.x + v.y*v.y + v.z*v.z)); return c;}
internal Vec3uint32 normalize_3s32(Vec3uint32 v)                      {v = scale_3s32(v, (uint32)(1.f/length_3s32(v))); return v;}
internal Vec3uint32 mix_3s32(Vec3uint32 a, Vec3uint32 b, float t)          {Vec3uint32 c = {(uint32)mix_1f32((float)a.x, (float)b.x, t), (uint32)mix_1f32((float)a.y, (float)b.y, t), (uint32)mix_1f32((float)a.z, (float)b.z, t)}; return c;}
internal Vec3uint32 cross_3s32(Vec3uint32 a, Vec3uint32 b)               {Vec3uint32 c = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; return c;}

internal Vec4float vec_4f32(float x, float y, float z, float w)           {Vec4float v = {x, y, z, w}; return v;}
internal Vec4float add_4f32(Vec4float a, Vec4float b)                 {Vec4float c = {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; return c;}
internal Vec4float sub_4f32(Vec4float a, Vec4float b)                 {Vec4float c = {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; return c;}
internal Vec4float mul_4f32(Vec4float a, Vec4float b)                 {Vec4float c = {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; return c;}
internal Vec4float div_4f32(Vec4float a, Vec4float b)                 {Vec4float c = {a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w}; return c;}
internal Vec4float scale_4f32(Vec4float v, float s)                   {Vec4float c = {v.x*s, v.y*s, v.z*s, v.w*s}; return c;}
internal float dot_4f32(Vec4float a, Vec4float b)                     {float c = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; return c;}
internal float length_squared_4f32(Vec4float v)                     {float c = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; return c;}
internal float length_4f32(Vec4float v)                             {float c = sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w); return c;}
internal Vec4float normalize_4f32(Vec4float v)                      {v = scale_4f32(v, 1.f/length_4f32(v)); return v;}
internal Vec4float mix_4f32(Vec4float a, Vec4float b, float t)          {Vec4float c = {mix_1f32(a.x, b.x, t), mix_1f32(a.y, b.y, t), mix_1f32(a.z, b.z, t), mix_1f32(a.w, b.w, t)}; return c;}

internal Vec4uint32 vec_4s32(uint32 x, uint32 y, uint32 z, uint32 w)           {Vec4uint32 v = {x, y, z, w}; return v;}
internal Vec4uint32 add_4s32(Vec4uint32 a, Vec4uint32 b)                 {Vec4uint32 c = {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; return c;}
internal Vec4uint32 sub_4s32(Vec4uint32 a, Vec4uint32 b)                 {Vec4uint32 c = {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; return c;}
internal Vec4uint32 mul_4s32(Vec4uint32 a, Vec4uint32 b)                 {Vec4uint32 c = {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; return c;}
internal Vec4uint32 div_4s32(Vec4uint32 a, Vec4uint32 b)                 {Vec4uint32 c = {a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w}; return c;}
internal Vec4uint32 scale_4s32(Vec4uint32 v, uint32 s)                   {Vec4uint32 c = {v.x*s, v.y*s, v.z*s, v.w*s}; return c;}
internal uint32 dot_4s32(Vec4uint32 a, Vec4uint32 b)                     {uint32 c = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; return c;}
internal uint32 length_squared_4s32(Vec4uint32 v)                     {uint32 c = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; return c;}
internal uint32 length_4s32(Vec4uint32 v)                             {uint32 c = (uint32)sqrt_f32((float)(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w)); return c;}
internal Vec4uint32 normalize_4s32(Vec4uint32 v)                      {v = scale_4s32(v, (uint32)(1.f/length_4s32(v))); return v;}
internal Vec4uint32 mix_4s32(Vec4uint32 a, Vec4uint32 b, float t)          {Vec4uint32 c = {(uint32)mix_1f32((float)a.x, (float)b.x, t), (uint32)mix_1f32((float)a.y, (float)b.y, t), (uint32)mix_1f32((float)a.z, (float)b.z, t), (uint32)mix_1f32((float)a.w, (float)b.w, t)}; return c;}

////////////////////////////////
//~ rjf: Matrix Ops

internal Mat3x3float
mat_3x3f32(float diagonal)
{
  Mat3x3float result = {0};
  result.v[0][0] = diagonal;
  result.v[1][1] = diagonal;
  result.v[2][2] = diagonal;
  return result;
}

internal Mat3x3float
make_translate_3x3f32(Vec2float delta)
{
  Mat3x3float mat = mat_3x3f32(1.f);
  mat.v[2][0] = delta.x;
  mat.v[2][1] = delta.y;
  return mat;
}

internal Mat3x3float
make_scale_3x3f32(Vec2float scale)
{
  Mat3x3float mat = mat_3x3f32(1.f);
  mat.v[0][0] = scale.x;
  mat.v[1][1] = scale.y;
  return mat;
}

internal Mat3x3float
mul_3x3f32(Mat3x3float a, Mat3x3float b)
{
  Mat3x3float c = {0};
  for(int j = 0; j < 3; j += 1)
  {
    for(int i = 0; i < 3; i += 1)
    {
      c.v[i][j] = (a.v[0][j]*b.v[i][0] +
                   a.v[1][j]*b.v[i][1] +
                   a.v[2][j]*b.v[i][2]);
    }
  }
  return c;
}

internal Mat4x4float
mat_4x4f32(float diagonal)
{
  Mat4x4float result = {0};
  result.v[0][0] = diagonal;
  result.v[1][1] = diagonal;
  result.v[2][2] = diagonal;
  result.v[3][3] = diagonal;
  return result;
}

internal Mat4x4float
make_translate_4x4f32(Vec3float delta)
{
  Mat4x4float result = mat_4x4f32(1.f);
  result.v[3][0] = delta.x;
  result.v[3][1] = delta.y;
  result.v[3][2] = delta.z;
  return result;
}

internal Mat4x4float
make_scale_4x4f32(Vec3float scale)
{
  Mat4x4float result = mat_4x4f32(1.f);
  result.v[0][0] = scale.x;
  result.v[1][1] = scale.y;
  result.v[2][2] = scale.z;
  return result;
}

internal Mat4x4float
make_perspective_4x4f32(float fov, float aspect_ratio, float near_z, float far_z)
{
  Mat4x4float result = mat_4x4f32(1.f);
  float tan_theta_over_2 = tan_f32(fov / 2);
  result.v[0][0] = 1.f / tan_theta_over_2;
  result.v[1][1] = aspect_ratio / tan_theta_over_2;
  result.v[2][3] = 1.f;
  result.v[2][2] = -(near_z + far_z) / (near_z - far_z);
  result.v[3][2] = (2.f * near_z * far_z) / (near_z - far_z);
  result.v[3][3] = 0.f;
  return result;
}

internal Mat4x4float
make_orthographic_4x4f32(float left, float right, float bottom, float top, float near_z, float far_z)
{
  Mat4x4float result = mat_4x4f32(1.f);
  
  result.v[0][0] = 2.f / (right - left);
  result.v[1][1] = 2.f / (top - bottom);
  result.v[2][2] = 2.f / (far_z - near_z);
  result.v[3][3] = 1.f;
  
  result.v[3][0] = (left + right) / (left - right);
  result.v[3][1] = (bottom + top) / (bottom - top);
  result.v[3][2] = (near_z + far_z) / (near_z - far_z);
  
  return result;
}

internal Mat4x4float
make_look_at_4x4f32(Vec3float eye, Vec3float center, Vec3float up)
{
  Mat4x4float result;
  Vec3float f = normalize_3f32(sub_3f32(eye, center));
  Vec3float s = normalize_3f32(cross_3f32(f, up));
  Vec3float u = cross_3f32(s, f);
  result.v[0][0] = s.x;
  result.v[0][1] = u.x;
  result.v[0][2] = -f.x;
  result.v[0][3] = 0.0f;
  result.v[1][0] = s.y;
  result.v[1][1] = u.y;
  result.v[1][2] = -f.y;
  result.v[1][3] = 0.0f;
  result.v[2][0] = s.z;
  result.v[2][1] = u.z;
  result.v[2][2] = -f.z;
  result.v[2][3] = 0.0f;
  result.v[3][0] = -dot_3f32(s, eye);
  result.v[3][1] = -dot_3f32(u, eye);
  result.v[3][2] = dot_3f32(f, eye);
  result.v[3][3] = 1.0f;
  return result;
}

internal Mat4x4float
make_rotate_4x4f32(Vec3float axis, float turns)
{
  Mat4x4float result = mat_4x4f32(1.f);
  axis = normalize_3f32(axis);
  float sin_theta = sin_f32(turns);
  float cos_theta = cos_f32(turns);
  float cos_value = 1.f - cos_theta;
  result.v[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
  result.v[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
  result.v[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);
  result.v[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
  result.v[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
  result.v[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);
  result.v[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
  result.v[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
  result.v[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
  return result;
}

internal Mat4x4float
mul_4x4f32(Mat4x4float a, Mat4x4float b)
{
  Mat4x4float c = {0};
  for(int j = 0; j < 4; j += 1)
  {
    for(int i = 0; i < 4; i += 1)
    {
      c.v[i][j] = (a.v[0][j]*b.v[i][0] +
                   a.v[1][j]*b.v[i][1] +
                   a.v[2][j]*b.v[i][2] +
                   a.v[3][j]*b.v[i][3]);
    }
  }
  return c;
}

internal Mat4x4float
scale_4x4f32(Mat4x4float m, float scale)
{
  for(int j = 0; j < 4; j += 1)
  {
    for(int i = 0; i < 4; i += 1)
    {
      m.v[i][j] *= scale;
    }
  }
  return m;
}

internal Mat4x4float
inverse_4x4f32(Mat4x4float m)
{
  float coef00 = m.v[2][2] * m.v[3][3] - m.v[3][2] * m.v[2][3];
  float coef02 = m.v[1][2] * m.v[3][3] - m.v[3][2] * m.v[1][3];
  float coef03 = m.v[1][2] * m.v[2][3] - m.v[2][2] * m.v[1][3];
  float coef04 = m.v[2][1] * m.v[3][3] - m.v[3][1] * m.v[2][3];
  float coef06 = m.v[1][1] * m.v[3][3] - m.v[3][1] * m.v[1][3];
  float coef07 = m.v[1][1] * m.v[2][3] - m.v[2][1] * m.v[1][3];
  float coef08 = m.v[2][1] * m.v[3][2] - m.v[3][1] * m.v[2][2];
  float coef10 = m.v[1][1] * m.v[3][2] - m.v[3][1] * m.v[1][2];
  float coef11 = m.v[1][1] * m.v[2][2] - m.v[2][1] * m.v[1][2];
  float coef12 = m.v[2][0] * m.v[3][3] - m.v[3][0] * m.v[2][3];
  float coef14 = m.v[1][0] * m.v[3][3] - m.v[3][0] * m.v[1][3];
  float coef15 = m.v[1][0] * m.v[2][3] - m.v[2][0] * m.v[1][3];
  float coef16 = m.v[2][0] * m.v[3][2] - m.v[3][0] * m.v[2][2];
  float coef18 = m.v[1][0] * m.v[3][2] - m.v[3][0] * m.v[1][2];
  float coef19 = m.v[1][0] * m.v[2][2] - m.v[2][0] * m.v[1][2];
  float coef20 = m.v[2][0] * m.v[3][1] - m.v[3][0] * m.v[2][1];
  float coef22 = m.v[1][0] * m.v[3][1] - m.v[3][0] * m.v[1][1];
  float coef23 = m.v[1][0] * m.v[2][1] - m.v[2][0] * m.v[1][1];
  
  Vec4float fac0 = { coef00, coef00, coef02, coef03 };
  Vec4float fac1 = { coef04, coef04, coef06, coef07 };
  Vec4float fac2 = { coef08, coef08, coef10, coef11 };
  Vec4float fac3 = { coef12, coef12, coef14, coef15 };
  Vec4float fac4 = { coef16, coef16, coef18, coef19 };
  Vec4float fac5 = { coef20, coef20, coef22, coef23 };
  
  Vec4float vec0 = { m.v[1][0], m.v[0][0], m.v[0][0], m.v[0][0] };
  Vec4float vec1 = { m.v[1][1], m.v[0][1], m.v[0][1], m.v[0][1] };
  Vec4float vec2 = { m.v[1][2], m.v[0][2], m.v[0][2], m.v[0][2] };
  Vec4float vec3 = { m.v[1][3], m.v[0][3], m.v[0][3], m.v[0][3] };
  
  Vec4float inv0 = add_4f32(sub_4f32(mul_4f32(vec1, fac0), mul_4f32(vec2, fac1)), mul_4f32(vec3, fac2));
  Vec4float inv1 = add_4f32(sub_4f32(mul_4f32(vec0, fac0), mul_4f32(vec2, fac3)), mul_4f32(vec3, fac4));
  Vec4float inv2 = add_4f32(sub_4f32(mul_4f32(vec0, fac1), mul_4f32(vec1, fac3)), mul_4f32(vec3, fac5));
  Vec4float inv3 = add_4f32(sub_4f32(mul_4f32(vec0, fac2), mul_4f32(vec1, fac4)), mul_4f32(vec2, fac5));
  
  Vec4float sign_a = { +1, -1, +1, -1 };
  Vec4float sign_b = { -1, +1, -1, +1 };
  
  Mat4x4float inverse;
  for(uint32 i = 0; i < 4; i += 1)
  {
    inverse.v[0][i] = inv0.v[i] * sign_a.v[i];
    inverse.v[1][i] = inv1.v[i] * sign_b.v[i];
    inverse.v[2][i] = inv2.v[i] * sign_a.v[i];
    inverse.v[3][i] = inv3.v[i] * sign_b.v[i];
  }
  
  Vec4float row0 = { inverse.v[0][0], inverse.v[1][0], inverse.v[2][0], inverse.v[3][0] };
  Vec4float m0 = { m.v[0][0], m.v[0][1], m.v[0][2], m.v[0][3] };
  Vec4float dot0 = mul_4f32(m0, row0);
  float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);
  
  float one_over_det = 1 / dot1;
  
  return scale_4x4f32(inverse, one_over_det);
}

internal Mat4x4float
derotate_4x4f32(Mat4x4float mat)
{
  Vec3float scale =
  {
    length_3f32(v3f32(mat.v[0][0], mat.v[0][1], mat.v[0][2])),
    length_3f32(v3f32(mat.v[1][0], mat.v[1][1], mat.v[1][2])),
    length_3f32(v3f32(mat.v[2][0], mat.v[2][1], mat.v[2][2])),
  };
  mat.v[0][0] = scale.x;
  mat.v[1][0] = 0.f;
  mat.v[2][0] = 0.f;
  mat.v[0][1] = 0.f;
  mat.v[1][1] = scale.y;
  mat.v[2][1] = 0.f;
  mat.v[0][2] = 0.f;
  mat.v[1][2] = 0.f;
  mat.v[2][2] = scale.z;
  return mat;
}

////////////////////////////////
//~ rjf: Range Ops

internal Rng1uint32 rng_1u32(uint32 min, uint32 max)                     {Rng1uint32 r = {min, max}; if(r.min > r.max) { Swap(uint32, r.min, r.max); } return r;}
internal Rng1uint32 shift_1u32(Rng1uint32 r, uint32 x)                   {r.min += x; r.max += x; return r;}
internal Rng1uint32 pad_1u32(Rng1uint32 r, uint32 x)                     {r.min -= x; r.max += x; return r;}
internal uint32 center_1u32(Rng1uint32 r)                             {uint32 c = (r.min+r.max)/2; return c;}
internal B32 contains_1u32(Rng1uint32 r, uint32 x)                    {B32 c = (r.min <= x && x < r.max); return c;}
internal uint32 dim_1u32(Rng1uint32 r)                                {uint32 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
internal Rng1uint32 union_1u32(Rng1uint32 a, Rng1uint32 b)               {Rng1uint32 c = {Min(a.min, b.min), Max(a.max, b.max)}; return c;}
internal Rng1uint32 intersect_1u32(Rng1uint32 a, Rng1uint32 b)           {Rng1uint32 c = {Max(a.min, b.min), Min(a.max, b.max)}; return c;}
internal uint32 clamp_1u32(Rng1uint32 r, uint32 v)                       {v = Clamp(r.min, v, r.max); return v;}

internal Rng1uint32 rng_1s32(uint32 min, uint32 max)                     {Rng1uint32 r = {min, max}; if(r.min > r.max) { Swap(uint32, r.min, r.max); } return r;}
internal Rng1uint32 shift_1s32(Rng1uint32 r, uint32 x)                   {r.min += x; r.max += x; return r;}
internal Rng1uint32 pad_1s32(Rng1uint32 r, uint32 x)                     {r.min -= x; r.max += x; return r;}
internal uint32 center_1s32(Rng1uint32 r)                             {uint32 c = (r.min+r.max)/2; return c;}
internal B32 contains_1s32(Rng1uint32 r, uint32 x)                    {B32 c = (r.min <= x && x < r.max); return c;}
internal uint32 dim_1s32(Rng1uint32 r)                                {uint32 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
internal Rng1uint32 union_1s32(Rng1uint32 a, Rng1uint32 b)               {Rng1uint32 c = {Min(a.min, b.min), Max(a.max, b.max)}; return c;}
internal Rng1uint32 intersect_1s32(Rng1uint32 a, Rng1uint32 b)           {Rng1uint32 c = {Max(a.min, b.min), Min(a.max, b.max)}; return c;}
internal uint32 clamp_1s32(Rng1uint32 r, uint32 v)                       {v = Clamp(r.min, v, r.max); return v;}

internal Rng1uint64 rng_1u64(uint64 min, uint64 max)                     {Rng1uint64 r = {min, max}; if(r.min > r.max) { Swap(uint64, r.min, r.max); } return r;}
internal Rng1uint64 shift_1u64(Rng1uint64 r, uint64 x)                   {r.min += x; r.max += x; return r;}
internal Rng1uint64 pad_1u64(Rng1uint64 r, uint64 x)                     {r.min -= x; r.max += x; return r;}
internal uint64 center_1u64(Rng1uint64 r)                             {uint64 c = (r.min+r.max)/2; return c;}
internal B32 contains_1u64(Rng1uint64 r, uint64 x)                    {B32 c = (r.min <= x && x < r.max); return c;}
internal uint64 dim_1u64(Rng1uint64 r)                                {uint64 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
internal Rng1uint64 union_1u64(Rng1uint64 a, Rng1uint64 b)               {Rng1uint64 c = {Min(a.min, b.min), Max(a.max, b.max)}; return c;}
internal Rng1uint64 intersect_1u64(Rng1uint64 a, Rng1uint64 b)           {Rng1uint64 c = {Max(a.min, b.min), Min(a.max, b.max)}; return c;}
internal uint64 clamp_1u64(Rng1uint64 r, uint64 v)                       {v = Clamp(r.min, v, r.max); return v;}

internal Rng1uint64 rng_1s64(uint64 min, uint64 max)                     {Rng1uint64 r = {min, max}; if(r.min > r.max) { Swap(uint64, r.min, r.max); } return r;}
internal Rng1uint64 shift_1s64(Rng1uint64 r, uint64 x)                   {r.min += x; r.max += x; return r;}
internal Rng1uint64 pad_1s64(Rng1uint64 r, uint64 x)                     {r.min -= x; r.max += x; return r;}
internal uint64 center_1s64(Rng1uint64 r)                             {uint64 c = (r.min+r.max)/2; return c;}
internal B32 contains_1s64(Rng1uint64 r, uint64 x)                    {B32 c = (r.min <= x && x < r.max); return c;}
internal uint64 dim_1s64(Rng1uint64 r)                                {uint64 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
internal Rng1uint64 union_1s64(Rng1uint64 a, Rng1uint64 b)               {Rng1uint64 c = {Min(a.min, b.min), Max(a.max, b.max)}; return c;}
internal Rng1uint64 intersect_1s64(Rng1uint64 a, Rng1uint64 b)           {Rng1uint64 c = {Max(a.min, b.min), Min(a.max, b.max)}; return c;}
internal uint64 clamp_1s64(Rng1uint64 r, uint64 v)                       {v = Clamp(r.min, v, r.max); return v;}

internal Rng1float rng_1f32(float min, float max)                     {Rng1float r = {min, max}; if(r.min > r.max) { Swap(float, r.min, r.max); } return r;}
internal Rng1float shift_1f32(Rng1float r, float x)                   {r.min += x; r.max += x; return r;}
internal Rng1float pad_1f32(Rng1float r, float x)                     {r.min -= x; r.max += x; return r;}
internal float center_1f32(Rng1float r)                             {float c = (r.min+r.max)/2; return c;}
internal B32 contains_1f32(Rng1float r, float x)                    {B32 c = (r.min <= x && x < r.max); return c;}
internal float dim_1f32(Rng1float r)                                {float c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
internal Rng1float union_1f32(Rng1float a, Rng1float b)               {Rng1float c = {Min(a.min, b.min), Max(a.max, b.max)}; return c;}
internal Rng1float intersect_1f32(Rng1float a, Rng1float b)           {Rng1float c = {Max(a.min, b.min), Min(a.max, b.max)}; return c;}
internal float clamp_1f32(Rng1float r, float v)                       {v = Clamp(r.min, v, r.max); return v;}

internal Rng2uint16 rng_2s16(Vec2uint16 min, Vec2uint16 max)             {Rng2uint16 r = {min, max}; return r;}
internal Rng2uint16 shift_2s16(Rng2uint16 r, Vec2uint16 x)               {r.min = add_2s16(r.min, x); r.max = add_2s16(r.max, x); return r;}
internal Rng2uint16 pad_2s16(Rng2uint16 r, uint16 x)                     {Vec2uint16 xv = {x, x}; r.min = sub_2s16(r.min, xv); r.max = add_2s16(r.max, xv); return r;}
internal Vec2uint16 center_2s16(Rng2uint16 r)                         {Vec2uint16 c = {(uint16)((r.min.x+r.max.x)/2), (uint16)((r.min.y+r.max.y)/2)}; return c;}
internal B32 contains_2s16(Rng2uint16 r, Vec2uint16 x)                {B32 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
internal Vec2uint16 dim_2s16(Rng2uint16 r)                            {Vec2uint16 dim = {(uint16)(((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0)), (uint16)(((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0))}; return dim;}
internal Rng2uint16 union_2s16(Rng2uint16 a, Rng2uint16 b)               {Rng2uint16 c; c.p0.x = Min(a.min.x, b.min.x); c.p0.y = Min(a.min.y, b.min.y); c.p1.x = Max(a.max.x, b.max.x); c.p1.y = Max(a.max.y, b.max.y); return c;}
internal Rng2uint16 intersect_2s16(Rng2uint16 a, Rng2uint16 b)           {Rng2uint16 c; c.p0.x = Max(a.min.x, b.min.x); c.p0.y = Max(a.min.y, b.min.y); c.p1.x = Min(a.max.x, b.max.x); c.p1.y = Min(a.max.y, b.max.y); return c;}
internal Vec2uint16 clamp_2s16(Rng2uint16 r, Vec2uint16 v)               {v.x = Clamp(r.min.x, v.x, r.max.x); v.y = Clamp(r.min.y, v.y, r.max.y); return v;}

internal Rng2uint32 rng_2s32(Vec2uint32 min, Vec2uint32 max)             {Rng2uint32 r = {min, max}; return r;}
internal Rng2uint32 shift_2s32(Rng2uint32 r, Vec2uint32 x)               {r.min = add_2s32(r.min, x); r.max = add_2s32(r.max, x); return r;}
internal Rng2uint32 pad_2s32(Rng2uint32 r, uint32 x)                     {Vec2uint32 xv = {x, x}; r.min = sub_2s32(r.min, xv); r.max = add_2s32(r.max, xv); return r;}
internal Vec2uint32 center_2s32(Rng2uint32 r)                         {Vec2uint32 c = {(r.min.x+r.max.x)/2, (r.min.y+r.max.y)/2}; return c;}
internal B32 contains_2s32(Rng2uint32 r, Vec2uint32 x)                {B32 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
internal Vec2uint32 dim_2s32(Rng2uint32 r)                            {Vec2uint32 dim = {((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0), ((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0)}; return dim;}
internal Rng2uint32 union_2s32(Rng2uint32 a, Rng2uint32 b)               {Rng2uint32 c; c.p0.x = Min(a.min.x, b.min.x); c.p0.y = Min(a.min.y, b.min.y); c.p1.x = Max(a.max.x, b.max.x); c.p1.y = Max(a.max.y, b.max.y); return c;}
internal Rng2uint32 intersect_2s32(Rng2uint32 a, Rng2uint32 b)           {Rng2uint32 c; c.p0.x = Max(a.min.x, b.min.x); c.p0.y = Max(a.min.y, b.min.y); c.p1.x = Min(a.max.x, b.max.x); c.p1.y = Min(a.max.y, b.max.y); return c;}
internal Vec2uint32 clamp_2s32(Rng2uint32 r, Vec2uint32 v)               {v.x = Clamp(r.min.x, v.x, r.max.x); v.y = Clamp(r.min.y, v.y, r.max.y); return v;}

internal Rng2uint64 rng_2s64(Vec2uint64 min, Vec2uint64 max)             {Rng2uint64 r = {min, max}; return r;}
internal Rng2uint64 shift_2s64(Rng2uint64 r, Vec2uint64 x)               {r.min = add_2s64(r.min, x); r.max = add_2s64(r.max, x); return r;}
internal Rng2uint64 pad_2s64(Rng2uint64 r, uint64 x)                     {Vec2uint64 xv = {x, x}; r.min = sub_2s64(r.min, xv); r.max = add_2s64(r.max, xv); return r;}
internal Vec2uint64 center_2s64(Rng2uint64 r)                         {Vec2uint64 c = {(r.min.x+r.max.x)/2, (r.min.y+r.max.y)/2}; return c;}
internal B32 contains_2s64(Rng2uint64 r, Vec2uint64 x)                {B32 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
internal Vec2uint64 dim_2s64(Rng2uint64 r)                            {Vec2uint64 dim = {((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0), ((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0)}; return dim;}
internal Rng2uint64 union_2s64(Rng2uint64 a, Rng2uint64 b)               {Rng2uint64 c; c.p0.x = Min(a.min.x, b.min.x); c.p0.y = Min(a.min.y, b.min.y); c.p1.x = Max(a.max.x, b.max.x); c.p1.y = Max(a.max.y, b.max.y); return c;}
internal Rng2uint64 intersect_2s64(Rng2uint64 a, Rng2uint64 b)           {Rng2uint64 c; c.p0.x = Max(a.min.x, b.min.x); c.p0.y = Max(a.min.y, b.min.y); c.p1.x = Min(a.max.x, b.max.x); c.p1.y = Min(a.max.y, b.max.y); return c;}
internal Vec2uint64 clamp_2s64(Rng2uint64 r, Vec2uint64 v)               {v.x = Clamp(r.min.x, v.x, r.max.x); v.y = Clamp(r.min.y, v.y, r.max.y); return v;}

internal Rng2float rng_2f32(Vec2float min, Vec2float max)             {Rng2float r = {min, max}; return r;}
internal Rng2float shift_2f32(Rng2float r, Vec2float x)               {r.min = add_2f32(r.min, x); r.max = add_2f32(r.max, x); return r;}
internal Rng2float pad_2f32(Rng2float r, float x)                     {Vec2float xv = {x, x}; r.min = sub_2f32(r.min, xv); r.max = add_2f32(r.max, xv); return r;}
internal Vec2float center_2f32(Rng2float r)                         {Vec2float c = {(r.min.x+r.max.x)/2, (r.min.y+r.max.y)/2}; return c;}
internal B32 contains_2f32(Rng2float r, Vec2float x)                {B32 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
internal Vec2float dim_2f32(Rng2float r)                            {Vec2float dim = {((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0), ((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0)}; return dim;}
internal Rng2float union_2f32(Rng2float a, Rng2float b)               {Rng2float c; c.p0.x = Min(a.min.x, b.min.x); c.p0.y = Min(a.min.y, b.min.y); c.p1.x = Max(a.max.x, b.max.x); c.p1.y = Max(a.max.y, b.max.y); return c;}
internal Rng2float intersect_2f32(Rng2float a, Rng2float b)           {Rng2float c; c.p0.x = Max(a.min.x, b.min.x); c.p0.y = Max(a.min.y, b.min.y); c.p1.x = Min(a.max.x, b.max.x); c.p1.y = Min(a.max.y, b.max.y); return c;}
internal Vec2float clamp_2f32(Rng2float r, Vec2float v)               {v.x = Clamp(r.min.x, v.x, r.max.x); v.y = Clamp(r.min.y, v.y, r.max.y); return v;}

////////////////////////////////
//~ rjf: Miscellaneous Ops

internal Vec3float
hsv_from_rgb(Vec3float rgb)
{
  float c_max = Max(rgb.x, Max(rgb.y, rgb.z));
  float c_min = Min(rgb.x, Min(rgb.y, rgb.z));
  float delta = c_max - c_min;
  float h = ((delta == 0.f) ? 0.f :
           (c_max == rgb.x) ? mod_f32((rgb.y - rgb.z)/delta + 6.f, 6.f) :
           (c_max == rgb.y) ? (rgb.z - rgb.x)/delta + 2.f :
           (c_max == rgb.z) ? (rgb.x - rgb.y)/delta + 4.f :
           0.f);
  float s = (c_max == 0.f) ? 0.f : (delta/c_max);
  float v = c_max;
  Vec3float hsv = {h/6.f, s, v};
  return hsv;
}

internal Vec3float
rgb_from_hsv(Vec3float hsv)
{
  float h = mod_f32(hsv.x * 360.f, 360.f);
  float s = hsv.y;
  float v = hsv.z;
  
  float c = v*s;
  float x = c*(1.f - abs_f32(mod_f32(h/60.f, 2.f) - 1.f));
  float m = v - c;
  
  float r = 0;
  float g = 0;
  float b = 0;
  
  if ((h >= 0.f && h < 60.f) || (h >= 360.f && h < 420.f)){
    r = c;
    g = x;
    b = 0;
  }
  else if (h >= 60.f && h < 120.f){
    r = x;
    g = c;
    b = 0;
  }
  else if (h >= 120.f && h < 180.f){
    r = 0;
    g = c;
    b = x;
  }
  else if (h >= 180.f && h < 240.f){
    r = 0;
    g = x;
    b = c;
  }
  else if (h >= 240.f && h < 300.f){
    r = x;
    g = 0;
    b = c;
  }
  else if ((h >= 300.f && h <= 360.f) || (h >= -60.f && h <= 0.f)){
    r = c;
    g = 0;
    b = x;
  }
  
  Vec3float rgb = {r + m, g + m, b + m};
  return(rgb);
}

internal Vec4float
hsva_from_rgba(Vec4float rgba)
{
  Vec3float rgb = v3f32(rgba.x, rgba.y, rgba.z);
  Vec3float hsv = hsv_from_rgb(rgb);
  Vec4float hsva = v4f32(hsv.x, hsv.y, hsv.z, rgba.w);
  return hsva;
}

internal Vec4float
rgba_from_hsva(Vec4float hsva)
{
  Vec3float hsv = v3f32(hsva.x, hsva.y, hsva.z);
  Vec3float rgb = rgb_from_hsv(hsv);
  Vec4float rgba = v4f32(rgb.x, rgb.y, rgb.z, hsva.w);
  return rgba;
}

internal Vec4float
rgba_from_u32(uint32 hex)
{
  Vec4float result = v4f32(((hex&0xff000000)>>24)/255.f,
                         ((hex&0x00ff0000)>>16)/255.f,
                         ((hex&0x0000ff00)>> 8)/255.f,
                         ((hex&0x000000ff)>> 0)/255.f);
  return result;
}

internal uint32
u32_from_rgba(Vec4float rgba)
{
  uint32 result = 0;
  result |= ((uint32)((uint8)(rgba.x*255.f))) << 24;
  result |= ((uint32)((uint8)(rgba.y*255.f))) << 16;
  result |= ((uint32)((uint8)(rgba.z*255.f))) <<  8;
  result |= ((uint32)((uint8)(rgba.w*255.f))) <<  0;
  return result;
}

////////////////////////////////
//~ rjf: List Type Functions

internal void
rng1u64_list_push(Arena *arena, Rng1uint64List *list, Rng1uint64 rng)
{
  Rng1uint64Node *n = push_array(arena, Rng1uint64Node, 1);
  MemoryCopyStruct(&n->v, &rng);
  SLLQueuePush(list->first, list->last, n);
  list->count += 1;
}

internal void
rng1u64_list_concat(Rng1uint64List *list, Rng1uint64List *to_concat)
{
  if(to_concat->first)
  {
    if(list->first)
    {
      list->last->next = to_concat->first;
      list->last       = to_concat->last;
    }
    else
    {
      list->first = to_concat->first;
      list->last  = to_concat->last;
    }
    MemoryZeroStruct(to_concat);
  }
}

internal Rng1uint64Array
rng1u64_array_from_list(Arena *arena, Rng1uint64List *list)
{
  Rng1uint64Array arr = {0};
  arr.count        = list->count;
  arr.v            = push_array_no_zero(arena, Rng1uint64, arr.count);
  uint64 idx = 0;
  for(Rng1uint64Node *n = list->first; n != 0; n = n->next)
  {
    arr.v[idx] = n->v;
    idx += 1;
  }
  return arr;
}

internal void
rng1s64_list_push(Arena *arena, Rng1uint64List *list, Rng1uint64 rng)
{
  Rng1uint64Node *n = push_array(arena, Rng1uint64Node, 1);
  MemoryCopyStruct(&n->v, &rng);
  SLLQueuePush(list->first, list->last, n);
  list->count += 1;
}

internal Rng1uint64Array
rng1s64_array_from_list(Arena *arena, Rng1uint64List *list)
{
  Rng1uint64Array arr = {0};
  arr.count = list->count;
  arr.v = push_array_no_zero(arena, Rng1uint64, arr.count);
  uint64 idx = 0;
  for(Rng1uint64Node *n = list->first; n != 0; n = n->next)
  {
    arr.v[idx] = n->v;
    idx += 1;
  }
  return arr;
}

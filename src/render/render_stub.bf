// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- rjf: top-level layer initialization

[CLink, CallingConvention(.Cdecl)]
public static void r_init(CmdLine* cmdln)
{
}

//- rjf: window setup/teardown

[CLink, CallingConvention(.Cdecl)]
public static R_Handle r_window_equip(OS_Handle window)
{
  R_Handle handle = {0};
  handle.u64[0] = 1;
  return handle;
}

[CLink, CallingConvention(.Cdecl)]
public static void r_window_unequip(OS_Handle window, R_Handle window_equip)
{
}

//- rjf: textures

[CLink, CallingConvention(.Cdecl)]
public static R_Handle r_tex2d_alloc(R_ResourceKind kind, Vec2S32 size, R_Tex2DFormat format, void* data)
{
  R_Handle handle = {0};
  handle.u64[0] = 1;
  return handle;
}

[CLink, CallingConvention(.Cdecl)]
public static void r_tex2d_release(R_Handle texture)
{
}

[CLink, CallingConvention(.Cdecl)]
public static R_ResourceKind r_kind_from_tex2d(R_Handle texture)
{
  return R_ResourceStatic;
}

[CLink, CallingConvention(.Cdecl)]
public static Vec2S32 r_size_from_tex2d(R_Handle texture)
{
  return v2s32(1, 1);
}

[CLink, CallingConvention(.Cdecl)]
public static R_Tex2DFormat r_format_from_tex2d(R_Handle texture)
{
  return R_Tex2DFormat_RGBA8;
}

[CLink, CallingConvention(.Cdecl)]
public static void r_fill_tex2d_region(R_Handle texture, Rng2S32 subrect, void* data)
{
}

//- rjf: buffers

[CLink, CallingConvention(.Cdecl)]
public static R_Handle r_buffer_alloc(R_ResourceKind kind, uint64 size, void* data)
{
  R_Handle handle = {0};
  handle.u64[0] = 1;
  return handle;
}

[CLink, CallingConvention(.Cdecl)]
public static void r_buffer_release(R_Handle buffer)
{
}

//- rjf: frame markers

[CLink, CallingConvention(.Cdecl)]
public static void r_begin_frame()
{
}

[CLink, CallingConvention(.Cdecl)]
public static void r_end_frame()
{
}

[CLink, CallingConvention(.Cdecl)]
public static void r_window_begin_frame(OS_Handle window, R_Handle window_equip)
{
}

[CLink, CallingConvention(.Cdecl)]
public static void r_window_end_frame(OS_Handle window, R_Handle window_equip)
{
}

//- rjf: render pass submission

[CLink, CallingConvention(.Cdecl)]
public static void r_window_submit(OS_Handle window, R_Handle window_equip, R_PassList* passes)
{
}

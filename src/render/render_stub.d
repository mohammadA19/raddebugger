module render.render_stub;

//- rjf: top-level layer initialization

public extern(C) void r_init(CmdLine* cmdln)
{
}

//- rjf: window setup/teardown

public extern(C) R_Handle r_window_equip(OS_Handle window)
{
  R_Handle handle;
  handle.u64[0] = 1;
  return handle;
}

public extern(C) void r_window_unequip(OS_Handle window, R_Handle window_equip)
{
}

//- rjf: textures

public extern(C) R_Handle r_tex2d_alloc(R_ResourceKind kind, Vec2S32 size, R_Tex2DFormat format, void* data)
{
  R_Handle handle;
  handle.u64[0] = 1;
  return handle;
}

public extern(C) void r_tex2d_release(R_Handle texture)
{
}

public extern(C) R_ResourceKind r_kind_from_tex2d(R_Handle texture)
{
  return R_ResourceStatic;
}

public extern(C) Vec2S32 r_size_from_tex2d(R_Handle texture)
{
  return v2s32(1, 1);
}

public extern(C) R_Tex2DFormat r_format_from_tex2d(R_Handle texture)
{
  return R_Tex2DFormat_RGBA8;
}

public extern(C) void r_fill_tex2d_region(R_Handle texture, Rng2S32 subrect, void* data)
{
}

//- rjf: buffers

public extern(C) R_Handle r_buffer_alloc(R_ResourceKind kind, ulong size, void* data)
{
  R_Handle handle;
  handle.u64[0] = 1;
  return handle;
}

public extern(C) void r_buffer_release(R_Handle buffer)
{
}

//- rjf: frame markers

public extern(C) void r_begin_frame()
{
}

public extern(C) void r_end_frame()
{
}

public extern(C) void r_window_begin_frame(OS_Handle window, R_Handle window_equip)
{
}

public extern(C) void r_window_end_frame(OS_Handle window, R_Handle window_equip)
{
}

//- rjf: render pass submission

public extern(C) void r_window_submit(OS_Handle window, R_Handle window_equip, R_PassList* passes)
{
}

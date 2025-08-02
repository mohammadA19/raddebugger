//- rjf: top-level layer initialization

export fn r_init(cmdln: *CmdLine) {
}

//- rjf: window setup/teardown

export fn r_window_equip(window: OS_Handle) R_Handle {
  var handle = R_Handle {};
  handle.u64[0] = 1;
  return handle;
}

export fn r_window_unequip(window: OS_Handle, window_equip: R_Handle) {
}

//- rjf: textures

export fn r_tex2d_alloc(kind: R_ResourceKind, size: Vec2S32, format: R_Tex2DFormat, data: *void) R_Handle {
  var handle = R_Handle {};
  handle.u64[0] = 1;
  return handle;
}

export fn r_tex2d_release(texture: R_Handle) {
}

export fn r_kind_from_tex2d(texture: R_Handle) R_ResourceKind {
  return R_ResourceStatic;
}

export fn r_size_from_tex2d(texture: R_Handle) Vec2S32 {
  return v2s32(1, 1);
}

export fn r_format_from_tex2d(texture: R_Handle) R_Tex2DFormat {
  return R_Tex2DFormat_RGBA8;
}

export fn r_fill_tex2d_region(texture: R_Handle, subrect: Rng2S32, data: *void) {
}

//- rjf: buffers

export fn r_buffer_alloc(kind: R_ResourceKind, size: u64, data: *void) R_Handle {
  var handle = R_Handle {};
  handle.u64[0] = 1;
  return handle;
}

export fn r_buffer_release(buffer: R_Handle) {
}

//- rjf: frame markers

export fn r_begin_frame() {
}

export fn r_end_frame() {
}

export fn r_window_begin_frame(window: OS_Handle, window_equip: R_Handle) {
}

export fn r_window_end_frame(window: OS_Handle, window_equip: R_Handle) {
}

//- rjf: render pass submission

export fn r_window_submit(window: OS_Handle, window_equip: R_Handle, passes: *R_PassList) {
}

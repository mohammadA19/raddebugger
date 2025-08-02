
//- rjf: top-level layer initialization

r_init :: proc "c" (cmdln: ^CmdLine) {
}

//- rjf: window setup/teardown

r_window_equip :: proc "c" (window: OS_Handle) -> R_Handle {
  handle := R_Handle {}
  handle.u64[0] = 1
  return handle
}

r_window_unequip :: proc "c" (window: OS_Handle, window_equip: R_Handle) {
}

//- rjf: textures

r_tex2d_alloc :: proc "c" (kind: R_ResourceKind, size: Vec2S32, format: R_Tex2DFormat, data: ^void) -> R_Handle {
  handle := R_Handle {}
  handle.u64[0] = 1
  return handle
}

r_tex2d_release :: proc "c" (texture: R_Handle) {
}

r_kind_from_tex2d :: proc "c" (texture: R_Handle) -> R_ResourceKind {
  return R_ResourceStatic
}

r_size_from_tex2d :: proc "c" (texture: R_Handle) -> Vec2S32 {
  return v2s32(1, 1)
}

r_format_from_tex2d :: proc "c" (texture: R_Handle) -> R_Tex2DFormat {
  return R_Tex2DFormat_RGBA8
}

r_fill_tex2d_region :: proc "c" (texture: R_Handle, subrect: Rng2S32, data: rawptr) {
}

//- rjf: buffers

r_buffer_alloc :: proc "c" (kind: R_ResourceKind, size: U64, data: rawptr) -> R_Handle {
  handle := R_Handle {}
  handle.u64[0] = 1
  return handle
}

r_buffer_release :: proc "c" (buffer: R_Handle) {
}

//- rjf: frame markers

r_begin_frame :: proc "c" () {
}

r_end_frame :: proc "c" () {
}

r_window_begin_frame :: proc "c" (window: OS_Handle, window_equip: R_Handle) {
}

r_window_end_frame :: proc "c" (window: OS_Handle, window_equip: R_Handle) {
}

//- rjf: render pass submission

r_window_submit :: proc "c" (window: OS_Handle, window_equip: R_Handle, passes: ^R_PassList) {
}

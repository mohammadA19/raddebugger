// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- GENERATED CODE

package ui

UI_ParentNode :: struct {next: ^UI_ParentNode, v: ^UI_Box}
UI_ChildLayoutAxisNode :: struct {next: ^UI_ChildLayoutAxisNode, v: Axis2}
UI_FixedXNode :: struct {next: ^UI_FixedXNode, v: f32}
UI_FixedYNode :: struct {next: ^UI_FixedYNode, v: f32}
UI_FixedWidthNode :: struct {next: ^UI_FixedWidthNode, v: f32}
UI_FixedHeightNode :: struct {next: ^UI_FixedHeightNode, v: f32}
UI_PrefWidthNode :: struct {next: ^UI_PrefWidthNode, v: UI_Size}
UI_PrefHeightNode :: struct {next: ^UI_PrefHeightNode, v: UI_Size}
UI_MinWidthNode :: struct {next: ^UI_MinWidthNode, v: f32}
UI_MinHeightNode :: struct {next: ^UI_MinHeightNode, v: f32}
UI_PermissionFlagsNode :: struct {next: ^UI_PermissionFlagsNode, v: UI_PermissionFlags}
UI_FlagsNode :: struct {next: ^UI_FlagsNode, v: UI_BoxFlags}
UI_OmitFlagsNode :: struct {next: ^UI_OmitFlagsNode, v: UI_BoxFlags}
UI_FocusHotNode :: struct {next: ^UI_FocusHotNode, v: UI_FocusKind}
UI_FocusActiveNode :: struct {next: ^UI_FocusActiveNode, v: UI_FocusKind}
UI_FastpathCodepointNode :: struct {next: ^UI_FastpathCodepointNode, v: U32}
UI_GroupKeyNode :: struct {next: ^UI_GroupKeyNode, v: UI_Key}
UI_TransparencyNode :: struct {next: ^UI_TransparencyNode, v: f32}
UI_TagNode :: struct {next: ^UI_TagNode, v: String8}
UI_BackgroundColorNode :: struct {next: ^UI_BackgroundColorNode, v: Vec4F32}
UI_TextColorNode :: struct {next: ^UI_TextColorNode, v: Vec4F32}
UI_BorderColorNode :: struct {next: ^UI_BorderColorNode, v: Vec4F32}
UI_SquishNode :: struct {next: ^UI_SquishNode, v: f32}
UI_HoverCursorNode :: struct {next: ^UI_HoverCursorNode, v: OS_Cursor}
UI_FontNode :: struct {next: ^UI_FontNode, v: FNT_Tag}
UI_FontSizeNode :: struct {next: ^UI_FontSizeNode, v: f32}
UI_TextRasterFlagsNode :: struct {next: ^UI_TextRasterFlagsNode, v: FNT_RasterFlags}
UI_TabSizeNode :: struct {next: ^UI_TabSizeNode, v: f32}
UI_CornerRadius00Node :: struct {next: ^UI_CornerRadius00Node, v: f32}
UI_CornerRadius01Node :: struct {next: ^UI_CornerRadius01Node, v: f32}
UI_CornerRadius10Node :: struct {next: ^UI_CornerRadius10Node, v: f32}
UI_CornerRadius11Node :: struct {next: ^UI_CornerRadius11Node, v: f32}
UI_BlurSizeNode :: struct {next: ^UI_BlurSizeNode, v: f32}
UI_TextPaddingNode :: struct {next: ^UI_TextPaddingNode, v: f32}
UI_TextAlignmentNode :: struct {next: ^UI_TextAlignmentNode, v: UI_TextAlign}

UI_StackNils :: struct {
  parent_nil_stack_top: UI_ParentNode,
  child_layout_axis_nil_stack_top: UI_ChildLayoutAxisNode,
  fixed_x_nil_stack_top: UI_FixedXNode,
  fixed_y_nil_stack_top: UI_FixedYNode,
  fixed_width_nil_stack_top: UI_FixedWidthNode,
  fixed_height_nil_stack_top: UI_FixedHeightNode,
  pref_width_nil_stack_top: UI_PrefWidthNode,
  pref_height_nil_stack_top: UI_PrefHeightNode,
  min_width_nil_stack_top: UI_MinWidthNode,
  min_height_nil_stack_top: UI_MinHeightNode,
  permission_flags_nil_stack_top: UI_PermissionFlagsNode,
  flags_nil_stack_top: UI_FlagsNode,
  omit_flags_nil_stack_top: UI_OmitFlagsNode,
  focus_hot_nil_stack_top: UI_FocusHotNode,
  focus_active_nil_stack_top: UI_FocusActiveNode,
  fastpath_codepoint_nil_stack_top: UI_FastpathCodepointNode,
  group_key_nil_stack_top: UI_GroupKeyNode,
  transparency_nil_stack_top: UI_TransparencyNode,
  tag_nil_stack_top: UI_TagNode,
  background_color_nil_stack_top: UI_BackgroundColorNode,
  text_color_nil_stack_top: UI_TextColorNode,
  border_color_nil_stack_top: UI_BorderColorNode,
  squish_nil_stack_top: UI_SquishNode,
  hover_cursor_nil_stack_top: UI_HoverCursorNode,
  font_nil_stack_top: UI_FontNode,
  font_size_nil_stack_top: UI_FontSizeNode,
  text_raster_flags_nil_stack_top: UI_TextRasterFlagsNode,
  tab_size_nil_stack_top: UI_TabSizeNode,
  corner_radius_00_nil_stack_top: UI_CornerRadius00Node,
  corner_radius_01_nil_stack_top: UI_CornerRadius01Node,
  corner_radius_10_nil_stack_top: UI_CornerRadius10Node,
  corner_radius_11_nil_stack_top: UI_CornerRadius11Node,
  blur_size_nil_stack_top: UI_BlurSizeNode,
  text_padding_nil_stack_top: UI_TextPaddingNode,
  text_alignment_nil_stack_top: UI_TextAlignmentNode,
}

UI_InitStackNils :: proc (state: ^UI_State) {
  state.parent_nil_stack_top.v = &ui_nil_box
  state.child_layout_axis_nil_stack_top.v = Axis2_X
  state.fixed_x_nil_stack_top.v = 0
  state.fixed_y_nil_stack_top.v = 0
  state.fixed_width_nil_stack_top.v = 0
  state.fixed_height_nil_stack_top.v = 0
  state.pref_width_nil_stack_top.v = ui_px(250.f, 1.f)
  state.pref_height_nil_stack_top.v = ui_px(30.f, 1.f)
  state.min_width_nil_stack_top.v = 0
  state.min_height_nil_stack_top.v = 0
  state.permission_flags_nil_stack_top.v = UI_PermissionFlag_All
  state.flags_nil_stack_top.v = 0
  state.omit_flags_nil_stack_top.v = 0
  state.focus_hot_nil_stack_top.v = UI_FocusKind_Null
  state.focus_active_nil_stack_top.v = UI_FocusKind_Null
  state.fastpath_codepoint_nil_stack_top.v = 0
  state.group_key_nil_stack_top.v = ui_key_zero()
  state.transparency_nil_stack_top.v = 0
  state.tag_nil_stack_top.v = str8_lit("")
  state.background_color_nil_stack_top.v = v4f32(0, 0, 0, 0)
  state.text_color_nil_stack_top.v = v4f32(0, 0, 0, 0)
  state.border_color_nil_stack_top.v = v4f32(0, 0, 0, 0)
  state.squish_nil_stack_top.v = 0
  state.hover_cursor_nil_stack_top.v = OS_Cursor_Pointer
  state.font_nil_stack_top.v = fnt_tag_zero()
  state.font_size_nil_stack_top.v = 24.f
  state.text_raster_flags_nil_stack_top.v = FNT_RasterFlag_Hinted
  state.tab_size_nil_stack_top.v = 24.f*4.f
  state.corner_radius_00_nil_stack_top.v = 0
  state.corner_radius_01_nil_stack_top.v = 0
  state.corner_radius_10_nil_stack_top.v = 0
  state.corner_radius_11_nil_stack_top.v = 0
  state.blur_size_nil_stack_top.v = 0
  state.text_padding_nil_stack_top.v = 0
  state.text_alignment_nil_stack_top.v = UI_TextAlign_Left
}

UI_Stacks :: struct {
  parent_stack: struct { top: ^UI_ParentNode, bottom_val: ^UI_Box, free: ^UI_ParentNode, gen: u64, auto_pop: b32 },
  child_layout_axis_stack: struct { top: ^UI_ChildLayoutAxisNode, bottom_val: Axis2, free: ^UI_ChildLayoutAxisNode, gen: u64, auto_pop: b32 },
  fixed_x_stack: struct { top: ^UI_FixedXNode, bottom_val: f32, free: ^UI_FixedXNode, gen: u64, auto_pop: b32 },
  fixed_y_stack: struct { top: ^UI_FixedYNode, bottom_val: f32, free: ^UI_FixedYNode, gen: u64, auto_pop: b32 },
  fixed_width_stack: struct { top: ^UI_FixedWidthNode, bottom_val: f32, free: ^UI_FixedWidthNode, gen: u64, auto_pop: b32 },
  fixed_height_stack: struct { top: ^UI_FixedHeightNode, bottom_val: f32, free: ^UI_FixedHeightNode, gen: u64, auto_pop: b32 },
  pref_width_stack: struct { top: ^UI_PrefWidthNode, bottom_val: UI_Size, free: ^UI_PrefWidthNode, gen: u64, auto_pop: b32 },
  pref_height_stack: struct { top: ^UI_PrefHeightNode, bottom_val: UI_Size, free: ^UI_PrefHeightNode, gen: u64, auto_pop: b32 },
  min_width_stack: struct { top: ^UI_MinWidthNode, bottom_val: f32, free: ^UI_MinWidthNode, gen: u64, auto_pop: b32 },
  min_height_stack: struct { top: ^UI_MinHeightNode, bottom_val: f32, free: ^UI_MinHeightNode, gen: u64, auto_pop: b32 },
  permission_flags_stack: struct { top: ^UI_PermissionFlagsNode, bottom_val: UI_PermissionFlags, free: ^UI_PermissionFlagsNode, gen: u64, auto_pop: b32 },
  flags_stack: struct { top: ^UI_FlagsNode, bottom_val: UI_BoxFlags, free: ^UI_FlagsNode, gen: u64, auto_pop: b32 },
  omit_flags_stack: struct { top: ^UI_OmitFlagsNode, bottom_val: UI_BoxFlags, free: ^UI_OmitFlagsNode, gen: u64, auto_pop: b32 },
  focus_hot_stack: struct { top: ^UI_FocusHotNode, bottom_val: UI_FocusKind, free: ^UI_FocusHotNode, gen: u64, auto_pop: b32 },
  focus_active_stack: struct { top: ^UI_FocusActiveNode, bottom_val: UI_FocusKind, free: ^UI_FocusActiveNode, gen: u64, auto_pop: b32 },
  fastpath_codepoint_stack: struct { top: ^UI_FastpathCodepointNode, bottom_val: U32, free: ^UI_FastpathCodepointNode, gen: u64, auto_pop: b32 },
  group_key_stack: struct { top: ^UI_GroupKeyNode, bottom_val: UI_Key, free: ^UI_GroupKeyNode, gen: u64, auto_pop: b32 },
  transparency_stack: struct { top: ^UI_TransparencyNode, bottom_val: f32, free: ^UI_TransparencyNode, gen: u64, auto_pop: b32 },
  tag_stack: struct { top: ^UI_TagNode, bottom_val: String8, free: ^UI_TagNode, gen: u64, auto_pop: b32 },
  background_color_stack: struct { top: ^UI_BackgroundColorNode, bottom_val: Vec4F32, free: ^UI_BackgroundColorNode, gen: u64, auto_pop: b32 },
  text_color_stack: struct { top: ^UI_TextColorNode, bottom_val: Vec4F32, free: ^UI_TextColorNode, gen: u64, auto_pop: b32 },
  border_color_stack: struct { top: ^UI_BorderColorNode, bottom_val: Vec4F32, free: ^UI_BorderColorNode, gen: u64, auto_pop: b32 },
  squish_stack: struct { top: ^UI_SquishNode, bottom_val: f32, free: ^UI_SquishNode, gen: u64, auto_pop: b32 },
  hover_cursor_stack: struct { top: ^UI_HoverCursorNode, bottom_val: OS_Cursor, free: ^UI_HoverCursorNode, gen: u64, auto_pop: b32 },
  font_stack: struct { top: ^UI_FontNode, bottom_val: FNT_Tag, free: ^UI_FontNode, gen: u64, auto_pop: b32 },
  font_size_stack: struct { top: ^UI_FontSizeNode, bottom_val: f32, free: ^UI_FontSizeNode, gen: u64, auto_pop: b32 },
  text_raster_flags_stack: struct { top: ^UI_TextRasterFlagsNode, bottom_val: FNT_RasterFlags, free: ^UI_TextRasterFlagsNode, gen: u64, auto_pop: b32 },
  tab_size_stack: struct { top: ^UI_TabSizeNode, bottom_val: f32, free: ^UI_TabSizeNode, gen: u64, auto_pop: b32 },
  corner_radius_00_stack: struct { top: ^UI_CornerRadius00Node, bottom_val: f32, free: ^UI_CornerRadius00Node, gen: u64, auto_pop: b32 },
  corner_radius_01_stack: struct { top: ^UI_CornerRadius01Node, bottom_val: f32, free: ^UI_CornerRadius01Node, gen: u64, auto_pop: b32 },
  corner_radius_10_stack: struct { top: ^UI_CornerRadius10Node, bottom_val: f32, free: ^UI_CornerRadius10Node, gen: u64, auto_pop: b32 },
  corner_radius_11_stack: struct { top: ^UI_CornerRadius11Node, bottom_val: f32, free: ^UI_CornerRadius11Node, gen: u64, auto_pop: b32 },
  blur_size_stack: struct { top: ^UI_BlurSizeNode, bottom_val: f32, free: ^UI_BlurSizeNode, gen: u64, auto_pop: b32 },
  text_padding_stack: struct { top: ^UI_TextPaddingNode, bottom_val: f32, free: ^UI_TextPaddingNode, gen: u64, auto_pop: b32 },
  text_alignment_stack: struct { top: ^UI_TextAlignmentNode, bottom_val: UI_TextAlign, free: ^UI_TextAlignmentNode, gen: u64, auto_pop: b32 },
}

UI_InitStacks :: proc (state: ^UI_State) {
  state.parent_stack.top = &state.parent_nil_stack_top; state.parent_stack.bottom_val = &ui_nil_box; state.parent_stack.free = 0; state.parent_stack.auto_pop = 0;
  state.child_layout_axis_stack.top = &state.child_layout_axis_nil_stack_top; state.child_layout_axis_stack.bottom_val = Axis2_X; state.child_layout_axis_stack.free = 0; state.child_layout_axis_stack.auto_pop = 0;
  state.fixed_x_stack.top = &state.fixed_x_nil_stack_top; state.fixed_x_stack.bottom_val = 0; state.fixed_x_stack.free = 0; state.fixed_x_stack.auto_pop = 0;
  state.fixed_y_stack.top = &state.fixed_y_nil_stack_top; state.fixed_y_stack.bottom_val = 0; state.fixed_y_stack.free = 0; state.fixed_y_stack.auto_pop = 0;
  state.fixed_width_stack.top = &state.fixed_width_nil_stack_top; state.fixed_width_stack.bottom_val = 0; state.fixed_width_stack.free = 0; state.fixed_width_stack.auto_pop = 0;
  state.fixed_height_stack.top = &state.fixed_height_nil_stack_top; state.fixed_height_stack.bottom_val = 0; state.fixed_height_stack.free = 0; state.fixed_height_stack.auto_pop = 0;
  state.pref_width_stack.top = &state.pref_width_nil_stack_top; state.pref_width_stack.bottom_val = ui_px(250.f, 1.f); state.pref_width_stack.free = 0; state.pref_width_stack.auto_pop = 0;
  state.pref_height_stack.top = &state.pref_height_nil_stack_top; state.pref_height_stack.bottom_val = ui_px(30.f, 1.f); state.pref_height_stack.free = 0; state.pref_height_stack.auto_pop = 0;
  state.min_width_stack.top = &state.min_width_nil_stack_top; state.min_width_stack.bottom_val = 0; state.min_width_stack.free = 0; state.min_width_stack.auto_pop = 0;
  state.min_height_stack.top = &state.min_height_nil_stack_top; state.min_height_stack.bottom_val = 0; state.min_height_stack.free = 0; state.min_height_stack.auto_pop = 0;
  state.permission_flags_stack.top = &state.permission_flags_nil_stack_top; state.permission_flags_stack.bottom_val = UI_PermissionFlag_All; state.permission_flags_stack.free = 0; state.permission_flags_stack.auto_pop = 0;
  state.flags_stack.top = &state.flags_nil_stack_top; state.flags_stack.bottom_val = 0; state.flags_stack.free = 0; state.flags_stack.auto_pop = 0;
  state.omit_flags_stack.top = &state.omit_flags_nil_stack_top; state.omit_flags_stack.bottom_val = 0; state.omit_flags_stack.free = 0; state.omit_flags_stack.auto_pop = 0;
  state.focus_hot_stack.top = &state.focus_hot_nil_stack_top; state.focus_hot_stack.bottom_val = UI_FocusKind_Null; state.focus_hot_stack.free = 0; state.focus_hot_stack.auto_pop = 0;
  state.focus_active_stack.top = &state.focus_active_nil_stack_top; state.focus_active_stack.bottom_val = UI_FocusKind_Null; state.focus_active_stack.free = 0; state.focus_active_stack.auto_pop = 0;
  state.fastpath_codepoint_stack.top = &state.fastpath_codepoint_nil_stack_top; state.fastpath_codepoint_stack.bottom_val = 0; state.fastpath_codepoint_stack.free = 0; state.fastpath_codepoint_stack.auto_pop = 0;
  state.group_key_stack.top = &state.group_key_nil_stack_top; state.group_key_stack.bottom_val = ui_key_zero(); state.group_key_stack.free = 0; state.group_key_stack.auto_pop = 0;
  state.transparency_stack.top = &state.transparency_nil_stack_top; state.transparency_stack.bottom_val = 0; state.transparency_stack.free = 0; state.transparency_stack.auto_pop = 0;
  state.tag_stack.top = &state.tag_nil_stack_top; state.tag_stack.bottom_val = str8_lit(""); state.tag_stack.free = 0; state.tag_stack.auto_pop = 0;
  state.background_color_stack.top = &state.background_color_nil_stack_top; state.background_color_stack.bottom_val = v4f32(0, 0, 0, 0); state.background_color_stack.free = 0; state.background_color_stack.auto_pop = 0;
  state.text_color_stack.top = &state.text_color_nil_stack_top; state.text_color_stack.bottom_val = v4f32(0, 0, 0, 0); state.text_color_stack.free = 0; state.text_color_stack.auto_pop = 0;
  state.border_color_stack.top = &state.border_color_nil_stack_top; state.border_color_stack.bottom_val = v4f32(0, 0, 0, 0); state.border_color_stack.free = 0; state.border_color_stack.auto_pop = 0;
  state.squish_stack.top = &state.squish_nil_stack_top; state.squish_stack.bottom_val = 0; state.squish_stack.free = 0; state.squish_stack.auto_pop = 0;
  state.hover_cursor_stack.top = &state.hover_cursor_nil_stack_top; state.hover_cursor_stack.bottom_val = OS_Cursor_Pointer; state.hover_cursor_stack.free = 0; state.hover_cursor_stack.auto_pop = 0;
  state.font_stack.top = &state.font_nil_stack_top; state.font_stack.bottom_val = fnt_tag_zero(); state.font_stack.free = 0; state.font_stack.auto_pop = 0;
  state.font_size_stack.top = &state.font_size_nil_stack_top; state.font_size_stack.bottom_val = 24.f; state.font_size_stack.free = 0; state.font_size_stack.auto_pop = 0;
  state.text_raster_flags_stack.top = &state.text_raster_flags_nil_stack_top; state.text_raster_flags_stack.bottom_val = FNT_RasterFlag_Hinted; state.text_raster_flags_stack.free = 0; state.text_raster_flags_stack.auto_pop = 0;
  state.tab_size_stack.top = &state.tab_size_nil_stack_top; state.tab_size_stack.bottom_val = 24.f*4.f; state.tab_size_stack.free = 0; state.tab_size_stack.auto_pop = 0;
  state.corner_radius_00_stack.top = &state.corner_radius_00_nil_stack_top; state.corner_radius_00_stack.bottom_val = 0; state.corner_radius_00_stack.free = 0; state.corner_radius_00_stack.auto_pop = 0;
  state.corner_radius_01_stack.top = &state.corner_radius_01_nil_stack_top; state.corner_radius_01_stack.bottom_val = 0; state.corner_radius_01_stack.free = 0; state.corner_radius_01_stack.auto_pop = 0;
  state.corner_radius_10_stack.top = &state.corner_radius_10_nil_stack_top; state.corner_radius_10_stack.bottom_val = 0; state.corner_radius_10_stack.free = 0; state.corner_radius_10_stack.auto_pop = 0;
  state.corner_radius_11_stack.top = &state.corner_radius_11_nil_stack_top; state.corner_radius_11_stack.bottom_val = 0; state.corner_radius_11_stack.free = 0; state.corner_radius_11_stack.auto_pop = 0;
  state.blur_size_stack.top = &state.blur_size_nil_stack_top; state.blur_size_stack.bottom_val = 0; state.blur_size_stack.free = 0; state.blur_size_stack.auto_pop = 0;
  state.text_padding_stack.top = &state.text_padding_nil_stack_top; state.text_padding_stack.bottom_val = 0; state.text_padding_stack.free = 0; state.text_padding_stack.auto_pop = 0;
  state.text_alignment_stack.top = &state.text_alignment_nil_stack_top; state.text_alignment_stack.bottom_val = UI_TextAlign_Left; state.text_alignment_stack.free = 0; state.text_alignment_stack.auto_pop = 0;
}

UI_AutoPopStacks :: proc (state: ^UI_State) {
 if (state.parent_stack.auto_pop) { ui_pop_parent(); state.parent_stack.auto_pop = 0; }
 if (state.child_layout_axis_stack.auto_pop) { ui_pop_child_layout_axis(); state.child_layout_axis_stack.auto_pop = 0; }
 if (state.fixed_x_stack.auto_pop) { ui_pop_fixed_x(); state.fixed_x_stack.auto_pop = 0; }
 if (state.fixed_y_stack.auto_pop) { ui_pop_fixed_y(); state.fixed_y_stack.auto_pop = 0; }
 if (state.fixed_width_stack.auto_pop) { ui_pop_fixed_width(); state.fixed_width_stack.auto_pop = 0; }
 if (state.fixed_height_stack.auto_pop) { ui_pop_fixed_height(); state.fixed_height_stack.auto_pop = 0; }
 if (state.pref_width_stack.auto_pop) { ui_pop_pref_width(); state.pref_width_stack.auto_pop = 0; }
 if (state.pref_height_stack.auto_pop) { ui_pop_pref_height(); state.pref_height_stack.auto_pop = 0; }
 if (state.min_width_stack.auto_pop) { ui_pop_min_width(); state.min_width_stack.auto_pop = 0; }
 if (state.min_height_stack.auto_pop) { ui_pop_min_height(); state.min_height_stack.auto_pop = 0; }
 if (state.permission_flags_stack.auto_pop) { ui_pop_permission_flags(); state.permission_flags_stack.auto_pop = 0; }
 if (state.flags_stack.auto_pop) { ui_pop_flags(); state.flags_stack.auto_pop = 0; }
 if (state.omit_flags_stack.auto_pop) { ui_pop_omit_flags(); state.omit_flags_stack.auto_pop = 0; }
 if (state.focus_hot_stack.auto_pop) { ui_pop_focus_hot(); state.focus_hot_stack.auto_pop = 0; }
 if (state.focus_active_stack.auto_pop) { ui_pop_focus_active(); state.focus_active_stack.auto_pop = 0; }
 if (state.fastpath_codepoint_stack.auto_pop) { ui_pop_fastpath_codepoint(); state.fastpath_codepoint_stack.auto_pop = 0; }
 if (state.group_key_stack.auto_pop) { ui_pop_group_key(); state.group_key_stack.auto_pop = 0; }
 if (state.transparency_stack.auto_pop) { ui_pop_transparency(); state.transparency_stack.auto_pop = 0; }
 if (state.tag_stack.auto_pop) { ui_pop_tag(); state.tag_stack.auto_pop = 0; }
 if (state.background_color_stack.auto_pop) { ui_pop_background_color(); state.background_color_stack.auto_pop = 0; }
 if (state.text_color_stack.auto_pop) { ui_pop_text_color(); state.text_color_stack.auto_pop = 0; }
 if (state.border_color_stack.auto_pop) { ui_pop_border_color(); state.border_color_stack.auto_pop = 0; }
 if (state.squish_stack.auto_pop) { ui_pop_squish(); state.squish_stack.auto_pop = 0; }
 if (state.hover_cursor_stack.auto_pop) { ui_pop_hover_cursor(); state.hover_cursor_stack.auto_pop = 0; }
 if (state.font_stack.auto_pop) { ui_pop_font(); state.font_stack.auto_pop = 0; }
 if (state.font_size_stack.auto_pop) { ui_pop_font_size(); state.font_size_stack.auto_pop = 0; }
 if (state.text_raster_flags_stack.auto_pop) { ui_pop_text_raster_flags(); state.text_raster_flags_stack.auto_pop = 0; }
 if (state.tab_size_stack.auto_pop) { ui_pop_tab_size(); state.tab_size_stack.auto_pop = 0; }
 if (state.corner_radius_00_stack.auto_pop) { ui_pop_corner_radius_00(); state.corner_radius_00_stack.auto_pop = 0; }
 if (state.corner_radius_01_stack.auto_pop) { ui_pop_corner_radius_01(); state.corner_radius_01_stack.auto_pop = 0; }
 if (state.corner_radius_10_stack.auto_pop) { ui_pop_corner_radius_10(); state.corner_radius_10_stack.auto_pop = 0; }
 if (state.corner_radius_11_stack.auto_pop) { ui_pop_corner_radius_11(); state.corner_radius_11_stack.auto_pop = 0; }
 if (state.blur_size_stack.auto_pop) { ui_pop_blur_size(); state.blur_size_stack.auto_pop = 0; }
 if (state.text_padding_stack.auto_pop) { ui_pop_text_padding(); state.text_padding_stack.auto_pop = 0; }
 if (state.text_alignment_stack.auto_pop) { ui_pop_text_alignment(); state.text_alignment_stack.auto_pop = 0; }
}

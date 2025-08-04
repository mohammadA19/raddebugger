
////////////////////////////////
//~ rjf: C-side Shader Types

R_D3D11_Uniforms_Rect :: struct {
  viewport_size: Vec2F32,
  opacity: F32,
  _padding0_: F32,
  texture_sample_channel_map: Mat4x4F32,
  texture_t2d_size: Vec2F32,
  translate: Vec2F32,
  xform: [3]Vec4F32,
  xform_scale: Vec2F32,
}

R_D3D11_Uniforms_BlurPass :: struct {
  rect: Rng2F32,
  corner_radii: Vec4F32,
  direction: Vec2F32,
  viewport_size: Vec2F32,
  blur_count: u32,
  _padding0_: [204]U8,
}
#assert(sizeof(R_D3D11_Uniforms_BlurPass) % 256 == 0) // NotAligned // constant count/offset must be aligned to 256 bytes

R_D3D11_Uniforms_Blur :: struct {
  passes: [Axis2_COUNT]R_D3D11_Uniforms_BlurPass,
  kernel: [32]Vec4F32,
}

R_D3D11_Uniforms_Mesh :: struct {
  xform: Mat4x4F32,
}

////////////////////////////////
//~ rjf: Main State Types

R_D3D11_Tex2D :: struct {
  next: ^R_D3D11_Tex2D,
  generation: u64,
  texture: ^ITexture2D,
  view: ^IShaderResourceView,
  kind: R_ResourceKind,
  size: Vec2S32,
  format: R_Tex2DFormat,
}

R_D3D11_Buffer :: struct {
  next: ^R_D3D11_Buffer,
  generation: u64,
  buffer: ^IBuffer,
  kind: R_ResourceKind,
  size: u64,
}

R_D3D11_Window :: struct {
  next: ^R_D3D11_Window,
  generation: u64,
  
  // rjf: swapchain/framebuffer
  swapchain:        ^ISwapChain1, // dxgi
  framebuffer:        ^ITexture2D,
  framebuffer_rtv: ^IRenderTargetView,
  
  // rjf: staging buffer
  stage_color: ^ITexture2D,
  stage_color_rtv: ^IRenderTargetView,
  stage_color_srv: ^IShaderResourceView,
  stage_scratch_color: ^ITexture2D,
  stage_scratch_color_rtv: ^IRenderTargetView,
  stage_scratch_color_srv: ^IShaderResourceView,
  
  // rjf: geo3d buffer
  geo3d_color: ^ITexture2D,
  geo3d_color_rtv: ^IRenderTargetView,
  geo3d_color_srv: ^IShaderResourceView,
  geo3d_depth: ^ITexture2D,
  geo3d_depth_dsv: ^IDepthStencilView,
  geo3d_depth_srv: ^IShaderResourceView,
  
  // rjf: last state
  last_resolution: Vec2S32,
}

R_D3D11_FlushBuffer :: struct {
  next: ^R_D3D11_FlushBuffer,
  buffer: ^IBuffer,
}

R_D3D11_State :: struct {
  // rjf: state
  arena: ^Arena,
  window_count: u64,
  first_free_window: ^R_D3D11_Window,
  first_free_tex2d: ^R_D3D11_Tex2D,
  first_free_buffer: ^R_D3D11_Buffer,
  first_to_free_tex2d: ^R_D3D11_Tex2D,
  first_to_free_buffer: ^R_D3D11_Buffer,
  device_rw_mutex: OS_Handle,
  
  // rjf: base d3d11 objects
  base_device: ^IDevice,
  base_device_ctx: ^IDeviceContext,
  device: ^IDevice1,
  device_ctx: ^IDeviceContext1,
  dxgi_device: ^IDevice1, // dxgi
  dxgi_adapter: ^IAdapter, // dxgi
  dxgi_factory: ^IFactory2, // dxgi
  main_rasterizer: ^IRasterizerState1,
  main_blend_state: ^IBlendState,
  no_blend_state: ^IBlendState,
  samplers: [R_Tex2DSampleKind_COUNT]^ID3D11SamplerState,
  noop_depth_stencil: ^IDepthStencilState,
  plain_depth_stencil: ^IDepthStencilState,
  instance_scratch_buffer_64kb: ^IBuffer,
  
  // rjf: backups
  backup_texture: R_Handle,
  
  // rjf: vertex shaders
  vshads: [R_D3D11_VShadKind_COUNT]^ID3D11VertexShader,
  ilays: [R_D3D11_VShadKind_COUNT]^ID3D11InputLayout,
  pshads: [R_D3D11_PShadKind_COUNT]^ID3D11PixelShader,
  uniform_type_kind_buffers: [R_D3D11_UniformTypeKind_COUNT]^ID3D11Buffer,
  
  // rjf: buffers to flush at subsequent frame
  buffer_flush_arena: ^Arena,
  first_buffer_to_flush: ^R_D3D11_FlushBuffer,
  last_buffer_to_flush: ^R_D3D11_FlushBuffer,
}

////////////////////////////////
//~ rjf: Input Layout Element Tables

global D3D11_INPUT_ELEMENT_DESC r_d3d11_g_rect_ilay_elements[] =
{
  { "POS",  0, FORMAT.R32G32B32A32_FLOAT, 0,                            0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "TEX",  0, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "COL",  0, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "COL",  1, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "COL",  2, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "COL",  3, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "CRAD", 0, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  { "STY",  0, FORMAT.R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};

global D3D11_INPUT_ELEMENT_DESC r_d3d11_g_mesh_ilay_elements[] =
{
  { "POS", 0, FORMAT.R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "NOR", 0, FORMAT.R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TEX", 0, FORMAT.R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "COL", 0, FORMAT.R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

////////////////////////////////
//~ rjf: Generated Code

#include "generated/render_d3d11.meta.c"

////////////////////////////////
//~ rjf: Helpers

r_d3d11_window_from_handle :: proc (handle: R_Handle) -> ^R_D3D11_Window
{
  window := (^R_D3D11_Window)handle.u64[0]
  if window == nil {
    window = &r_d3d11_window_nil
  }
  return window
}
 
r_d3d11_handle_from_window :: proc (window: ^R_D3D11_Window) -> R_Handle
{
  handle := R_Handle{}
  handle.u64[0] = (u64)window
  return handle
}

r_d3d11_tex2d_from_handle :: proc (handle: R_Handle) -> ^R_D3D11_Tex2D
{
  texture := (^R_D3D11_Tex2D)handle.u64[0]
  if texture == nil {
    texture = &r_d3d11_tex2d_nil
  }
  return texture
}
 
r_d3d11_handle_from_tex2d :: proc (texture: ^R_D3D11_Tex2D) -> R_Handle
{
  handle := R_Handle{}
  handle.u64[0] = (u64)texture
  return handle
}

r_d3d11_buffer_from_handle:: proc (handle: R_Handle) -> ^R_D3D11_Buffer
{
  buffer := (^R_D3D11_Buffer)handle.u64[0];
  if buffer == nil {
    buffer = &r_d3d11_buffer_nil
  }
  return buffer
}

r_d3d11_handle_from_buffer :: proc (buffer: ^R_D3D11_Buffer) -> R_Handle
{
  handle := R_Handle{}
  handle.u64[0] = (u64)buffer
  return handle
}

r_d3d11_instance_buffer_from_size :: proc (size: u64) -> ^IBuffer
{
  buffer := r_d3d11_state.instance_scratch_buffer_64kb;
  if size > KB(64) {
    flushed_buffer_size := size
    flushed_buffer_size += MB(1)-1
    flushed_buffer_size -= flushed_buffer_size%MB(1)
    
    // rjf: build buffer
    {
      desc = BUFFER_DESC{};
      {
        desc.ByteWidth      = flushed_buffer_size
        desc.Usage          = D3D11_USAGE_DYNAMIC
        desc.BindFlags      = BIND.VERTEX_BUFFER
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
      }
      error := r_d3d11_state.device.lpVtbl.CreateBuffer(r_d3d11_state.device, &desc, 0, &buffer)
    }
    
    // rjf: push buffer to flush list
    n := push_array(r_d3d11_state.buffer_flush_arena, R_D3D11_FlushBuffer, 1);
    n.buffer = buffer;
    SLLQueuePush(r_d3d11_state.first_buffer_to_flush, r_d3d11_state.last_buffer_to_flush, n);
  }
  return buffer;
}

r_usage_access_flags_from_resource_kind :: proc (kind: R_ResourceKind, out_d3d11_usage: ^USAGE, out_cpu_access_flags: ^UINT)
{
  switch kind {
    case R_ResourceKind_Static:
      out_d3d11_usage^ = USAGE.IMMUTABLE
      out_cpu_access_flags^ = 0

    case R_ResourceKind_Dynamic:
      out_d3d11_usage^ = USAGE.DEFAULT
      out_cpu_access_flags^ = 0

    
    case R_ResourceKind_Stream:
      out_d3d11_usage^ = USAGE.DYNAMIC
      out_cpu_access_flags^ = CPU_ACCESS_FLAG.WRITE
    
    case:
      InvalidPath()
  }
}

////////////////////////////////
//~ rjf: Backend Hook Implementations

//- rjf: top-level layer initialization

r_init :: proc "c" (cmdln: ^CmdLine)
{
  ProfBeginFunction()
  error := HRESULT(0)
  arena = arena_alloc()
  r_d3d11_state = push_array(arena, R_D3D11_State, 1)
  r_d3d11_state.arena = arena
  r_d3d11_state.device_rw_mutex = os_rw_mutex_alloc()
  
  //- rjf: create base device
  ProfBegin("create base device")
  creation_flags := CREATE_DEVICE_FLAG.BGRA_SUPPORT
#if BUILD_DEBUG
  if cmd_line_has_flag(cmdln, "d3d11_debug") {
    creation_flags |= CREATE_DEVICE_FLAG.DEBUG
  }
#endif
  feature_levels := [?]FEATURE_LEVEL { FEATURE_LEVEL._11_0 }
  driver_type := DRIVER_TYPE.HARDWARE
  if cmd_line_has_flag(cmdln, "force_d3d11_software") {
    driver_type = DRIVER_TYPE.WARP
  }
  error = D3D11CreateDevice(0,
                            driver_type,
                            0,
                            creation_flags,
                            feature_levels, len(feature_levels),
                            SDK_VERSION,
                            &r_d3d11_state.base_device, 0, &r_d3d11_state.base_device_ctx);
  if FAILED(error) && driver_type == DRIVER_TYPE.HARDWARE {
    // try with WARP driver as backup solution in case HW device is not available
    error = D3D11CreateDevice(0,
                              DRIVER_TYPE.WARP,
                              0,
                              creation_flags,
                              feature_levels, len(feature_levels),
                              SDK_VERSION,
                              &r_d3d11_state.base_device, 0, &r_d3d11_state.base_device_ctx);
  }
  if FAILED(error) {
    buffer := [256]u8 {}
    raddbg_snprintf(buffer, sizeof(buffer), "D3D11 device creation failure (%lx). The process is terminating.", error)
    os_graphical_message(1, "Fatal Error", str8_cstring(buffer))
    os_abort(1)
  }
  ProfEnd()
  
  //- rjf: enable break-on-error
#if BUILD_DEBUG
  if cmd_line_has_flag(cmdln, "d3d11_debug") ProfScope("enable break-on-error")
  {
    info = (^IInfoQueue)nil
    error = r_d3d11_state.base_device.lpVtbl.QueryInterface(r_d3d11_state.base_device, &IInfoQueue_UUID, (^rawptr)(&info))
    if SUCCEEDED(error) {
      error = info.lpVtbl.SetBreakOnSeverity(info, MESSAGE_SEVERITY.CORRUPTION, TRUE)
      error = info.lpVtbl.SetBreakOnSeverity(info, MESSAGE_SEVERITY.ERROR, TRUE)
      info.lpVtbl.Release(info)
    }
  }
#endif
  
  //- rjf: get main device
  ProfBegin("get main device")
  error = r_d3d11_state.base_device.lpVtbl.QueryInterface(r_d3d11_state.base_device, &IDevice1_UUID, (^rawptr)(&r_d3d11_state.device))
  error = r_d3d11_state.base_device_ctx.lpVtbl.QueryInterface(r_d3d11_state.base_device_ctx, &IDeviceContext1_UUID, (^rawptr)(&r_d3d11_state.device_ctx))
  ProfEnd()
  
  //- rjf: get dxgi device/adapter/factory
  ProfBegin("get dxgi device/adapter/factory")
  error = r_d3d11_state.device.lpVtbl.QueryInterface(r_d3d11_state.device, &IDevice1_UUID, (^rawptr)(&r_d3d11_state.dxgi_device))
  error = r_d3d11_state.dxgi_device.lpVtbl.GetAdapter(r_d3d11_state.dxgi_device, &r_d3d11_state.dxgi_adapter)
  error = r_d3d11_state.dxgi_adapter.lpVtbl.GetParent(r_d3d11_state.dxgi_adapter, &IFactory2_UUID, (^rawptr)(&r_d3d11_state.dxgi_factory))
  error = r_d3d11_state.dxgi_device.lpVtbl.SetMaximumFrameLatency(r_d3d11_state.dxgi_device, 1)
  ProfEnd()
  
  //- rjf: create main rasterizer
  ProfScope("create main rasterizer")
  {
    desc := RASTERIZER_DESC1 {}
    {
      desc.FillMode = FILL_MODE.SOLID
      desc.CullMode = CULL_MODE.BACK
      desc.ScissorEnable = true
    }
    error = r_d3d11_state.device.lpVtbl.CreateRasterizerState1(r_d3d11_state.device, &desc, &r_d3d11_state.main_rasterizer)
  }
  
  //- rjf: create main blend state
  ProfScope("create main blend state")
  {
    desc := BLEND_DESC {}
    {
      desc.RenderTarget[0].BlendEnable            = 1
      desc.RenderTarget[0].SrcBlend               = BLEND.SRC_ALPHA
      desc.RenderTarget[0].DestBlend              = BLEND.INV_SRC_ALPHA
      desc.RenderTarget[0].BlendOp                = BLEND.OP_ADD
      desc.RenderTarget[0].SrcBlendAlpha          = BLEND.ONE
      desc.RenderTarget[0].DestBlendAlpha         = BLEND.ZERO
      desc.RenderTarget[0].BlendOpAlpha           = BLEND.OP_ADD
      desc.RenderTarget[0].RenderTargetWriteMask  = COLOR_WRITE_ENABLE_ALL
    }
    error = r_d3d11_state.device.lpVtbl.CreateBlendState(r_d3d11_state.device, &desc, &r_d3d11_state.main_blend_state)
  }
  
  //- rjf: create empty blend state
  ProfScope("create empty blend state")
  {
    desc := BLEND_DESC{}
    {
      desc.RenderTarget[0].BlendEnable           = FALSE
      desc.RenderTarget[0].RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL
    }
    error = r_d3d11_state.device.lpVtbl.CreateBlendState(r_d3d11_state.device, &desc, &r_d3d11_state.no_blend_state)
  }
  
  //- rjf: create nearest-neighbor sampler
  ProfScope("create nearest-neighbor sampler")
  {
    desc := SAMPLER_DESC{}
    {
      desc.Filter         = FILTER.MIN_MAG_MIP_POINT
      desc.AddressU       = TEXTURE_ADDRESS_MODE.WRAP
      desc.AddressV       = TEXTURE_ADDRESS_MODE.WRAP
      desc.AddressW       = TEXTURE_ADDRESS_MODE.WRAP
      desc.ComparisonFunc = COMPARISON.NEVER
    }
    error = r_d3d11_state.device.lpVtbl.CreateSamplerState(r_d3d11_state.device, &desc, &r_d3d11_state.samplers[R_Tex2DSampleKind_Nearest])
  }
  
  //- rjf: create bilinear sampler
  ProfScope("create bilinear sampler")
  {
    desc := SAMPLER_DESC{}
    {
      desc.Filter         = FILTER.MIN_MAG_MIP_LINEAR;
      desc.AddressU       = TEXTURE_ADDRESS.WRAP
      desc.AddressV       = TEXTURE_ADDRESS.WRAP
      desc.AddressW       = TEXTURE_ADDRESS.WRAP
      desc.ComparisonFunc = COMPARISON.NEVER
    }
    error = r_d3d11_state.device.lpVtbl.CreateSamplerState(r_d3d11_state.device, &desc, &r_d3d11_state.samplers[R_Tex2DSampleKind_Linear])
  }
  
  //- rjf: create noop depth/stencil state
  ProfScope("create noop depth/stencil state")
  {
    desc := DEPTH_STENCIL_DESC{}
    {
      desc.DepthEnable    = false
      desc.DepthWriteMask = DEPTH_WRITE_MASK.ALL
      desc.DepthFunc      = COMPARISON.LESS
    }
    error = r_d3d11_state.device.lpVtbl.CreateDepthStencilState(r_d3d11_state.device, &desc, &r_d3d11_state.noop_depth_stencil)
  }
  
  //- rjf: create plain depth/stencil state
  ProfScope("create plain depth/stencil state")
  {
    desc := DEPTH_STENCIL_DESC{}
    {
      desc.DepthEnable    = true
      desc.DepthWriteMask = DEPTH_WRITE_MASK.ALL
      desc.DepthFunc      = COMPARISON.LESS
    }
    error = r_d3d11_state.device.lpVtbl.CreateDepthStencilState(r_d3d11_state.device, &desc, &r_d3d11_state.plain_depth_stencil)
  }
  
  //- rjf: create buffers
  ProfScope("create buffers")
  {
    desc := BUFFER_DESC{}
    {
      desc.ByteWidth      = KB(64)
      desc.Usage          = USAGE.DYNAMIC
      desc.BindFlags      = BIND_FLAG.VERTEX_BUFFER
      desc.CPUAccessFlags = CPU_ACCESS_FLAG.WRITE
    }
    error = r_d3d11_state.device.lpVtbl.CreateBuffer(r_d3d11_state.device, &desc, 0, &r_d3d11_state.instance_scratch_buffer_64kb)
  }
  
  //- rjf: build vertex shaders & input layouts
  ProfScope("build vertex shaders & input layouts")
    for (kind := (R_D3D11_VShadKind)0;
        kind < R_D3D11_VShadKind_COUNT;
        kind = (R_D3D11_VShadKind)(kind+1))
  {
    source := r_d3d11_g_vshad_kind_source_table[kind]^
    source_name := r_d3d11_g_vshad_kind_source_name_table[kind]
    ilay_elements := r_d3d11_g_vshad_kind_elements_ptr_table[kind]
    ilay_elements_count := r_d3d11_g_vshad_kind_elements_count_table[kind]
    
    // rjf: compile vertex shader
    vshad_source_blob := (^IBlob)nil
    vshad_source_errors := (^IBlob)nil
    vshad := (^IVertexShader)nil
    ProfScope("compile vertex shader")
    {
      error = D3DCompile(source.str,
                         source.size,
                         source_name.str,
                         0,
                         0,
                         "vs_main",
                         "vs_5_0",
                         0,
                         0,
                         &vshad_source_blob,
                         &vshad_source_errors)

      if FAILED(error) {
        errors := str8((^u8)vshad_source_errors.lpVtbl.GetBufferPointer(vshad_source_errors),
                      (u64)vshad_source_errors.lpVtbl.GetBufferSize(vshad_source_errors))
        os_graphical_message(1, str8_lit("Vertex Shader Compilation Failure"), errors);
      }
      else
      {
        error = r_d3d11_state.device.lpVtbl.CreateVertexShader(r_d3d11_state.device, vshad_source_blob.lpVtbl.GetBufferPointer(vshad_source_blob), vshad_source_blob.lpVtbl.GetBufferSize(vshad_source_blob), 0, &vshad)
      }
    }
    
    // rjf: make input layout
    ilay : ^IInputLayout
    if ilay_elements != nil {
      error = r_d3d11_state.device.lpVtbl.CreateInputLayout(r_d3d11_state.device, ilay_elements, ilay_elements_count,
                                                               vshad_source_blob.lpVtbl.GetBufferPointer(vshad_source_blob),
                                                               vshad_source_blob.lpVtbl.GetBufferSize(vshad_source_blob),
                                                               &ilay)
    }
    
    vshad_source_blob.lpVtbl.Release(vshad_source_blob)
    
    // rjf: store
    r_d3d11_state.vshads[kind] = vshad
    r_d3d11_state.ilays[kind] = ilay
  }
  
  //- rjf: build pixel shaders
  for(kind := (R_D3D11_PShadKind)0;
      kind < R_D3D11_PShadKind_COUNT;
      kind = (R_D3D11_PShadKind)(kind+1))
  {
    source := r_d3d11_g_pshad_kind_source_table[kind]^
    source_name := r_d3d11_g_pshad_kind_source_name_table[kind]
    
    // rjf: compile pixel shader
    pshad_source_blob : ^IBlob
    pshad_source_errors : ^IBlob
    pshad : ^IPixelShader
    ProfScope("compile pixel shader")
    {
      error = D3DCompile(source.str,
                         source.size,
                         source_name.str,
                         0,
                         0,
                         "ps_main",
                         "ps_5_0",
                         0,
                         0,
                         &pshad_source_blob,
                         &pshad_source_errors)
      if FAILED(error) {
        errors := str8((^u8)pshad_source_errors.lpVtbl.GetBufferPointer(pshad_source_errors),
                      (u64)pshad_source_errors.lpVtbl.GetBufferSize(pshad_source_errors))
        os_graphical_message(1, "Pixel Shader Compilation Failure", errors)
      }
      else
      {
        error = r_d3d11_state.device.lpVtbl.CreatePixelShader(r_d3d11_state.device, pshad_source_blob.lpVtbl.GetBufferPointer(pshad_source_blob), pshad_source_blob.lpVtbl.GetBufferSize(pshad_source_blob), 0, &pshad)
      }
    }
    
    pshad_source_blob.lpVtbl.Release(pshad_source_blob)
    
    // rjf: store
    r_d3d11_state.pshads[kind] = pshad
  }
  
  //- rjf: build uniform type buffers
  ProfScope("build uniform type buffers")
    for(kind := (R_D3D11_UniformTypeKind)0;
        kind < R_D3D11_UniformTypeKind_COUNT;
        kind = (R_D3D11_UniformTypeKind)(kind+1))
  {
    buffer : IBuffer
    {
      desc := BUFFER_DESC{}
      {
        desc.ByteWidth = r_d3d11_g_uniform_type_kind_size_table[kind]
        desc.ByteWidth += 15
        desc.ByteWidth -= desc.ByteWidth % 16
        desc.Usage          = USAGE.DYNAMIC
        desc.BindFlags      = BIND_FLAG._CONSTANT_BUFFER
        desc.CPUAccessFlags = CPU_FLAG.ACCESS_WRITE
      }
      r_d3d11_state.device.lpVtbl.CreateBuffer(r_d3d11_state.device, &desc, 0, &buffer)
    }
    r_d3d11_state.uniform_type_kind_buffers[kind] = buffer
  }
  
  //- rjf: create backup texture
  ProfScope("create backup texture")
  {
    backup_texture_data := [?]u32 {
      0xff00ffff, 0x330033ff,
      0x330033ff, 0xff00ffff,
    }
    r_d3d11_state.backup_texture = r_tex2d_alloc(R_ResourceKind_Static, v2s32(2, 2), R_Tex2DFormat_RGBA8, backup_texture_data)
  }
  
  //- rjf: initialize buffer flush state
  {
    r_d3d11_state.buffer_flush_arena = arena_alloc()
  }
  
  ProfEnd()
}

//- rjf: window setup/teardown

r_window_equip :: proc "c" (handle: OS_Handle) -> R_Handle
{
  ProfBeginFunction()
  result : R_Handle
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    //- rjf: allocate per-window-state
    window := r_d3d11_state.first_free_window
    {
      if window == nil {
        window = push_array(r_d3d11_state.arena, R_D3D11_Window, 1)
      }
      else
      {
        gen := window.generation
        SLLStackPop(r_d3d11_state.first_free_window)
        MemoryZeroStruct(window)
        window.generation = gen
      }
      window.generation += 1
    }
    
    //- rjf: map os window handle . hwnd
    hwnd : HWND
    {
      w32_layer_window := os_w32_window_from_handle(handle)
      hwnd = os_w32_hwnd_from_window(w32_layer_window)
    }
    
    //- rjf: create swapchain
    swapchain_desc : SWAP_CHAIN_DESC1
    {
      swapchain_desc.Width              = 0 // NOTE(rjf): use window width
      swapchain_desc.Height             = 0 // NOTE(rjf): use window height
      swapchain_desc.Format             = FORMAT.B8G8R8A8_UNORM
      swapchain_desc.Stereo             = FALSE
      swapchain_desc.SampleDesc.Count   = 1
      swapchain_desc.SampleDesc.Quality = 0
      swapchain_desc.BufferUsage        = USAGE_FLAG.RENDER_TARGET_OUTPUT
      swapchain_desc.BufferCount        = 2
      swapchain_desc.Scaling            = SCALING.NONE
      swapchain_desc.SwapEffect         = SWAP_EFFECT.FLIP_DISCARD
      swapchain_desc.AlphaMode          = ALPHA_MODE.UNSPECIFIED
      swapchain_desc.Flags              = 0
    }
    error := r_d3d11_state.dxgi_factory.lpVtbl.CreateSwapChainForHwnd(r_d3d11_state.dxgi_factory, (^IUnknown)r_d3d11_state.device, hwnd, &swapchain_desc, 0, 0, &window.swapchain)
    if FAILED(error) {
      buffer : [256]u8
      raddbg_snprintf(buffer, sizeof(buffer), "DXGI swap chain creation failure (%lx). The process is terminating.", error)
      os_graphical_message(1, "Fatal Error", str8_cstring(buffer))
      os_abort(1)
    }
    
    r_d3d11_state.dxgi_factory.lpVtbl.MakeWindowAssociation(r_d3d11_state.dxgi_factory, hwnd, MWA_FLAG.NO_ALT_ENTER)
    
    //- rjf: create framebuffer & view
    framebuffer_rtv_desc := RENDER_TARGET_VIEW_DESC{}
    framebuffer_rtv_desc.Format        = FORMAT.B8G8R8A8_UNORM_SRGB
    framebuffer_rtv_desc.ViewDimension = RTV_DIMENSION.TEXTURE2D
    window.swapchain.lpVtbl.GetBuffer(window.swapchain, 0, &IID_ID3D11Texture2D, (^rawptr)(&window.framebuffer))
    r_d3d11_state.device.lpVtbl.CreateRenderTargetView(r_d3d11_state.device, (^IResource)window.framebuffer, &framebuffer_rtv_desc, &window.framebuffer_rtv)
    
    result = r_d3d11_handle_from_window(window)
    r_d3d11_state.window_count += 1
    r_d3d11_state.dxgi_device.lpVtbl.SetMaximumFrameLatency(r_d3d11_state.dxgi_device, clamp(r_d3d11_state.window_count, 1, 16))
  }
  ProfEnd()
  return result
}

r_hook void
r_window_unequip(handle: OS_Handle, equip_handle: R_Handle)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    window := r_d3d11_window_from_handle(equip_handle)
    window.stage_color_srv.lpVtbl.Release(window.stage_color_srv)
    window.stage_color_rtv.lpVtbl.Release(window.stage_color_rtv)
    window.stage_color.lpVtbl.Release(window.stage_color)
    window.stage_scratch_color_srv.lpVtbl.Release(window.stage_scratch_color_srv)
    window.stage_scratch_color_rtv.lpVtbl.Release(window.stage_scratch_color_rtv)
    window.stage_scratch_color.lpVtbl.Release(window.stage_scratch_color)
    window.framebuffer_rtv.lpVtbl.Release(window.framebuffer_rtv)
    window.framebuffer.lpVtbl.Release(window.framebuffer)
    window.swapchain.lpVtbl.Release(window.swapchain)
    window.generation += 1
    SLLStackPush(r_d3d11_state.first_free_window, window)
    r_d3d11_state.window_count -= 1
    r_d3d11_state.dxgi_device.lpVtbl.SetMaximumFrameLatency(r_d3d11_state.dxgi_device, clamp(r_d3d11_state.window_count, 1, 16))
  }
  ProfEnd()
}

//- rjf: textures

r_tex2d_alloc :: proc "c" (kind: R_ResourceKind, size: Vec2S32, format: R_Tex2DFormat, data: rawptr) -> R_Handle
{
  ProfBeginFunction()
  
  //- rjf: allocate
  texture : ^R_D3D11_Tex2D
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    texture = r_d3d11_state.first_free_tex2d
    if texture == nil {
      texture = push_array(r_d3d11_state.arena, R_D3D11_Tex2D, 1)
    } else {
      gen := texture.generation
      SLLStackPop(r_d3d11_state.first_free_tex2d)
      MemoryZeroStruct(texture)
      texture.generation = gen
    }
    texture.generation += 1
  }
  
  d3d11_usage := USAGE.DEFAULT
  cpu_access_flags := UINT(0)
  r_usage_access_flags_from_resource_kind(kind, &d3d11_usage, &cpu_access_flags)
  if kind == R_ResourceKind_Static {
    assert(data != nil, "static texture must have initial data provided")
  }
  
  //- rjf: format . dxgi format
  dxgi_format := FORMAT.R8G8B8A8_UNORM
  {
    switch format {
      case R_Tex2DFormat_R8:     dxgi_format = FORMAT.R8_UNORM
      case R_Tex2DFormat_RG8:    dxgi_format = FORMAT.R8G8_UNORM
      case R_Tex2DFormat_RGBA8:  dxgi_format = FORMAT.R8G8B8A8_UNORM
      case R_Tex2DFormat_BGRA8:  dxgi_format = FORMAT.B8G8R8A8_UNORM
      case R_Tex2DFormat_R16:    dxgi_format = FORMAT.R16_UNORM
      case R_Tex2DFormat_RGBA16: dxgi_format = FORMAT.R16G16B16A16_UNORM
      case R_Tex2DFormat_R32:    dxgi_format = FORMAT.R32_FLOAT
      case R_Tex2DFormat_RG32:   dxgi_format = FORMAT.R32G32_FLOAT
      case R_Tex2DFormat_RGBA32: dxgi_format = FORMAT.R32G32B32A32_FLOAT
      case: {}
    }
  }
  
  //- rjf: prep data: initial, if passed
  initial_data_ := SUBRESOURCE_DATA{}
  initial_data : ^D3D11_SUBRESOURCE_DATA
  if data != nil {
    initial_data = &initial_data_
    initial_data.pSysMem = data
    initial_data.SysMemPitch = r_tex2d_format_bytes_per_pixel_table[format] * size.x
  }
  
  //- rjf: create texture
  texture_desc := TEXTURE2D_DESC{}
  {
    texture_desc.Width              = size.x
    texture_desc.Height             = size.y
    texture_desc.MipLevels          = 1
    texture_desc.ArraySize          = 1
    texture_desc.Format             = dxgi_format
    texture_desc.SampleDesc.Count   = 1
    texture_desc.Usage              = d3d11_usage
    texture_desc.BindFlags          = BIND_FLAG.SHADER_RESOURCE
    texture_desc.CPUAccessFlags     = cpu_access_flags
  }
  r_d3d11_state.device.lpVtbl.CreateTexture2D(r_d3d11_state.device, &texture_desc, initial_data, &texture.texture)
  
  //- rjf: create texture srv
  r_d3d11_state.device.lpVtbl.CreateShaderResourceView(r_d3d11_state.device, (^IResource)texture.texture, 0, &texture.view)
  
  //- rjf: fill basics
  {
    texture.kind = kind
    texture.size = size
    texture.format = format
  }
  
  result := r_d3d11_handle_from_tex2d(texture)
  ProfEnd()
  return result
}

r_tex2d_release :: proc "c" (handle: R_Handle)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    texture := r_d3d11_tex2d_from_handle(handle)
    if texture != &r_d3d11_tex2d_nil {
      SLLStackPush(r_d3d11_state.first_to_free_tex2d, texture)
    }
  }
  ProfEnd()
}

r_kind_from_tex2d :: proc "c" (handle: R_Handle) -> R_ResourceKind
{
  texture := r_d3d11_tex2d_from_handle(handle)
  return texture.kind
}

r_size_from_tex2d :: proc "c" (handle: R_Handle) -> Vec2S32
{
  texture := r_d3d11_tex2d_from_handle(handle)
  return texture.size
}

r_format_from_tex2d :: proc "c" (handle: R_Handle) -> R_Tex2DFormat
{
  texture := r_d3d11_tex2d_from_handle(handle)
  return texture.format
}

r_fill_tex2d_region :: proc "c" (handle: R_Handle, subrect: Rng2S32, data: rawptr)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    texture := r_d3d11_tex2d_from_handle(handle)
    if texture != &r_d3d11_tex2d_nil {
      assert(texture.kind == R_ResourceKind_Dynamic, "only dynamic texture can update region")
      bytes_per_pixel := r_tex2d_format_bytes_per_pixel_table[texture.format]
      dim := v2s32(subrect.x1 - subrect.x0, subrect.y1 - subrect.y0)
      dst_box := D3D11_BOX {
        (UINT)subrect.x0, (UINT)subrect.y0, 0,
        (UINT)subrect.x1, (UINT)subrect.y1, 1,
      }
      r_d3d11_state.device_ctx.lpVtbl.UpdateSubresource(r_d3d11_state.device_ctx, (^IResource)texture.texture, 0, &dst_box, data, dim.x*bytes_per_pixel, 0)
    }
  }
  ProfEnd()
}

//- rjf: buffers

r_buffer_alloc :: proc "c" (kind: R_ResourceKind, size: u64, data: rawptr) -> R_Handle
{
  ProfBeginFunction()
  
  //- rjf: allocate
  buffer : ^R_D3D11_Buffer
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    buffer = r_d3d11_state.first_free_buffer
    if buffer == nil {
      buffer = push_array(r_d3d11_state.arena, R_D3D11_Buffer, 1)
    } else {
      gen := buffer.generation
      SLLStackPop(r_d3d11_state.first_free_buffer)
      MemoryZeroStruct(buffer)
      buffer.generation = gen
    }
    buffer.generation += 1
  }
  
  d3d11_usage := USAGE_FLAG.DEFAULT
  cpu_access_flags := UINT(0)
  r_usage_access_flags_from_resource_kind(kind, &d3d11_usage, &cpu_access_flags)
  if kind == R_ResourceKind_Static {
    assert(data != nil, "static buffer must have initial data provided")
  }
  
  //- rjf: prep data: initial, if passed
  initial_data_ := SUBRESOURCE_DATA{}
  initial_data : ^SUBRESOURCE_DATA
  if data != nil {
    initial_data = &initial_data_
    initial_data.pSysMem = data
  }
  
  //- rjf: create buffer
  desc := BUFFER_DESC{}
  {
    desc.ByteWidth      = size
    desc.Usage          = d3d11_usage
    desc.BindFlags      = BIND_FLAG.VERTEX_BUFFER|BIND_FLAG.INDEX_BUFFER
    desc.CPUAccessFlags = cpu_access_flags
  }
  r_d3d11_state.device.lpVtbl.CreateBuffer(r_d3d11_state.device, &desc, initial_data, &buffer.buffer)
  
  //- rjf: fill basics
  {
    buffer.kind = kind
    buffer.size = size
  }
  
  R_Handle result = r_d3d11_handle_from_buffer(buffer)
  ProfEnd()
  return result
}

r_buffer_release :: proc "c" (handle: R_Handle)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    buffer := r_d3d11_buffer_from_handle(handle)
    SLLStackPush(r_d3d11_state.first_to_free_buffer, buffer)
  }
  ProfEnd()
}

//- rjf: frame markers

r_begin_frame :: proc "c" ()
{
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    // NOTE(rjf): no-op
  }
}

r_end_frame :: proc "c" ()
{
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    for (buffer := r_d3d11_state.first_buffer_to_flush; buffer != nil; buffer = buffer.next)
    {
      buffer.buffer.lpVtbl.Release(buffer.buffer)
    }
    for(tex := r_d3d11_state.first_to_free_tex2d, next : ^R_D3D11_Tex2D = nil;
        tex != nil;
        tex = next)
    {
      next = tex.next
      tex.view.lpVtbl.Release(tex.view)
      tex.texture.lpVtbl.Release(tex.texture)
      tex.view = 0
      tex.texture = 0
      tex.generation += 1
      SLLStackPush(r_d3d11_state.first_free_tex2d, tex)
    }
    for(buf := r_d3d11_state.first_to_free_buffer, next : ^R_D3D11_Buffer = 0;
        buf != 0;
        buf = next)
    {
      next = buf.next
      buf.buffer.lpVtbl.Release(buf.buffer)
      buf.generation += 1
      SLLStackPush(r_d3d11_state.first_free_buffer, buf)
    }
    arena_clear(r_d3d11_state.buffer_flush_arena)
    r_d3d11_state.first_buffer_to_flush = r_d3d11_state.last_buffer_to_flush = 0
    r_d3d11_state.first_to_free_tex2d  = 0
    r_d3d11_state.first_to_free_buffer = 0
  }
}

r_window_begin_frame :: proc "c" (window: OS_Handle, window_equip: R_Handle)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    wnd := r_d3d11_window_from_handle(window_equip)
    d_ctx := r_d3d11_state.device_ctx
    
    //- rjf: get resolution
    client_rect := os_client_rect_from_window(window)
    resolution := Vec2S32 {(S32)(client_rect.x1 - client_rect.x0), (S32)(client_rect.y1 - client_rect.y0)}
    
    //- rjf: resolution change
    resize_done := false
    if(wnd.last_resolution.x != resolution.x ||
       wnd.last_resolution.y != resolution.y)
    {
      resize_done = true
      wnd.last_resolution = resolution
      
      // rjf: release screen-sized render resources: target, if there
      if wnd.stage_scratch_color_srv do wnd.stage_scratch_color_srv.lpVtbl.Release(wnd.stage_scratch_color_srv)
      if wnd.stage_scratch_color_rtv do wnd.stage_scratch_color_rtv.lpVtbl.Release(wnd.stage_scratch_color_rtv)
      if wnd.stage_scratch_color     do wnd.stage_scratch_color.lpVtbl.Release(wnd.stage_scratch_color)
      if wnd.stage_color_srv         do wnd.stage_color_srv.lpVtbl.Release(wnd.stage_color_srv)
      if wnd.stage_color_rtv         do wnd.stage_color_rtv.lpVtbl.Release(wnd.stage_color_rtv)
      if wnd.stage_color             do wnd.stage_color.lpVtbl.Release(wnd.stage_color)
      if wnd.geo3d_color_srv         do wnd.geo3d_color_srv.lpVtbl.Release(wnd.geo3d_color_srv)
      if wnd.geo3d_color_rtv         do wnd.geo3d_color_rtv.lpVtbl.Release(wnd.geo3d_color_rtv)
      if wnd.geo3d_color             do wnd.geo3d_color.lpVtbl.Release(wnd.geo3d_color)
      if wnd.geo3d_depth_srv         do wnd.geo3d_depth_srv.lpVtbl.Release(wnd.geo3d_depth_srv)
      if wnd.geo3d_depth_dsv         do wnd.geo3d_depth_dsv.lpVtbl.Release(wnd.geo3d_depth_dsv)
      if wnd.geo3d_depth             do wnd.geo3d_depth.lpVtbl.Release(wnd.geo3d_depth)
      
      // rjf: resize swapchain & main framebuffer
      wnd.framebuffer_rtv.lpVtbl.Release(wnd.framebuffer_rtv)
      wnd.framebuffer.lpVtbl.Release(wnd.framebuffer)
      wnd.swapchain.lpVtbl.ResizeBuffers(wnd.swapchain, 0, 0, 0, FORMAT.UNKNOWN, 0)
      wnd.swapchain.lpVtbl.GetBuffer(wnd.swapchain, 0, &IID_ID3D11Texture2D, (^rawptr)(&wnd.framebuffer))
      framebuffer_rtv_desc := RENDER_TARGET_VIEW_DESC{}
      framebuffer_rtv_desc.Format        = FORMAT.B8G8R8A8_UNORM_SRGB
      framebuffer_rtv_desc.ViewDimension = RTV_DIMENSION.TEXTURE2D
      r_d3d11_state.device.lpVtbl.CreateRenderTargetView(r_d3d11_state.device, (^IResource)wnd.framebuffer, &framebuffer_rtv_desc, &wnd.framebuffer_rtv)
      
      // rjf: create stage color targets
      {
        color_desc := TEXTURE2D_DESC{}
        {
          wnd.framebuffer.lpVtbl.GetDesc(wnd.framebuffer, &color_desc)
          color_desc.Format = FORMAT.R16G16B16A16_FLOAT
          color_desc.BindFlags = BIND.RENDER_TARGET|BIND.SHADER_RESOURCE
        }
        rtv_desc := RENDER_TARGET_VIEW_DESC{}
        {
          rtv_desc.Format         = color_desc.Format
          rtv_desc.ViewDimension  = RTV_DIMENSION.TEXTURE2D
        }
        srv_desc := SHADER_RESOURCE_VIEW_DESC{}
        {
          srv_desc.Format                    = FORMAT.R16G16B16A16_FLOAT
          srv_desc.ViewDimension             = SRV_DIMENSION.TEXTURE2D
          srv_desc.Texture2D.MipLevels       = -1
        }
        r_d3d11_state.device.lpVtbl.CreateTexture2D(r_d3d11_state.device, &color_desc, 0, &wnd.stage_color)
        r_d3d11_state.device.lpVtbl.CreateRenderTargetView(r_d3d11_state.device, (^IResource)wnd.stage_color, &rtv_desc, &wnd.stage_color_rtv)
        r_d3d11_state.device.lpVtbl.CreateShaderResourceView(r_d3d11_state.device, (^IResource)wnd.stage_color, &srv_desc, &wnd.stage_color_srv)
        r_d3d11_state.device.lpVtbl.CreateTexture2D(r_d3d11_state.device, &color_desc, 0, &wnd.stage_scratch_color)
        r_d3d11_state.device.lpVtbl.CreateRenderTargetView(r_d3d11_state.device, (^IResource)wnd.stage_scratch_color, &rtv_desc, &wnd.stage_scratch_color_rtv)
        r_d3d11_state.device.lpVtbl.CreateShaderResourceView(r_d3d11_state.device, (^IResource)wnd.stage_scratch_color, &srv_desc, &wnd.stage_scratch_color_srv)
      }
      
      // rjf: create geo3d targets
      {
        color_desc := TEXTURE2D_DESC{}
        {
          wnd.framebuffer.lpVtbl.GetDesc(wnd.framebuffer, &color_desc)
          color_desc.Format = FORMAT.R8G8B8A8_UNORM;
          color_desc.BindFlags = BIND.RENDER_TARGET|BIND.SHADER_RESOURCE
        }
        color_rtv_desc := RENDER_TARGET_VIEW_DESC{}
        {
          color_rtv_desc.Format         = color_desc.Format
          color_rtv_desc.ViewDimension  = RTV_DIMENSION.TEXTURE2D
        }
        color_srv_desc := SHADER_RESOURCE_VIEW_DESC{}
        {
          color_srv_desc.Format                    = FORMAT.R8G8B8A8_UNORM
          color_srv_desc.ViewDimension             = SRV_DIMENSION.TEXTURE2D
          color_srv_desc.Texture2D.MipLevels       = -1
        }
        depth_desc := TEXTURE2D_DESC{}
        {
          wnd.framebuffer.lpVtbl.GetDesc(wnd.framebuffer, &depth_desc)
          depth_desc.Format = FORMAT.R24G8_TYPELESS
          depth_desc.BindFlags = BIND.DEPTH_STENCIL|BIND.SHADER_RESOURCE
        }
        depth_dsv_desc := DEPTH_STENCIL_VIEW_DESC{}
        {
          depth_dsv_desc.Flags = 0
          depth_dsv_desc.Format = FORMAT.D24_UNORM_S8_UINT
          depth_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D
          depth_dsv_desc.Texture2D.MipSlice = 0
        }
        depth_srv_desc := SHADER_RESOURCE_VIEW_DESC{}
        {
          depth_srv_desc.Format                    = FORMAT.R24_UNORM_X8_TYPELESS
          depth_srv_desc.ViewDimension             = SRV_DIMENSION.TEXTURE2D
          depth_srv_desc.Texture2D.MostDetailedMip = 0
          depth_srv_desc.Texture2D.MipLevels       = -1
        }
        r_d3d11_state.device.lpVtbl.CreateTexture2D(r_d3d11_state.device, &color_desc, 0, &wnd.geo3d_color)
        r_d3d11_state.device.lpVtbl.CreateRenderTargetView(r_d3d11_state.device, (^IResource)wnd.geo3d_color, &color_rtv_desc, &wnd.geo3d_color_rtv)
        r_d3d11_state.device.lpVtbl.CreateShaderResourceView(r_d3d11_state.device, (^IResource)wnd.geo3d_color, &color_srv_desc, &wnd.geo3d_color_srv)
        r_d3d11_state.device.lpVtbl.CreateTexture2D(r_d3d11_state.device, &depth_desc, 0, &wnd.geo3d_depth)
        r_d3d11_state.device.lpVtbl.CreateDepthStencilView(r_d3d11_state.device, (^IResource)wnd.geo3d_depth, &depth_dsv_desc, &wnd.geo3d_depth_dsv)
        r_d3d11_state.device.lpVtbl.CreateShaderResourceView(r_d3d11_state.device, (^IResource)wnd.geo3d_depth, &depth_srv_desc, &wnd.geo3d_depth_srv)
      }
    }
    
    //- rjf: clear framebuffers
    clear_color := Vec4F32 {0, 0, 0, 0}
    d_ctx.lpVtbl.ClearRenderTargetView(d_ctx, wnd.framebuffer_rtv, clear_color.v)
    d_ctx.lpVtbl.ClearRenderTargetView(d_ctx, wnd.stage_color_rtv, clear_color.v)
    if resize_done {
      d_ctx.lpVtbl.Flush(d_ctx)
    }
  }
  ProfEnd()
}

r_window_end_frame :: proc "c" (window: OS_Handle, window_equip: R_Handle)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    wnd := r_d3d11_window_from_handle(window_equip)
    d_ctx := r_d3d11_state.device_ctx
    
    ////////////////////////////
    //- rjf: finalize, by writing staging buffer out to window framebuffer
    //
    {
      sampler   := r_d3d11_state.samplers[R_Tex2DSampleKind_Nearest]
      vshad     := r_d3d11_state.vshads[R_D3D11_VShadKind_Finalize]
      pshad      := r_d3d11_state.pshads[R_D3D11_PShadKind_Finalize]
      
      // rjf: setup output merger
      d_ctx.lpVtbl.OMSetRenderTargets(d_ctx, 1, &wnd.framebuffer_rtv, 0)
      d_ctx.lpVtbl.OMSetDepthStencilState(d_ctx, r_d3d11_state.noop_depth_stencil, 0)
      d_ctx.lpVtbl.OMSetBlendState(d_ctx, r_d3d11_state.main_blend_state, 0, 0xffffffff)
      
      // rjf: set up rasterizer
      resolution := wnd.last_resolution
      viewport := VIEWPORT { 0.0f, 0.0f, (F32)resolution.x, (F32)resolution.y, 0.0f, 1.0f }
      d_ctx.lpVtbl.RSSetViewports(d_ctx, 1, &viewport)
      d_ctx.lpVtbl.RSSetState(d_ctx, (^IRasterizerState)r_d3d11_state.main_rasterizer)
      
      // rjf: setup input assembly
      d_ctx.lpVtbl.IASetPrimitiveTopology(d_ctx, PRIMITIVE_TOPOLOGY.TRIANGLESTRIP)
      d_ctx.lpVtbl.IASetInputLayout(d_ctx, 0)
      
      // rjf: setup shaders
      d_ctx.lpVtbl.VSSetShader(d_ctx, vshad, 0, 0)
      d_ctx.lpVtbl.PSSetShader(d_ctx, pshad, 0, 0)
      d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &wnd.stage_color_srv)
      d_ctx.lpVtbl.PSSetSamplers(d_ctx, 0, 1, &sampler)
      
      // rjf: setup scissor rect
      {
        rect := RECT{}
        rect.left = 0
        rect.right = (LONG)wnd.last_resolution.x
        rect.top = 0
        rect.bottom = (LONG)wnd.last_resolution.y
        d_ctx.lpVtbl.RSSetScissorRects(d_ctx, 1, &rect)
      }
      
      // rjf: draw
      d_ctx.lpVtbl.Draw(d_ctx, 4, 0)
    }
    
    ////////////////////////////
    //- rjf: present
    //
    HRESULT error = wnd.swapchain.lpVtbl.Present(wnd.swapchain, 1, 0)
    if FAILED(error) {
      buffer : [256]u8
      raddbg_snprintf(buffer, sizeof(buffer), "D3D11 present failure (%lx). The process is terminating.", error)
      os_graphical_message(1, "Fatal Error", str8_cstring(buffer))
      os_abort(1)
    }
    d_ctx.lpVtbl.ClearState(d_ctx)
  }
  ProfEnd()
}

//- rjf: render pass submission

r_window_submit :: proc "c" (window: OS_Handle, window_equip: R_Handle, passes: ^R_PassList)
{
  ProfBeginFunction()
  OS_MutexScopeW(r_d3d11_state.device_rw_mutex)
  {
    ////////////////////////////
    //- rjf: unpack arguments
    //
    wnd := r_d3d11_window_from_handle(window_equip)
    d_ctx := r_d3d11_state.device_ctx
    
    ////////////////////////////
    //- rjf: do passes
    //
    for pass_n := passes.first; pass_n != 0; pass_n = pass_n.next {
      pass := &pass_n.v
      
      switch pass.kind {
        
        ////////////////////////
        //- rjf: ui rendering pass
        //
        case R_PassKind_UI:
        {
          //- rjf: unpack params
          params := pass.params_ui;
          rect_batch_groups := &params.rects
          
          //- rjf: set up rasterizer
          resolution := wnd.last_resolution
          viewport := VIEWPORT { 0.0f, 0.0f, (F32)resolution.x, (F32)resolution.y, 0.0f, 1.0f }
          d_ctx.lpVtbl.RSSetViewports(d_ctx, 1, &viewport)
          d_ctx.lpVtbl.RSSetState(d_ctx, (^IRasterizerState)r_d3d11_state.main_rasterizer)
          
          //- rjf: draw each batch group
          for group_n = rect_batch_groups.first; group_n != nil; group_n = group_n.next {
            batches := &group_n.batches
            group_params := &group_n.params
            
            // rjf: unpack pipeline info
            sampler   := r_d3d11_state.samplers[group_params.tex_sample_kind]
            vshad     := r_d3d11_state.vshads[R_D3D11_VShadKind_Rect]
            ilay       := r_d3d11_state.ilays[R_D3D11_VShadKind_Rect]
            pshad      := r_d3d11_state.pshads[R_D3D11_PShadKind_Rect]
            uniforms_buffer := r_d3d11_state.uniform_type_kind_buffers[R_D3D11_UniformTypeKind_Rect]
            
            // rjf: get & fill buffer
            buffer := r_d3d11_instance_buffer_from_size(batches.byte_count)
            {
              sub_rsrc := MAPPED_SUBRESOURCE{}
              d_ctx.lpVtbl.Map(d_ctx, (^IResource)buffer, 0, MAP.WRITE_DISCARD, 0, &sub_rsrc)
              dst_ptr := (^u8)sub_rsrc.pData
              off := u64(0)
              for batch_n := batches.first; batch_n != nil; batch_n = batch_n.next {
                MemoryCopy(dst_ptr + off, batch_n.v.v, batch_n.v.byte_count)
                off += batch_n.v.byte_count
              }
              d_ctx.lpVtbl.Unmap(d_ctx, (^IResource)buffer, 0)
            }
            
            // rjf: get texture
            texture_handle := group_params.tex
            if r_handle_match(texture_handle, r_handle_zero()) {
              texture_handle = r_d3d11_state.backup_texture
            }
            texture := r_d3d11_tex2d_from_handle(texture_handle)
            
            // rjf: get texture sample matrix: map, based on format
            texture_sample_channel_map := r_sample_channel_map_from_tex2dformat(texture.format)
            
            // rjf: upload uniforms
            uniforms : R_D3D11_Uniforms_Rect
            {
              uniforms.viewport_size             = v2f32(resolution.x, resolution.y)
              uniforms.opacity                   = 1-group_params.transparency
              uniforms.texture_sample_channel_map = texture_sample_channel_map
              uniforms.texture_t2d_size          = v2f32(texture.size.x, texture.size.y)
              uniforms.xform[0] = v4f32(group_params.xform.v[0][0], group_params.xform.v[1][0], group_params.xform.v[2][0], 0)
              uniforms.xform[1] = v4f32(group_params.xform.v[0][1], group_params.xform.v[1][1], group_params.xform.v[2][1], 0)
              uniforms.xform[2] = v4f32(group_params.xform.v[0][2], group_params.xform.v[1][2], group_params.xform.v[2][2], 0)
              Vec2F32 xform_2x2_col0 = v2f32(uniforms.xform[0].x, uniforms.xform[1].x)
              Vec2F32 xform_2x2_col1 = v2f32(uniforms.xform[0].y, uniforms.xform[1].y)
              uniforms.xform_scale.x = length_2f32(xform_2x2_col0)
              uniforms.xform_scale.y = length_2f32(xform_2x2_col1)
            }
            {
              sub_rsrc := MAPPED_SUBRESOURCE{}
              d_ctx.lpVtbl.Map(d_ctx, (^IResource)uniforms_buffer, 0, MAP.WRITE_DISCARD, 0, &sub_rsrc)
              MemoryCopy((^u8)sub_rsrc.pData, &uniforms, sizeof(uniforms))
              d_ctx.lpVtbl.Unmap(d_ctx, (^IResource)uniforms_buffer, 0)
            }
            
            // rjf: setup output merger
            d_ctx.lpVtbl.OMSetRenderTargets(d_ctx, 1, &wnd.stage_color_rtv, 0)
            d_ctx.lpVtbl.OMSetDepthStencilState(d_ctx, r_d3d11_state.noop_depth_stencil, 0)
            d_ctx.lpVtbl.OMSetBlendState(d_ctx, r_d3d11_state.main_blend_state, 0, 0xffffffff)
            
            // rjf: setup input assembly
            stride := batches.bytes_per_inst;
            offset := u32(0)
            d_ctx.lpVtbl.IASetPrimitiveTopology(d_ctx, PRIMITIVE_TOPOLOGY.TRIANGLESTRIP)
            d_ctx.lpVtbl.IASetInputLayout(d_ctx, ilay)
            d_ctx.lpVtbl.IASetVertexBuffers(d_ctx, 0, 1, &buffer, &stride, &offset)
            
            // rjf: setup shaders
            d_ctx.lpVtbl.VSSetShader(d_ctx, vshad, 0, 0)
            d_ctx.lpVtbl.VSSetConstantBuffers(d_ctx, 0, 1, &uniforms_buffer)
            d_ctx.lpVtbl.PSSetShader(d_ctx, pshad, 0, 0)
            d_ctx.lpVtbl.PSSetConstantBuffers(d_ctx, 0, 1, &uniforms_buffer)
            d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &texture.view)
            d_ctx.lpVtbl.PSSetSamplers(d_ctx, 0, 1, &sampler)
            
            // rjf: setup scissor rect
            {
              clip := group_params.clip
              rect := RECT{}
              {
                if clip.x0 == 0 && clip.y0 == 0 && clip.x1 == 0 && clip.y1 == 0 {
                  rect.left = 0
                  rect.right = (LONG)wnd.last_resolution.x
                  rect.top = 0
                  rect.bottom = (LONG)wnd.last_resolution.y
                } else if clip.x0 > clip.x1 || clip.y0 > clip.y1 {
                  rect.left = 0
                  rect.right = 0
                  rect.top = 0
                  rect.bottom = 0
                } else {
                  rect.left = (LONG)clip.x0
                  rect.right = (LONG)clip.x1
                  rect.top = (LONG)clip.y0
                  rect.bottom = (LONG)clip.y1
                }
              }
              d_ctx.lpVtbl.RSSetScissorRects(d_ctx, 1, &rect)
            }
            
            // rjf: draw
            d_ctx.lpVtbl.DrawInstanced(d_ctx, 4, batches.byte_count / batches.bytes_per_inst, 0, 0)
          }
        }
        
        ////////////////////////
        //- rjf: blur rendering pass
        //
        case R_PassKind_Blur:
        {
          params = pass.params_blur
          sampler   = r_d3d11_state.samplers[R_Tex2DSampleKind_Linear]
          vshad     = r_d3d11_state.vshads[R_D3D11_VShadKind_Blur]
          pshad      = r_d3d11_state.pshads[R_D3D11_PShadKind_Blur]
          uniforms_buffer = r_d3d11_state.uniform_type_kind_buffers[R_D3D11_VShadKind_Blur]
          
          // rjf: setup output merger
          d_ctx.lpVtbl.OMSetDepthStencilState(d_ctx, r_d3d11_state.noop_depth_stencil, 0)
          d_ctx.lpVtbl.OMSetBlendState(d_ctx, r_d3d11_state.no_blend_state, 0, 0xffffffff)
          
          // rjf: set up viewport
          resolution := wnd.last_resolution
          viewport = VIEWPORT { 0.0f, 0.0f, (F32)resolution.x, (F32)resolution.y, 0.0f, 1.0f }
          d_ctx.lpVtbl.RSSetViewports(d_ctx, 1, &viewport)
          d_ctx.lpVtbl.RSSetState(d_ctx, (^IRasterizerState)r_d3d11_state.main_rasterizer)
          
          // rjf: setup input assembly
          d_ctx.lpVtbl.IASetPrimitiveTopology(d_ctx, PRIMITIVE_TOPOLOGY.TRIANGLESTRIP)
          d_ctx.lpVtbl.IASetInputLayout(d_ctx, 0)
          
          // rjf: setup shaders
          d_ctx.lpVtbl.VSSetShader(d_ctx, vshad, 0, 0)
          d_ctx.lpVtbl.VSSetConstantBuffers(d_ctx, 0, 1, &uniforms_buffer)
          d_ctx.lpVtbl.PSSetShader(d_ctx, pshad, 0, 0)
          d_ctx.lpVtbl.PSSetSamplers(d_ctx, 0, 1, &sampler)
          
          // rjf: setup scissor rect
          {
            rect : RECT
            rect.left = 0
            rect.right = (LONG)wnd.last_resolution.x
            rect.top = 0
            rect.bottom = (LONG)wnd.last_resolution.y
            d_ctx.lpVtbl.RSSetScissorRects(d_ctx, 1, &rect)
          }
          
          // rjf: set up uniforms
          uniforms : R_D3D11_Uniforms_Blur
          {
            weights: [len(uniforms.kernel)*2]f32
            
            blur_size := min(params.blur_size, len(weights))
            blur_count := (u64)round_f32(blur_size)
            
            stdev := (blur_size-1.f)/2.f
            one_over_root_2pi_stdev2 := 1/sqrt_f32(2*pi32*stdev*stdev)
            euler32 := 2.718281828459045f
            
            weights[0] = 1.f
            if stdev > 0.f {
              for idx := 0; idx < blur_count; idx += 1 {
                kernel_x := (F32)idx
                weights[idx] = one_over_root_2pi_stdev2*pow_f32(euler32, -kernel_x*kernel_x/(2.f*stdev*stdev));
              }
            }
            if weights[0] > 1.f {
              MemoryZeroArray(weights)
              weights[0] = 1.f
            } else {
              // prepare weights & offsets for bilinear lookup
              // blur filter wants to calculate w0*pixel[pos] + w1*pixel[pos+1] + ...
              // with bilinear filter we can do this calulation by doing only w*sample(pos+t) = w*((1-t)*pixel[pos] + t*pixel[pos+1])
              // we can see w0=w*(1-t) and w1=w*t
              // thus w=w0+w1 and t=w1/w
              for idx := 1; idx < blur_count; idx += 2 {
                w0 := weights[idx + 0]
                w1 := weights[idx + 1]
                w := w0 + w1
                t := w1 / w
                
                // each kernel element is float2(weight, offset)
                // weights & offsets are adjusted for bilinear sampling
                // zw elements are used: not, a bit of waste but it allows for simpler shader code
                uniforms.kernel[(idx+1)/2] = v4f32(w, (F32)idx + t, 0, 0)
              }
            }
            uniforms.kernel[0].x = weights[0]
            
            // technically we need just direction be different
            // but there are 256 bytes of usable space anyway for each constant buffer chunk
            
            uniforms.passes[Axis2_X].viewport_size = v2f32(resolution.x, resolution.y)
            uniforms.passes[Axis2_X].rect          = params.rect
            uniforms.passes[Axis2_X].direction     = v2f32(1.f / resolution.x, 0)
            uniforms.passes[Axis2_X].blur_count    = 1 + blur_count / 2 // 2x smaller because of bilinear sampling
            MemoryCopyArray(uniforms.passes[Axis2_X].corner_radii.v, params.corner_radii)
            
            uniforms.passes[Axis2_Y].viewport_size = v2f32(resolution.x, resolution.y)
            uniforms.passes[Axis2_Y].rect          = params.rect
            uniforms.passes[Axis2_Y].direction     = v2f32(0, 1.f / resolution.y)
            uniforms.passes[Axis2_Y].blur_count    = 1 + blur_count / 2 // 2x smaller because of bilinear sampling
            MemoryCopyArray(uniforms.passes[Axis2_Y].corner_radii.v, params.corner_radii)
            
            sub_rsrc := MAPPED_SUBRESOURCE{}
            d_ctx.lpVtbl.Map(d_ctx, (^IResource)uniforms_buffer, 0, MAP.WRITE_DISCARD, 0, &sub_rsrc)
            MemoryCopy((^u8)sub_rsrc.pData, &uniforms, sizeof(uniforms))
            d_ctx.lpVtbl.Unmap(d_ctx, (^ID3D11Resource)uniforms_buffer, 0)
          }
          
          uniforms_buffers := [?]^IBuffer { uniforms_buffer, uniforms_buffer }
          
          uniform_offset := [2][Axis2_COUNT]u32 {
            { 0 * sizeof(R_D3D11_Uniforms_BlurPass) / 16, (u32)OffsetOf(R_D3D11_Uniforms_Blur, kernel) / 16 },
            { 1 * sizeof(R_D3D11_Uniforms_BlurPass) / 16, (u32)OffsetOf(R_D3D11_Uniforms_Blur, kernel) / 16 },
          };
          
          uniform_count :=  := [2][Axis2_COUNT]u32 {
            { sizeof(R_D3D11_Uniforms_BlurPass) / 16, sizeof(uniforms.kernel) / 16 },
            { sizeof(R_D3D11_Uniforms_BlurPass) / 16, sizeof(uniforms.kernel) / 16 },
          }
          
          // rjf: setup scissor rect
          {
            clip := params.clip
            rect := RECT{}
            {
              if clip.x0 == 0 && clip.y0 == 0 && clip.x1 == 0 && clip.y1 == 0 {
                rect.left = 0
                rect.right = (LONG)wnd.last_resolution.x
                rect.top = 0
                rect.bottom = (LONG)wnd.last_resolution.y
              } else if clip.x0 > clip.x1 || clip.y0 > clip.y1 {
                rect.left = 0
                rect.right = 0
                rect.top = 0
                rect.bottom = 0
              } else {
                rect.left = (LONG)clip.x0
                rect.right = (LONG)clip.x1
                rect.top = (LONG)clip.y0
                rect.bottom = (LONG)clip.y1
              }
            }
            d_ctx.lpVtbl.RSSetScissorRects(d_ctx, 1, &rect)
          }
          
          // rjf: for unsetting srv
          srv : ^IShaderResourceView
          
          // horizontal pass
          d_ctx.lpVtbl.OMSetRenderTargets(d_ctx, 1, &wnd.stage_scratch_color_rtv, 0)
          d_ctx.lpVtbl.PSSetConstantBuffers1(d_ctx, 0, len(uniforms_buffers), uniforms_buffers, uniform_offset[Axis2_X], uniform_count[Axis2_X])
          d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &wnd.stage_color_srv)
          d_ctx.lpVtbl.Draw(d_ctx, 4, 0)
          d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &srv)
          
          // vertical pass
          d_ctx.lpVtbl.OMSetRenderTargets(d_ctx, 1, &wnd.stage_color_rtv, 0)
          d_ctx.lpVtbl.PSSetConstantBuffers1(d_ctx, 0, len(uniforms_buffers), uniforms_buffers, uniform_offset[Axis2_Y], uniform_count[Axis2_Y])
          d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &wnd.stage_scratch_color_srv)
          d_ctx.lpVtbl.Draw(d_ctx, 4, 0)
          d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &srv)
        }
        
        
        ////////////////////////
        //- rjf: 3d geometry rendering pass
        //
        case R_PassKind_Geo3D:
        {
          //- rjf: unpack params
          params := pass.params_geo3d
          mesh_group_map := &params.mesh_batches
          
          //- rjf: set up rasterizer
          viewport_dim := dim_2f32(params.viewport)
          viewport := VIEWPORT { params.viewport.x0, params.viewport.y0, viewport_dim.x, viewport_dim.y, 0.f, 1.f }
          d_ctx.lpVtbl.RSSetViewports(d_ctx, 1, &viewport)
          d_ctx.lpVtbl.RSSetState(d_ctx, (^IRasterizerState)r_d3d11_state.main_rasterizer)
          
          //- rjf: clear render targets
          {
            bg_color := v4f32(0, 0, 0, 0)
            d_ctx.lpVtbl.ClearRenderTargetView(d_ctx, wnd.geo3d_color_rtv, bg_color.v)
            d_ctx.lpVtbl.ClearDepthStencilView(d_ctx, wnd.geo3d_depth_dsv, CLEAR_FLAG.DEPTH, 1.f, 0)
          }
          
          //- rjf: draw mesh batches
          {
            // rjf: grab pipeline info
            vshad     = r_d3d11_state.vshads[R_D3D11_VShadKind_Mesh]
            ilay       = r_d3d11_state.ilays[R_D3D11_VShadKind_Mesh]
            pshad      = r_d3d11_state.pshads[R_D3D11_PShadKind_Mesh]
            uniforms_buffer = r_d3d11_state.uniform_type_kind_buffers[R_D3D11_VShadKind_Mesh]
            
            // rjf: setup output merger
            d_ctx.lpVtbl.OMSetRenderTargets(d_ctx, 1, &wnd.geo3d_color_rtv, wnd.geo3d_depth_dsv)
            d_ctx.lpVtbl.OMSetDepthStencilState(d_ctx, r_d3d11_state.plain_depth_stencil, 0)
            d_ctx.lpVtbl.OMSetBlendState(d_ctx, r_d3d11_state.main_blend_state, 0, 0xffffffff)
            
            // rjf: draw all batches
            for slot_idx := 0; slot_idx < mesh_group_map.slots_count; slot_idx += 1 {
              for n := mesh_group_map.slots[slot_idx]; n != nil; n = n.next {
                // rjf: unpack group params
                batches := &n.batches
                group_params := &n.params
                mesh_vertices := r_d3d11_buffer_from_handle(group_params.mesh_vertices)
                mesh_indices := r_d3d11_buffer_from_handle(group_params.mesh_indices)
                
                // rjf: setup input assembly
                stride := 11 * sizeof(F32)
                offset := 0
                d_ctx.lpVtbl.IASetPrimitiveTopology(d_ctx, PRIMITIVE_TOPOLOGY.TRIANGLELIST)
                d_ctx.lpVtbl.IASetInputLayout(d_ctx, ilay)
                d_ctx.lpVtbl.IASetVertexBuffers(d_ctx, 0, 1, &mesh_vertices.buffer, &stride, &offset)
                d_ctx.lpVtbl.IASetIndexBuffer(d_ctx, mesh_indices.buffer, FORMAT.R32_UINT, 0)
                
                // rjf: setup uniforms buffer
                uniforms : R_D3D11_Uniforms_Mesh
                {
                  uniforms.xform = mul_4x4f32(params.projection, params.view)
                }
                {
                  sub_rsrc := MAPPED_SUBRESOURCE{}
                  d_ctx.lpVtbl.Map(d_ctx, (^IResource)uniforms_buffer, 0, MAP.WRITE_DISCARD, 0, &sub_rsrc)
                  MemoryCopy((^u8)sub_rsrc.pData, &uniforms, sizeof(uniforms))
                  d_ctx.lpVtbl.Unmap(d_ctx, (^IResource)uniforms_buffer, 0)
                }
                
                
                // rjf: setup shaders
                d_ctx.lpVtbl.VSSetShader(d_ctx, vshad, 0, 0)
                d_ctx.lpVtbl.VSSetConstantBuffers(d_ctx, 0, 1, &uniforms_buffer)
                d_ctx.lpVtbl.PSSetShader(d_ctx, pshad, 0, 0)
                d_ctx.lpVtbl.PSSetConstantBuffers(d_ctx, 0, 1, &uniforms_buffer)
                
                // rjf: setup scissor rect
                {
                  rect := RECT{}
                  {
                    rect.left = 0
                    rect.right = (LONG)wnd.last_resolution.x
                    rect.top = 0
                    rect.bottom = (LONG)wnd.last_resolution.y
                  }
                  d_ctx.lpVtbl.RSSetScissorRects(d_ctx, 1, &rect)
                }
                
                // rjf: draw
                d_ctx.lpVtbl.DrawIndexed(d_ctx, mesh_indices.size/sizeof(u32), 0, 0)
              }
            }
          }
          
          //- rjf: composite to main staging buffer
          {
            sampler   := r_d3d11_state.samplers[R_Tex2DSampleKind_Nearest]
            vshad     := r_d3d11_state.vshads[R_D3D11_VShadKind_Geo3DComposite]
            pshad     := r_d3d11_state.pshads[R_D3D11_PShadKind_Geo3DComposite]
            
            // rjf: setup output merger
            d_ctx.lpVtbl.OMSetRenderTargets(d_ctx, 1, &wnd.stage_color_rtv, 0)
            d_ctx.lpVtbl.OMSetDepthStencilState(d_ctx, r_d3d11_state.noop_depth_stencil, 0)
            d_ctx.lpVtbl.OMSetBlendState(d_ctx, r_d3d11_state.main_blend_state, 0, 0xffffffff)
            
            // rjf: set up rasterizer
            resolution := wnd.last_resolution
            viewport := VIEWPORT { 0.0f, 0.0f, (F32)resolution.x, (F32)resolution.y, 0.0f, 1.0f }
            d_ctx.lpVtbl.RSSetViewports(d_ctx, 1, &viewport)
            d_ctx.lpVtbl.RSSetState(d_ctx, (^IRasterizerState)r_d3d11_state.main_rasterizer)
            
            // rjf: setup input assembly
            d_ctx.lpVtbl.IASetPrimitiveTopology(d_ctx, PRIMITIVE_TOPOLOGY.TRIANGLESTRIP)
            d_ctx.lpVtbl.IASetInputLayout(d_ctx, 0)
            
            // rjf: setup shaders
            d_ctx.lpVtbl.VSSetShader(d_ctx, vshad, 0, 0)
            d_ctx.lpVtbl.PSSetShader(d_ctx, pshad, 0, 0)
            d_ctx.lpVtbl.PSSetShaderResources(d_ctx, 0, 1, &wnd.geo3d_color_srv)
            d_ctx.lpVtbl.PSSetSamplers(d_ctx, 0, 1, &sampler)
            
            // rjf: setup scissor rect
            {
              rect := RECT{}
              Rng2F32 clip = params.clip
              if clip.x0 == 0 && clip.y0 == 0 && clip.x1 == 0 && clip.y1 == 0 {
                rect.left = 0
                rect.right = (LONG)wnd.last_resolution.x
                rect.top = 0
                rect.bottom = (LONG)wnd.last_resolution.y
              } else if clip.x0 > clip.x1 || clip.y0 > clip.y1 {
                rect.left = 0
                rect.right = 0
                rect.top = 0
                rect.bottom = 0
              } else {
                rect.left = (LONG)clip.x0
                rect.right = (LONG)clip.x1
                rect.top = (LONG)clip.y0
                rect.bottom = (LONG)clip.y1
              }
              d_ctx.lpVtbl.RSSetScissorRects(d_ctx, 1, &rect)
            }
            
            // rjf: draw
            d_ctx.lpVtbl.Draw(d_ctx, 4, 0);
          }
        }

        case: {}
      }
    }
  }
  ProfEnd()
}

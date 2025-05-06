// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#include <combaseapi.h>
#include <dcommon.h>
#include <initguid.h>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

////////////////////////////////
//~ rjf: Generated Code

#include "generated/render_d3d11.meta.h"

////////////////////////////////
//~ rjf: C-side Shader Types

public struct R_D3D11_Uniforms_Rect
{
  Vec2F32 viewport_size;
  float opacity;
  float _padding0_;
  Vec4F32[4] texture_sample_channel_map;
  Vec2F32 texture_t2d_size;
  Vec2F32 translate;
  Vec4F32[3] xform;
  Vec2F32 xform_scale;
}

public struct R_D3D11_Uniforms_BlurPass
{
  Rng2F32 rect;
  Vec4F32 corner_radii;
  Vec2F32 direction;
  Vec2F32 viewport_size;
  uint32 blur_count;
  uint8[204] _padding0_;
}
StaticAssert(sizeof(R_D3D11_Uniforms_BlurPass) % 256 == 0, NotAligned); // constant count/offset must be aligned to 256 bytes

public struct R_D3D11_Uniforms_Blur
{
  R_D3D11_Uniforms_BlurPass[Axis2_COUNT] passes;
  Vec4F32[32] kernel;
}

public struct R_D3D11_Uniforms_Mesh
{
  Mat4x4F32 xform;
}

////////////////////////////////
//~ rjf: Main State Types

public struct R_D3D11_Tex2D
{
  R_D3D11_Tex2D* next;
  uint64 generation;
  ID3D11Texture2D* texture;
  ID3D11ShaderResourceView* view;
  R_ResourceKind kind;
  Vec2S32 size;
  R_Tex2DFormat format;
}

public struct R_D3D11_Buffer
{
  R_D3D11_Buffer* next;
  uint64 generation;
  ID3D11Buffer* buffer;
  R_ResourceKind kind;
  uint64 size;
}

public struct R_D3D11_Window
{
  R_D3D11_Window* next;
  uint64 generation;
  
  // rjf: swapchain/framebuffer
  IDXGISwapChain1*        swapchain;
  ID3D11Texture2D*        framebuffer;
  ID3D11RenderTargetView* framebuffer_rtv;
  
  // rjf: staging buffer
  ID3D11Texture2D* stage_color;
  ID3D11RenderTargetView* stage_color_rtv;
  ID3D11ShaderResourceView* stage_color_srv;
  ID3D11Texture2D* stage_scratch_color;
  ID3D11RenderTargetView* stage_scratch_color_rtv;
  ID3D11ShaderResourceView* stage_scratch_color_srv;
  
  // rjf: geo3d buffer
  ID3D11Texture2D* geo3d_color;
  ID3D11RenderTargetView* geo3d_color_rtv;
  ID3D11ShaderResourceView* geo3d_color_srv;
  ID3D11Texture2D* geo3d_depth;
  ID3D11DepthStencilView* geo3d_depth_dsv;
  ID3D11ShaderResourceView* geo3d_depth_srv;
  
  // rjf: last state
  Vec2S32 last_resolution;
}

public struct R_D3D11_FlushBuffer
{
   R_D3D11_FlushBuffer* next;
   ID3D11Buffer* buffer;
}

public struct R_D3D11_State
{
  // rjf: state
  Arena* arena;
  R_D3D11_Window* first_free_window;
  R_D3D11_Tex2D* first_free_tex2d;
  R_D3D11_Buffer* first_free_buffer;
  R_D3D11_Tex2D* first_to_free_tex2d;
  R_D3D11_Buffer* first_to_free_buffer;
  OS_Handle device_rw_mutex;
  
  // rjf: base d3d11 objects
  ID3D11Device*            base_device;
  ID3D11DeviceContext*     base_device_ctx;
  ID3D11Device1*           device;
  ID3D11DeviceContext1*    device_ctx;
  IDXGIDevice1*            dxgi_device;
  IDXGIAdapter*            dxgi_adapter;
  IDXGIFactory2*           dxgi_factory;
  ID3D11RasterizerState1*  main_rasterizer;
  ID3D11BlendState*        main_blend_state;
  ID3D11BlendState*        no_blend_state;
  ID3D11SamplerState*[R_Tex2DSampleKind_COUNT]      samplers;
  ID3D11DepthStencilState* noop_depth_stencil;
  ID3D11DepthStencilState* plain_depth_stencil;
  ID3D11Buffer*            instance_scratch_buffer_64kb;
  
  // rjf: backups
  R_Handle backup_texture;
  
  // rjf: vertex shaders
  ID3D11VertexShader*[R_D3D11_VShadKind_COUNT] vshads;
  ID3D11InputLayout*[R_D3D11_VShadKind_COUNT] ilays;
  ID3D11PixelShader*[R_D3D11_PShadKind_COUNT] pshads;
  ID3D11Buffer*[R_D3D11_UniformTypeKind_COUNT] uniform_type_kind_buffers;
  
  // rjf: buffers to flush at subsequent frame
  Arena* buffer_flush_arena;
  R_D3D11_FlushBuffer* first_buffer_to_flush;
  R_D3D11_FlushBuffer* last_buffer_to_flush;
}

////////////////////////////////
//~ rjf: Globals

public static R_D3D11_State* r_d3d11_state = null;
public static R_D3D11_Window r_d3d11_window_nil = {&r_d3d11_window_nil};
public static R_D3D11_Tex2D r_d3d11_tex2d_nil = {&r_d3d11_tex2d_nil};
public static R_D3D11_Buffer r_d3d11_buffer_nil = {&r_d3d11_buffer_nil};

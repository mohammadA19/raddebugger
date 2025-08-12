// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


#define glTexImage3D glTexImage3D__static
#define glTexSubImage3D glTexSubImage3D__static
#define glActiveTexture glActiveTexture__static
#include <GL/gl.h>
#include <EGL/egl.h>
#undef glTexImage3D
#undef glTexSubImage3D
#undef glActiveTexture

struct R_OGL_LNX_Window
{
  R_OGL_LNX_Window *next;
  EGLSurface *surface;
};

struct R_OGL_LNX_State
{
  Arena *arena;
  EGLDisplay *display;
  EGLConfig config;
  EGLContext *context;
  R_OGL_LNX_Window *free_window;
};

public static R_OGL_LNX_State *r_ogl_lnx_state = 0;


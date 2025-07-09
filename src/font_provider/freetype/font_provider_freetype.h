// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef FONT_PROVIDER_FREETYPE_H
#define FONT_PROVIDER_FREETYPE_H

////////////////////////////////
//~ rjf: Freetype Includes

#undef internal
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#define internal static

////////////////////////////////
//~ rjf: State Types

typedef struct FP_FT_Font FP_FT_Font;
struct FP_FT_Font
{
  FT_Face face;
};

typedef struct FP_FT_State FP_FT_State;
struct FP_FT_State
{
  Arena *arena;
  FT_Library library;
};

////////////////////////////////
//~ rjf: Globals

global FP_FT_State *fp_ft_state = 0;

////////////////////////////////
//~ rjf: Helpers




#endif // FONT_PROVIDER_FREETYPE_H

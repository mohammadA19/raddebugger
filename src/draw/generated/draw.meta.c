// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- GENERATED CODE

R_Tex2DSampleKind          dr_push_tex2d_sample_kind(R_Tex2DSampleKind v) {DR_StackPushImpl(Tex2DSampleKind, tex2d_sample_kind, R_Tex2DSampleKind, v);}
Mat3x3F32                  dr_push_xform2d(Mat3x3F32 v) {DR_StackPushImpl(XForm2D, xform2d, Mat3x3F32, v);}
Rng2F32                    dr_push_clip(Rng2F32 v) {DR_StackPushImpl(Clip, clip, Rng2F32, v);}
F32                        dr_push_transparency(F32 v) {DR_StackPushImpl(Transparency, transparency, F32, v);}
R_Tex2DSampleKind          dr_pop_tex2d_sample_kind() {DR_StackPopImpl(Tex2DSampleKind, tex2d_sample_kind, R_Tex2DSampleKind);}
Mat3x3F32                  dr_pop_xform2d() {DR_StackPopImpl(XForm2D, xform2d, Mat3x3F32);}
Rng2F32                    dr_pop_clip() {DR_StackPopImpl(Clip, clip, Rng2F32);}
F32                        dr_pop_transparency() {DR_StackPopImpl(Transparency, transparency, F32);}
R_Tex2DSampleKind          dr_top_tex2d_sample_kind() {DR_StackTopImpl(Tex2DSampleKind, tex2d_sample_kind, R_Tex2DSampleKind);}
Mat3x3F32                  dr_top_xform2d() {DR_StackTopImpl(XForm2D, xform2d, Mat3x3F32);}
Rng2F32                    dr_top_clip() {DR_StackTopImpl(Clip, clip, Rng2F32);}
F32                        dr_top_transparency() {DR_StackTopImpl(Transparency, transparency, F32);}

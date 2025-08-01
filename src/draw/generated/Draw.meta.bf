
public struct DR_Tex2DSampleKindNode
{
    public DR_Tex2DSampleKindNode* next;
    public R_Tex2DSampleKind v;
}

public struct DR_XForm2DNode
{
    public DR_XForm2DNode* next;
    public Mat3x3F32 v;
}

public struct DR_ClipNode
{
    public DR_ClipNode* next;
    public Rng2F32 v;
}

public struct DR_TransparencyNode
{
    public DR_TransparencyNode* next;
    public F32 v;
}

struct DR_BucketStackDecls
{
    public DR_Tex2DSampleKindNode* top_tex2d_sample_kind;
    public DR_XForm2DNode* top_xform2d;
    public DR_ClipNode* top_clip;
    public DR_TransparencyNode* top_transparency;
}

public readonly static DR_Tex2DSampleKindNode dr_nil_tex2d_sample_kind = .(0, R_Tex2DSampleKind_Nearest);
public readonly static DR_XForm2DNode dr_nil_xform2d = .(0, .(1, 0, 0, 0, 1, 0, 0, 0, 1));
public readonly static DR_ClipNode dr_nil_clip = .(0, (0));
public readonly static DR_TransparencyNode dr_nil_transparency = .(0, 0);

public static mixin DR_BucketStackInits(var b)
{
    b.top_tex2d_sample_kind = &dr_nil_tex2d_sample_kind;
    b.top_xform2d = &dr_nil_xform2d;
    b.top_clip = &dr_nil_clip;
    b.top_transparency = &dr_nil_transparency;
}

public static R_Tex2DSampleKind dr_push_tex2d_sample_kind(R_Tex2DSampleKind v)
{
    DR_StackPushImpl!((bucket) => &bucket.top_tex2d_sample_kind, v);
}

public static Mat3x3F32 dr_push_xform2d(Mat3x3F32 v)
{
    DR_StackPushImpl!((bucket) => &bucket.top_xform2d, v);
}

public static Rng2F32 dr_push_clip(Rng2F32 v)
{
    DR_StackPushImpl!((bucket) => &bucket.top_clip, v);
}

public static F32 dr_push_transparency(F32 v)
{
    DR_StackPushImpl!((bucket) => &bucket.top_transparency, v);
}

public static R_Tex2DSampleKind dr_pop_tex2d_sample_kind()
{
    DR_StackPopImpl!((bucket) => &bucket.top_tex2d_sample_kind);
}

public static Mat3x3F32 dr_pop_xform2d()
{
    DR_StackPopImpl!((bucket) => &bucket.top_xform2d);
}

public static Rng2F32 dr_pop_clip()
{
    DR_StackPopImpl!((bucket) => &bucket.top_clip);
}

public static F32 dr_pop_transparency()
{
    DR_StackPopImpl!((bucket) => &bucket.top_transparency);
}

public static R_Tex2DSampleKind dr_top_tex2d_sample_kind()
{
    DR_StackTopImpl!((bucket) => &bucket.top_tex2d_sample_kind);
}

public static Mat3x3F32 dr_top_xform2d()
{
    DR_StackTopImpl!((bucket) => &bucket.top_xform2d);
}

public static Rng2F32 dr_top_clip()
{
    DR_StackTopImpl!((bucket) => &bucket.top_clip);
}

public static F32 dr_top_transparency()
{
    DR_StackTopImpl!((bucket) => &bucket.top_transparency);
}

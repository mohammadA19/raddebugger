
public static class DR
{
    struct ListNode<T>
    {
        ListNode<T>* next;
        T v;
    }

    struct List<T> : IEnumerator<T*>
    {
        ListNode<T>* first;
        ListNode<T>* last;
        uint count;

        public ListEnumerable<T> GetEnumerator() => .(&this);
    }

    public struct ListEnumerable<T> : IEnumerator<T*>
    {
        ListNode<T>* current;

        public this(List<T>* list)
        {
            this.current = list.first;
        }

        public Result<FStrNode*> GetNext()
        {
            let result = current;
            if (result != null)
            {
                current = current.next;
                return result;
            }
            else
            {
                return .Err;
            }
        }
    }

    ////////////////////////////////
    //~ rjf: Fancy String Types

    public struct FStrParams
    {
        public FNT.Tag font;
        public FNT.RasterFlags raster_flags;
        public Vec4F32 color;
        public float size;
        public float underline_thickness;
        public float strikethrough_thickness;
    }

    public struct FStr
    {
        public String8 string;
        public FStrParams params;
    }

    public struct FStrNode : ListNode<FStr>
    {
        // public FStrNode* next;
        // public FStr v;
    }

    public struct FStrList : List<FStr> // : IEnumerable<FStrNode*>
    {
        // public FStrNode* first;
        // public FStrNode* last;
        // public uint64 node_count;
        public uint64 total_size;

        //public struct Iterator : IEnumerator<FStrNode*>
        //{
        //    FStrNode* current;
//
        //    public this(FStrList* list)
        //    {
        //        this.current = list.first;
        //    }
//
        //    public Result<FStrNode*> GetNext()
        //    {
        //        let result = current;
        //        if (result != null)
        //        {
        //            current = current.next;
        //            return result;
        //        }
        //        else
        //        {
        //            return .Err;
        //        }
        //    }
        //}

        // public Iterator GetEnumerator() => Iterator(&this);

        public void Push(Arena arena, FStr* str) mut
        {
            FStrNode* n = new:arena FStrNode(?);
            MemoryCopyStruct(&n.v, str);
            SLLQueuePush(first, last, n);
            node_count += 1;
            total_size += str.string.size;
        }

        public void PushNew_(Arena arena, FStrParams* params, FStrParams* overrides, String8 string)
        {
            FStr fstr = {string, *params};
            if (overrides.font != default)
            {
                fstr.params.font = overrides.font;
            }
            if (overrides.raster_flags != 0)
            {
                fstr.params.raster_flags = overrides.raster_flags;
            }
            if (overrides.color.x != 0 || overrides.color.y != 0 || overrides.color.z != 0 || overrides.color.w != 0)
            {
                fstr.params.color = overrides.color;
            }
            if (overrides.size != 0)
            {
                fstr.params.size = overrides.size;
            }
            if (overrides.underline_thickness != 0)
            {
                fstr.params.underline_thickness = overrides.underline_thickness;
            }
            if (overrides.strikethrough_thickness != 0)
            {
                fstr.params.strikethrough_thickness = overrides.strikethrough_thickness;
            }
            Push(arena, &fstr);
        }

        public void ConcatInPlace(FStrList* to_push) mut
        {
            if (last != null && to_push.first != null)
            {
                last.next = to_push.first;
                last = to_push.last;
                node_count += to_push.node_count;
                total_size += to_push.total_size;
            }
            else if(to_push.first != null)
            {
                MemoryCopyStruct(&this, to_push);
            }
            MemoryZeroStruct(to_push);
        }

        public static FStrList Copy(Arena arena, FStrList* src)
        {
            FStrList dst = default;
            // for (FStrNode* src_n = src.first; src_n != 0; src_n = src_n.next)
            for (FStrNode* src_n in src)
            {
                FStr fstr = src_n.v;
                fstr.string = PushStr8Copy(arena, fstr.string);
                dst.Push(arena, &fstr);
            }
            return dst;
        }

        public StringView MakeString(Arena arena)
        {
            String8 result = default;
            {
                Temp scratch = ScratchBegin(&arena, 1);
                defer ScratchEnd(scratch);

                String8List parts = default;
                //for (FStrNode* n = first; n != null; n = n.next)
                for (FStrNode* n in this)
                {
                    if (n.v.params.font != thread_ctx.icon_font)
                    {
                        Str8ListPush(scratch.arena, &parts, n.v.string);
                    }
                }
                result = Str8ListJoin(arena, &parts, 0);
                result = Str8SkipChopWhitespace(result);
            }
            return result;
        }

        public FuzzyMatchRangeList FuzzyMatchFind(Arena arena, StringView needle)
        {
            Temp scratch = ScratchBegin(&arena, 1);
            defer ScratchEnd(scratch);

            char8[] fstrs_string = new:arena char8[total_size];
            {
                // TODO(rjf): the fact that we only increment on non-icon portions is super weird?
                // we are only doing that because of the rendering of the fuzzy matches, so maybe
                // once that is straightened out, we can fix the code here too...
                uint64 off = 0;
                //for (FStrNode* n = first; n != null; n = n.next)
                for (FStrNode* n in this)
                {
                    if (n.v.params.font != thread_ctx.icon_font)
                    {
                        MemoryCopy(&fstrs_string[off], n.v.string.str, n.v.string.size);
                        off += n.v.string.size;
                    }
                }
            }
            FuzzyMatchRangeList ranges = FuzzyMatchFind(arena, needle, fstrs_string);
            return ranges;
        }

        public FRunList MakeFRuns(Arena arena, float tab_size_px)
        {
            ProfBeginFunction();
            FRunList run_list = default;
            float base_align_px = 0;
            //for (FStrNode* n = first; n != null; n = n.next)
            for (FStrNode* n in this)
            {
                FRunNode* dst_n = new:arena FRunNode();
                dst_n.v = .() {
                    run: FNT.RunFromString(n.v.params.font, n.v.params.size, base_align_px, tab_size_px, n.v.params.raster_flags, n.v.string),
                    color: n.v.params.color,
                    underline_thickness: n.v.params.underline_thickness,
                    strikethrough_thickness: n.v.params.strikethrough_thickness,
                    icon: n.v.params.font == thread_ctx.icon_font,
                };
                SLLQueuePush(run_list.first, run_list.last, dst_n);
                run_list.node_count += 1;
                run_list.dim.x += dst_n.v.run.dim.x;
                run_list.dim.y = Math.Max(run_list.dim.y, dst_n.v.run.dim.y);
                base_align_px += dst_n.v.run.dim.x;
            }
            ProfEnd();
            return run_list;
        }

        public Vec2F32 GetDim(float tab_size_px)
        {
            Temp scratch = ScratchBegin(0, 0);
            defer ScratchEnd(scratch);

            FRunList fruns = MakeFRuns(scratch.arena, tab_size_px);
            Vec2F32 dim = fruns.dim;
            return dim;
        }
    }

    public struct FRun
    {
        public FNT.Run run;
        public Vec4F32 color;
        public float underline_thickness;
        public float strikethrough_thickness;
        public bool icon;
    }

    public struct FRunNode : ListNode<FRun>
    {
        // public FRunNode* next;
        // public FRun v;
    }

    public struct FRunList : List<FRun>
    {
        // public FRunNode* first;
        // public FRunNode* last;
        // public uint64 node_count;
        public Vec2F32 dim;
    }

    ////////////////////////////////
    //~ rjf: Draw Bucket Types

    public class Bucket
    {
        public R.PassList passes;
        public uint64 stack_gen;
        public uint64 last_cmd_stack_gen;
        public using BucketStackDecls decls;
    }

    ////////////////////////////////
    //~ rjf: Thread Context

    public struct BucketSelectionNode
    {
        public BucketSelectionNode* next;
        public Bucket bucket;
    }

    public struct ThreadCtx
    {
        public Arena arena;
        public uint64 arena_frame_start_pos;
        public FNT.Tag icon_font;
        public BucketSelectionNode* TopBucket;
        public BucketSelectionNode* free_bucket_selection;
    }

    ////////////////////////////////
    //~ rjf: Globals

    [ThreadStatic]
    public static ThreadCtx* thread_ctx = null;

    ////////////////////////////////
    //~ rjf: Fancy String Type Functions

    #define fstrs_push_new(arena, list, params, string, ...) list.PushNew_((arena), (params), &(FStrParams){.size = 0, __VA_ARGS__}, (string))

    ////////////////////////////////
    //~ rjf: Bucket Construction & Selection API
    //
    // (Bucket: Handle to sequence of many render passes, constructed by this layer)

    #define BucketScope(b) DeferLoop(push_bucket(b), pop_bucket())


    #define Tex2DSampleKindScope(v)   public DeferLoop(push_tex2d_sample_kind(v), pop_tex2d_sample_kind())
    #define XForm2DScope(v)           public DeferLoop(push_xform2d(v), pop_xform2d())
    #define ClipScope(v)              public DeferLoop(push_clip(v), pop_clip())
    #define TransparencyScope(v)      public DeferLoop(push_transparency(v), pop_transparency())


    ////////////////////////////////
    //~ rjf: Generated Code

    public static mixin StackPushImpl<TNode>(function TNode*(Bucket) member, var val)
    {
        Bucket bucket = TopBucket();
        var old_val = member().v;
        TNode* node = new:(thread_ctx.arena) TNode();
        node.v = val;
        SLLStackPush(member(), node);
        bucket.stack_gen += 1;
        return old_val;
    }

    public static mixin StackPopImpl<TNode>(function TNode*(Bucket) member)
    {
        Bucket bucket = TopBucket();
        var popped_val = member().v;
        SLLStackPop(member());
        bucket.stack_gen += 1;
        return popped_val;
    }

    public static mixin StackTopImpl<TNode>(function TNode*(Bucket) member)
    {
        Bucket bucket = TopBucket();
        var top_val = member().v;
        return top_val;
    }

    ////////////////////////////////
    //~ rjf: Basic Helpers

    public static uint64 HashFromString(StringView str)
    {
        uint64 result = 5381;
        for (uint8 c in str)
        {
            result = ((result << 5) + result) + c;
        }
        return result;
    }

    ////////////////////////////////
    //~ rjf: Top-Level API
    //
    // (Frame boundaries)

    public static void BeginFrame(FNT.Tag icon_font)
    {
        if (thread_ctx == null)
        {
            Arena arena = ArenaAlloc(.reserve_size = GB(64), .commit_size = MB(8));
            thread_ctx = new:arena ThreadCtx();
            thread_ctx.arena = arena;
            thread_ctx.arena_frame_start_pos = ArenaPos(arena);
        }
        ArenaPopTo(thread_ctx.arena, thread_ctx.arena_frame_start_pos);
        thread_ctx.free_bucket_selection = 0;
        thread_ctx.TopBucket = 0;
        thread_ctx.icon_font = icon_font;
    }

    public static void SubmitBucket(OS.Handle os_window, R.Handle r_window, Bucket bucket)
    {
        R.WindowSubmit(os_window, r_window, &bucket.passes);
    }

    ////////////////////////////////
    //~ rjf: Bucket Construction & Selection API
    //
    // (Bucket: Handle to sequence of many render passes, constructed by this layer)

    public static Bucket BucketMake()
    {
        Bucket bucket = new:(thread_ctx.arena) Bucket();
        BucketStackInits(bucket);
        return bucket;
    }

    public static void PushBucket(Bucket bucket)
    {
        BucketSelectionNode* node = thread_ctx.free_bucket_selection;
        if (node != null)
        {
            SLLStackPop(thread_ctx.free_bucket_selection);
        }
        else
        {
            node = new:(thread_ctx.arena) BucketSelectionNode();
        }
        SLLStackPush(thread_ctx.TopBucket, node);
        node.bucket = bucket;
    }

    public static void PopBucket()
    {
        BucketSelectionNode* node = thread_ctx.TopBucket;
        SLLStackPop(thread_ctx.TopBucket);
        SLLStackPush(thread_ctx.free_bucket_selection, node);
    }

    public static Bucket TopBucket()
    {
        Bucket bucket = null;
        if (thread_ctx.TopBucket != null)
        {
            bucket = thread_ctx.TopBucket.bucket;
        }
        return bucket;
    }

    ////////////////////////////////
    //~ rjf: Bucket Stacks
    //
    // (Pushing/popping implicit draw parameters)

    // NOTE(rjf): (The implementation of the push/pop/top functions is auto-generated)

    ////////////////////////////////
    //~ rjf: Draw Calls
    //
    // (Apply to the calling thread's currently selected bucket)

    //- rjf: rectangles

    [Inline]
    public static R.Rect2DInst* Rect(Rng2F32 dst, Vec4F32 color, float corner_radius, float border_thickness, float edge_softness)
    {
        Arena arena = thread_ctx.arena;
        Bucket bucket = TopBucket();
        R.Pass* pass = R.PassFromLind(arena, &bucket.passes, R.PassKind.UI);
        R.PassParams_UI* params = pass.params_ui;
        R.BatchGroup2DList* rects = &params.rects;
        R.BatchGroup2DNode* node = rects.last;
        if (node == null || bucket.stack_gen != bucket.last_cmd_stack_gen)
        {
            node = new:arena R.BatchGroup2DNode();
            SLLQueuePush(rects.first, rects.last, node);
            rects.count += 1;
            node.batches = R.BatchListMake(sizeof(R.Rect2DInst));
            node.params.tex = R.HandleZero();
            node.params.tex_sample_kind = bucket.top_tex2d_sample_kind.v;
            node.params.xform           = bucket.top_xform2d.v;
            node.params.clip            = bucket.top_clip.v;
            node.params.transparency    = bucket.top_transparency.v;
        }
        R.Rect2DInst* inst = (R.Rect2DInst*)R.BatchListPushInst(arena, &node.batches, 256);
        inst.dst = dst;
        inst.src = r2f32p(0, 0, 0, 0);
        inst.colors[Corner_00] = color;
        inst.colors[Corner_01] = color;
        inst.colors[Corner_10] = color;
        inst.colors[Corner_11] = color;
        inst.corner_radii[Corner_00] = corner_radius;
        inst.corner_radii[Corner_01] = corner_radius;
        inst.corner_radii[Corner_10] = corner_radius;
        inst.corner_radii[Corner_11] = corner_radius;
        inst.border_thickness = border_thickness;
        inst.edge_softness = edge_softness;
        inst.white_texture_override = 1.f;
        bucket.last_cmd_stack_gen = bucket.stack_gen;
        return inst;
    }

    //- rjf: images

    [Inline]
    public static R.Rect2DInst* Img(Rng2F32 dst, Rng2F32 src, R.Handle texture, Vec4F32 color, float corner_radius, float border_thickness, float edge_softness)
    {
        Arena arena = thread_ctx.arena;
        Bucket bucket = TopBucket();
        R.Pass* pass = R.PassFromKind(arena, &bucket.passes, R.PassKind.UI);
        R.PassParams_UI* params = pass.params_ui;
        R.BatchGroup2DList* rects = &params.rects;
        R.BatchGroup2DNode* node = rects.last;
        if (node != null && bucket.stack_gen == bucket.last_cmd_stack_gen && R.HandleMatch(node.params.tex, R.HandleZero()))
        {
            node.params.tex = texture;
        }
        else if(node == null || bucket.stack_gen != bucket.last_cmd_stack_gen || !R.HandleMatch(texture, node.params.tex))
        {
            node = new:arena R.BatchGroup2DNode();
            SLLQueuePush(rects.first, rects.last, node);
            rects.count += 1;
            node.batches = R.BatchListMake(sizeof(R.Rect2DInst));
            node.params.tex             = texture;
            node.params.tex_sample_kind = bucket.top_tex2d_sample_kind.v;
            node.params.xform           = bucket.top_xform2d.v;
            node.params.clip            = bucket.top_clip.v;
            node.params.transparency    = bucket.top_transparency.v;
        }
        R.Rect2DInst* inst = (R.Rect2DInst*)R.BatchListPushInst(arena, &node.batches, 256);
        inst.dst = dst;
        inst.src = src;
        inst.colors[Corner_00] = color;
        inst.colors[Corner_01] = color;
        inst.colors[Corner_10] = color;
        inst.colors[Corner_11] = color;
        inst.corner_radii[Corner_00] = corner_radius;
        inst.corner_radii[Corner_01] = corner_radius;
        inst.corner_radii[Corner_10] = corner_radius;
        inst.corner_radii[Corner_11] = corner_radius;
        inst.border_thickness = border_thickness;
        inst.edge_softness = edge_softness;
        inst.white_texture_override = 0.f;
        bucket.last_cmd_stack_gen = bucket.stack_gen;
        return inst;
    }

    //- rjf: blurs

    public static R.PassParams_Blur* Blur(Rng2F32 rect, float blur_size, float corner_radius)
    {
        Arena arena = thread_ctx.arena;
        Bucket bucket = TopBucket();
        R.Pass* pass = R.PassFromKind(arena, &bucket.passes, R.PassKind_Blur);
        R.PassParams_Blur* params = pass.params_blur;
        params.rect = rect;
        params.clip = TopClip();
        params.blur_size = blur_size;
        params.corner_radii[Corner_00] = corner_radius;
        params.corner_radii[Corner_01] = corner_radius;
        params.corner_radii[Corner_10] = corner_radius;
        params.corner_radii[Corner_11] = corner_radius;
        return params;
    }

    //- rjf: 3d rendering pass params

    public static R.PassParams_Geo3D * Geo3DBegin(Rng2F32 viewport, Mat4x4F32 view, Mat4x4F32 projection)
    {
        Arena arena = thread_ctx.arena;
        Bucket bucket = TopBucket();
        R.Pass* pass = R.PassFromKind(arena, &bucket.passes, R.PassKind_Geo3D);
        R.PassParams_Geo3D* params = pass.params_geo3d;
        params.viewport = viewport;
        params.view = view;
        params.projection = projection;
        return params;
    }

    //- rjf: meshes

    public static R.Mesh3DInst* Mesh(R.Handle mesh_vertices, R.Handle mesh_indices, R.GeoTopologyKind mesh_geo_topology, R.GeoVertexFlags mesh_geo_vertex_flags, R.Handle albedo_tex, Mat4x4F32 inst_xform)
    {
        Arena arena = thread_ctx.arena;
        Bucket bucket = TopBucket();
        R.Pass* pass = R.PassFromKind(arena, &bucket.passes, R.PassKind_Geo3D);
        R.PassParams_Geo3D* params = pass.params_geo3d;
        
        // rjf: mesh batch map not made yet . make
        if (params.mesh_batches.slots_count == 0)
        {
            params.mesh_batches.slots_count = 64;
            params.mesh_batches.slots = new:arena R.BatchGroup3DMapNode*[params.mesh_batches.slots_count]*;
        }
        
        // rjf: hash batch group 3d params
        uint64 hash = 0;
        uint64 slot_idx = 0;
        {
            uint64[?] buffer =
            (
                mesh_vertices.uint64[0],
                mesh_vertices.uint64[1],
                mesh_indices.uint64[0],
                mesh_indices.uint64[1],
                (uint64)mesh_geo_topology,
                (uint64)mesh_geo_vertex_flags,
                albedo_tex.uint64[0],
                albedo_tex.uint64[1],
                (uint64)TopTex2dSampleKind(),
            );
            hash = HashFromString(str8((uint8*)buffer, sizeof(buffer)));
            slot_idx = hash%params.mesh_batches.slots_count;
        }
        
        // rjf: map hash . existing batch group node
        R.BatchGroup3DMapNode* node = null;
        {
            for (R.BatchGroup3DMapNode* n = params.mesh_batches.slots[slot_idx]; n != null; n = n.next)
            {
                if (n.hash == hash)
                {
                    node = n;
                    break;
                }
            }
        }
        
        // rjf: no batch group node? . make one
        if (node == null)
        {
            node = new:arena R.BatchGroup3DMapNode();
            SLLStackPush(params.mesh_batches.slots[slot_idx], node);
            node.hash = hash;
            node.batches = R.BatchListMake(sizeof(R.Mesh3DInst));
            node.params.mesh_vertices = mesh_vertices;
            node.params.mesh_indices = mesh_indices;
            node.params.mesh_geo_topology = mesh_geo_topology;
            node.params.mesh_geo_vertex_flags = mesh_geo_vertex_flags;
            node.params.albedo_tex = albedo_tex;
            node.params.albedo_tex_sample_kind = TopTex2dSampleKind();
            node.params.xform = mat_4x4f32(1.f);
        }
        
        // rjf: push new instance to batch group
        R.Mesh3DInst* inst = (R.Mesh3DInst*)R.BatchListPushInst(arena, &node.batches, 256);
        inst.xform = inst_xform;
        return inst;
    }

    //- rjf: collating one pre-prepped bucket into parent bucket

    public static void SubBucket(Bucket bucket)
    {
        Arena arena = thread_ctx.arena;
        Bucket src = bucket;
        Bucket dst = TopBucket();
        Rng2F32 dst_clip = TopClip();
        bool dst_clip_is_set = !(dst_clip.x0 == 0 && dst_clip.x1 == 0 &&
                                                        dst_clip.y0 == 0 && dst_clip.y1 == 0);
        for (R.PassNode* n = src.passes.first; n != 0; n = n.next)
        {
            R.Pass* src_pass = &n.v;
            R.Pass* dst_pass = R.PassFromKind(arena, &dst.passes, src_pass.kind);
            switch(dst_pass.kind)
            {
                default:{dst_pass.params = src_pass.params;}
                case R.PassKind_UI:
                {
                    R.PassParams_UI* src_ui = src_pass.params_ui;
                    R.PassParams_UI* dst_ui = dst_pass.params_ui;
                    for (R.BatchGroup2DNode* src_group_n = src_ui.rects.first;
                            src_group_n != null;
                            src_group_n = src_group_n.next)
                    {
                        R.BatchGroup2DNode* dst_group_n = new:arena R.BatchGroup2DNode();
                        SLLQueuePush(dst_ui.rects.first, dst_ui.rects.last, dst_group_n);
                        dst_ui.rects.count += 1;
                        MemoryCopyStruct(&dst_group_n.params, &src_group_n.params);
                        dst_group_n.batches = src_group_n.batches;
                        dst_group_n.params.xform = top_xform2d();
                        if (dst_clip_is_set)
                        {
                            bool clip_is_set = !(dst_group_n.params.clip.x0 == 0 &&
                                                                    dst_group_n.params.clip.y0 == 0 &&
                                                                    dst_group_n.params.clip.x1 == 0 &&
                                                                    dst_group_n.params.clip.y1 == 0);
                            dst_group_n.params.clip = clip_is_set ? intersect_2f32(dst_clip, dst_group_n.params.clip) : dst_clip;
                        }
                    }
                }
            }
        }
    }

    ////////////////////////////////
    //~ rjf: Draw Call Helpers

    //- rjf: text

    public static void TruncatedFancyRunList(Vec2F32 p, FRunList* list, float max_x, FNT.Run trailer_run)
    {
        ProfBeginFunction();
        
        //- rjf: total advance > max? . enable trailer
        bool trailer_enabled = (list.dim.x > max_x && trailer_run.dim.x < max_x);
        
        //- rjf: draw runs
        float advance = 0;
        bool trailer_found = false;
        Vec4F32 last_color = default;
        uint64 byte_off = 0;

        draw_loop: for (FRunNode* n = list.first; n != null; n = n.next)
        {
            FRun* fr = &n.v;
            Rng1F32 pixel_range = default;
            {
                pixel_range.min = 100000;
                pixel_range.max = 0;
            }
            FNT_Piece* piece_first = fr.run.pieces.v;
            FNT_Piece* piece_opl = piece_first + fr.run.pieces.count;
            float pre_advance = advance;
            last_color = fr.color;
            for (FNT_Piece* piece = piece_first;
                    piece < piece_opl;
                    piece += 1)
            {
                if (trailer_enabled && advance + piece.advance > (max_x - trailer_run.dim.x))
                {
                    trailer_found = true;
                    break;
                }
                if (!trailer_enabled && advance + piece.advance > max_x)
                {
                    break draw_loop;
                }
                R.Handle texture = piece.texture;
                Rng2F32 src = r2f32p((float)piece.subrect.x0, (float)piece.subrect.y0, (float)piece.subrect.x1, (float)piece.subrect.y1);
                Vec2F32 size = dim_2f32(src);
                Rng2F32 dst = r2f32p(p.x + piece.offset.x + advance,
                                                        p.y + piece.offset.y,
                                                        p.x + piece.offset.x + advance + size.x,
                                                        p.y + piece.offset.y + size.y);
                if (!R.HandleMatch(texture, R.HandleZero()))
                {
                    Img(dst, src, texture, fr.color, 0, 0, 0);
                    // Rect(dst, v4f32(0, 1, 0, 0.5f), 0, 1.f, 0.f);
                }
                advance += piece.advance;
                pixel_range.min = Math.Min(pre_advance, pixel_range.min);
                pixel_range.max = Math.Max(advance, pixel_range.max);
            }
            if (fr.underline_thickness > 0)
            {
                Rect(r2f32p(p.x + pixel_range.min,
                                            p.y+fr.run.descent+fr.run.descent/8,
                                            p.x + pixel_range.max,
                                            p.y+fr.run.descent+fr.run.descent/8+fr.underline_thickness),
                                fr.color, 0, 0, 0.8f);
            }
            if (fr.strikethrough_thickness > 0)
            {
                Rect(r2f32p(p.x+pre_advance, p.y+fr.run.descent - fr.run.ascent/2, p.x+advance, p.y+fr.run.descent - fr.run.ascent/2 + fr.strikethrough_thickness), fr.color, 0, 0, 1.f);
            }
            if (trailer_found)
            {
                break;
            }
        }
        
        //- rjf: draw trailer
        if (trailer_found)
        {
            FNT.Piece* piece_first = trailer_run.pieces.v;
            FNT.Piece* piece_opl = piece_first + trailer_run.pieces.count;
            float pre_advance = advance;
            Vec4F32 trailer_piece_color = last_color;
            for (FNT.Piece* piece = piece_first;
                    piece < piece_opl;
                    piece += 1)
            {
                R.Handle texture = piece.texture;
                Rng2F32 src = r2f32p((float)piece.subrect.x0, (float)piece.subrect.y0, (float)piece.subrect.x1, (float)piece.subrect.y1);
                Vec2F32 size = dim_2f32(src);
                Rng2F32 dst = r2f32p(p.x + piece.offset.x + advance,
                                                        p.y + piece.offset.y,
                                                        p.x + piece.offset.x + advance + size.x,
                                                        p.y + piece.offset.y + size.y);
                if (!R.HandleMatch(texture, R.HandleZero()))
                {
                    Img(dst, src, texture, trailer_piece_color, 0, 0, 0);
                    trailer_piece_color.w *= 0.5f;
                }
                advance += piece.advance;
            }
        }
        
        ProfEnd();
    }

    public static void TruncatedFancyRunFuzzyMatches(Vec2F32 p, FRunList* list, float max_x, FuzzyMatchRangeList* ranges, Vec4F32 color)
    {
        for (FuzzyMatchRangeNode* match_n = ranges.first; match_n != null; match_n = match_n.next)
        {
            Rng1U64 byte_range = match_n.range;
            Rng1F32 pixel_range = default;
            {
                pixel_range.min = 100000;
                pixel_range.max = 0;
            }
            float last_piece_end_pad = 0;
            uint64 byte_off = 0;
            float advance = 0;
            float ascent = 0;
            float descent = 0;
            for (FRunNode* fr_n = list.first; fr_n != null; fr_n = fr_n.next)
            {
                FRun* fr = &fr_n.v;
                FNT_Run* run = &fr.run;
                ascent = run.ascent;
                descent = run.descent;
                for (uint64 piece_idx = 0; piece_idx < run.pieces.count; piece_idx += 1)
                {
                    FNT_Piece* piece = &run.pieces.v[piece_idx];
                    if (contains_1u64(byte_range, byte_off))
                    {
                        float pre_advance  = advance + piece.offset.x;
                        float post_advance = advance + piece.advance;
                        pixel_range.min = Math.Min(pre_advance,  pixel_range.min);
                        pixel_range.max = Math.Max(post_advance, pixel_range.max);
                    }
                    if (!fr.icon)
                    {
                        byte_off += piece.decode_size;
                    }
                    advance += piece.advance;
                }
            }
            if (pixel_range.min < pixel_range.max)
            {
                Rng2F32 rect = r2f32p(p.x + pixel_range.min - ascent/4.f,
                                                            p.y - descent - ascent - ascent/8.f,
                                                            p.x + pixel_range.max + ascent/4.f,
                                                            p.y - descent - ascent + ascent/8.f + list.dim.y);
                rect.x0 = Math.Min(rect.x0, p.x+max_x);
                rect.x1 = Math.Min(rect.x1, p.x+max_x);
                Rect(rect, color, (descent+ascent)/4.f, 0, 1.f);
            }
        }
    }

    public static void TextRun(Vec2F32 p, Vec4F32 color, FNT.Run run)
    {
        ProfBeginFunction();
        float advance = 0;
        FNT.Piece* piece_first = run.pieces.v;
        FNT.Piece* piece_opl = piece_first + run.pieces.count;
        for (FNT_Piece* piece = piece_first;
                piece < piece_opl;
                piece += 1)
        {
            R.Handle texture = piece.texture;
            Rng2F32 src = r2f32p((float)piece.subrect.x0, (float)piece.subrect.y0, (float)piece.subrect.x1, (float)piece.subrect.y1);
            Vec2F32 size = dim_2f32(src);
            Rng2F32 dst = r2f32p(p.x + piece.offset.x + advance,
                                                    p.y + piece.offset.y,
                                                    p.x + piece.offset.x + advance + size.x,
                                                    p.y + piece.offset.y + size.y);
            if (size.x != 0 && size.y != 0 && texture != default)
            {
                Img(dst, src, texture, color, 0, 0, 0);
            }
            advance += piece.advance;
        }
        ProfEnd();
    }

    public static void Text(FNT.Tag font, float size, float base_align_px, float tab_size_px, FNT_RasterFlags flags, Vec2F32 p, Vec4F32 color, StringView str)
    {
        Temp scratch = ScratchBegin(0, 0);
        defer ScratchEnd(scratch);

        FNT_Run run = FNT.RunFromString(font, size, base_align_px, tab_size_px, flags, str);
        TextRun(p, color, run);
    }
}
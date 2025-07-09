// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: Meta Markup Features

#define EmbedFile(name, path)
#define TweakB32(name, default)           (TWEAK_##name)
#define TweakF32(name, default, min, max) (TWEAK_##name)

////////////////////////////////
//~ rjf: Tweak Info Tables



struct TweakB32Info
{
  String8 name;
  b32 default_value;
  b32 *value_ptr;
};



struct TweakF32Info
{
  String8 name;
  f32 default_value;
  Rng1F32 value_range;
  f32 *value_ptr;
};



struct TweakB32InfoTable
{
  TweakB32Info *v;
  u64 count;
};



struct TweakF32InfoTable
{
  TweakF32Info *v;
  u64 count;
};



struct EmbedInfo
{
  String8 name;
  String8 *data;
  u128 *hash;
};



struct EmbedInfoTable
{
  EmbedInfo *v;
  u64 count;
};

////////////////////////////////
//~ rjf: Type Info Types

typedef enum TypeKind
{
  TypeKind_Null,
  
  // rjf: leaves
  TypeKind_Void, TypeKind_FirstLeaf = TypeKind_Void,
  TypeKind_U8,
  TypeKind_U16,
  TypeKind_U32,
  TypeKind_U64,
  TypeKind_S8,
  TypeKind_S16,
  TypeKind_S32,
  TypeKind_S64,
  TypeKind_B8,
  TypeKind_B16,
  TypeKind_B32,
  TypeKind_B64,
  TypeKind_F32,
  TypeKind_F64, TypeKind_LastLeaf = TypeKind_F64,
  
  // rjf: operators
  TypeKind_Ptr,
  TypeKind_Array,
  
  // rjf: user-defined-types
  TypeKind_Struct,
  TypeKind_Union,
  TypeKind_Enum,
  
  TypeKind_COUNT
}
TypeKind;

typedef u32 TypeFlags;
enum
{
  TypeFlag_IsPlainText = (1<<0),
  TypeFlag_IsCodeText  = (1<<1),
  TypeFlag_IsPathText  = (1<<2),
};

typedef u32 MemberFlags;
enum
{
  MemberFlag_DoNotSerialize  = (1<<0),
};





struct Member
{
  String8 name;
  String8 pretty_name;
  Type *type;
  u64 value;
  MemberFlags flags;
};



struct Type
{
  TypeKind kind;
  TypeFlags flags;
  u64 size;
  Type *direct;
  String8 name;
  String8 count_delimiter_name; // gathered from surrounding members, turns *->[1] into *->[N]
  u64 count;
  Member *members;
};

////////////////////////////////
//~ rjf: Type Serialization Parameters



struct TypeSerializePtrRefInfo
{
  Type *type;           // pointers to this
  void *indexify_base;  // can be indexified using this
  void *offsetify_base; // can be offsetified using this
  void *nil_ptr;        // is terminal if matching 0 or this
};



struct TypeSerializeParams
{
  u64 *advance_out;
  TypeSerializePtrRefInfo *ptr_ref_infos;
  u64 ptr_ref_infos_count;
};

////////////////////////////////
//~ rjf: Type Name -> Type Info

#define type(T) (&T##__type)

////////////////////////////////
//~ rjf: Type Info Table Initializer Helpers

#define member_lit_comp(S, ti, m, ...) {str8_lit_comp(#m), {0}, (ti), OffsetOf(S, m), __VA_ARGS__}
#define struct_members(S) @(rodata) global Member S##__members[] =
#define struct_type(S, ...) @(rodata) global Type S##__type = {TypeKind_Struct, 0, sizeof(S), &type_nil, str8_lit_comp(#S), {0}, len(S##__members), S##__members, __VA_ARGS__}
#define named_struct_type(name, S, ...) @(rodata) global Type name##__type = {TypeKind_Struct, 0, sizeof(S), &type_nil, str8_lit_comp(#name), {0}, len(name##__members), name##__members, __VA_ARGS__}
#define ptr_type(name, ti, ...) @(rodata) global Type name = {TypeKind_Ptr, 0, sizeof(void *), (ti), __VA_ARGS__}

////////////////////////////////
//~ rjf: Globals

@(rodata) global Type type_nil   = {TypeKind_Null, 0, 0, &type_nil};
@(rodata) global Member member_nil = {{0}, {0}, &type_nil};

////////////////////////////////
//~ rjf: Built-In Types

//- rjf: leaves
@(rodata) global Type void__type = {TypeKind_Void, 0, 0,           &type_nil, str8_lit_comp("void")};
@(rodata) global Type u8__type   = {TypeKind_U8,   0, sizeof(u8),  &type_nil, str8_lit_comp("u8")};
@(rodata) global Type u16__type  = {TypeKind_U16,  0, sizeof(u16), &type_nil, str8_lit_comp("u16")};
@(rodata) global Type u32__type  = {TypeKind_U32,  0, sizeof(u32), &type_nil, str8_lit_comp("u32")};
@(rodata) global Type u64__type  = {TypeKind_U64,  0, sizeof(u64), &type_nil, str8_lit_comp("u64")};
@(rodata) global Type i8__type   = {TypeKind_S8,   0, sizeof(i8),  &type_nil, str8_lit_comp("i8")};
@(rodata) global Type i16__type  = {TypeKind_S16,  0, sizeof(i16), &type_nil, str8_lit_comp("i16")};
@(rodata) global Type i32__type  = {TypeKind_S32,  0, sizeof(i32), &type_nil, str8_lit_comp("i32")};
@(rodata) global Type i64__type  = {TypeKind_S64,  0, sizeof(i64), &type_nil, str8_lit_comp("i64")};
@(rodata) global Type b8__type   = {TypeKind_B8,   0, sizeof(b8),  &type_nil, str8_lit_comp("b8")};
@(rodata) global Type b16__type  = {TypeKind_B16,  0, sizeof(b16), &type_nil, str8_lit_comp("b16")};
@(rodata) global Type b32__type  = {TypeKind_B32,  0, sizeof(b32), &type_nil, str8_lit_comp("b32")};
@(rodata) global Type b64__type  = {TypeKind_B64,  0, sizeof(b64), &type_nil, str8_lit_comp("b64")};
@(rodata) global Type f32__type  = {TypeKind_F32,  0, sizeof(f32), &type_nil, str8_lit_comp("f32")};
@(rodata) global Type f64__type  = {TypeKind_F64,  0, sizeof(f64), &type_nil, str8_lit_comp("f64")};
@(rodata) global Type *type_kind_type_table[] =
{
  &type_nil,
  type(void),
  type(u8),
  type(u16),
  type(u32),
  type(u64),
  type(i8),
  type(i16),
  type(i32),
  type(i64),
  type(b8),
  type(b16),
  type(b32),
  type(b64),
  type(f32),
  type(f64),
  &type_nil,
  &type_nil,
  &type_nil,
  &type_nil,
  &type_nil,
};

//- rjf: Rng1U64
struct_members(Rng1U64)
{
  member_lit_comp(Rng1U64, type(u64), min),
  member_lit_comp(Rng1U64, type(u64), max),
};
struct_type(Rng1U64);

//- rjf: String8
ptr_type(String8__str_ptr_type, type(u8), str8_lit_comp("size"));
struct_members(String8)
{
  member_lit_comp(String8, &String8__str_ptr_type, str),
  member_lit_comp(String8, type(u64),              size),
};
struct_type(String8);

//- rjf: String8Node
extern Type String8Node__type;
Type String8Node__ptr_type = {TypeKind_Ptr, 0, sizeof(void *), &String8Node__type};
Member String8Node__members[] =
{
  {str8_lit_comp("next"),   {0}, &String8Node__ptr_type,     OffsetOf(String8Node, next)},
  {str8_lit_comp("string"), {0}, type(String8),              OffsetOf(String8Node, string)},
};
Type String8Node__type =
{
  TypeKind_Struct,
  0,
  sizeof(String8Node),
  &type_nil,
  str8_lit_comp("String8Node"),
  {0},
  len(String8Node__members),
  String8Node__members,
};

//- rjf: String8List
Member String8List__members[] =
{
  {str8_lit_comp("first"),      {0}, &String8Node__ptr_type,     OffsetOf(String8List, first)},
  {str8_lit_comp("last"),       {0}, &String8Node__ptr_type,     OffsetOf(String8List, last), MemberFlag_DoNotSerialize},
  {str8_lit_comp("node_count"), {0}, type(u64), OffsetOf(String8List, node_count)},
  {str8_lit_comp("total_size"), {0}, type(u64), OffsetOf(String8List, total_size)},
};
Type String8List__type =
{
  TypeKind_Struct,
  0,
  sizeof(String8List),
  &type_nil,
  str8_lit_comp("String8List"),
  {0},
  len(String8List__members),
  String8List__members,
};

////////////////////////////////
//~ rjf: Type Info Lookups


#define EachMember(T, it) (Member *it = (type(T))->members; it != 0 && it < (type(T))->members + (type(T))->count; it += 1)

////////////////////////////////
//~ rjf: Type Info * Instance Operations





#define struct_rebase_ptrs(T, ptr, base)                   typed_data_rebase_ptrs(type(T), str8_struct(ptr), (base))
#define serialized_from_struct(arena, T, ptr, ...)         serialized_from_typed_data((arena), type(T), str8_struct(ptr), &(TypeSerializeParams){.ptr_ref_infos = 0, __VA_ARGS__})
#define struct_from_serialized(arena, T, string, ...) (T *)deserialized_from_typed_data((arena), type(T), (string), &(TypeSerializeParams){.ptr_ref_infos = 0, __VA_ARGS__}).str
#define deep_copy_from_struct(arena, T, ptr, ...)     (T *)deep_copy_from_typed_data((arena), type(T), str8_struct(ptr), &(TypeSerializeParams){.ptr_ref_infos = 0, __VA_ARGS__}).str



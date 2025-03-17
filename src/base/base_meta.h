// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_META_H
#define BASE_META_H

////////////////////////////////
//~ rjf: Meta Markup Features

#define EmbedFile(name, path)
#define TweakB32(name, default)           (TWEAK_##name)
#define TweakF32(name, default, min, max) (TWEAK_##name)

////////////////////////////////
//~ rjf: Tweak Info Tables

struct TweakB32Info
{
  StringView name;
  B32 default_value;
  B32* value_ptr;
}

struct TweakF32Info
{
  StringView name;
  float default_value;
  Rng1F32 value_range;
  float* value_ptr;
}

struct TweakB32InfoTable
{
  TweakB32Info* v;
  uint64 count;
}

struct TweakF32InfoTable
{
  TweakF32Info* v;
  uint64 count;
}

struct EmbedInfo
{
  StringView name;
  StringView* data;
  U128* hash;
}

struct EmbedInfoTable
{
  EmbedInfo* v;
  uint64 count;
}

////////////////////////////////
//~ rjf: Type Info Types

enum TypeKind
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

enum TypeFlags : uint32
{
  TypeFlag_IsExternal  = (1<<0),
  TypeFlag_IsPlainText = (1<<1),
  TypeFlag_IsCodeText  = (1<<2),
  TypeFlag_IsPathText  = (1<<3),
}

enum MemberFlags : uint32
{
  MemberFlag_DoNotSerialize  = (1<<0),
}

struct Member
{
  StringView name;
  StringView pretty_name;
  Type* type;
  uint64 value;
  MemberFlags flags;
}

struct Type
{
  TypeKind kind;
  TypeFlags flags;
  uint64 size;
  Type* direct;
  StringView name;
  StringView count_delimiter_name; // gathered from surrounding members, turns *.[1] into *.[N]
  uint64 count;
  Member* members;
}

////////////////////////////////
//~ rjf: Type Serialization Parameters

struct TypeSerializePtrRefInfo
{
  Type* type;           // pointers to this
  void* indexify_base;  // can be indexified using this
  void* offsetify_base; // can be offsetified using this
  void* nil_ptr;        // is terminal if matching 0 or this
}

struct TypeSerializeParams
{
  uint64* advance_out;
  TypeSerializePtrRefInfo* ptr_ref_infos;
  uint64 ptr_ref_infos_count;
}

////////////////////////////////
//~ rjf: Type Name . Type Info

#define type(T) (&T##__type)

////////////////////////////////
//~ rjf: Type Info Table Initializer Helpers

#define member_lit_comp(S, ti, m, ...) {(#m), {0}, (ti), OffsetOf(S, m), __VA_ARGS__}
#define struct_members(S) read_only static Member S##__members[] =
#define struct_type(S, ...) read_only static Type S##__type = {TypeKind_Struct, 0, sizeof(S), &type_nil, (#S), {0}, ArrayCount(S##__members), S##__members, __VA_ARGS__}
#define named_struct_type(name, S, ...) read_only static Type name##__type = {TypeKind_Struct, 0, sizeof(S), &type_nil, (#name), {0}, ArrayCount(name##__members), name##__members, __VA_ARGS__}
#define ptr_type(name, ti, ...) read_only static Type name = {TypeKind_Ptr, 0, sizeof(void *), (ti), __VA_ARGS__}

////////////////////////////////
//~ rjf: Globals

read_only static Type type_nil   = {TypeKind_Null, 0, 0, &type_nil};
read_only static Member member_nil = {{0}, {0}, &type_nil};

////////////////////////////////
//~ rjf: Built-In Types

//- rjf: leaves
read_only static Type void__type = {TypeKind_Void, 0, 0,           &type_nil, ("void")};
read_only static Type U8__type   = {TypeKind_U8,   0, sizeof(uint8),  &type_nil, ("uint8")};
read_only static Type U16__type  = {TypeKind_U16,  0, sizeof(uint16), &type_nil, ("uint16")};
read_only static Type U32__type  = {TypeKind_U32,  0, sizeof(uint32), &type_nil, ("uint32")};
read_only static Type U64__type  = {TypeKind_U64,  0, sizeof(uint64), &type_nil, ("uint64")};
read_only static Type S8__type   = {TypeKind_S8,   0, sizeof(int8),  &type_nil, ("int8")};
read_only static Type S16__type  = {TypeKind_S16,  0, sizeof(int16), &type_nil, ("int16")};
read_only static Type S32__type  = {TypeKind_S32,  0, sizeof(int32), &type_nil, ("int32")};
read_only static Type S64__type  = {TypeKind_S64,  0, sizeof(int64), &type_nil, ("int64")};
read_only static Type B8__type   = {TypeKind_B8,   0, sizeof(B8),  &type_nil, ("B8")};
read_only static Type B16__type  = {TypeKind_B16,  0, sizeof(B16), &type_nil, ("B16")};
read_only static Type B32__type  = {TypeKind_B32,  0, sizeof(B32), &type_nil, ("B32")};
read_only static Type B64__type  = {TypeKind_B64,  0, sizeof(B64), &type_nil, ("B64")};
read_only static Type F32__type  = {TypeKind_F32,  0, sizeof(float), &type_nil, ("float")};
read_only static Type F64__type  = {TypeKind_F64,  0, sizeof(double), &type_nil, ("double")};
read_only static Type* type_kind_type_table[] =
{
  &type_nil,
  type(),
  type(uint8),
  type(uint16),
  type(uint32),
  type(uint64),
  type(int8),
  type(int16),
  type(int32),
  type(int64),
  type(B8),
  type(B16),
  type(B32),
  type(B64),
  type(float),
  type(double),
  &type_nil,
  &type_nil,
  &type_nil,
  &type_nil,
  &type_nil,
}

//- rjf: Rng1U64
struct_members(Rng1U64)
{
  member_lit_comp(Rng1U64, type(uint64), min),
  member_lit_comp(Rng1U64, type(uint64), max),
}
struct_type(Rng1U64);

//- rjf: StringView
ptr_type(String8__str_ptr_type, type(uint8), ("size"));
struct_members(StringView)
{
  member_lit_comp(StringView, &String8__str_ptr_type, str),
  member_lit_comp(StringView, type(uint64),              size),
}
struct_type(StringView);

//- rjf: String8Node
extern Type String8Node__type;
Type String8Node__ptr_type = {TypeKind_Ptr, 0, sizeof(void *), &String8Node__type};
Member String8Node__members[] =
{
  {("next"),   {0}, &String8Node__ptr_type,     OffsetOf(String8Node, next)},
  {("str"), {0}, type(StringView),              OffsetOf(String8Node, str)},
}
Type String8Node__type =
{
  TypeKind_Struct,
  0,
  sizeof(String8Node),
  &type_nil,
  ("String8Node"),
  {0},
  ArrayCount(String8Node__members),
  String8Node__members,
}

//- rjf: String8List
Member String8List__members[] =
{
  {("first"),      {0}, &String8Node__ptr_type,     OffsetOf(String8List, first)},
  {("last"),       {0}, &String8Node__ptr_type,     OffsetOf(String8List, last), MemberFlag_DoNotSerialize},
  {("node_count"), {0}, type(uint64), OffsetOf(String8List, node_count)},
  {("total_size"), {0}, type(uint64), OffsetOf(String8List, total_size)},
}
Type String8List__type =
{
  TypeKind_Struct,
  0,
  sizeof(String8List),
  &type_nil,
  ("String8List"),
  {0},
  ArrayCount(String8List__members),
  String8List__members,
}

////////////////////////////////
//~ rjf: Type Info Lookups

Member* member_from_name(Type* type, StringView name);
#define EachMember(T, it) (Member* it = (type(T)).members; it != 0 && it < (type(T)).members + (type(T)).count; it += 1)

////////////////////////////////
//~ rjf: Type Info * Instance Operations

void typed_data_rebase_ptrs(Type* type, StringView data, void* base_ptr);
StringView serialized_from_typed_data(Arena* arena, Type* type, StringView data, TypeSerializeParams* params);
StringView deserialized_from_typed_data(Arena* arena, Type* type, StringView data, TypeSerializeParams* params);
StringView deep_copy_from_typed_data(Arena* arena, Type* type, StringView data, TypeSerializeParams* params);
#define struct_rebase_ptrs(T, ptr, base)                   typed_data_rebase_ptrs(type(T), str8_struct(ptr), (base))
#define serialized_from_struct(arena, T, ptr, ...)         serialized_from_typed_data((arena), type(T), str8_struct(ptr), &(TypeSerializeParams){.ptr_ref_infos = 0, __VA_ARGS__})
#define struct_from_serialized(arena, T, str, ...) (T *)deserialized_from_typed_data((arena), type(T), (str), &(TypeSerializeParams){.ptr_ref_infos = 0, __VA_ARGS__}).Ptr
#define deep_copy_from_struct(arena, T, ptr, ...)     (T *)deep_copy_from_typed_data((arena), type(T), str8_struct(ptr), &(TypeSerializeParams){.ptr_ref_infos = 0, __VA_ARGS__}).Ptr

#endif // BASE_META_H

// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////////////////////////////////////
//~ (R)AD (D)ebug (I)nfo Format Library
//
// Defines standard RDI debug information format types and
// functions.

#ifndef RDI_C
#define RDI_C

RDI_uint16 rdi_section_element_size_table[40] =
{
sizeof(RDI_uint8),
sizeof(RDI_TopLevelInfo),
sizeof(RDI_uint8),
sizeof(RDI_uint32),
sizeof(RDI_uint32),
sizeof(RDI_BinarySection),
sizeof(RDI_FilePathNode),
sizeof(RDI_SourceFile),
sizeof(RDI_LineTable),
sizeof(RDI_uint64),
sizeof(RDI_Line),
sizeof(RDI_Column),
sizeof(RDI_SourceLineMap),
sizeof(RDI_uint32),
sizeof(RDI_uint32),
sizeof(RDI_uint64),
sizeof(RDI_Unit),
sizeof(RDI_VMapEntry),
sizeof(RDI_TypeNode),
sizeof(RDI_UDT),
sizeof(RDI_Member),
sizeof(RDI_EnumMember),
sizeof(RDI_GlobalVariable),
sizeof(RDI_VMapEntry),
sizeof(RDI_ThreadVariable),
sizeof(RDI_Constant),
sizeof(RDI_Procedure),
sizeof(RDI_Scope),
sizeof(RDI_uint64),
sizeof(RDI_VMapEntry),
sizeof(RDI_InlineSite),
sizeof(RDI_Local),
sizeof(RDI_LocationBlock),
sizeof(RDI_uint8),
sizeof(RDI_uint8),
sizeof(RDI_uint32),
sizeof(RDI_NameMap),
sizeof(RDI_NameMapBucket),
sizeof(RDI_NameMapNode),
sizeof(RDI_uint8),
};

RDI_uint8 rdi_section_is_required_table[40] =
{
0,
0,
1,
1,
1,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
};

RDI_uint16 rdi_eval_op_ctrlbits_table[52] =
{
RDI_EVAL_CTRLBITS(0, 0, 0),
RDI_EVAL_CTRLBITS(0, 0, 0),
RDI_EVAL_CTRLBITS(1, 1, 0),
RDI_EVAL_CTRLBITS(2, 0, 0),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(4, 0, 1),
RDI_EVAL_CTRLBITS(0, 1, 1),
RDI_EVAL_CTRLBITS(8, 0, 1),
RDI_EVAL_CTRLBITS(4, 0, 1),
RDI_EVAL_CTRLBITS(4, 0, 1),
RDI_EVAL_CTRLBITS(0, 0, 0),
RDI_EVAL_CTRLBITS(0, 0, 0),
RDI_EVAL_CTRLBITS(1, 0, 1),
RDI_EVAL_CTRLBITS(2, 0, 1),
RDI_EVAL_CTRLBITS(4, 0, 1),
RDI_EVAL_CTRLBITS(8, 0, 1),
RDI_EVAL_CTRLBITS(16, 0, 1),
RDI_EVAL_CTRLBITS(1, 0, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(2, 1, 1),
RDI_EVAL_CTRLBITS(1, 0, 1),
RDI_EVAL_CTRLBITS(0, 1, 0),
RDI_EVAL_CTRLBITS(1, 0, 0),
RDI_EVAL_CTRLBITS(1, 2, 1),
RDI_EVAL_CTRLBITS(1, 1, 1),
RDI_EVAL_CTRLBITS(4, 0, 0),
RDI_EVAL_CTRLBITS(4, 0, 0),
RDI_EVAL_CTRLBITS(8, 0, 0),
RDI_EVAL_CTRLBITS(0, 0, 0),
};

struct {RDI_EvalConversionKind dst_typegroups[RDI_EvalTypeGroup_COUNT];} rdi_eval_typegroup_conversion_kind_matrix[6] =
{
{{RDI_EvalConversionKind_OtherToOther, RDI_EvalConversionKind_FromOther, RDI_EvalConversionKind_FromOther, RDI_EvalConversionKind_FromOther, RDI_EvalConversionKind_FromOther}},
{{RDI_EvalConversionKind_ToOther, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Legal}},
{{RDI_EvalConversionKind_ToOther, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Legal}},
{{RDI_EvalConversionKind_ToOther, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Legal}},
{{RDI_EvalConversionKind_ToOther, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Legal, RDI_EvalConversionKind_Noop}},
{{RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Noop, RDI_EvalConversionKind_Noop}},
};

struct {RDI_uint8 *str; RDI_uint64 size;} rdi_eval_conversion_kind_message_string_table[6] =
{
{(RDI_uint8 *)"", sizeof("")},
{(RDI_uint8 *)"", sizeof("")},
{(RDI_uint8 *)"Cannot convert between these types.", sizeof("Cannot convert between these types.")},
{(RDI_uint8 *)"Cannot convert to this type.", sizeof("Cannot convert to this type.")},
{(RDI_uint8 *)"Cannot convert this type.", sizeof("Cannot convert this type.")},
{(RDI_uint8 *)"", sizeof("")},
};

RDI_PROC RDI_uint64
rdi_hash(RDI_uint8 *ptr, RDI_uint64 size)
{
  RDI_uint64 result = 5381;
  RDI_uint8 *opl = ptr + size;
  for(;ptr < opl; ptr += 1)
  {
    result = ((result << 5) + result) + *ptr;
  }
  return result;
}

RDI_PROC RDI_uint8 *
rdi_string_from_type_kind(RDI_TypeKind kind, RDI_uint64 *size_out)
{
RDI_uint8 *result = 0;
*size_out = 0;
switch (kind)
{
default:{}break;
case RDI_TypeKind_NULL: {result = (RDI_uint8*)"NULL"; *size_out = sizeof("NULL")-1;}break;
case RDI_TypeKind_Void: {result = (RDI_uint8*)"Void"; *size_out = sizeof("Void")-1;}break;
case RDI_TypeKind_Handle: {result = (RDI_uint8*)"Handle"; *size_out = sizeof("Handle")-1;}break;
case RDI_TypeKind_HResult: {result = (RDI_uint8*)"HResult"; *size_out = sizeof("HResult")-1;}break;
case RDI_TypeKind_Char8: {result = (RDI_uint8*)"Char8"; *size_out = sizeof("Char8")-1;}break;
case RDI_TypeKind_Char16: {result = (RDI_uint8*)"Char16"; *size_out = sizeof("Char16")-1;}break;
case RDI_TypeKind_Char32: {result = (RDI_uint8*)"Char32"; *size_out = sizeof("Char32")-1;}break;
case RDI_TypeKind_UChar8: {result = (RDI_uint8*)"UChar8"; *size_out = sizeof("UChar8")-1;}break;
case RDI_TypeKind_UChar16: {result = (RDI_uint8*)"UChar16"; *size_out = sizeof("UChar16")-1;}break;
case RDI_TypeKind_UChar32: {result = (RDI_uint8*)"UChar32"; *size_out = sizeof("UChar32")-1;}break;
case RDI_TypeKind_uint8: {result = (RDI_uint8*)"uint8"; *size_out = sizeof("uint8")-1;}break;
case RDI_TypeKind_uint16: {result = (RDI_uint8*)"uint16"; *size_out = sizeof("uint16")-1;}break;
case RDI_TypeKind_uint32: {result = (RDI_uint8*)"uint32"; *size_out = sizeof("uint32")-1;}break;
case RDI_TypeKind_uint64: {result = (RDI_uint8*)"uint64"; *size_out = sizeof("uint64")-1;}break;
case RDI_TypeKind_uint128: {result = (RDI_uint8*)"uint128"; *size_out = sizeof("uint128")-1;}break;
case RDI_TypeKind_U256: {result = (RDI_uint8*)"U256"; *size_out = sizeof("U256")-1;}break;
case RDI_TypeKind_U512: {result = (RDI_uint8*)"U512"; *size_out = sizeof("U512")-1;}break;
case RDI_TypeKind_uint8: {result = (RDI_uint8*)"uint8"; *size_out = sizeof("uint8")-1;}break;
case RDI_TypeKind_uint16: {result = (RDI_uint8*)"uint16"; *size_out = sizeof("uint16")-1;}break;
case RDI_TypeKind_uint32: {result = (RDI_uint8*)"uint32"; *size_out = sizeof("uint32")-1;}break;
case RDI_TypeKind_uint64: {result = (RDI_uint8*)"uint64"; *size_out = sizeof("uint64")-1;}break;
case RDI_TypeKind_uint128: {result = (RDI_uint8*)"uint128"; *size_out = sizeof("uint128")-1;}break;
case RDI_TypeKind_S256: {result = (RDI_uint8*)"S256"; *size_out = sizeof("S256")-1;}break;
case RDI_TypeKind_S512: {result = (RDI_uint8*)"S512"; *size_out = sizeof("S512")-1;}break;
case RDI_TypeKind_Bool: {result = (RDI_uint8*)"Bool"; *size_out = sizeof("Bool")-1;}break;
case RDI_TypeKind_F16: {result = (RDI_uint8*)"F16"; *size_out = sizeof("F16")-1;}break;
case RDI_TypeKind_float: {result = (RDI_uint8*)"float"; *size_out = sizeof("float")-1;}break;
case RDI_TypeKind_floatPP: {result = (RDI_uint8*)"floatPP"; *size_out = sizeof("floatPP")-1;}break;
case RDI_TypeKind_F48: {result = (RDI_uint8*)"F48"; *size_out = sizeof("F48")-1;}break;
case RDI_TypeKind_double: {result = (RDI_uint8*)"double"; *size_out = sizeof("double")-1;}break;
case RDI_TypeKind_F80: {result = (RDI_uint8*)"F80"; *size_out = sizeof("F80")-1;}break;
case RDI_TypeKind_F128: {result = (RDI_uint8*)"F128"; *size_out = sizeof("F128")-1;}break;
case RDI_TypeKind_Complexfloat: {result = (RDI_uint8*)"Complexfloat"; *size_out = sizeof("Complexfloat")-1;}break;
case RDI_TypeKind_Complexdouble: {result = (RDI_uint8*)"Complexdouble"; *size_out = sizeof("Complexdouble")-1;}break;
case RDI_TypeKind_ComplexF80: {result = (RDI_uint8*)"ComplexF80"; *size_out = sizeof("ComplexF80")-1;}break;
case RDI_TypeKind_ComplexF128: {result = (RDI_uint8*)"ComplexF128"; *size_out = sizeof("ComplexF128")-1;}break;
case RDI_TypeKind_Modifier: {result = (RDI_uint8*)"Modifier"; *size_out = sizeof("Modifier")-1;}break;
case RDI_TypeKind_Ptr: {result = (RDI_uint8*)"Ptr"; *size_out = sizeof("Ptr")-1;}break;
case RDI_TypeKind_LRef: {result = (RDI_uint8*)"LRef"; *size_out = sizeof("LRef")-1;}break;
case RDI_TypeKind_RRef: {result = (RDI_uint8*)"RRef"; *size_out = sizeof("RRef")-1;}break;
case RDI_TypeKind_Array: {result = (RDI_uint8*)"Array"; *size_out = sizeof("Array")-1;}break;
case RDI_TypeKind_Function: {result = (RDI_uint8*)"Function"; *size_out = sizeof("Function")-1;}break;
case RDI_TypeKind_Method: {result = (RDI_uint8*)"Method"; *size_out = sizeof("Method")-1;}break;
case RDI_TypeKind_MemberPtr: {result = (RDI_uint8*)"MemberPtr"; *size_out = sizeof("MemberPtr")-1;}break;
case RDI_TypeKind_Struct: {result = (RDI_uint8*)"Struct"; *size_out = sizeof("Struct")-1;}break;
case RDI_TypeKind_Class: {result = (RDI_uint8*)"Class"; *size_out = sizeof("Class")-1;}break;
case RDI_TypeKind_Union: {result = (RDI_uint8*)"Union"; *size_out = sizeof("Union")-1;}break;
case RDI_TypeKind_Enum: {result = (RDI_uint8*)"Enum"; *size_out = sizeof("Enum")-1;}break;
case RDI_TypeKind_Alias: {result = (RDI_uint8*)"Alias"; *size_out = sizeof("Alias")-1;}break;
case RDI_TypeKind_IncompleteStruct: {result = (RDI_uint8*)"IncompleteStruct"; *size_out = sizeof("IncompleteStruct")-1;}break;
case RDI_TypeKind_IncompleteUnion: {result = (RDI_uint8*)"IncompleteUnion"; *size_out = sizeof("IncompleteUnion")-1;}break;
case RDI_TypeKind_IncompleteClass: {result = (RDI_uint8*)"IncompleteClass"; *size_out = sizeof("IncompleteClass")-1;}break;
case RDI_TypeKind_IncompleteEnum: {result = (RDI_uint8*)"IncompleteEnum"; *size_out = sizeof("IncompleteEnum")-1;}break;
case RDI_TypeKind_Bitfield: {result = (RDI_uint8*)"Bitfield"; *size_out = sizeof("Bitfield")-1;}break;
case RDI_TypeKind_Variadic: {result = (RDI_uint8*)"Variadic"; *size_out = sizeof("Variadic")-1;}break;
case RDI_TypeKind_Count: {result = (RDI_uint8*)"Count"; *size_out = sizeof("Count")-1;}break;
}
return result;
}

RDI_PROC RDI_uint32
rdi_size_from_basic_type_kind(RDI_TypeKind kind)
{
RDI_uint32 result = 0;
switch(kind)
{
default:{}break;
case RDI_TypeKind_Handle:{result = 0xFFFFFFFF;}break;
case RDI_TypeKind_HResult:{result = 4;}break;
case RDI_TypeKind_Char8:{result = 1;}break;
case RDI_TypeKind_Char16:{result = 2;}break;
case RDI_TypeKind_Char32:{result = 4;}break;
case RDI_TypeKind_UChar8:{result = 1;}break;
case RDI_TypeKind_UChar16:{result = 2;}break;
case RDI_TypeKind_UChar32:{result = 4;}break;
case RDI_TypeKind_uint8:{result = 1;}break;
case RDI_TypeKind_uint16:{result = 2;}break;
case RDI_TypeKind_uint32:{result = 4;}break;
case RDI_TypeKind_uint64:{result = 8;}break;
case RDI_TypeKind_uint128:{result = 16;}break;
case RDI_TypeKind_U256:{result = 32;}break;
case RDI_TypeKind_U512:{result = 64;}break;
case RDI_TypeKind_uint8:{result = 1;}break;
case RDI_TypeKind_uint16:{result = 2;}break;
case RDI_TypeKind_uint32:{result = 4;}break;
case RDI_TypeKind_uint64:{result = 8;}break;
case RDI_TypeKind_uint128:{result = 16;}break;
case RDI_TypeKind_S256:{result = 32;}break;
case RDI_TypeKind_S512:{result = 64;}break;
case RDI_TypeKind_Bool:{result = 1;}break;
case RDI_TypeKind_F16:{result = 2;}break;
case RDI_TypeKind_float:{result = 4;}break;
case RDI_TypeKind_floatPP:{result = 4;}break;
case RDI_TypeKind_F48:{result = 6;}break;
case RDI_TypeKind_double:{result = 8;}break;
case RDI_TypeKind_F80:{result = 10;}break;
case RDI_TypeKind_F128:{result = 16;}break;
case RDI_TypeKind_Complexfloat:{result = 8;}break;
case RDI_TypeKind_Complexdouble:{result = 16;}break;
case RDI_TypeKind_ComplexF80:{result = 20;}break;
case RDI_TypeKind_ComplexF128:{result = 32;}break;
}
return result;
}

RDI_PROC RDI_uint32
rdi_addr_size_from_arch(RDI_Arch arch)
{
RDI_uint32 result = 0;
switch(arch)
{
default:{}break;
case RDI_Arch_X86:{result = 4;}break;
case RDI_Arch_X64:{result = 8;}break;
}
return result;
}

RDI_PROC RDI_EvalConversionKind
rdi_eval_conversion_kind_from_typegroups(RDI_EvalTypeGroup in, RDI_EvalTypeGroup out)
{
  RDI_EvalConversionKind k = rdi_eval_typegroup_conversion_kind_matrix[in].dst_typegroups[out];
  return k;
}

RDI_PROC RDI_uint32
rdi_eval_op_typegroup_are_compatible(RDI_EvalOp op, RDI_EvalTypeGroup group)
{
  RDI_uint32 result = 0;
  switch(op)
  {
    case RDI_EvalOp_Neg: case RDI_EvalOp_Add: case RDI_EvalOp_Sub:
    case RDI_EvalOp_Mul: case RDI_EvalOp_Div:
    case RDI_EvalOp_EqEq:case RDI_EvalOp_NtEq:
    case RDI_EvalOp_LsEq:case RDI_EvalOp_GrEq:
    case RDI_EvalOp_Less:case RDI_EvalOp_Grtr:
    {
      if(group != RDI_EvalTypeGroup_Other)
      {
        result = 1;
      }
    }break;
    case RDI_EvalOp_Mod:case RDI_EvalOp_LShift:case RDI_EvalOp_RShift:
    case RDI_EvalOp_BitNot:case RDI_EvalOp_BitAnd:case RDI_EvalOp_BitXor:
    case RDI_EvalOp_BitOr:case RDI_EvalOp_LogNot:case RDI_EvalOp_LogAnd:
    case RDI_EvalOp_LogOr: 
    {
      if(group == RDI_EvalTypeGroup_S || group == RDI_EvalTypeGroup_U)
      {
        result = 1;
      }
    }break;
  }
  return result;
}

RDI_PROC RDI_uint8 *
rdi_explanation_string_from_eval_conversion_kind(RDI_EvalConversionKind kind, RDI_uint64 *size_out)
{
  *size_out = rdi_eval_conversion_kind_message_string_table[kind].size;
  return rdi_eval_conversion_kind_message_string_table[kind].str;
}

#endif // RDI_C

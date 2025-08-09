// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ Generated Code

#include "generated/codeview.meta.c"

////////////////////////////////

internal CV_Arch
cv_arch_from_coff_machine(COFF_MachineType machine)
{
  CV_Arch arch = 0;
  switch (machine)
  {
    case COFF_MachineType_X64:       arch = CV_Arch_X64;    break;
    case COFF_MachineType_X86:       arch = CV_Arch_8086;   break;
    case COFF_MachineType_Am33:      arch = CV_Arch_AM33;   break;
    case COFF_MachineType_Arm:       NotImplemented;        break;
    case COFF_MachineType_Arm64:     arch = CV_Arch_ARM64;  break;
    case COFF_MachineType_ArmNt:     arch = CV_Arch_ARMNT;  break;
    case COFF_MachineType_Ebc:       arch = CV_Arch_EBC;    break;
    case COFF_MachineType_Ia64:      arch = CV_Arch_IA64;   break;
    case COFF_MachineType_M32R:      arch = CV_Arch_M32R;   break;
    case COFF_MachineType_Mips16:    arch = CV_Arch_MIPS16; break;
    case COFF_MachineType_MipsFpu:   NotImplemented;        break;
    case COFF_MachineType_MipsFpu16: NotImplemented;        break;
    case COFF_MachineType_PowerPc:   NotImplemented;        break;
    case COFF_MachineType_PowerPcFp: arch = CV_Arch_PPCFP;  break;
    case COFF_MachineType_R4000:     NotImplemented;        break;
    case COFF_MachineType_RiscV32:   NotImplemented;        break;
    case COFF_MachineType_RiscV64:   NotImplemented;        break;
    case COFF_MachineType_RiscV128:  NotImplemented;        break;
    case COFF_MachineType_Sh3:       arch = CV_Arch_SH3;    break;
    case COFF_MachineType_Sh3Dsp:    arch = CV_Arch_SH3DSP; break;
    case COFF_MachineType_Sh4:       arch = CV_Arch_SH4;    break;
    case COFF_MachineType_Sh5:       NotImplemented;        break;
    case COFF_MachineType_Thumb:     arch = CV_Arch_THUMB;  break;
    case COFF_MachineType_WceMipsV2: NotImplemented;        break;
  }
  return arch;
}

internal U64
cv_size_from_reg(CV_Arch arch, CV_Reg reg)
{
  switch (arch)
  {
    case CV_Arch_8086: return cv_size_from_reg_x86(reg);
    case CV_Arch_X64 : return cv_size_from_reg_x64(reg);
    default: NotImplemented;
  }
  return 0;
}

internal B32
cv_is_reg_sp(CV_Arch arch, CV_Reg reg)
{
  switch (arch)
  {
    case CV_Arch_8086: return reg == CV_Regx86_ESP;
    case CV_Arch_X64:  return reg == CV_Regx64_RSP;
    default: NotImplemented;
  }
  return 0;
}

internal U64
cv_size_from_reg_x86(CV_Reg reg)
{
  switch (reg)
  {
#define X(NAME, CODE, RDI_NAME, BYTE_POS, BYTE_SIZE) case CV_Regx86_##NAME: return BYTE_SIZE;
    CV_Reg_X86_XList(X)
#undef X
  }
  return 0;
}

internal U64
cv_size_from_reg_x64(CV_Reg reg)
{
  switch (reg)
  {
#define X(NAME, CODE, RDI_NAME, BYTE_POS, BYTE_SIZE) case CV_Regx64_##NAME: return BYTE_SIZE;
    CV_Reg_X64_XList(X)
#undef X 
  }
  return 0;
}

internal CV_EncodedFramePtrReg
cv_pick_fp_encoding(CV_SymFrameproc* frameproc, B32 is_local_param)
{
  CV_EncodedFramePtrReg fp_reg = 0;
  if (is_local_param)
  {
    fp_reg = CV_FrameprocFlags_Extract_ParamBasePointer(frameproc.flags);
  }
  else
  {
    fp_reg = CV_FrameprocFlags_Extract_LocalBasePointer(frameproc.flags);
  }
  return fp_reg;
}

internal CV_Reg
cv_decode_fp_reg(CV_Arch arch, CV_EncodedFramePtrReg encoded_reg)
{
  CV_Reg fp_reg = 0;
  switch (arch)
  {
    case CV_Arch_8086:
    {
      switch (encoded_reg)
      {
        case CV_EncodedFramePtrReg_None    : break;
        case CV_EncodedFramePtrReg_StackPtr: AssertAlways(!"TODO: not tested, this is a guess");
        fp_reg = CV_Regx86_ESP; break;
        case CV_EncodedFramePtrReg_FramePtr: fp_reg = CV_Regx86_EBP; break;
        case CV_EncodedFramePtrReg_BasePtr : fp_reg = CV_Regx86_EBX; break;
        default: InvalidPath;
      }
    } break;
    case CV_Arch_X64:
    {
      switch (encoded_reg)
      {
        case CV_EncodedFramePtrReg_None    : break;
        case CV_EncodedFramePtrReg_StackPtr: fp_reg = CV_Regx64_RSP; break;
        case CV_EncodedFramePtrReg_FramePtr: fp_reg = CV_Regx64_RBP; break;
        case CV_EncodedFramePtrReg_BasePtr : fp_reg = CV_Regx64_R13; break;
        default: InvalidPath;
      }
    } break;
    default: NotImplemented;
  }
  return fp_reg;
}

internal U32
cv_map_encoded_base_pointer(CV_Arch arch, U32 encoded_frame_reg)
{
  U32 r = 0;
  switch (arch) {
    case CV_Arch_8086: {
      switch (encoded_frame_reg) {
        case 0: r = 0;                    break;
        case 1: r = CV_AllReg_VFRAME; break;
        case 2: r = CV_Regx86_EBP;    break;
        case 3: r = CV_Regx86_EBX;    break;
      }
    } break;
    case CV_Arch_X64: {
      switch (encoded_frame_reg) {
        case 0: r = 0; break;
        case 1: r = CV_Regx64_RSP; break;
        case 2: r = CV_Regx64_RBP; break;
        case 3: r = CV_Regx64_R13; break;
      }
    } break;
    default: NotImplemented;
  }
  return r;
}


////////////////////////////////
//~ rjf: Enum -> String

internal string
cv_string_from_inline_range_kind(CV_InlineRangeKind kind)
{
  switch (kind) {
    case CV_InlineRangeKind_Expr: return ("Expr");
    case CV_InlineRangeKind_Stmt: return ("Stmt");
  }
  return str8_zero();
}

internal string
cv_string_from_type_index_source(CV_TypeIndexSource ti_source)
{
  switch (ti_source) {
    case CV_TypeIndexSource_NULL:  return ("");    break;
    case CV_TypeIndexSource_TPI:   return ("TPI"); break;
    case CV_TypeIndexSource_IPI:   return ("IPI"); break;
    case CV_TypeIndexSource_COUNT: break;
  }
  return str8_zero();
}

internal string
cv_string_from_language(CV_Language x)
{
  switch (x) {
#define X(_n,_i) case _i: return (Stringify(_n));
    CV_LanguageXList(X)
#undef X
  }
  return str8_zero();
}

internal string 
cv_string_from_reg_id(Arena* arena, CV_Arch arch, U32 id)
{
  string result = str8_zero();
  switch (arch) {
    case CV_Arch_8086: {
      switch (id) {
#define X(_N, _ID, ...) case _ID: result = (Stringify(_N)); break;
        CV_Reg_X86_XList(X)
#undef X
      }
    } break;
    case CV_Arch_X64: {
      switch (id) {
#define X(_N, _ID, ...) case _ID: result = (Stringify(_N)); break;
        CV_Reg_X64_XList(X)
#undef X
      }
    } break;
    default: NotImplemented; break;
  }
  if (result.size == 0) {
    result = push_str8f(arena, "%x", id);
  }
  return result;
}

internal string
cv_string_from_member_access(CV_MemberAccess x)
{
  switch (x) {
    case CV_MemberAccess_Null:      break;
    case CV_MemberAccess_Private:   return ("Private");
    case CV_MemberAccess_Protected: return ("Protected");
    case CV_MemberAccess_Public:    return ("Public");
  }
  return str8_zero();
}

internal string
cv_string_from_method_prop(CV_MethodProp x)
{
  switch (x) {
    case CV_MethodProp_Vanilla:     return ("Vanilla");
    case CV_MethodProp_Virtual:     return ("Virtual");
    case CV_MethodProp_Static:      return ("Static");
    case CV_MethodProp_Friend:      return ("Friend");
    case CV_MethodProp_Intro:       return ("Intro");
    case CV_MethodProp_PureVirtual: return ("PureVirtual");
    case CV_MethodProp_PureIntro:   return ("PureIntro");
  }
  return str8_zero();
}

internal string
cv_string_from_hfa(CV_HFAKind x)
{
  switch (x) {
    case CV_HFAKind_None:   return ("None");
    case CV_HFAKind_Float:  return ("Float");
    case CV_HFAKind_Double: return ("Double");
    case CV_HFAKind_Other:  return ("Other");
  }
  return str8_zero();
}

internal string
cv_string_from_mcom(CV_MoComUDTKind x)
{
  switch (x) {
    case CV_MoComUDTKind_None:      return ("None");
    case CV_MoComUDTKind_Ref:       return ("Ref");
    case CV_MoComUDTKind_Value:     return ("Value");
    case CV_MoComUDTKind_Interface: return ("Interface");
  }
  return str8_zero();
}

internal string
cv_string_from_binary_opcode(CV_InlineBinaryAnnotation x)
{
  switch (x) {
    case CV_InlineBinaryAnnotation_Null:                          break;
    case CV_InlineBinaryAnnotation_CodeOffset:                    return ("CodeOffset");
    case CV_InlineBinaryAnnotation_ChangeCodeOffsetBase:          return ("ChangeCodeOffsetBase");
    case CV_InlineBinaryAnnotation_ChangeCodeOffset:              return ("ChangeCodeOffset");
    case CV_InlineBinaryAnnotation_ChangeCodeLength:              return ("ChangeCodeLength");
    case CV_InlineBinaryAnnotation_ChangeFile:                    return ("ChangeFile");
    case CV_InlineBinaryAnnotation_ChangeLineOffset:              return ("ChangeLineOffset");
    case CV_InlineBinaryAnnotation_ChangeLineEndDelta:            return ("ChangeLineEndDelta");
    case CV_InlineBinaryAnnotation_ChangeRangeKind:               return ("ChangeRangeKind");
    case CV_InlineBinaryAnnotation_ChangeColumnStart:             return ("ChangeColumnStart");
    case CV_InlineBinaryAnnotation_ChangeColumnEndDelta:          return ("ChangeColumnEndDelta");
    case CV_InlineBinaryAnnotation_ChangeCodeOffsetAndLineOffset: return ("ChangeCodeOffsetAndLineOffset");
    case CV_InlineBinaryAnnotation_ChangeCodeLengthAndCodeOffset: return ("ChangeCodeLengthAndCodeOffset");
    case CV_InlineBinaryAnnotation_ChangeColumnEnd:               return ("ChangeColumnEnd");
  }
  return str8_zero();
}

internal string
cv_string_from_thunk_ordinal(CV_ThunkOrdinal x)
{
  switch (x) {
    case CV_ThunkOrdinal_NoType:            return ("NoType");
    case CV_ThunkOrdinal_Adjustor:          return ("Adjustor");
    case CV_ThunkOrdinal_VCall:             return ("VCall");
    case CV_ThunkOrdinal_PCode:             return ("PCode");
    case CV_ThunkOrdinal_Load:              return ("Load");
    case CV_ThunkOrdinal_TrampIncremental:  return ("TrampIncremental");
    case CV_ThunkOrdinal_TrampBranchIsland: return ("TrampBranchIsland");
  }
  return str8_zero();
}

internal string
cv_string_from_frame_cookie_kind(CV_FrameCookieKind x)
{
  switch (x) {
    case CV_FrameCookieKind_Copy:  return ("Copy");
    case CV_FrameCookieKind_XorSP: return ("XorSP");
    case CV_FrameCookieKind_XorBP: return ("XorR13");
  }
  return str8_zero();
}

internal string
cv_string_from_generic_style(CV_GenericStyle x)
{
  switch (x) {
    case CV_GenericStyle_VOID:   return ("VOID");
    case CV_GenericStyle_REG:    return ("REG");
    case CV_GenericStyle_ICAN:   return ("ICAN");
    case CV_GenericStyle_ICAF:   return ("ICAF");
    case CV_GenericStyle_IRAN:   return ("IRAN");
    case CV_GenericStyle_IRAF:   return ("IRAF");
    case CV_GenericStyle_UNUSED: return ("UNUSED");
  }
  return str8_zero();
}

internal string
cv_string_from_trampoline_kind(CV_TrampolineKind x)
{
  switch (x) {
    case CV_TrampolineKind_Incremental:  return ("Incremental");
    case CV_TrampolineKind_BranchIsland: return ("BranchIsland");
  }
  return str8_zero();
}

internal string
cv_string_from_virtual_table_shape_kind(CV_VirtualTableShape x)
{
  switch (x) {
    case CV_VirtualTableShape_Near:   return ("Near");
    case CV_VirtualTableShape_Far:    return ("Far");
    case CV_VirtualTableShape_Thin:   return ("Thin");
    case CV_VirtualTableShape_Outer:  return ("Outer");
    case CV_VirtualTableShape_Meta:   return ("Meta");
    case CV_VirtualTableShape_Near32: return ("Near32");
    case CV_VirtualTableShape_Far32:  return ("Far32");
  }
  return str8_zero();
}

internal string
cv_string_from_call_kind(CV_CallKind x)
{
  switch (x) {
    case CV_CallKind_NearC:          return ("NearC");
    case CV_CallKind_FarC:           return ("FarC");
    case CV_CallKind_NearPascal:     return ("NearPascal");
    case CV_CallKind_FarPascal:      return ("FarPascal");
    case CV_CallKind_NearFast:       return ("NearFast");
    case CV_CallKind_FarFast:        return ("FarFast");
    case CV_CallKind_UNUSED:         return ("UNUSED");
    case CV_CallKind_NearStd:        return ("NearStd");
    case CV_CallKind_FarStd:         return ("FarStd");
    case CV_CallKind_NearSys:        return ("NearSys");
    case CV_CallKind_FarSys:         return ("FarSys");
    case CV_CallKind_This:           return ("This");
    case CV_CallKind_Mips:           return ("Mips");
    case CV_CallKind_Generic:        return ("Generic");
    case CV_CallKind_Alpha:          return ("Alpha");
    case CV_CallKind_PPC:            return ("PPC");
    case CV_CallKind_HitachiSuperH:  return ("HitachiSuperH");
    case CV_CallKind_Arm:            return ("Arm");
    case CV_CallKind_AM33:           return ("AM33");
    case CV_CallKind_TriCore:        return ("TriCore");
    case CV_CallKind_HitachiSuperH5: return ("HitachiSuperH5");
    case CV_CallKind_M32R:           return ("M32R");
    case CV_CallKind_Clr:            return ("Clr");
    case CV_CallKind_Inline:         return ("Inline");
    case CV_CallKind_NearVector:     return ("NearVector");
  }
  return str8_zero();
}

internal string
cv_string_from_member_pointer_kind(CV_MemberPointerKind x)
{
  switch (x) {
    case CV_MemberPointerKind_Undef:        return ("Undef");
    case CV_MemberPointerKind_DataSingle:   return ("DataSingle");
    case CV_MemberPointerKind_DataMultiple: return ("DataMultiple");
    case CV_MemberPointerKind_DataVirtual:  return ("DataVirtual");
    case CV_MemberPointerKind_DataGeneral:  return ("DataGeneral");
    case CV_MemberPointerKind_FuncSingle:   return ("FuncSingle");
    case CV_MemberPointerKind_FuncMultiple: return ("FuncMultiple");
    case CV_MemberPointerKind_FuncGeneral:  return ("FuncGeneral");
  }
  return str8_zero();
}

internal string
cv_string_from_pointer_kind(CV_PointerKind x)
{
  switch (x) {
    case CV_PointerKind_Near:        return ("Near");
    case CV_PointerKind_Far:         return ("Far");
    case CV_PointerKind_Huge:        return ("Huge");
    case CV_PointerKind_BaseSeg:     return ("BaseSeg");
    case CV_PointerKind_BaseVal:     return ("BaseVal");
    case CV_PointerKind_BaseSegVal:  return ("BaseSegVal");
    case CV_PointerKind_BaseAddr:    return ("BaseAddr");
    case CV_PointerKind_BaseSegAddr: return ("BaseSegAddr");
    case CV_PointerKind_BaseType:    return ("BaseType");
    case CV_PointerKind_BaseSelf:    return ("BaseSelf");
    case CV_PointerKind_Near32:      return ("Near32");
    case CV_PointerKind_Far32:       return ("Far32");
    case CV_PointerKind_64:          return ("64Bit");
  }
  return str8_zero();
}

internal string
cv_string_from_pointer_mode(CV_PointerMode x)
{
  switch (x) {
    case CV_PointerMode_Ptr:       return ("Ptr");
    case CV_PointerMode_LRef:      return ("LRef");
    case CV_PointerMode_PtrMem:    return ("PtrMem");
    case CV_PointerMode_PtrMethod: return ("PtrMethod");
    case CV_PointerMode_RRef:      return ("RRef");
  }
  return str8_zero();
}

internal string
cv_string_from_c13_checksum_kind(CV_C13ChecksumKind x)
{
  switch (x) {
    case CV_C13ChecksumKind_Null:   break;
    case CV_C13ChecksumKind_MD5:    return ("MD5");
    case CV_C13ChecksumKind_SHA1:   return ("SHA1");
    case CV_C13ChecksumKind_SHA256: return ("SHA256");
  }
  return str8_zero();
}

internal string
cv_string_from_label_kind(Arena* arena, CV_LabelKind x)
{
  switch (x) {
    case CV_LabelKind_Near: return ("Near");
    case CV_LabelKind_Far:  return ("Far");
  }
  return push_str8f(arena, "%#x", x);
}

internal string
cv_string_from_c13_subsection_kind(CV_C13SubSectionKind x)
{
  switch (x) {
#define X(_N, _ID) case CV_C13SubSectionKind_##_N: return (Stringify(_N));
    CV_C13SubSectionKindXList(X)
#undef X
  }
  return str8_zero();
}

internal string
cv_string_from_modifier_flags(Arena* arena, CV_ModifierFlags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_ModifierFlag_Const) {
    str8_list_pushf(scratch.arena, &list, "Const");
  }
  if (x & CV_ModifierFlag_Volatile) {
    str8_list_pushf(scratch.arena, &list, "Volatile");
  }
  if (x & CV_ModifierFlag_Unaligned) {
    str8_list_pushf(scratch.arena, &list, "Unaligned");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_pointer_attribs(Arena* arena, CV_PointerAttribs x)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  String8List list = {0};
  if (x & CV_PointerAttrib_IsFlat) {
    x &= ~CV_PointerAttrib_IsFlat;
    str8_list_pushf(scratch.arena, &list, "IsFlat");
  }
  if (x & CV_PointerAttrib_Volatile) {
    x &= ~CV_PointerAttrib_Volatile;
    str8_list_pushf(scratch.arena, &list, "Volatile");
  }
  if (x & CV_PointerAttrib_Const) {
    x &= ~CV_PointerAttrib_Const;
    str8_list_pushf(scratch.arena, &list, "Const");
  }
  if (x & CV_PointerAttrib_Unaligned) {
    x &= ~CV_PointerAttrib_Unaligned;
    str8_list_pushf(scratch.arena, &list, "Unaligned");
  }
  if (x & CV_PointerAttrib_Restricted) {
    x &= ~CV_PointerAttrib_Restricted;
    str8_list_pushf(scratch.arena, &list, "Restricted");
  }
  if (x & CV_PointerAttrib_MOCOM) {
    x &= ~CV_PointerAttrib_MOCOM;
    str8_list_pushf(scratch.arena, &list, "MOCOM");
  }
  if (x & CV_PointerAttrib_LRef) {
    x &= ~CV_PointerAttrib_LRef;
    str8_list_pushf(scratch.arena, &list, "LRef");
  }
  if (x & CV_PointerAttrib_RRef) {
    x &= ~CV_PointerAttrib_RRef;
    str8_list_pushf(scratch.arena, &list, "RRef");
  }
  
  CV_PointerKind kind = CV_PointerAttribs_Extract_Kind(x);
  CV_PointerMode mode = CV_PointerAttribs_Extract_Mode(x);
  U64            size = CV_PointerAttribs_Extract_Size(x);
  
  x &= ~(0x1f|(0x7<<5)|(0x3f<<13));
  
  if (kind) {
    string kind_str = cv_string_from_pointer_kind(kind);
    str8_list_pushf(scratch.arena, &list, "Kind=%S", kind_str);
  }
  if (mode) {
    string mode_str = cv_string_from_pointer_mode(mode);
    str8_list_pushf(scratch.arena, &list, "Mode=%S", mode_str);
  }
  if (size) {
    str8_list_pushf(scratch.arena, &list, "Size=%llu", size);
  }
  
  if (x != 0) {
    str8_list_pushf(scratch.arena, &list, "Unknown=%x", x);
  }
  
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_function_attribs(Arena* arena, CV_FunctionAttribs x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_FunctionAttrib_CxxReturnUDT) {
    str8_list_pushf(scratch.arena, &list, "CxxReturnUDT");
  }
  if (x & CV_FunctionAttrib_Constructor) {
    str8_list_pushf(scratch.arena, &list, "Constructor");
  }
  if (x & CV_FunctionAttrib_ConstructorVBase) {
    str8_list_pushf(scratch.arena, &list, "ConstructorVBase");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_export_flags(Arena* arena, CV_ExportFlags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_ExportFlag_Constant) {
    str8_list_pushf(scratch.arena, &list, "Constant");
  }
  if (x & CV_ExportFlag_Data) {
    str8_list_pushf(scratch.arena, &list, "Data");
  }
  if (x & CV_ExportFlag_Private) {
    str8_list_pushf(scratch.arena, &list, "Private");
  }
  if (x & CV_ExportFlag_NoName) {
    str8_list_pushf(scratch.arena, &list, "NoName");
  }
  if (x & CV_ExportFlag_Ordinal) {
    str8_list_pushf(scratch.arena, &list, "Ordinal");
  }
  if (x & CV_ExportFlag_Forwarder) {
    str8_list_pushf(scratch.arena, &list, "Forwarder");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_sepcode(Arena* arena, CV_SepcodeFlags x)
{
  Temp scratch = scratch_begin(&arena,1);
  String8List list = {0};
  if (x & CV_SepcodeFlag_IsLexicalScope) {
    str8_list_pushf(scratch.arena, &list, "IsLexicalScope");
  }
  if (x & CV_SepcodeFlag_ReturnsToParent) {
    str8_list_pushf(scratch.arena, &list, "ReturnsToParent");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_pub32_flags(Arena* arena, CV_Pub32Flags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_Pub32Flag_Code) {
    str8_list_pushf(scratch.arena, &list, "Code");
  }
  if (x & CV_Pub32Flag_Function) {
    str8_list_pushf(scratch.arena, &list, "Function");
  }
  if (x & CV_Pub32Flag_ManagedCode) {
    str8_list_pushf(scratch.arena, &list, "ManagedCode");
  }
  if (x & CV_Pub32Flag_MSIL) {
    str8_list_pushf(scratch.arena, &list, "MSIL");
  }
  string result = str8_list_join(scratch.arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_generic_flags(Arena* arena, CV_GenericFlags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_GenericFlags_CSTYLE) {
    str8_list_pushf(scratch.arena, &list, "CSTYLE");
  }
  if (x & CV_GenericFlags_RSCLEAN) {
    str8_list_pushf(scratch.arena, &list, "RSCLEAN");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_frame_proc_flags(Arena* arena, CV_FrameprocFlags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_FrameprocFlag_UsesAlloca) {
    str8_list_pushf(scratch.arena, &list, "UsesAlloca");
  }
  if (x & CV_FrameprocFlag_UsesSetJmp) {
    str8_list_pushf(scratch.arena, &list, "UsesSetJmp");
  }
  if (x & CV_FrameprocFlag_UsesLongJmp) {
    str8_list_pushf(scratch.arena, &list, "UsesLongJmp");
  }
  if (x & CV_FrameprocFlag_UsesInlAsm) {
    str8_list_pushf(scratch.arena, &list, "UsesInlAsm");
  }
  if (x & CV_FrameprocFlag_UsesEH) {
    str8_list_pushf(scratch.arena, &list, "UsesEH");
  }
  if (x & CV_FrameprocFlag_Inline) {
    str8_list_pushf(scratch.arena, &list, "Inline");
  }
  if (x & CV_FrameprocFlag_HasSEH) {
    str8_list_pushf(scratch.arena, &list, "HasSEH");
  }
  if (x & CV_FrameprocFlag_Naked) {
    str8_list_pushf(scratch.arena, &list, "Naked");
  }
  if (x & CV_FrameprocFlag_HasSecurityChecks) {
    str8_list_pushf(scratch.arena, &list, "HasSecurityChecks");
  }
  if (x & CV_FrameprocFlag_AsyncEH) {
    str8_list_pushf(scratch.arena, &list, "AsyncEH");
  }
  if (x & CV_FrameprocFlag_GSNoStackOrdering) {
    str8_list_pushf(scratch.arena, &list, "GSNoStackOrdering");
  }
  if (x & CV_FrameprocFlag_WasInlined) {
    str8_list_pushf(scratch.arena, &list, "WasInlined");
  }
  if (x & CV_FrameprocFlag_GSCheck) {
    str8_list_pushf(scratch.arena, &list, "GSCheck");
  }
  if (x & CV_FrameprocFlag_SafeBuffers) {
    str8_list_pushf(scratch.arena, &list, "SafeBuffers");
  }
  if (x & CV_FrameprocFlag_PogoOn) {
    str8_list_pushf(scratch.arena, &list, "PogoOn");
  }
  if (x & CV_FrameprocFlag_PogoCountsValid) {
    str8_list_pushf(scratch.arena, &list, "PogoCountsValid");
  }
  if (x & CV_FrameprocFlag_OptSpeed) {
    str8_list_pushf(scratch.arena, &list, "OptSpeed");
  }
  if (x & CV_FrameprocFlag_HasCFG) {
    str8_list_pushf(scratch.arena, &list, "HasCFG");
  }
  if (x & CV_FrameprocFlag_HasCFW) {
    str8_list_pushf(scratch.arena, &list, "HasCFW");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_type_props(Arena* arena, CV_TypeProps32 x)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  U32 hfa  = CV_TypeProps_Extract_HFA(x);
  U32 mcom = CV_TypeProps_Extract_MOCOM(x);
  
  string hfa_str  = cv_string_from_hfa(hfa);
  string mcom_str = cv_string_from_mcom(mcom);
  
  string flags_str;
  {
    String8List list = {0};
    if (x & CV_TypeProp_Packed) {
      x &= ~CV_TypeProp_Packed;
      str8_list_pushf(scratch.arena, &list, "Packed");
    }
    if (x & CV_TypeProp_HasConstructorsDestructors) {
      x &= ~CV_TypeProp_HasConstructorsDestructors;
      str8_list_pushf(scratch.arena, &list, "HasConstructorsDestructors");
    }
    if (x & CV_TypeProp_OverloadedOperators) {
      x &= ~CV_TypeProp_OverloadedOperators;
      str8_list_pushf(scratch.arena, &list, "OverloadedOperators");
    }
    if (x & CV_TypeProp_IsNested) {
      x &= ~CV_TypeProp_IsNested;
      str8_list_pushf(scratch.arena, &list, "IsNested");
    }
    if (x & CV_TypeProp_ContainsNested) {
      x &= ~CV_TypeProp_ContainsNested;
      str8_list_pushf(scratch.arena, &list, "ContainsNested");
    }
    if (x & CV_TypeProp_OverloadedAssignment) {
      x &= ~CV_TypeProp_OverloadedAssignment;
      str8_list_pushf(scratch.arena, &list, "OverloadedAssignment");
    }
    if (x & CV_TypeProp_OverloadedCasting) {
      x &= ~CV_TypeProp_OverloadedCasting;
      str8_list_pushf(scratch.arena, &list, "OverloadedCasting");
    }
    if (x & CV_TypeProp_FwdRef) {
      x &= ~CV_TypeProp_FwdRef;
      str8_list_pushf(scratch.arena, &list, "FwdRef");
    }
    if (x & CV_TypeProp_Scoped) {
      x &= ~CV_TypeProp_Scoped;
      str8_list_pushf(scratch.arena, &list, "Scoped");
    }
    if (x & CV_TypeProp_HasUniqueName) {
      x &= ~CV_TypeProp_HasUniqueName;
      str8_list_pushf(scratch.arena, &list, "HasUniqueName");
    }
    if (x & CV_TypeProp_Sealed) {
      x &= ~CV_TypeProp_Sealed;
      str8_list_pushf(scratch.arena, &list, "Sealed");
    }
    if (x & CV_TypeProp_Intrinsic) {
      x &= ~CV_TypeProp_Intrinsic;
      str8_list_pushf(scratch.arena, &list, "Intrinsic");
    }
    if (x != 0) {
      str8_list_pushf(scratch.arena, &list, "%x", x);
    }
    flags_str = str8_list_join(scratch.arena, &list, &(StringJoin){.sep=(", ") });
    
    if (hfa) {
      str8_list_pushf(scratch.arena, &list, "HFA = %S", hfa_str);
    }
    if (mcom) {
      str8_list_pushf(scratch.arena, &list, "MCOM = %S", mcom_str);
    }
  }
  
  string result = push_str8f(arena, "%S", flags_str);
  
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_local_flags(Arena* arena, CV_LocalFlags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_LocalFlag_Param) {
    str8_list_pushf(scratch.arena, &list, "Param");
  }
  if (x & CV_LocalFlag_AddrTaken) {
    str8_list_pushf(scratch.arena, &list, "AddrTaken");
  }
  if (x & CV_LocalFlag_Compgen) {
    str8_list_pushf(scratch.arena, &list, "Compgen");
  }
  if (x & CV_LocalFlag_Aggregate) {
    str8_list_pushf(scratch.arena, &list, "Aggregate");
  }
  if (x & CV_LocalFlag_PartOfAggregate) {
    str8_list_pushf(scratch.arena, &list, "PartOfAggregate");
  }
  if (x & CV_LocalFlag_Aliased) {
    str8_list_pushf(scratch.arena, &list, "Aliased");
  }
  if (x & CV_LocalFlag_Alias) {
    str8_list_pushf(scratch.arena, &list, "Alias");
  }
  if (x & CV_LocalFlag_Retval) {
    str8_list_pushf(scratch.arena, &list, "Retval");
  }
  if (x & CV_LocalFlag_OptOut) {
    str8_list_pushf(scratch.arena, &list, "OptOut");
  }
  if (x & CV_LocalFlag_Global) {
    str8_list_pushf(scratch.arena, &list, "Global");
  }
  if (x & CV_LocalFlag_Static) {
    str8_list_pushf(scratch.arena, &list, "Static");
  }
  string result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_proc_flags(Arena* arena, CV_ProcFlags x)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (x & CV_ProcFlag_NoFPO) {
    x &= ~CV_ProcFlag_NoFPO;
    str8_list_pushf(scratch.arena, &list, "NoFPO");
  }
  if (x & CV_ProcFlag_IntReturn) {
    x &= ~CV_ProcFlag_IntReturn;
    str8_list_pushf(scratch.arena, &list, "IntReturn");
  }
  if (x & CV_ProcFlag_FarReturn) {
    x &= ~CV_ProcFlag_FarReturn;
    str8_list_pushf(scratch.arena, &list, "FarReturn");
  }
  if (x & CV_ProcFlag_NeverReturn) {
    x &= ~CV_ProcFlag_NeverReturn;
    str8_list_pushf(scratch.arena, &list, "NeverReturn");
  }
  if (x & CV_ProcFlag_NotReached) {
    x &= ~CV_ProcFlag_NotReached;
    str8_list_pushf(scratch.arena, &list, "NotReached");
  }
  if (x & CV_ProcFlag_CustomCall) {
    x &= ~CV_ProcFlag_CustomCall;
    str8_list_pushf(scratch.arena, &list, "CustomCall");
  }
  if (x & CV_ProcFlag_NoInline) {
    x &= ~CV_ProcFlag_NoInline;
    str8_list_pushf(scratch.arena, &list, "NoInline");
  }
  if (x & CV_ProcFlag_OptDbgInfo) {
    x &= ~CV_ProcFlag_OptDbgInfo;
    str8_list_pushf(scratch.arena, &list, "OptDbgInfo");
  }
  if (x != 0) {
    str8_list_pushf(scratch.arena, &list, "%#x", x);
  }
  string result;
  if (list.node_count == 0) {
    result = ("None");
  } else {
    result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});
  }
  temp_end(scratch);
  return result;
}

internal string
cv_string_from_range_attribs(Arena* arena, CV_RangeAttribs x)
{ (void)arena;
  string result = ("None");
  if (x == CV_RangeAttrib_Maybe) {
    result = ("Maybe");
  }
  return result;
}

internal string
cv_string_from_defrange_register_rel_flags(Arena* arena, CV_DefrangeRegisterRelFlags x)
{ (void)arena;
  string result = ("None");
  if (x == CV_DefrangeRegisterRelFlag_SpilledOutUDTMember) {
    result = ("SpilledOutUDTMember");
  }
  return result;
}

internal string
cv_string_from_field_attribs(Arena* arena, CV_FieldAttribs attribs)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  U32 access = CV_FieldAttribs_Extract_Access(attribs);
  U32 mprop  = CV_FieldAttribs_Extract_MethodProp(attribs);
  attribs &= ~(0x3 | 0x7);
  
  string access_str = cv_string_from_member_access(access);
  string mprop_str  = cv_string_from_method_prop(mprop);
  
  String8List list = {0};
  {
    if (attribs & CV_FieldAttrib_Pseudo) {
      attribs &= ~CV_FieldAttrib_Pseudo;
      str8_list_pushf(scratch.arena, &list, "Pseudo");
    }
    if (attribs & CV_FieldAttrib_NoInherit) {
      attribs &= ~CV_FieldAttrib_NoInherit;
      str8_list_pushf(scratch.arena, &list, "NoInherit");
    }
    if (attribs & CV_FieldAttrib_NoConstruct) {
      attribs &= ~CV_FieldAttrib_NoConstruct;
      str8_list_pushf(scratch.arena, &list, "NoConstruct");
    }
    if (attribs & CV_FieldAttrib_CompilerGenated) {
      attribs &= ~CV_FieldAttrib_CompilerGenated;
      str8_list_pushf(scratch.arena, &list, "CompilerGenerated");
    }
    if (attribs & CV_FieldAttrib_Sealed) {
      attribs &= ~CV_FieldAttrib_Sealed;
      str8_list_pushf(scratch.arena, &list, "Sealed");
    }
    if (attribs) {
      str8_list_pushf(scratch.arena, &list, "Unknown: %x", attribs);
    }
  }
  
  if (access) {
    str8_list_pushf(scratch.arena, &list, "%S", access_str);
  }
  if (mprop) {
    str8_list_pushf(scratch.arena, &list, "%S", mprop_str);
  }
  
  string result = str8_list_join(scratch.arena, &list, &(StringJoin){.sep=(", ")});
  
  scratch_end(scratch);
  return result;
}

internal string
cv_string_from_itype(Arena* arena, CV_TypeIndex min_itype, CV_TypeIndex itype)
{
  string result = str8_zero();
  if (itype < min_itype) {
    string n = cv_type_name_from_basic_type((CV_BasicType)itype);
    if (n.size) {
      Temp scratch = scratch_begin(&arena, 1);
      
      U64 type = CV_BasicTypeFromTypeId(itype);
      char *type_str = "???";
      switch (type) {
        case CV_BasicType_NOTYPE:     type_str = "NOTYPE";     break;
        case CV_BasicType_ABS:        type_str = "ABS";        break;
        case CV_BasicType_SEGMENT:    type_str = "SEGMENT";    break;
        case CV_BasicType_VOID:       type_str = "VOID";       break;
        case CV_BasicType_CURRENCY:   type_str = "CURRENCY";   break;
        case CV_BasicType_NBASICSTR:  type_str = "NBASICSTR";  break;
        case CV_BasicType_FBASICSTR:  type_str = "FBASICSTR";  break;
        case CV_BasicType_NOTTRANS:   type_str = "NOTTRANS";   break;
        case CV_BasicType_HRESULT:    type_str = "HRESULT";    break;
        case CV_BasicType_CHAR:       type_str = "CHAR";       break;
        case CV_BasicType_SHORT:      type_str = "SHORT";      break;
        case CV_BasicType_LONG:       type_str = "LONG";       break;
        case CV_BasicType_QUAD:       type_str = "QUAD";       break;
        case CV_BasicType_OCT:        type_str = "OCT";        break;
        case CV_BasicType_UCHAR:      type_str = "UCHAR";      break;
        case CV_BasicType_USHORT:     type_str = "USHORT";     break;
        case CV_BasicType_ULONG:      type_str = "ULONG";      break;
        case CV_BasicType_UQUAD:      type_str = "UQUAD";      break;
        case CV_BasicType_UOCT:       type_str = "UOCT";       break;
        case CV_BasicType_BOOL8:      type_str = "BOOL8";      break;
        case CV_BasicType_BOOL16:     type_str = "BOOL16";     break;
        case CV_BasicType_BOOL32:     type_str = "BOOL32";     break;
        case CV_BasicType_BOOL64:     type_str = "BOOL64";     break;
        case CV_BasicType_FLOAT32:    type_str = "FLOAT32";    break;
        case CV_BasicType_FLOAT64:    type_str = "FLOAT64";    break;
        case CV_BasicType_FLOAT80:    type_str = "FLOAT80";    break;
        case CV_BasicType_FLOAT128:   type_str = "FLOAT128";   break;
        case CV_BasicType_FLOAT48:    type_str = "FLOAT48";    break;
        case CV_BasicType_FLOAT32PP:  type_str = "FLOAT32PP";  break;
        case CV_BasicType_FLOAT16:    type_str = "FLOAT16";    break;
        case CV_BasicType_COMPLEX32:  type_str = "COMPLEX32";  break;
        case CV_BasicType_COMPLEX64:  type_str = "COMPLEX64";  break;
        case CV_BasicType_COMPLEX80:  type_str = "COMPLEX80";  break;
        case CV_BasicType_COMPLEX128: type_str = "COMPLEX128"; break;
        case CV_BasicType_BIT:        type_str = "BIT";        break;
        case CV_BasicType_PASCHAR:    type_str = "PASCHAR";    break;
        case CV_BasicType_BOOL32FF:   type_str = "BOOL32FF";   break;
        case CV_BasicType_INT8:       type_str = "INT8";       break;
        case CV_BasicType_UINT8:      type_str = "UINT8";      break;
        case CV_BasicType_RCHAR:      type_str = "RCHAR";      break;
        case CV_BasicType_WCHAR:      type_str = "WCHAR";      break;
        case CV_BasicType_INT16:      type_str = "INT16";      break;
        case CV_BasicType_UINT16:     type_str = "UINT16";     break;
        case CV_BasicType_INT32:      type_str = "INT32";      break;
        case CV_BasicType_UINT32:     type_str = "UINT32";     break;
        case CV_BasicType_INT64:      type_str = "INT64";      break;
        case CV_BasicType_UINT64:     type_str = "UINT64";     break;
        case CV_BasicType_INT128:     type_str = "INT128";     break;
        case CV_BasicType_UINT128:    type_str = "UINT128";    break;
        case CV_BasicType_CHAR16:     type_str = "CHAR16";     break;
        case CV_BasicType_CHAR32:     type_str = "CHAR32";     break;
        case CV_BasicType_CHAR8:      type_str = "CHAR8";      break;
        case CV_BasicType_PTR:        type_str = "PTR";        break;
      }
      
      U64 ptr = CV_BasicPointerKindFromTypeId(itype);
      char *ptr_str = "";
      switch (ptr) {
        case 0x1: ptr_str = "P";    break;
        case 0x2: ptr_str = "PF";   break;
        case 0x3: ptr_str = "PH";   break;
        case 0x4: ptr_str = "32P";  break;
        case 0x5: ptr_str = "32PF"; break;
        case 0x6: ptr_str = "64P";  break;
      }
      
      n = upper_from_str8(scratch.arena, n);
      result = push_str8f(arena, "T_%s%s(%x)", ptr_str, type_str, itype);
      scratch_end(scratch);
    } else {
      result = push_str8f(arena, "%x", itype);
    }
  } else {
    result = push_str8f(arena, "%x", itype);
  }
  return result;
}

internal string
cv_string_from_itemid(Arena* arena, CV_ItemId itemid)
{
  string result = push_str8f(arena, "%x", itemid);
  return result;
}

internal string
cv_string_from_symbol_type(Arena* arena, CV_SymKind symbol_type)
{
  string str    = cv_string_from_sym_kind(symbol_type);
  string result = push_str8f(arena, "S_%S", str);
  return result;
}

internal string
cv_string_from_symbol_kind(Arena* arena, CV_SymKind kind)
{
  string str    = cv_string_from_sym_kind(kind);
  string result = push_str8f(arena, "S_%S", str);
  return result;
}

internal string
cv_string_from_leaf_name(Arena* arena, U32 leaf_type)
{
  string str    = cv_string_from_leaf_kind(leaf_type);
  string result = push_str8f(arena, "LF_%S", str);
  return result;
}

internal string 
cv_string_sec_off(Arena* arena, U32 sec, U32 off)
{
  return push_str8f(arena, "%04x:%08x", sec, off);
}


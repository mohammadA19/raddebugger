// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

StringView
coff_string_from_time_stamp(Arena* arena, COFF_TimeStamp time_stamp)
{
  StringView result;
  if (time_stamp == 0) {
    result = ("0");
  } else if (time_stamp >= max_U32) {
    result = ("-1");
  } else {
    DateTime dt = date_time_from_unix_time(time_stamp);
    result = push_date_time_string(arena, &dt);
  }
  return result;
}

read_only struct
{
  StringView          string;
  COFF_MachineType machine;
} g_coff_machine_map[] = {
  { (""),          COFF_Machine_Unknown   },
  { ("X86"),       COFF_Machine_X86       },
  { ("Amd64"),     COFF_Machine_X64       },
  { ("X64"),       COFF_Machine_X64       },
  { ("Am33"),      COFF_Machine_Am33      },
  { ("Arm"),       COFF_Machine_Arm       },
  { ("Arm64"),     COFF_Machine_Arm64     },
  { ("ArmNt"),     COFF_Machine_ArmNt     },
  { ("Ebc"),       COFF_Machine_Ebc       },
  { ("Ia64"),      COFF_Machine_Ia64      },
  { ("M32r"),      COFF_Machine_M32R      },
  { ("Mips16"),    COFF_Machine_Mips16    },
  { ("MipsFpu"),   COFF_Machine_MipsFpu   },
  { ("MipsFpu16"), COFF_Machine_MipsFpu16 },
  { ("PowerPc"),   COFF_Machine_PowerPc   },
  { ("PowerPcFp"), COFF_Machine_PowerPcFp },
  { ("R4000"),     COFF_Machine_R4000     },
  { ("RiscV32"),   COFF_Machine_RiscV32   },
  { ("RiscV64"),   COFF_Machine_RiscV64   },
  { ("Sh3"),       COFF_Machine_Sh3       },
  { ("Sh3Dsp"),    COFF_Machine_Sh3Dsp    },
  { ("Sh4"),       COFF_Machine_Sh4       },
  { ("Sh5"),       COFF_Machine_Sh5       },
  { ("Thumb"),     COFF_Machine_Thumb     },
  { ("WceMipsV2"), COFF_Machine_WceMipsV2 },
};

read_only static struct {
  char *                name;
  COFF_ImportType type;
} g_coff_import_header_type_map[] = {
  { "Code",  COFF_ImportHeader_Code  },
  { "Data",  COFF_ImportHeader_Data  },
  { "Const", COFF_ImportHeader_Const },
};

StringView
coff_string_from_comdat_select_type(COFF_ComdatSelectType type)
{
  StringView result = StringView();
  switch (type) {
  case COFF_ComdatSelect_Null:         result = ("Null");         break;
  case COFF_ComdatSelect_NoDuplicates: result = ("NoDuplicates"); break;
  case COFF_ComdatSelect_Any:          result = ("Any");          break;
  case COFF_ComdatSelect_SameSize:     result = ("SameSize");     break;
  case COFF_ComdatSelect_ExactMatch:   result = ("ExactMatch");   break;
  case COFF_ComdatSelect_Associative:  result = ("Associative");  break;
  case COFF_ComdatSelect_Largest:      result = ("Largest");      break;
  }
  return result;
}

StringView
coff_string_from_machine_type(COFF_MachineType machine)
{
  for (uint64 i = 0; i < ArrayCount(g_coff_machine_map); ++i) {
    if (g_coff_machine_map[i].machine == machine) {
      return g_coff_machine_map[i].string;
    }
  }
  return StringView();
}

StringView
coff_string_from_flags(Arena* arena, COFF_FileHeaderFlags flags)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};

  if (flags & COFF_FileHeaderFlag_RelocStripped) {
    str8_list_pushf(scratch.arena, &list, "Relocs Stripped");
  }
  if (flags & COFF_FileHeaderFlag_ExecutableImage) {
    str8_list_pushf(scratch.arena, &list, "Executable");
  }
  if (flags & COFF_FileHeaderFlag_LineNumbersStripped) {
    str8_list_pushf(scratch.arena, &list, "Line Numbers Stripped");
  }
  if (flags & COFF_FileHeaderFlag_SymbolsStripped) {
    str8_list_pushf(scratch.arena, &list, "Symbols Stripped");
  }
  if (flags & COFF_FileHeaderFlag_LargeAddressAware) {
    str8_list_pushf(scratch.arena, &list, "Large Address Aware");
  }
  if (flags & COFF_FileHeaderFlag_32BitMachine) {
    str8_list_pushf(scratch.arena, &list, "32-Bit Machine");
  }
  if (flags & COFF_FileHeaderFlag_DebugStripped) {
    str8_list_pushf(scratch.arena, &list, "Debug Stripped");
  }
  if (flags & COFF_FileHeaderFlag_RemovableRunFromSwap) {
    str8_list_pushf(scratch.arena, &list, "Removeable Run From Swap");
  }
  if (flags & COFF_FileHeaderFlag_NetRunFromSwap) {
    str8_list_pushf(scratch.arena, &list, "Net Run From Swap");
  }
  if (flags & COFF_FileHeaderFlag_System) {
    str8_list_pushf(scratch.arena, &list, "System");
  }
  if (flags & COFF_FileHeaderFlag_Dll) {
    str8_list_pushf(scratch.arena, &list, "DLL");
  }
  if (flags & COFF_FileHeaderFlag_UpSystemOnly) {
    str8_list_pushf(scratch.arena, &list, "Up System Only");
  }

  StringView result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});

  scratch_end(scratch);
  return result;
}

StringView
coff_string_from_section_flags(Arena* arena, COFF_SectionFlags flags)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  
  if (flags & COFF_SectionFlag_TypeNoPad) {
    str8_list_pushf(scratch.arena, &list, "TypeNoPad");
  }
  if (flags & COFF_SectionFlag_CntCode) {
    str8_list_pushf(scratch.arena, &list, "CntCode");
  }
  if (flags & COFF_SectionFlag_CntInitializedData) {
    str8_list_pushf(scratch.arena, &list, "CntInitializedData");
  }
  if (flags & COFF_SectionFlag_CntUninitializedData) {
    str8_list_pushf(scratch.arena, &list, "CntUninitializedData");
  }
  if (flags & COFF_SectionFlag_LnkOther) {
    str8_list_pushf(scratch.arena, &list, "LnkOther");
  }
  if (flags & COFF_SectionFlag_LnkInfo) {
    str8_list_pushf(scratch.arena, &list, "LnkInfo");
  }
  if (flags & COFF_SectionFlag_LnkCOMDAT) {
    str8_list_pushf(scratch.arena, &list, "LnkCOMDAT");
  }
  if (flags & COFF_SectionFlag_GpRel) {
    str8_list_pushf(scratch.arena, &list, "GpRel");
  }
  if (flags & COFF_SectionFlag_Mem16Bit) {
    str8_list_pushf(scratch.arena, &list, "Mem16Bit");
  }
  if (flags & COFF_SectionFlag_MemLocked) {
    str8_list_pushf(scratch.arena, &list, "MemLocked");
  }
  if (flags & COFF_SectionFlag_MemPreload) {
    str8_list_pushf(scratch.arena, &list, "MemPreload");
  }
  if (flags & COFF_SectionFlag_LnkNRelocOvfl) {
    str8_list_pushf(scratch.arena, &list, "LnkNRelocOvfl");
  }
  if (flags & COFF_SectionFlag_MemDiscardable) {
    str8_list_pushf(scratch.arena, &list, "MemDiscardable");
  }
  if (flags & COFF_SectionFlag_MemNotCached) {
    str8_list_pushf(scratch.arena, &list, "MemNotCached");
  }
  if (flags & COFF_SectionFlag_MemNotPaged) {
    str8_list_pushf(scratch.arena, &list, "MemNotPaged");
  }
  if (flags & COFF_SectionFlag_MemShared) {
    str8_list_pushf(scratch.arena, &list, "MemShared");
  }
  if (flags & COFF_SectionFlag_MemExecute) {
    str8_list_pushf(scratch.arena, &list, "MemExecute");
  }
  if (flags & COFF_SectionFlag_MemRead) {
    str8_list_pushf(scratch.arena, &list, "MemRead");
  }
  if (flags & COFF_SectionFlag_MemWrite) {
    str8_list_pushf(scratch.arena, &list, "MemWrite");
  }
  
  uint64 align = coff_align_size_from_section_flags(flags);
  if (align) {
    str8_list_pushf(scratch.arena, &list, "Align=%u", align);
  }

  if (!list.node_count) {
    str8_list_pushf(scratch.arena, &list, "None");
  }
  
  StringJoin join = {0};
  join.sep = (", ");
  StringView result = str8_list_join(arena, &list, &join);
  
  scratch_end(scratch);
  return result;
}

StringView
coff_string_from_resource_memory_flags(Arena* arena, COFF_ResourceMemoryFlags flags)
{
  Temp scratch = scratch_begin(&arena, 1);

  String8List list = {0};

  if (flags & COFF_ResourceMemoryFlag_Moveable) {
    flags &= COFF_ResourceMemoryFlag_Moveable;
    str8_list_pushf(scratch.arena, &list, "Moveable");
  }
  if (flags & COFF_ResourceMemoryFlag_Pure) {
    flags &= COFF_ResourceMemoryFlag_Pure;
    str8_list_pushf(scratch.arena, &list, "Pure");
  }
  if (flags & COFF_ResourceMemoryFlag_Discardable) {
    flags &= COFF_ResourceMemoryFlag_Discardable;
    str8_list_pushf(scratch.arena, &list, "Discardable");
  }
  if (flags != 0) {
    str8_list_pushf(scratch.arena, &list, "%#x", flags);
  }

  StringView result = str8_list_join(arena, &list, &(StringJoin){.sep=(", ")});

  scratch_end(scratch);
  return result;
}

StringView
coff_string_from_import_header_type(COFF_ImportType type)
{
  for (uint64 i = 0; i < ArrayCount(g_coff_import_header_type_map); ++i) {
    if (g_coff_import_header_type_map[i].type == type) {
      return str8_cstring(g_coff_import_header_type_map[i].name);
    }
  }
  return StringView(0,0);
}

StringView
coff_string_from_sym_dtype(COFF_SymDType x)
{
  switch (x) {
    case COFF_SymDType_Null:  return ("Null");
    case COFF_SymDType_Ptr :  return ("Ptr");
    case COFF_SymDType_Func:  return ("Func");
    case COFF_SymDType_Array: return ("Array");
  }
  return StringView();
}

StringView
coff_string_from_sym_type(COFF_SymType x)
{
  switch (x) {
    case COFF_SymType_Null:   return ("Null");
    case COFF_SymType_Void:   return ("Void");
    case COFF_SymType_Char:   return ("Char");
    case COFF_SymType_Short:  return ("Short");
    case COFF_SymType_Int:    return ("Int");
    case COFF_SymType_Long:   return ("Long");
    case COFF_SymType_Float:  return ("Float");
    case COFF_SymType_Double: return ("Double");
    case COFF_SymType_Struct: return ("Struct");
    case COFF_SymType_Union:  return ("Union");
    case COFF_SymType_Enum:   return ("Enum");
    case COFF_SymType_MemberOfEnumeration: return ("MOE");
    case COFF_SymType_Byte:   return ("Byte");
    case COFF_SymType_Word:   return ("Word");
    case COFF_SymType_UInt:   return ("UInt");
    case COFF_SymType_DWord:  return ("DWord");
  }
  return StringView();
}

StringView
coff_string_from_sym_storage_class(COFF_SymStorageClass x)
{
  switch (x) {
    case COFF_SymStorageClass_Null:            break;
    case COFF_SymStorageClass_EndOfFunction:   return ("EndOfFunction");
    case COFF_SymStorageClass_Automatic:       return ("Automatic");
    case COFF_SymStorageClass_External:        return ("External");
    case COFF_SymStorageClass_Static:          return ("Static");
    case COFF_SymStorageClass_Register:        return ("Register");
    case COFF_SymStorageClass_ExternalDef:     return ("Def");
    case COFF_SymStorageClass_Label:           return ("Label");
    case COFF_SymStorageClass_UndefinedLabel:  return ("UndefinedLabel");
    case COFF_SymStorageClass_MemberOfStruct:  return ("Struct");
    case COFF_SymStorageClass_Argument:        return ("Argument");
    case COFF_SymStorageClass_StructTag:       return ("Tag");
    case COFF_SymStorageClass_MemberOfUnion:   return ("Union");
    case COFF_SymStorageClass_UnionTag:        return ("Tag");
    case COFF_SymStorageClass_TypeDefinition:  return ("Definition");
    case COFF_SymStorageClass_UndefinedStatic: return ("Static");
    case COFF_SymStorageClass_EnumTag:         return ("Tag");
    case COFF_SymStorageClass_MemberOfEnum:    return ("Enum");
    case COFF_SymStorageClass_RegisterParam:   return ("Param");
    case COFF_SymStorageClass_BitField:        return ("Field");
    case COFF_SymStorageClass_Block:           return ("Block");
    case COFF_SymStorageClass_Function:        return ("Function");
    case COFF_SymStorageClass_EndOfStruct:     return ("Struct");
    case COFF_SymStorageClass_File:            return ("File");
    case COFF_SymStorageClass_Section:         return ("Section");
    case COFF_SymStorageClass_WeakExternal:    return ("External");
    case COFF_SymStorageClass_CLRToken:        return ("Token");
  }
  return StringView();
}

StringView
coff_string_from_weak_ext_type(COFF_WeakExtType x)
{
  switch (x) {
    case COFF_WeakExt_NoLibrary:     return ("NoLibrary");
    case COFF_WeakExt_SearchLibrary: return ("SearchLibrary");
    case COFF_WeakExt_SearchAlias:   return ("SearchAlias");
  }
  return StringView();
}

StringView
coff_string_from_reloc_x86(COFF_Reloc_X86 x)
{
  switch (x) {
    case COFF_Reloc_X86_Abs:      return ("Abs");
    case COFF_Reloc_X86_Dir16:    return ("Dir16");
    case COFF_Reloc_X86_Rel16:    return ("Rel16");
    case COFF_Reloc_X86_Unknown0: return ("Unknown0");
    case COFF_Reloc_X86_Unknown2: return ("Unknown2");
    case COFF_Reloc_X86_Unknown3: return ("Unknown3");
    case COFF_Reloc_X86_Dir32:    return ("Dir32");
    case COFF_Reloc_X86_Dir32Nb:  return ("Dir32Nb");
    case COFF_Reloc_X86_Seg12:    return ("Seg12");
    case COFF_Reloc_X86_Section:  return ("Section");
    case COFF_Reloc_X86_SecRel:   return ("SecRel");
    case COFF_Reloc_X86_Token:    return ("Token");
    case COFF_Reloc_X86_SecRel7:  return ("SecRel7");
    case COFF_Reloc_X86_Unknown4: return ("Unknown4");
    case COFF_Reloc_X86_Unknown5: return ("Unknown5");
    case COFF_Reloc_X86_Unknown6: return ("Unknown6");
    case COFF_Reloc_X86_Unknown7: return ("Unknown7");
    case COFF_Reloc_X86_Unknown8: return ("Unknown8");
    case COFF_Reloc_X86_Unknown9: return ("Unknown9");
    case COFF_Reloc_X86_Rel32:    return ("Rel32");
  }
  return StringView();
}

StringView
coff_string_from_reloc_x64(COFF_Reloc_X64 x)
{
  switch (x) {
    case COFF_Reloc_X64_Abs:      return ("Abs");
    case COFF_Reloc_X64_Addr64:   return ("Addr64");
    case COFF_Reloc_X64_Addr32:   return ("Addr32");
    case COFF_Reloc_X64_Addr32Nb: return ("Addr32Nb");
    case COFF_Reloc_X64_Rel32:    return ("Rel32");
    case COFF_Reloc_X64_Rel32_1:  return ("Rel32_1");
    case COFF_Reloc_X64_Rel32_2:  return ("Rel32_2");
    case COFF_Reloc_X64_Rel32_3:  return ("Rel32_3");
    case COFF_Reloc_X64_Rel32_4:  return ("Rel32_4");
    case COFF_Reloc_X64_Rel32_5:  return ("Rel32_5");
    case COFF_Reloc_X64_Section:  return ("Section");
    case COFF_Reloc_X64_SecRel:   return ("SecRel");
    case COFF_Reloc_X64_SecRel7:  return ("SecRel7");
    case COFF_Reloc_X64_Token:    return ("Token");
    case COFF_Reloc_X64_SRel32:   return ("SRel32");
    case COFF_Reloc_X64_Pair:     return ("Pair");
    case COFF_Reloc_X64_SSpan32:  return ("SSpan32");
  }
  return StringView();
}

StringView
coff_string_from_reloc_arm(COFF_Reloc_Arm x)
{
  switch (x) {
    case COFF_Reloc_Arm_Abs:           return ("Abs");
    case COFF_Reloc_Arm_Addr32:        return ("Addr32");
    case COFF_Reloc_Arm_Addr32Nb:      return ("Addr32Nb");
    case COFF_Reloc_Arm_Branch24:      return ("Branch24");
    case COFF_Reloc_Arm_Branch11:      return ("Branch11");
    case COFF_Reloc_Arm_Unknown1:      return ("Unknown1");
    case COFF_Reloc_Arm_Unknown2:      return ("Unknown2");
    case COFF_Reloc_Arm_Unknown3:      return ("Unknown3");
    case COFF_Reloc_Arm_Unknown4:      return ("Unknown4");
    case COFF_Reloc_Arm_Unknown5:      return ("Unknown5");
    case COFF_Reloc_Arm_Rel32:         return ("Rel32");
    case COFF_Reloc_Arm_Section:       return ("Section");
    case COFF_Reloc_Arm_SecRel:        return ("SecRel");
    case COFF_Reloc_Arm_Mov32:         return ("Mov32");
    case COFF_Reloc_Arm_ThumbMov32:    return ("ThumbMov32");
    case COFF_Reloc_Arm_ThumbBranch20: return ("ThumbBranch20");
    case COFF_Reloc_Arm_Unused:        return ("Unused");
    case COFF_Reloc_Arm_ThumbBranch24: return ("ThumbBranch24");
    case COFF_Reloc_Arm_ThumbBlx23:    return ("ThumbBlx23");
    case COFF_Reloc_Arm_Pair:          return ("Pair");
  }
  return StringView();
}

StringView
coff_string_from_reloc_arm64(COFF_Reloc_Arm64 x)
{
  switch (x) {
    case COFF_Reloc_Arm64_Abs:           return ("Abs");
    case COFF_Reloc_Arm64_Addr32:        return ("Addr32");
    case COFF_Reloc_Arm64_Addr32Nb:      return ("Addr32Nb");
    case COFF_Reloc_Arm64_Branch26:      return ("Branch26");
    case COFF_Reloc_Arm64_PageBaseRel21: return ("PageBaseRel21");
    case COFF_Reloc_Arm64_Rel21:         return ("Rel21");
    case COFF_Reloc_Arm64_PageOffset12a: return ("PageOffset12a");
    case COFF_Reloc_Arm64_SecRel:        return ("SecRel");
    case COFF_Reloc_Arm64_SecRelLow12a:  return ("SecRelLow12a");
    case COFF_Reloc_Arm64_SecRelHigh12a: return ("SecRelHigh12a");
    case COFF_Reloc_Arm64_SecRelLow12l:  return ("SecRelLow12l");
    case COFF_Reloc_Arm64_Token:         return ("Token");
    case COFF_Reloc_Arm64_Section:       return ("Section");
    case COFF_Reloc_Arm64_Addr64:        return ("Addr64");
    case COFF_Reloc_Arm64_Branch19:      return ("Branch19");
    case COFF_Reloc_Arm64_Branch14:      return ("Branch14");
    case COFF_Reloc_Arm64_Rel32:         return ("Rel32");
  }
  return StringView();
}

StringView
coff_string_from_reloc(COFF_MachineType machine, COFF_RelocType x)
{
  switch (machine) {
    case COFF_Machine_X86:   return coff_string_from_reloc_x86(x);
    case COFF_Machine_X64:   return coff_string_from_reloc_x64(x);
    case COFF_Machine_Arm:   return coff_string_from_reloc_arm(x);
    case COFF_Machine_Arm64: return coff_string_from_reloc_arm64(x);
  }
  return StringView();
}

COFF_MachineType
coff_machine_from_string(StringView string)
{
  for (uint64 i = 0; i < ArrayCount(g_coff_machine_map); ++i) {
    if (str8_match(g_coff_machine_map[i].string, string, StringMatchFlag_CaseInsensitive)) {
      return g_coff_machine_map[i].machine;
    }
  }
  return COFF_Machine_Unknown;
}

COFF_ImportType
coff_import_header_type_from_string(StringView name)
{
  for (uint64 i = 0; i < ArrayCount(g_coff_import_header_type_map); ++i) {
    if (str8_match(str8_cstring(g_coff_import_header_type_map[i].name), name, StringMatchFlag_CaseInsensitive)) {
      return g_coff_import_header_type_map[i].type;
    }
  }
  return COFF_ImportType_Invalid;
}




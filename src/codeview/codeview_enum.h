// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef CODEVIEW_ENUM_H
#define CODEVIEW_ENUM_H

String8 cv_string_from_type_index_source(CV_TypeIndexSource ti_source);
String8 cv_string_from_language(CV_Language x);
String8 cv_string_from_numeric(Arena* arena, CV_NumericParsed num);
String8 cv_string_from_reg_id(Arena* arena, CV_Arch arch, uint32 id);
String8 cv_string_from_member_access(CV_MemberAccess x);
String8 cv_string_from_method_prop(CV_MethodProp x);
String8 cv_string_from_hfa(CV_HFAKind x);
String8 cv_string_from_mcom(CV_MoComUDTKind x);
String8 cv_string_from_binary_opcode(CV_InlineBinaryAnnotation x);
String8 cv_string_from_thunk_ordinal(CV_ThunkOrdinal x);
String8 cv_string_from_frame_cookie_kind(CV_FrameCookieKind x);
String8 cv_string_from_generic_style(CV_GenericStyle x);
String8 cv_string_from_trampoline_kind(CV_TrampolineKind x);
String8 cv_string_from_virtual_table_shape_kind(CV_VirtualTableShape x);
String8 cv_string_from_call_kind(CV_CallKind x);
String8 cv_string_from_member_pointer_kind(CV_MemberPointerKind x);
String8 cv_string_from_pointer_kind(CV_PointerKind x);
String8 cv_string_from_pointer_mode(CV_PointerMode x);
String8 cv_string_from_c13_checksum_kind(CV_C13ChecksumKind x);
String8 cv_string_from_label_kind(Arena* arena, CV_LabelKind x);
String8 cv_string_from_c13_subsection_kind(CV_C13SubSectionKind x);
String8 cv_string_from_modifier_flags(Arena* arena, CV_ModifierFlags x);
String8 cv_string_from_pointer_attribs(Arena* arena, CV_PointerAttribs x);
String8 cv_string_from_function_attribs(Arena* arena, CV_FunctionAttribs x);
String8 cv_string_from_export_flags(Arena* arena, CV_ExportFlags x);
String8 cv_string_from_sepcode(Arena* arena, CV_SepcodeFlags x);
String8 cv_string_from_pub32_flags(Arena* arena, CV_Pub32Flags x);
String8 cv_string_from_generic_flags(Arena* arena, CV_GenericFlags x);
String8 cv_string_from_frame_proc_flags(Arena* arena, CV_FrameprocFlags x);
String8 cv_string_from_type_props(Arena* arena, CV_TypeProps32 x);
String8 cv_string_from_local_flags(Arena* arena, CV_LocalFlags x);
String8 cv_string_from_proc_flags(Arena* arena, CV_ProcFlags x);
String8 cv_string_from_range_attribs(Arena* arena, CV_RangeAttribs x);
String8 cv_string_from_defrange_register_rel_flags(Arena* arena, CV_DefrangeRegisterRelFlags x);
String8 cv_string_from_field_attribs(Arena* arena, CV_FieldAttribs attribs);
String8 cv_string_from_itype(Arena* arena, CV_TypeIndex min_itype, CV_TypeIndex itype);
String8 cv_string_from_itemid(Arena* arena, CV_ItemId itemid);
String8 cv_string_from_reg_off(Arena* arena, CV_Arch arch, uint32 reg, uint32 off);
String8 cv_string_from_symbol_type(Arena* arena, CV_SymKind symbol_type);
String8 cv_string_from_symbol_kind(Arena* arena, CV_SymKind kind);
String8 cv_string_from_leaf_name(Arena* arena, uint32 leaf_type);
String8 cv_string_sec_off(Arena* arena, uint32 sec, uint32 off);

#endif // CODEVIEW_ENUM_H



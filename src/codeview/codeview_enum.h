// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef CODEVIEW_ENUM_H
#define CODEVIEW_ENUM_H

StringView cv_string_from_type_index_source(CV_TypeIndexSource ti_source);
StringView cv_string_from_language(CV_Language x);
StringView cv_string_from_numeric(Arena* arena, CV_NumericParsed num);
StringView cv_string_from_reg_id(Arena* arena, CV_Arch arch, uint32 id);
StringView cv_string_from_member_access(CV_MemberAccess x);
StringView cv_string_from_method_prop(CV_MethodProp x);
StringView cv_string_from_hfa(CV_HFAKind x);
StringView cv_string_from_mcom(CV_MoComUDTKind x);
StringView cv_string_from_binary_opcode(CV_InlineBinaryAnnotation x);
StringView cv_string_from_thunk_ordinal(CV_ThunkOrdinal x);
StringView cv_string_from_frame_cookie_kind(CV_FrameCookieKind x);
StringView cv_string_from_generic_style(CV_GenericStyle x);
StringView cv_string_from_trampoline_kind(CV_TrampolineKind x);
StringView cv_string_from_virtual_table_shape_kind(CV_VirtualTableShape x);
StringView cv_string_from_call_kind(CV_CallKind x);
StringView cv_string_from_member_pointer_kind(CV_MemberPointerKind x);
StringView cv_string_from_pointer_kind(CV_PointerKind x);
StringView cv_string_from_pointer_mode(CV_PointerMode x);
StringView cv_string_from_c13_checksum_kind(CV_C13ChecksumKind x);
StringView cv_string_from_label_kind(Arena* arena, CV_LabelKind x);
StringView cv_string_from_c13_subsection_kind(CV_C13SubSectionKind x);
StringView cv_string_from_modifier_flags(Arena* arena, CV_ModifierFlags x);
StringView cv_string_from_pointer_attribs(Arena* arena, CV_PointerAttribs x);
StringView cv_string_from_function_attribs(Arena* arena, CV_FunctionAttribs x);
StringView cv_string_from_export_flags(Arena* arena, CV_ExportFlags x);
StringView cv_string_from_sepcode(Arena* arena, CV_SepcodeFlags x);
StringView cv_string_from_pub32_flags(Arena* arena, CV_Pub32Flags x);
StringView cv_string_from_generic_flags(Arena* arena, CV_GenericFlags x);
StringView cv_string_from_frame_proc_flags(Arena* arena, CV_FrameprocFlags x);
StringView cv_string_from_type_props(Arena* arena, CV_TypeProps32 x);
StringView cv_string_from_local_flags(Arena* arena, CV_LocalFlags x);
StringView cv_string_from_proc_flags(Arena* arena, CV_ProcFlags x);
StringView cv_string_from_range_attribs(Arena* arena, CV_RangeAttribs x);
StringView cv_string_from_defrange_register_rel_flags(Arena* arena, CV_DefrangeRegisterRelFlags x);
StringView cv_string_from_field_attribs(Arena* arena, CV_FieldAttribs attribs);
StringView cv_string_from_itype(Arena* arena, CV_TypeIndex min_itype, CV_TypeIndex itype);
StringView cv_string_from_itemid(Arena* arena, CV_ItemId itemid);
StringView cv_string_from_reg_off(Arena* arena, CV_Arch arch, uint32 reg, uint32 off);
StringView cv_string_from_symbol_type(Arena* arena, CV_SymKind symbol_type);
StringView cv_string_from_symbol_kind(Arena* arena, CV_SymKind kind);
StringView cv_string_from_leaf_name(Arena* arena, uint32 leaf_type);
StringView cv_string_sec_off(Arena* arena, uint32 sec, uint32 off);

#endif // CODEVIEW_ENUM_H



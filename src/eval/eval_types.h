// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_TYPES_H
#define EVAL_TYPES_H

////////////////////////////////
//~ rjf: Type Unwrapping

typedef U32 E_TypeUnwrapFlags;
enum
{
  E_TypeUnwrapFlag_Modifiers     = (1<<0),
  E_TypeUnwrapFlag_Pointers      = (1<<1),
  E_TypeUnwrapFlag_Lenses        = (1<<2),
  E_TypeUnwrapFlag_Meta          = (1<<3),
  E_TypeUnwrapFlag_Enums         = (1<<4),
  E_TypeUnwrapFlag_Aliases       = (1<<5),
  E_TypeUnwrapFlag_All           = 0xffffffff,
  E_TypeUnwrapFlag_AllDecorative = (E_TypeUnwrapFlag_All & ~E_TypeUnwrapFlag_Pointers)
};

////////////////////////////////
//~ rjf: Globals

global read_only E_Member e_member_nil = {E_MemberKind_Null};
global read_only E_Type e_type_nil = {E_TypeKind_Null};
E_TYPE_EXPAND_INFO_FUNCTION_DEF(default);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(default);
E_TYPE_EXPAND_ID_FROM_NUM_FUNCTION_DEF(identity);
E_TYPE_EXPAND_NUM_FROM_ID_FUNCTION_DEF(identity);
global read_only E_TypeExpandRule e_type_expand_rule__default =
{
  E_TYPE_EXPAND_INFO_FUNCTION_NAME(default),
  E_TYPE_EXPAND_RANGE_FUNCTION_NAME(default),
  E_TYPE_EXPAND_ID_FROM_NUM_FUNCTION_NAME(identity),
  E_TYPE_EXPAND_NUM_FROM_ID_FUNCTION_NAME(identity),
};

////////////////////////////////
//~ rjf: Member Functions

#define e_member_list_push_new(arena, list, ...) e_member_list_push((arena), (list), &(E_Member){.kind = E_MemberKind_DataField, __VA_ARGS__})

////////////////////////////////
//~ rjf: Enum Value Functions

#define e_enum_val_list_push_new(arena, list, ...) e_enum_val_list_push((arena), (list), &(E_EnumVal){.val = 0, __VA_ARGS__})

////////////////////////////////
//~ rjf: Type Operation Functions
//- rjf: constructed type construction
#define e_type_key_cons(...) e_type_key_cons_(&(E_ConsTypeParams){.kind = E_TypeKind_Null, __VA_ARGS__})
////////////////////////////////
//~ rjf: (Built-In Type Hooks) Default Hooks

E_TYPE_EXPAND_INFO_FUNCTION_DEF(default);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(default);
E_TYPE_EXPAND_ID_FROM_NUM_FUNCTION_DEF(identity);
E_TYPE_EXPAND_NUM_FROM_ID_FUNCTION_DEF(identity);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `only` lens

E_TYPE_EXPAND_INFO_FUNCTION_DEF(only);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(only);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `sequence` lens

E_TYPE_EXPAND_INFO_FUNCTION_DEF(sequence);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(sequence);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `array` lens

E_TYPE_EXPAND_INFO_FUNCTION_DEF(array);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(array);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `slice` lens

E_TYPE_IREXT_FUNCTION_DEF(slice);
E_TYPE_ACCESS_FUNCTION_DEF(slice);
E_TYPE_EXPAND_INFO_FUNCTION_DEF(slice);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(slice);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `only`, `omit` lenses

E_TYPE_EXPAND_INFO_FUNCTION_DEF(only_and_omit);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(only_and_omit);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `folder` type

E_TYPE_EXPAND_INFO_FUNCTION_DEF(folder);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(folder);
E_TYPE_EXPAND_ID_FROM_NUM_FUNCTION_DEF(folder);
E_TYPE_EXPAND_NUM_FROM_ID_FUNCTION_DEF(folder);

////////////////////////////////
//~ rjf: (Built-In Type Hooks) `file` type

E_TYPE_IREXT_FUNCTION_DEF(file);
E_TYPE_ACCESS_FUNCTION_DEF(file);
E_TYPE_EXPAND_INFO_FUNCTION_DEF(file);
E_TYPE_EXPAND_RANGE_FUNCTION_DEF(file);

#endif // EVAL_TYPES_H

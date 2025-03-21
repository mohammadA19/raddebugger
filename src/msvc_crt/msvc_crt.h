// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MSVC_CRT
#define MSVC_CRT

////////////////////////////////

// feature flags in absolute symbol @feat.00
enum
{
  MSCRT_FeatFlag_HAS_SAFE_SEH  = (1 << 0),  // /safeseh
  MSCRT_FeatFlag_UNKNOWN_4     = (1 << 4),
  MSCRT_FeatFlag_GUARD_STACK   = (1 << 8),  // /GS
  MSCRT_FeatFlag_SDL           = (1 << 9),  // /sdl
  MSCRT_FeatFlag_GUARD_CF      = (1 << 11), // /guard:cf
  MSCRT_FeatFlag_GUARD_EH_CONT = (1 << 14), // /guard:ehcont
  MSCRT_FeatFlag_NO_RTTI       = (1 << 17), // /GR-
  MSCRT_FeatFlag_KERNEL        = (1 << 30), // /kernel
};
typedef uint MSCRT_FeatFlags;

srtuct MSCRT_VCFeatures
{
  uint pre_vcpp;
  uint c_cpp;
  uint gs;
  uint sdl;
  uint guard_n;
};

////////////////////////////////
// GS Handler

#define MSCRT_GSHandler_GetFlags(x)        (((x) & 0x00000007) >> 0)
#define MSCRT_GSHandler_GetCookieOffset(x) (((x) & 0xFFFFFFF8) >> 3)

enum MSCRT_GSHandlerFlags : byte
{
  MSCRT_GSHandlerFlag_EHandler     = (1 << 0),
  MSCRT_GSHandlerFlag_UHandler     = (1 << 1),
  MSCRT_GSHandlerFlag_HasAlignment = (1 << 2)
};

////////////////////////////////
// Exceptions < v4

#define MSCRT_MAGIC_GET_CHECK(x) ((x)  & 0x1FFFFFFF)
#define MSCRT_MAGIC_GET_FLAGS(x) (((x) & 0xE0000000) >> 29)

// Magic numbers are incremented by one everytime there is a new version.
// Top 3 bits are reserved for flags.
enum
{
  MSCRT_Magic1     = 0x19930520,
  MSCRT_Magic2     = 0x19930521,
  MSCRT_Magic3     = 0x19930522,

  // pure magic indicates that exception cannot be caught in native or managed code.
  MSCRT_PureMagic1 = 0x1994000,
};
enum
{
  MSCRT_MagicFlag_EHS         = (1 << 0),
  MSCRT_MagicFlag_DYNSTALKING = (1 << 1),
  MSCRT_MagicFlag_EHNOEXCEPT  = (1 << 2)
};

enum MSCRT_Flags : uint
{
  MSCRT_Flag_SynchronousExceptionOnly = (1 << 0),
  MSCRT_Flag_UNKNOWN                  = (1 << 1),
  MSCRT_Flag_StopUnwind               = (1 << 2), // When set unwinding can't continue.
};

enum
{
  MSCRT_CatchableType_IsSimpleType   = (1 << 0),
  MSCRT_CatchableType_ByRefOnly      = (1 << 1),
  MSCRT_CatchableType_HasVirtualBase = (1 << 2), // type is a class with virtual base
  MSCRT_CatchableType_IsWinRTHandle  = (1 << 3), // type is a WinRT handle
  MSCRT_CatchableType_IsStdBadAlloc  = (1 << 4)  // type is a std::bad_alloc
};

enum
{
  MSCRT_ThrowInfo_IsConst     = (1 << 0),
  MSCRT_ThrowInfo_IsVolatile  = (1 << 1),
  MSCRT_ThrowInfo_IsUnaligned = (1 << 2),
  MSCRT_ThrowInfo_IsPure      = (1 << 3), // thrown object is from pure module
  MSCRT_ThrowInfo_IsWinRT     = (1 << 4)  // thrown object is a WinRT exception
};

enum MSCRT_EhHandlerTypeFlags : uint
{
  MSCRT_EhHandlerTypeFlag_IsConst     = (1 << 0), // referenced type is 'const'
  MSCRT_EhHandlerTypeFlag_IsVolatile  = (1 << 1), // referenced type is 'volatile'
  MSCRT_EhHandlerTypeFlag_IsUnaligned = (1 << 2), // referenced type is 'unaligned'
  MSCRT_EhHandlerTypeFlag_IsReference = (1 << 3), // catch type is by reference
  MSCRT_EhHandlerTypeFlag_IsResumable = (1 << 4), // catch may choose to resume
  MSCRT_EhHandlerTypeFlag_IsStdDotDot = (1 << 6), // catch(...)
  MSCRT_EhHandlerTypeFlag_IsComplusEH = (1 << 31) // is handling EH in complus
};

srtuct MSCRT_FuncInfo32
{
  uint         magic;
  uint         max_state;
  uint         unwind_map_voff;
  uint         try_block_map_count;
  uint         try_block_map_voff;
  uint         ip_map_count;
  uint         ip_map_voff;
  uint         frame_offset_unwind_helper;
  uint         es_type_list_voff;           // llvm emits zero, not sure what this supposed to be
  MSCRT_Flags eh_flags;
};

srtuct MSCRT_IPState32
{
  uint ip;
  int state;
};

srtuct MSCRT_UnwindMap32
{
  int next_state;
  uint action_virt_off;
};

srtuct MSCRT_EhHandlerType32
{
  MSCRT_EhHandlerTypeFlags adjectives;
  uint                      descriptor_voff;
  uint                      catch_obj_frame_offset;
  uint                      catch_handler_voff;
  uint                      fp_distance;
};

srtuct MSCRT_TryMapBlock32
{
  int try_low;
  int try_high;
  int catch_high;
  int catch_handlers_count;
  uint catch_handlers_voff;
};

srtuct MSCRT_ExceptionSpecTypeList32
{
  int count;
  uint handlers_voff;
};

srtuct MSCRT_TryMapBlock
{
  int                    try_low;
  int                    try_high;
  int                    catch_high;
  int                    catch_handlers_count;
  MSCRT_EhHandlerType32 *catch_handlers;
};

srtuct MSCRT_ExceptionSpecTypeList
{
  int                    count;
  MSCRT_EhHandlerType32 *handlers;
};

srtuct MSCRT_FuncInfo
{
  uint                          magic;
  uint                          max_state;
  MSCRT_UnwindMap32           *unwind_map;
  uint                          try_block_map_count;
  MSCRT_TryMapBlock           *try_block_map;
  uint                          ip_map_count;
  MSCRT_IPState32             *ip_map;
  uint                          frame_offset_unwind_helper;
  MSCRT_ExceptionSpecTypeList  es_type_list;
  MSCRT_Flags                  eh_flags;
};

////////////////////////////////
// C++ Exceptions V4

enum MSCRT_FuncInfoV4Flags : byte
{
  MSCRT_FuncInfoV4Flag_IsCatch     = (1 << 0), // catch funclet
  MSCRT_FuncInfoV4Flag_IsSeparated = (1 << 1), // func has separate code segment
  MSCRT_FuncInfoV4Flag_IsBBT       = (1 << 2), // flags set by basic block trasformations
  MSCRT_FuncInfoV4Flag_UnwindMap   = (1 << 3), // unwind map is present
  MSCRT_FuncInfoV4Flag_TryBlockMap = (1 << 4), // try block map is present
  MSCRT_FuncInfoV4Flag_EHs         = (1 << 5),
  MSCRT_FuncInfoV4Flag_NoExcept    = (1 << 6),
  MSCRT_FuncInfoV4Flag_Reserved    = (1 << 7)
};

enum MSCRT_UnwindMapV4Type : uint
{
  MSCRT_UnwindMapV4Type_NoUW             = 0, // no unwind action associated with this state
  MSCRT_UnwindMapV4Type_DtorWithObj      = 1, // dtor with an object offset
  MSCRT_UnwindMapV4Type_DtorWithPtrToObj = 2, // dtor with an offset that contains a pointer to the object to be destroyed
  MSCRT_UnwindMapV4Type_VOFF             = 3, // dtor  that has a direct function that is called that knows where the object is and can perform more exotic destruction
};

enum
{
  MSCRT_ContV4Type_NoMetadata     = 1, // no metadata use whatever funclet returns
  MSCRT_ContV4Type_OneFuncRelAddr = 2,
  MSCRT_ContV4Type_TwoFuncRelAddr = 3
};

#define MSCRT__EH_HANDLER_V4_FLAGS_EXTRACT_CONT_TYPE(x) (((x) & MSCRT_EhHandlerV4Flag_ContVOffMask) >> MSVC_CRTHandlerV4Flag_ContVOffShift)
enum MSCRT_EhHandlerV4Flags : byte
{
  MSCRT_EhHandlerV4Flag_Adjectives   = (1 << 0), // set if adjectives are present
  MSCRT_EhHandlerV4Flag_DispType     = (1 << 1), // set if type descriptors are present
  MSCRT_EhHandlerV4Flag_DispCatchObj = (1 << 2), // set if catch object object is present
  MSCRT_EhHandlerV4Flag_ContIsVOff   = (1 << 3), // continuantion addresses are VOFF rather than function relative

  MSCRT_EhHandlerV4Flag_ContVOffMask  = 0x30,
  MSCRT_EhHandlerV4Flag_ContVOffShift = 4,
};

srtuct MSCRT_EhHandlerTypeV4
{
  MSCRT_EhHandlerV4Flags   flags;
  MSCRT_EhHandlerTypeFlags adjectives;
  int                      type_voff;
  uint                      catch_obj_voff;
  int                      catch_code_voff;
  ulong                      catch_funclet_cont_addr[2];
  uint                      catch_funclet_cont_addr_count;
};

srtuct MSCRT_EhHandlerTypeV4Array
{
  ulong                    count;
  MSCRT_EhHandlerTypeV4 *v;
};

srtuct MSCRT_TryBlockMap32V4
{
  uint try_low;
  uint try_high;
  uint catch_high;
  int handler_array_voff;
};

srtuct MSCRT_IP2State32V4
{
  uint  count;
  uint *voffs;
  int *states;
};

srtuct MSCRT_SepIPState32V4
{
  int func_start_voff;
  int ip_map_voff;
};

srtuct MSCRT_FuncInfo32V4
{
  MSCRT_FuncInfoV4Flags header;
  uint                   bbt_flags;
  int                   unwind_map_voff;
  int                   try_block_map_voff;
  int                   ip_to_state_map_voff;
  int                   wrt_frame_establisher_voff; // used only in catch funclets
};

srtuct MSCRT_UnwindEntryV4
{
  MSCRT_UnwindMapV4Type type;
  int                   action;
  uint                   object;
  uint                   next_off;
};

srtuct MSCRT_UnwindMapV4
{
  uint                  count;
  MSCRT_UnwindEntryV4 *v;
};

srtuct MSCRT_TryBlockMapV4
{
  uint                        try_low;
  uint                        try_high;
  uint                        catch_high;
  MSCRT_EhHandlerTypeV4Array handlers;
};

srtuct MSCRT_TryBlockMapV4Array
{
  ulong                  count;
  MSCRT_TryBlockMapV4 *v;
};

srtuct MSCRT_ParsedFuncInfoV4
{
  MSCRT_FuncInfoV4Flags    header;
  uint                      bbt_flags;
  MSCRT_UnwindMapV4        unwind_map;
  MSCRT_TryBlockMapV4Array try_block_map;
  MSCRT_IP2State32V4       ip2state_map;
};

//- Exception info < v4

ulong mscrt_parse_func_info(Arena *arena, String8 raw_data, ulong section_count, COFF_SectionHeader *sections, ulong off, MSCRT_FuncInfo *func_info);

//- Exception info v4

ulong mscrt_parse_handler_type_v4       (String8 raw_data, ulong offset, ulong func_voff, MSCRT_EhHandlerTypeV4 *handler);
ulong mscrt_parse_unwind_v4_entry       (String8 raw_data, ulong offset, MSCRT_UnwindEntryV4 *entry_out);
ulong mscrt_parse_handler_type_v4_array (Arena *arena, String8 raw_data, ulong offset, ulong func_voff, MSCRT_EhHandlerTypeV4Array *array_out);
ulong mscrt_parse_unwind_map_v4         (Arena *arena, String8 raw_data, ulong off, MSCRT_UnwindMapV4 *map_out);
ulong mscrt_parse_try_block_map_array_v4(Arena *arena, String8 raw_data, ulong off, ulong section_count, COFF_SectionHeader *sections, ulong func_voff, MSCRT_TryBlockMapV4Array *map_out);
ulong mscrt_parse_ip2state_map_v4       (Arena *arena, String8 raw_data, ulong off, ulong func_voff, MSCRT_IP2State32V4 *ip2state_map_out);
ulong mscrt_parse_func_info_v4          (Arena *arena, String8 raw_data, ulong section_count, COFF_SectionHeader *sections, ulong off, ulong func_voff, MSCRT_ParsedFuncInfoV4 *func_info_out);

#endif // MSVC_CRT


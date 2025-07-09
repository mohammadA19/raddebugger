// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal u64
mscrt_parse_func_info(Arena              *arena,
                      String8             raw_data,
                      u64                 section_count,
                      COFF_SectionHeader *sections,
                      u64                 off,
                      MSCRT_FuncInfo     *func_info)
{
  u64 cursor = off;
  
  u32 handler_data_voff = 0;
  cursor += str8_deserial_read_struct(raw_data, cursor, &handler_data_voff);
  
  // TODO: what is this? padding?
  u32 unknown = 0;
  cursor += str8_deserial_read_struct(raw_data, cursor, &unknown);
  
  // read function info
  u64 handler_data_foff = coff_foff_from_voff(sections, section_count, handler_data_voff);
  
  MSCRT_FuncInfo32 func_info32 = {0};
  str8_deserial_read_struct(raw_data, handler_data_foff, &func_info32);
  
  // unwind map
  MSCRT_UnwindMap32 *unwind_map      = push_array(arena, MSCRT_UnwindMap32, func_info32.max_state);
  u64                unwind_map_foff = coff_foff_from_voff(sections, section_count, func_info32.unwind_map_voff);
  cursor += str8_deserial_read_array(raw_data, unwind_map_foff, &unwind_map[0], func_info32.max_state);
  
  // read ip states
  MSCRT_IPState32 *ip_map      = push_array(arena, MSCRT_IPState32, func_info32.ip_map_count);
  u64              ip_map_foff = coff_foff_from_voff(sections, section_count, func_info32.ip_map_voff);
  str8_deserial_read_array(raw_data, ip_map_foff, &ip_map[0], func_info32.ip_map_count);
  
  // read try map
  MSCRT_TryMapBlock *try_block_map = push_array(arena, MSCRT_TryMapBlock, func_info32.try_block_map_count);
  u64                try_map_foff  = coff_foff_from_voff(sections, section_count, func_info32.try_block_map_voff);
  for (u32 imap = 0; imap < func_info32.try_block_map_count; ++imap) {
    MSCRT_TryMapBlock32 map32 = {0};
    str8_deserial_read_struct(raw_data, try_map_foff + imap*sizeof(map32), &map32);
    
    // convert try map to in-memory version
    MSCRT_TryMapBlock *map    = &try_block_map[imap];
    map->try_low              = map32.try_low;
    map->try_high             = map32.try_high;
    map->catch_high           = map32.catch_high;
    map->catch_handlers_count = map32.catch_handlers_count;
    map->catch_handlers       = push_array(arena, MSCRT_EhHandlerType32, map32.catch_handlers_count);
    
    // read handlers
    u64 catch_handlers_foff = coff_foff_from_voff(sections, section_count, map32.catch_handlers_voff);
    str8_deserial_read_array(raw_data, catch_handlers_foff, &map->catch_handlers[0], map->catch_handlers_count);
  }
  
  // read exception spec list
  MSCRT_ExceptionSpecTypeList es_type_list = {0};
  if (func_info32.es_type_list_voff) {
    MSCRT_ExceptionSpecTypeList32 es_list32    = {0};
    u64                           es_list_foff = coff_foff_from_voff(sections, section_count, func_info32.es_type_list_voff);
    str8_deserial_read_struct(raw_data, es_list_foff, &es_list32);
    
    es_type_list.count    = es_list32.count;
    es_type_list.handlers = push_array(arena, MSCRT_EhHandlerType32, es_list32.count);
    
    u64 handlers_foff = coff_foff_from_voff(sections, section_count, es_list32.handlers_voff);
    str8_deserial_read_array(raw_data, handlers_foff, &es_type_list.handlers[0], es_type_list.count);
  }
  
  // pack result
  func_info->magic                      = func_info32.magic;
  func_info->max_state                  = func_info32.max_state;
  func_info->unwind_map                 = unwind_map;
  func_info->try_block_map_count        = func_info32.try_block_map_count;
  func_info->try_block_map              = try_block_map;
  func_info->ip_map_count               = func_info32.ip_map_count;
  func_info->ip_map                     = ip_map;
  func_info->frame_offset_unwind_helper = func_info32.frame_offset_unwind_helper;
  func_info->es_type_list               = es_type_list;
  func_info->eh_flags                   = func_info32.eh_flags;
  
  u64 parse_size = (cursor - off);
  return parse_size;
}

////////////////////////////////

internal u64
mscrt_v4_parse_u32(String8 raw_data, u64 offset, u32 *uint_out)
{
  u64 cursor = offset;
  
  u8 one = 0;
  cursor += str8_deserial_read_struct(raw_data, cursor, &one);
  
  if ((one & 0xF) == 15) {
    u8 two = 0, three = 0, four = 0, five = 0;
    cursor += str8_deserial_read_struct(raw_data, cursor, &two);
    cursor += str8_deserial_read_struct(raw_data, cursor, &three);
    cursor += str8_deserial_read_struct(raw_data, cursor, &four);
    cursor += str8_deserial_read_struct(raw_data, cursor, &five);
    *uint_out = (u32)two | ((u32)three << 8) | ((u32)four << 16) | ((u32)five << 24);
  } else if ((one & 0xF) == 7) {
    u8 two = 0, three = 0, four = 0;
    cursor += str8_deserial_read_struct(raw_data, cursor, &two);
    cursor += str8_deserial_read_struct(raw_data, cursor, &three);
    cursor += str8_deserial_read_struct(raw_data, cursor, &four);
    *uint_out = ((u32)one >> 4) | ((u32)two << 4) | ((u32)three << 12) | ((u32)four << 20);
  } else if ((one & 0x7) == 3) {
    u8 two = 0, three = 0;
    cursor += str8_deserial_read_struct(raw_data, cursor, &two);
    cursor += str8_deserial_read_struct(raw_data, cursor, &three);
    *uint_out = ((u32)one >> 3) | ((u32)two << 5) | ((u32)three << 13);
  } else if ((one & 0x3) == 1) {
    u8 two = 0;
    cursor += str8_deserial_read_struct(raw_data, cursor, &two);
    *uint_out = ((u32)one >> 2) | ((u32)two << 6);
  } else {
    *uint_out = one >> 1;
  }
  
  u64 read_size = cursor - offset;
  return read_size;
}

internal u64
mscrt_v4_parse_s32(String8 raw_data, u64 offset, i32 *int_out)
{
  return str8_deserial_read_struct(raw_data, offset, int_out);
}

internal u64
mscrt_parse_handler_type_v4(String8 raw_data, u64 offset, u64 func_voff, MSCRT_EhHandlerTypeV4 *handler)
{
  u64 cursor = offset;
  
  cursor += str8_deserial_read_struct(raw_data, cursor, &handler->flags);
  if (handler->flags & MSCRT_EhHandlerV4Flag_Adjectives) {
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &handler->adjectives);
  }
  if (handler->flags & MSCRT_EhHandlerV4Flag_DispType) {
    cursor += mscrt_v4_parse_s32(raw_data, cursor, &handler->type_voff);
  }
  if (handler->flags & MSCRT_EhHandlerV4Flag_DispCatchObj) {
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &handler->catch_obj_voff);
  }
  cursor += mscrt_v4_parse_s32(raw_data, cursor, &handler->catch_code_voff);
  
  u32 cont_type = (handler->flags & MSCRT_EhHandlerV4Flag_ContVOffMask) >> MSCRT_EhHandlerV4Flag_ContVOffShift;
  if (handler->flags & MSCRT_EhHandlerV4Flag_ContIsVOff) {
    switch (cont_type) {
      case MSCRT_ContV4Type_NoMetadata: break;
      case MSCRT_ContV4Type_OneFuncRelAddr: {
        i32 v = 0;
        cursor += mscrt_v4_parse_s32(raw_data, cursor, &v);
        handler->catch_funclet_cont_addr[0]    = (u64)v;
        handler->catch_funclet_cont_addr_count = 1;
      } break;
      case MSCRT_ContV4Type_TwoFuncRelAddr: {
        i32 v1 = 0, v2 = 0;
        cursor += mscrt_v4_parse_s32(raw_data, cursor, &v1);
        cursor += mscrt_v4_parse_s32(raw_data, cursor, &v2);
        handler->catch_funclet_cont_addr[0]    = (u64)v1;
        handler->catch_funclet_cont_addr[1]    = (u64)v2;
        handler->catch_funclet_cont_addr_count = 2;
      } break;
    }
  } else {
    switch (cont_type) {
      case MSCRT_ContV4Type_NoMetadata: {
      } break;
      case MSCRT_ContV4Type_OneFuncRelAddr: {
        u32 v = 0;
        cursor += mscrt_v4_parse_u32(raw_data, cursor, &v);
        handler->catch_funclet_cont_addr[0]    = func_voff + (u64)v;
        handler->catch_funclet_cont_addr_count = 1;
      } break;
      case MSCRT_ContV4Type_TwoFuncRelAddr: {
        u32 v1 = 0, v2 = 0;
        cursor += mscrt_v4_parse_u32(raw_data, cursor, &v1);
        cursor += mscrt_v4_parse_u32(raw_data, cursor, &v2);
        handler->catch_funclet_cont_addr[0]    = func_voff + (u64)v1;
        handler->catch_funclet_cont_addr[1]    = func_voff + (u64)v2;
        handler->catch_funclet_cont_addr_count = 2;
      } break;
    }
  }
  
  u64 read_size = cursor - offset;
  return read_size;
}

internal u64
mscrt_parse_handler_type_v4_array(Arena                      *arena,
                                  String8                     raw_data,
                                  u64                         offset,
                                  u64                         func_voff,
                                  MSCRT_EhHandlerTypeV4Array *array_out)
{
  u64 cursor = offset;
  u32 count  = 0;
  cursor += mscrt_v4_parse_u32(raw_data, cursor, &count);
  
  MSCRT_EhHandlerTypeV4 *handlers = 0;
  if (count) {
    handlers = push_array(arena, MSCRT_EhHandlerTypeV4, count);
    for (u32 i = 0; i < count; ++i) {
      cursor += mscrt_parse_handler_type_v4(raw_data, cursor, func_voff, &handlers[i]);
    }
  }
  
  array_out->count = count;
  array_out->v     = handlers;
  
  u64 read_size = cursor - offset;
  return read_size;
}

internal u64
mscrt_parse_unwind_v4_entry(String8 raw_data, u64 offset, MSCRT_UnwindEntryV4 *entry_out)
{
  u64 cursor = offset;
  
  u32 type_and_next_off = 0;
  cursor += mscrt_v4_parse_u32(raw_data, cursor, &type_and_next_off);
  
  entry_out->type     = type_and_next_off & 0x3;
  entry_out->next_off = type_and_next_off >> 2;
  
  switch (entry_out->type) {
    case MSCRT_UnwindMapV4Type_DtorWithObj:
    case MSCRT_UnwindMapV4Type_DtorWithPtrToObj: {
      cursor += mscrt_v4_parse_s32(raw_data, cursor, &entry_out->action);
      cursor += mscrt_v4_parse_u32(raw_data, cursor, &entry_out->object);
    } break;
    case MSCRT_UnwindMapV4Type_VOFF: {
      cursor += mscrt_v4_parse_s32(raw_data, cursor, &entry_out->action);
    } break;
    case MSCRT_UnwindMapV4Type_NoUW: {
      // no action and/or object is associated with this type
    } break;
    default: {
      assert(!"unknown unwind entry type");
    } break;
  }
  
  u64 read_size = cursor - offset;
  return read_size;
}

internal u64
mscrt_parse_unwind_map_v4(Arena *arena, String8 raw_data, u64 off, MSCRT_UnwindMapV4 *map_out)
{
  u64 cursor = off;
  cursor += mscrt_v4_parse_u32(raw_data, cursor, &map_out->count);
  map_out->v = push_array(arena, MSCRT_UnwindEntryV4, map_out->count);
  for (u32 i = 0; i < map_out->count; ++i) {
    cursor += mscrt_parse_unwind_v4_entry(raw_data, cursor, &map_out->v[i]);
  }
  u64 read_size = cursor - off;
  return read_size;
}

internal u64
mscrt_parse_try_block_map_array_v4(Arena                   *arena,
                                   String8                  raw_data,
                                   u64                      off,
                                   u64                      section_count,
                                   COFF_SectionHeader      *sections,
                                   u64                      func_voff,
                                   MSCRT_TryBlockMapV4Array *map_out)
{
  u64 cursor = off;
  
  u32 try_block_map_count = 0;
  cursor += mscrt_v4_parse_u32(raw_data, cursor, &try_block_map_count);
  
  MSCRT_TryBlockMapV4 *try_block_map = push_array(arena, MSCRT_TryBlockMapV4, try_block_map_count);
  for (u32 itry = 0; itry < try_block_map_count; ++itry) {
    MSCRT_TryBlockMapV4 *try_block = &try_block_map[itry];
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &try_block->try_low);
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &try_block->try_high);
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &try_block->catch_high);
    
    i32 handler_array_voff = 0;
    cursor += mscrt_v4_parse_s32(raw_data, cursor, &handler_array_voff);
    
    u64 handler_array_foff = coff_foff_from_voff(sections, section_count, (u32)handler_array_voff);
    mscrt_parse_handler_type_v4_array(arena, raw_data, handler_array_foff, func_voff, &try_block->handlers);
  }
  
  map_out->count = try_block_map_count;
  map_out->v     = try_block_map;
  
  u64 read_size = cursor - off;
  return read_size;
}

internal u64
mscrt_parse_ip2state_map_v4(Arena              *arena,
                            String8             raw_data,
                            u64                 off,
                            u64                 func_voff,
                            MSCRT_IP2State32V4 *ip2state_map_out)
{
  u64 cursor = off;
  
  u32 count = 0;
  cursor += mscrt_v4_parse_u32(raw_data, cursor, &count);
  
  u32 *voffs  = push_array(arena, u32, count);
  i32 *states = push_array(arena, i32, count);
  
  u32 prev_voff = func_voff;
  for (u32 i = 0; i < count; ++i) {
    // virtual offsets are encoded as deltas
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &voffs[i]);
    voffs[i] += prev_voff;
    prev_voff = voffs[i];
    
    // states are encoded with +1 to avoid encoding negative integers
    u32 encoded_state = 0;
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &encoded_state);
    states[i] = (i32)encoded_state - 1;
  }
  
  ip2state_map_out->count  = count;
  ip2state_map_out->voffs  = voffs;
  ip2state_map_out->states = states;
  
  u64 read_size = cursor - off;
  return read_size;
}

internal u64
mscrt_parse_func_info_v4(Arena                     *arena,
                         String8                 raw_data,
                         u64                     section_count,
                         COFF_SectionHeader     *sections,
                         u64                     off,
                         u64                     func_voff,
                         MSCRT_ParsedFuncInfoV4 *func_info_out)
{
  u64 cursor = off;
  
  MSCRT_FuncInfo32V4 func_info = {0};
  cursor += str8_deserial_read_struct(raw_data, cursor, &func_info.header);
  if (func_info.header & MSCRT_FuncInfoV4Flag_IsBBT) {
    cursor += mscrt_v4_parse_u32(raw_data, cursor, &func_info.bbt_flags);
  }
  if (func_info.header & MSCRT_FuncInfoV4Flag_UnwindMap) {
    cursor += mscrt_v4_parse_s32(raw_data, cursor, &func_info.unwind_map_voff);
  }
  if (func_info.header & MSCRT_FuncInfoV4Flag_TryBlockMap) {
    cursor += mscrt_v4_parse_s32(raw_data, cursor, &func_info.try_block_map_voff);
  }
  if (func_info.header & MSCRT_FuncInfoV4Flag_IsSeparated) {
    // TODO: separted IP state map
    NotImplemented;
  } else {
    cursor += mscrt_v4_parse_s32(raw_data, cursor, &func_info.ip_to_state_map_voff);
  }
  if (func_info.header & MSCRT_FuncInfoV4Flag_IsCatch) {
    cursor += mscrt_v4_parse_s32(raw_data, cursor, &func_info.wrt_frame_establisher_voff);
  }
  
  MSCRT_UnwindMapV4 unwind_map = {0};
  if (func_info.header & MSCRT_FuncInfoV4Flag_UnwindMap) {
    u64 unwind_map_foff = coff_foff_from_voff(sections, section_count, func_info.unwind_map_voff);
    mscrt_parse_unwind_map_v4(arena, raw_data, unwind_map_foff, &unwind_map);
  }
  
  MSCRT_TryBlockMapV4Array try_block_map = {0};
  if (func_info.header & MSCRT_FuncInfoV4Flag_TryBlockMap) {
    u64 try_block_map_foff = coff_foff_from_voff(sections, section_count, func_info.try_block_map_voff);
    mscrt_parse_try_block_map_array_v4(arena, raw_data, try_block_map_foff, section_count, sections, func_voff, &try_block_map);
  }
  
  MSCRT_IP2State32V4 ip2state_map = {0};
  if (func_info.header & MSCRT_FuncInfoV4Flag_IsSeparated) {
    assert(!"TODO: separated ip2state map");
  } else {
    u64 ip_to_state_map_foff = coff_foff_from_voff(sections, section_count, func_info.ip_to_state_map_voff);
    mscrt_parse_ip2state_map_v4(arena, raw_data, ip_to_state_map_foff, func_voff, &ip2state_map);
  }
  
  func_info_out->header        = func_info.header;
  func_info_out->bbt_flags     = func_info.bbt_flags;
  func_info_out->try_block_map = try_block_map;
  func_info_out->unwind_map    = unwind_map;
  func_info_out->ip2state_map  = ip2state_map;
  
  u64 read_size = cursor - off;
  return read_size;
}

////////////////////////////////

internal Rng1U64List
mscrt_catch_blocks_from_data_x8664(Arena              *arena,
                                   String8             raw_data,
                                   u64                 section_count,
                                   COFF_SectionHeader *sections,
                                   Rng1U64             except_frange)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  Rng1U64List result = {0};
  
  String8        raw_pdata   = str8_substr(raw_data, except_frange);
  u64            pdata_count = raw_pdata.size / sizeof(PE_IntelPdata);
  PE_IntelPdata *src_pdata   = (PE_IntelPdata *)raw_pdata.str;
  PE_IntelPdata *opl_pdata   = src_pdata + pdata_count;
  
  for (PE_IntelPdata *pdata = src_pdata; pdata < opl_pdata; ++pdata) {
    u64            uwinfo_foff = coff_foff_from_voff(sections, section_count, pdata->voff_unwind_info);
    PE_UnwindInfo *uwinfo      = str8_deserial_get_raw_ptr(raw_data, uwinfo_foff, sizeof(*uwinfo));
    
    u8  flags            = PE_UNWIND_INFO_FLAGS_FROM_HDR(uwinfo->header);
    b32 is_chained       = !!(flags & PE_UnwindInfoFlag_CHAINED);
    b32 has_handler_data = !is_chained && ((flags & (PE_UnwindInfoFlag_EHANDLER | PE_UnwindInfoFlag_UHANDLER)) != 0);
    
    if (has_handler_data) {
      Temp temp = temp_begin(scratch.arena);
      
      u32 actual_code_count = PE_UNWIND_INFO_GET_CODE_COUNT(uwinfo->codes_num);
      u64 handler_data_foff = uwinfo_foff + sizeof(PE_UnwindInfo) + actual_code_count * sizeof(PE_UnwindCode);
      u32 handler_voff      = *(u32 *)str8_deserial_get_raw_ptr(raw_data, handler_data_foff, sizeof(handler_voff));
      
      String8 handler_name = str8_zero();
      /* TODO:
      {
        UnitID     uid  = syms_group_uid_from_voff__accelerated(group, handler_voff);
        UnitAccel *unit = syms_group_unit_from_uid(group, uid);
        SYMS_SymbolID   sid  = syms_group_proc_sid_from_uid_voff__accelerated(group, uid, handler_voff);
        handler_name = syms_group_symbol_name_from_sid(temp.arena, group, unit, sid);
      }
      */
      
      b32 is_handler_v3_or_below = str8_match_lit("__CxxFrameHandler3",  handler_name, 0) ||
        str8_match_lit("__GSHandlerCheck_EH", handler_name, 0);
      if (is_handler_v3_or_below) {
        u64            func_info_foff = handler_data_foff + sizeof(handler_voff);
        MSCRT_FuncInfo func_info      = {0};
        mscrt_parse_func_info(temp.arena, raw_data, section_count, sections, func_info_foff, &func_info);
        
        for (u32 itry = 0; itry < func_info.try_block_map_count; ++itry) {
          MSCRT_TryMapBlock *try_block = &func_info.try_block_map[itry];
          for (u32 icatch = 0; icatch < try_block->catch_handlers_count; ++icatch) {
            MSCRT_EhHandlerType32 *catch_block     = &try_block->catch_handlers[icatch];
            u64                    catch_pdata_off = pe_pdata_off_from_voff__binary_search_x8664(raw_pdata, catch_block->catch_handler_voff);
            PE_IntelPdata         *catch_pdata     = str8_deserial_get_raw_ptr(raw_pdata, catch_pdata_off, sizeof(*catch_pdata));
            rng1u64_list_push(arena, &result, rng_1u64(catch_pdata->voff_first, catch_pdata->voff_one_past_last));
          }
        }
        goto next;
      }
      
      b32 is_handler_v4 = str8_match_lit("__CxxFrameHandler4", handler_name, 0) ||
        str8_match_lit("__GSHandlerCheck_EH4", handler_name, 0);
      if (is_handler_v4) {
        u32                   func_info_voff = *(u32 *)str8_deserial_get_raw_ptr(raw_data, handler_data_foff + sizeof(handler_voff), sizeof(func_info_voff));
        u64                   func_info_foff = coff_foff_from_voff(sections, section_count, func_info_voff);
        MSCRT_ParsedFuncInfoV4 func_info     = {0};
        mscrt_parse_func_info_v4(temp.arena, raw_data, section_count, sections, func_info_foff, pdata->voff_first, &func_info);
        
        for (u32 itry = 0; itry < func_info.try_block_map.count; ++itry) {
          MSCRT_TryBlockMapV4 *try_block = &func_info.try_block_map.v[itry];
          for (u32 icatch = 0; icatch < try_block->handlers.count; ++icatch) {
            MSCRT_EhHandlerTypeV4 *catch_block     = &try_block->handlers.v[icatch];
            u64                    catch_pdata_off = pe_pdata_off_from_voff__binary_search_x8664(raw_pdata, catch_block->catch_code_voff);
            PE_IntelPdata         *catch_pdata     = str8_deserial_get_raw_ptr(raw_pdata, catch_pdata_off, sizeof(*catch_pdata));
            rng1u64_list_push(arena, &result, rng_1u64(catch_pdata->voff_first, catch_pdata->voff_one_past_last));
          }
        }
        goto next;
      }
      
      next:;
      temp_end(temp);
    }
  }
  
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Enum -> String

internal String8
mscrt_string_from_eh_adjectives(Arena *arena, MSCRT_EhHandlerTypeFlags adjectives)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List adj_list = {0};
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsConst) {
    str8_list_pushf(scratch.arena, &adj_list, "Const");
  }
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsVolatile) {
    str8_list_pushf(scratch.arena, &adj_list, "Volatile");
  }
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsUnaligned) {
    str8_list_pushf(scratch.arena, &adj_list, "Unaligned");
  }
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsReference) {
    str8_list_pushf(scratch.arena, &adj_list, "Reference");
  }
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsResumable) {
    str8_list_pushf(scratch.arena, &adj_list, "Resumable");
  }
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsStdDotDot) {
    str8_list_pushf(scratch.arena, &adj_list, "StdDotDot");
  }
  if (adjectives & MSCRT_EhHandlerTypeFlag_IsComplusEH) {
    str8_list_pushf(scratch.arena, &adj_list, "ComplusEH");
  }
  String8 result = str8_list_join(arena, &adj_list, &(StringJoin){.sep=str8_lit(", ")});
  scratch_end(scratch);
  return result;
}

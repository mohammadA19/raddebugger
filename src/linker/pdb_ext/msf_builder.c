// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

internal u64
msf_get_data_node_size(MSF_UInt page_size)
{
  u64 interval = msf_get_fpm_interval_correct(page_size);
  u64 bytes_per_interval = interval * (u64)page_size;
  return bytes_per_interval;
}

internal void
msf_page_data_list_push(Arena *arena, MSF_PageDataList *list, MSF_UInt page_size, MSF_UInt count)
{
  u64 data_size = msf_get_data_node_size(page_size);
  for (MSF_UInt i = 0; i < count; i += 1) {
    // TODO: clearing memory to zero here is expensive,
    // with 4KiB pages we have to zero-out 128 MiB 
    // memory block
    // 
    // we can make API for stream allocation to let user 
    // choose between zeroed and dirty allocations
    //
    u8 *data = push_array_aligned(arena, u8, data_size, page_size);

    // init node
    MSF_PageDataNode *node = push_array_no_zero(arena, MSF_PageDataNode, 1);
    node->prev = 0;
    node->next = 0;
    node->data = data;

    // push node to list
    DLLPushBack(list->first, list->last, node);
    list->count += 1;
  }
}

internal MSF_PageDataList
msf_page_data_list_pop(MSF_PageDataList *list, MSF_UInt count)
{
  MSF_PageDataList result = {0};
  
  MSF_UInt to_remove = min(count, list->count);
  for (MSF_UInt i = 0; i < to_remove; i += 1) {
    MSF_PageDataNode *node = list->last;
    DLLRemove(list->first, list->last, node);
    
    node->prev = 0;
    node->next = 0;
    
    DLLPushBack(result.first, result.last, node);
    result.count += 1;
  }
  list->count -= to_remove;
  
  return result;
}

internal void
msf_page_data_list_concat_in_place(MSF_PageDataList *list, MSF_PageDataList *to_concat)
{
  DLLConcatInPlace(list, to_concat);
}

internal void
msf_set_page_data_list(Arena *arena, MSF_PageDataList *list, MSF_UInt page_size, String8 data)
{
  ProfBeginFunction();

  u64 node_size = msf_get_data_node_size(page_size);
  u64 node_count = CeilIntegerDiv(data.size, node_size);
  
  u64 node_idx;
  for (node_idx = 0; node_idx < node_count - 1; node_idx += 1) {
    MSF_PageDataNode *node = push_array(arena, MSF_PageDataNode, 1);
    node->data = data.str + node_idx * node_size;
    SLLQueuePush(list->first, list->last, node);
    list->count += 1;
  }
  
  ProfBegin("Last Page Handle");
  b32 is_last_node_size_aligned = (data.size & (node_size - 1)) == 0;
  u8 *last_node_data = 0;
  if (is_last_node_size_aligned) {
    last_node_data = data.str + node_idx * node_size;
  } else {
    u64 last_node_size = data.size % node_size;
    last_node_data = push_array_no_zero(arena, u8, node_size);
    MemoryCopy(last_node_data, data.str + node_idx * node_size, last_node_size);
  }
  ProfEnd();
  
  MSF_PageDataNode *last_node = push_array(arena, MSF_PageDataNode, 1);
  last_node->data = last_node_data;
  SLLQueuePush(list->first, list->last, last_node);
  list->count += 1;

  ProfEnd();
}

internal String8
msf_data_from_pn(MSF_PageDataList list, MSF_UInt page_size, MSF_PageNumber pn)
{
  u64 node_size = msf_get_data_node_size(page_size);
  u64 page_offset = (u64)pn * (u64)page_size;
  u64 data_node_idx = page_offset / node_size;
  MSF_PageDataNode *node = list.first;
  for (u64 i = 0; i < data_node_idx; i += 1) {
    node = node->next;
  }
  u64 node_offset = page_offset % node_size;
  u8 *ptr = node->data + node_offset;
  String8 data = str8(ptr, page_size);
  return data;
}

////////////////////////////////

internal MSF_StreamNode *
msf_stream_list_push(Arena *arena, MSF_StreamList *list)
{
  MSF_StreamNode *n = push_array(arena, MSF_StreamNode, 1);
  DLLPushBack(list->first, list->last, n);
  list->count += 1;
  return n;
}

internal void
msf_stream_list_remove(MSF_StreamList *list, MSF_StreamNode *node)
{
  assert(list->count > 0);
  DLLRemove(list->first, list->last, node);
  list->count -= 1;
}

////////////////////////////////

internal void
msf_page_list_push_node(MSF_PageList *list, MSF_PageNode *node)
{
  DLLPushBack(list->first, list->last, node);
  list->count += 1;
}

internal MSF_PageNode *
msf_page_list_push(Arena *arena, MSF_PageList *list)
{
  MSF_PageNode *node = push_array(arena, MSF_PageNode, 1);
  msf_page_list_push_node(list, node);
  return node;
}

internal MSF_PageNode *
msf_page_list_pop_last(MSF_PageList *list)
{
  MSF_PageNode *node = NULL;
  if (list->count) {
    node = list->last;
    DLLRemove(list->first, list->last, node);
    list->count -= 1;
  }
  return node;
}

internal void
msf_page_list_concat_in_place(MSF_PageList *list, MSF_PageList *to_concat)
{
  DLLConcatInPlace(list, to_concat);
}

internal MSF_PageNumber *
msf_page_list_to_arr(Arena *arena, MSF_PageList list)
{
  MSF_PageNumber *arr = push_array(arena, MSF_PageNumber, list.count);
  MSF_UInt i = 0;
  for (MSF_PageNode *node = list.first; node != 0; node = node->next, i += 1) {
    arr[i] = node->pn;
  }
  return arr;
}

internal MSF_PageNode *
msf_page_from_index(MSF_PageList page_list, MSF_UInt index)
{
  MSF_PageNode *page;
  
  b32 scan_from_last_node = index > page_list.count/2;
  if (scan_from_last_node) {
    page = page_list.last;
    if (page_list.count > 0) {
      for (MSF_UInt i = page_list.count - 1; i > index; i -= 1) {
        page = page->prev;
        if (!page) {
          return 0;
        }
      }
    }
  } else {
    page = page_list.first;
    for (MSF_UInt i = 0; i < index; i += 1) {
      page = page->next;
      if (!page) {
        return 0;
      }
    }
  }
  return page;
}

internal void
msf_page_list_push_extant_page_arr(Arena *arena, MSF_PageList *list,
                                   MSF_PageDataList page_data_list, MSF_UInt page_size,
                                   MSF_PageNumber *pn_arr, MSF_UInt pn_count)
{
  u64 node_size = msf_get_data_node_size(page_size);
  u64 data_max = page_data_list.count * node_size;
  for (MSF_PageNumber *pn_ptr = pn_arr, *pn_opl = pn_ptr + pn_count; pn_ptr < pn_opl; pn_ptr += 1) {
    // is page number valid?
    assert(*pn_ptr * page_size + page_size <= data_max);
    
    // init page node
    MSF_PageNode *page_node = msf_page_list_push(arena, list);
    page_node->pn = *pn_ptr;
  }
}

internal void
msf_page_list_push_extant_page(Arena *arena, MSF_PageList *list, MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber pn)
{
  msf_page_list_push_extant_page_arr(arena, list, page_data_list, page_size, &pn, 1);
}

#if LNK_PARANOID
internal void
msf_check_fpm_bits_for_page_list(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageList page_list, MSF_UInt test_state)
{
  for (MSF_PageNode *page_node = page_list.first; page_node != 0; page_node = page_node->next) {
    MSF_UInt state = msf_get_fpm_page_bit_state(page_data_list, page_size, active_fpm, page_node->pn);
    if (state != test_state) {
      //assert(!"state bit doesn't match");
    }
  }
}
#endif

////////////////////////////////

internal MSF_UInt
msf_count_pages(MSF_UInt page_size, u64 data_size)
{
  MSF_UInt page_count = CeilIntegerDiv(data_size, page_size);
  return page_count;
}

internal MSF_PageNumber
msf_get_page_count_cap(MSF_PageDataList page_data_list, MSF_UInt page_size)
{
  u64 node_size = msf_get_data_node_size(page_size);
  u64 file_size = page_data_list.count * node_size;
  u64 count = CeilIntegerDiv(file_size, (u64)page_size);
  return safe_cast_u32(count);
}

////////////////////////////////

internal []u32
msf_fpm_data_from_pn(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber fpm_pn)
{
  String8 raw_fpm = msf_data_from_pn(page_data_list, page_size, fpm_pn);
  []u32 fpm_data;
  fpm_data.count = raw_fpm.size / sizeof(fpm_data.v[0]);
  fpm_data.v = (u32*)raw_fpm.str;
  return fpm_data;
}

internal MSF_UInt
msf_get_fpm_interval_correct(MSF_UInt page_size)
{
  return page_size * MSF_BITS_PER_CHAR;
}

internal MSF_UInt
msf_get_fpm_interval_wrong(MSF_UInt page_size)
{
  return page_size;
}

internal MSF_UInt
msf_get_fpm_idx_from_pn(MSF_UInt page_size, MSF_PageNumber pn)
{
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(page_size);
  MSF_UInt fpm_idx = pn / fpm_interval_correct;
  return fpm_idx;
}

internal MSF_UInt
msf_get_fpm_page_count(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_UInt fpm_interval)
{
  u64 node_size = msf_get_data_node_size(page_size);
  u64 file_size = (u64)page_data_list.count * node_size;
  u64 file_page_count = CeilIntegerDiv(file_size, page_size);
  u64 fpm_page_count = CeilIntegerDiv(file_page_count, (u64)fpm_interval);
  return safe_cast_u32(fpm_page_count);
}

internal MSF_PageNumberArray
msf_get_fpm_page_arr(Arena *arena, MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_UInt active_fpm)
{
  assert(active_fpm == MSF_FPM0 || active_fpm == MSF_FPM1);
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(page_size);
  MSF_UInt fpm_interval_wrong = msf_get_fpm_interval_wrong(page_size);
  MSF_UInt page_count = msf_get_page_count_cap(page_data_list, page_size);
  MSF_PageNumberArray arr;
  arr.count = CeilIntegerDiv(page_count, fpm_interval_correct);
  arr.v = push_array(arena, MSF_PageNumber, arr.count);
  for (MSF_UInt interval_idx = 0; interval_idx < arr.count; interval_idx += 1) {
    arr.v[interval_idx] = active_fpm + interval_idx * fpm_interval_wrong;
  }
  return arr;
}

internal MSF_PageNumber
msf_get_fpm_from_page_number(MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageNumber pn)
{
  assert(active_fpm == 1 || active_fpm == 2);
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(page_size);
  MSF_UInt fpm_interval_wrong = msf_get_fpm_interval_wrong(page_size);
  MSF_PageNumber fpm_pn = active_fpm;
  fpm_pn += (pn / fpm_interval_correct) * fpm_interval_wrong;
  return fpm_pn;
}

internal MSF_UInt
msf_get_fpm_page_bit_state(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageNumber pn)
{
  // fetch FPM
  MSF_PageNumber fpm_pn = msf_get_fpm_from_page_number(page_size, active_fpm, pn);
  []u32 fpm_data = msf_fpm_data_from_pn(page_data_list, page_size, fpm_pn);
  
  // get page bit
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(page_size);
  MSF_UInt page_bit_idx = pn % fpm_interval_correct;
  MSF_UInt state = bit_array_get_bit32(fpm_data, page_bit_idx);
  
  return state;
}

internal void
msf_set_fpm_bit_(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageNumber pn, b32 state)
{
  // fetch FPM
  MSF_PageNumber fpm_pn = msf_get_fpm_from_page_number(page_size, active_fpm, pn);
  []u32 fpm_data = msf_fpm_data_from_pn(page_data_list, page_size, fpm_pn);
  
  // set page bit
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(page_size);
  MSF_UInt page_bit_idx = pn % fpm_interval_correct;
  bit_array_set_bit32(fpm_data, page_bit_idx, state);
}

internal void
msf_set_fpm_bit(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber active_fpm, MSF_PageNumber pn, b32 state)
{
  msf_set_fpm_bit_(page_data_list, page_size, active_fpm, pn, state);
}

internal b32
msf_grow(MSF_Context *msf, MSF_PageNumber new_page_count)
{
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(msf->page_size);
  MSF_UInt fpm_interval_wrong = msf_get_fpm_interval_wrong(msf->page_size);
  
  // check alloc limit
  u64 new_page_count64 = AlignPow2((u64)new_page_count, (u64)fpm_interval_correct);
  b32 is_overflowed = new_page_count64 > MSF_PN_MAX;
  if (is_overflowed) {
    return 0;
  }
  
  // check can only grow MSF here
  new_page_count = safe_cast_u32(new_page_count64);
  if (new_page_count < msf->page_count) {
    return 0;
  }

  // compute number of FPM pages to allocate
  //
  // we allocate 8 times more FPMs because in MS impl they use wrong interval:
  //    https://github.com/microsoft/microsoft-pdb/blob/master/PDB/msf/msf.cpp#L509
  //
  MSF_PageNumber prev_fpm_page_cap_wrong = msf_get_fpm_page_count(msf->page_data_list, msf->page_size, fpm_interval_wrong);
  MSF_PageNumber curr_fpm_page_cap_wrong = CeilIntegerDiv(new_page_count, fpm_interval_wrong);
  MSF_PageNumber alloc_count_wrong = curr_fpm_page_cap_wrong - prev_fpm_page_cap_wrong;
  MSF_PageNumber next_pn_wrong = prev_fpm_page_cap_wrong * fpm_interval_wrong;
  MSF_PageNumber end_pn_wrong = next_pn_wrong + alloc_count_wrong * fpm_interval_wrong;
  
  // compute correct number of FPM pages to grow
  MSF_PageNumber prev_fpm_page_cap_correct = msf_get_fpm_page_count(msf->page_data_list, msf->page_size, fpm_interval_correct);
  MSF_PageNumber curr_fpm_page_cap_correct = CeilIntegerDiv(new_page_count, fpm_interval_correct);
  MSF_PageNumber alloc_count_correct = curr_fpm_page_cap_correct - prev_fpm_page_cap_correct;
  MSF_PageNumber next_pn_correct = prev_fpm_page_cap_correct * fpm_interval_correct;
  MSF_PageNumber end_pn_correct = next_pn_correct + alloc_count_correct * fpm_interval_correct;
  
  MSF_PageNumber to_alloc = alloc_count_correct;
  
  // are there unused data nodes?
  if (msf->page_data_pool.count) {
    MSF_PageNumber pool_alloc_count = min(msf->page_data_pool.count, alloc_count_correct);
    MSF_PageDataList page_data_list = msf_page_data_list_pop(&msf->page_data_pool, pool_alloc_count);
    msf_page_data_list_concat_in_place(&msf->page_data_list, &page_data_list);
    to_alloc -= pool_alloc_count;
  }
  
  // push enough data nodes to encompass allocated FPMs
  msf_page_data_list_push(msf->arena, &msf->page_data_list, msf->page_size, to_alloc);
  
  // set FPM bits to free
  for (MSF_PageNumber pn = next_pn_wrong; pn < end_pn_wrong; pn += fpm_interval_wrong) {
    MSF_PageNumber fpm0_pn = pn + MSF_FPM0;
    MSF_PageNumber fpm1_pn = pn + MSF_FPM1;
    String8 fpm0_data = msf_data_from_pn(msf->page_data_list, msf->page_size, fpm0_pn);
    String8 fpm1_data = msf_data_from_pn(msf->page_data_list, msf->page_size, fpm1_pn);
    MemorySet(fpm0_data.str, 0xFF, msf->page_size);
    MemorySet(fpm1_data.str, 0xFF, msf->page_size);
  }
  
  // set correct FPM bits
  for (MSF_PageNumber pn = next_pn_correct; pn < end_pn_correct; pn += fpm_interval_correct) {
    MSF_PageNumber fpm0_pn = pn + MSF_FPM0;
    MSF_PageNumber fpm1_pn = pn + MSF_FPM1;
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, MSF_FPM0, fpm0_pn, MSF_PAGE_STATE_ALLOC);
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, MSF_FPM0, fpm1_pn, MSF_PAGE_STATE_ALLOC);
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, MSF_FPM1, fpm0_pn, MSF_PAGE_STATE_ALLOC);
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, MSF_FPM1, fpm1_pn, MSF_PAGE_STATE_ALLOC);
  }
  
  // update context
  msf->page_count += alloc_count_wrong * 2;

  return 1;
}

#if 0
internal b32
msf_shrink(MSF_Context *msf, MSF_PageNumber new_page_count)
{
  MSF_UInt fpm_interval_wrong = msf_get_fpm_interval_wrong(msf->page_size);
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(msf->page_size);
  
  u64 new_page_count64 = AlignPow2((u64)new_page_count, (u64)fpm_interval_correct);
  new_page_count = safe_cast_u32(new_page_count64);
  assert(new_page_count < msf->page_count);
  
  // compute number of FPM pages to deallocate
  MSF_PageNumber prev_fpm_page_count_wrong = msf_get_fpm_page_count(msf->page_data_list, msf->page_size, fpm_interval_wrong);
  MSF_PageNumber curr_fpm_page_count_wrong = CeilIntegerDiv(new_page_count, fpm_interval_wrong);
  MSF_PageNumber dealloc_count_wrong = prev_fpm_page_count_wrong - curr_fpm_page_count_wrong;
  
  // compute next FPM page number
  MSF_PageNumber next_pn = prev_fpm_page_count_wrong * fpm_interval_wrong;
  MSF_PageNumber end_pn = next_pn - dealloc_count_wrong * fpm_interval_wrong;
  
  // pop data nodes
  MSF_PageNumber prev_fpm_page_count_correct = msf_get_fpm_page_count(msf->page_data_list, msf->page_size, fpm_interval_correct);
  MSF_PageNumber curr_fpm_page_count_correct = CeilIntegerDiv(new_page_count, fpm_interval_correct);
  MSF_PageNumber dealloc_count_correct = prev_fpm_page_count_correct - curr_fpm_page_count_correct;
  MSF_PageDataList free_page_data_list = msf_page_data_list_pop(&msf->page_data_list, dealloc_count_correct);
  msf_page_data_list_concat_in_place(&msf->page_data_pool, &free_page_data_list);
  
  for (MSF_PageNumber pn = next_pn; pn > end_pn; pn -= fpm_interval_wrong) {
    MSF_PageNumber fpm0_pn = pn + MSF_FPM0;
    MSF_PageNumber fpm1_pn = pn + MSF_FPM1;
    
    // free FPM pages
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, 1, fpm0_pn, MSF_PAGE_STATE_FREE);
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, 1, fpm1_pn, MSF_PAGE_STATE_FREE);
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, 2, fpm0_pn, MSF_PAGE_STATE_FREE);
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, 2, fpm1_pn, MSF_PAGE_STATE_FREE);
  }
  
  // update context
  msf->page_count -= dealloc_count_wrong * 2;
  
  return true;
}
#endif

internal MSF_PageNumber *
msf_alloc_pn_arr(Arena *arena, MSF_Context *msf, MSF_UInt alloc_count)
{
  // make sure FPM has enough space for new page numbers
  // 
  // we grow FPM at correct intervals here because we pre-alloc unused FPM pages ahead of time
  MSF_UInt curr_page_cap = msf_get_page_count_cap(msf->page_data_list, msf->page_size);
  MSF_UInt new_page_count = msf->page_count + alloc_count;
  if (new_page_count > curr_page_cap) {
    b32 is_fpm_alloced = msf_grow(msf, new_page_count);
    if (!is_fpm_alloced) {
      return 0;
    }
  }

  Temp scratch = scratch_begin(&arena, 1);

  // reserve memory for page numbers
  MSF_PageNumber *pn_arr = push_array(arena, MSF_PageNumber, alloc_count);
  
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(msf->page_size);
  MSF_UInt fpm_interval_wrong = msf_get_fpm_interval_wrong(msf->page_size);
  
  // get first FPM page
  MSF_PageNumberArray fpm_pn_arr = msf_get_fpm_page_arr(scratch.arena, msf->page_data_list, msf->page_size, msf->active_fpm);
  
  for (MSF_UInt alloc_idx = 0; alloc_idx < alloc_count; ) {
    // get FPM bits
    MSF_UInt fpm_idx = msf->fpm_rover / fpm_interval_correct;
    assert(fpm_idx < fpm_pn_arr.count);
    MSF_PageNumber fpm_pn = fpm_pn_arr.v[fpm_idx];
    []u32 fpm_data = msf_fpm_data_from_pn(msf->page_data_list, msf->page_size, fpm_pn);
    
    // scan FPM for free bit
    MSF_UInt fpm_rover_page_relative = msf->fpm_rover % fpm_interval_correct;
    u32 bit_idx = bit_array_scan_left_to_right32(fpm_data, fpm_rover_page_relative, fpm_interval_correct, MSF_PAGE_STATE_FREE);
    
    b32 is_full = (bit_idx >= fpm_interval_correct);
    if (is_full) {
      msf->fpm_rover = (fpm_idx + 1) * fpm_interval_correct;
      continue;
    }
    
    // compute page number
    MSF_PageNumber pn = bit_idx + (fpm_idx * fpm_interval_correct);
    
    // make sure unused FPMs aren't allocated for regular streams,
    // we used to mark with free bits unused FPMs but in VS2022
    // update they started to check for these bits and VS began
    // to error out with "PDB format is not supported" message
    b32 is_pn_valid = (pn % fpm_interval_wrong) != MSF_FPM0 &&
                      (pn % fpm_interval_wrong) != MSF_FPM1;
    if (is_pn_valid) {
      // update FPM
      bit_array_set_bit32(fpm_data, bit_idx, MSF_PAGE_STATE_ALLOC);
      
      // store page number
      pn_arr[alloc_idx++] = pn;
    }
    
    // advance FPM rover
    msf->fpm_rover = pn + 1;
  }
  
  // update context
  msf->page_count += alloc_count;
  
  scratch_end(scratch);
  return pn_arr;
}

internal void
msf_free_pn_arr(MSF_Context *msf, MSF_PageNumber *pn_arr, MSF_UInt pn_count)
{
  // set FPM bits
  for (MSF_UInt i = 0; i < pn_count; i += 1) {
    MSF_PageNumber pn = pn_arr[i];
    msf_set_fpm_bit(msf->page_data_list, msf->page_size, msf->active_fpm, pn, MSF_PAGE_STATE_FREE);

    // update FPM cursor
    msf->fpm_rover = min(msf->fpm_rover, pn);
  }
  
  // update context
  assert(msf->page_count >= pn_count);
  msf->page_count -= pn_count;
}

internal MSF_PageList
msf_alloc_pages(MSF_Context *msf, MSF_UInt alloc_count)
{
  Temp scratch = scratch_begin(0, 0);
  
  MSF_PageList alloc_list = {0};
  MSF_PageNumber *pn_arr = msf_alloc_pn_arr(scratch.arena, msf, alloc_count);
  if (pn_arr) {
    for (MSF_UInt page_idx = 0; page_idx < alloc_count; page_idx += 1) {
      // get page node
      MSF_PageNode *page_node = 0;
      if (msf->page_pool.count) {
        page_node = msf_page_list_pop_last(&msf->page_pool);
        msf_page_list_push_node(&alloc_list, page_node);
      } else {
        page_node = msf_page_list_push(msf->arena, &alloc_list);
      }
      
      // copy page number
      page_node->pn = pn_arr[page_idx];
    }
  }
  
  scratch_end(scratch);
  return alloc_list;
}

internal void
msf_free_pages(MSF_Context *msf, MSF_PageList *page_list)
{
  Temp scratch = scratch_begin(0, 0);
  
  // free page numbers
  MSF_PageNumber *pn_arr = msf_page_list_to_arr(scratch.arena, *page_list);
  msf_free_pn_arr(msf, pn_arr, page_list->count);
  
  // push free nodes
  msf_page_list_concat_in_place(&msf->page_pool, page_list);

  scratch_end(scratch);
}

internal MSF_PageNumber
msf_find_max_pn_(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumberArray fpm_pn_arr)
{
  MSF_PageNumber max_pn = 0;
  
  MSF_UInt fpm_interval_correct = msf_get_fpm_interval_correct(page_size);
  MSF_UInt fpm_interval_wrong = msf_get_fpm_interval_wrong(page_size);
  MSF_UInt fpm_page_count = fpm_interval_correct / fpm_interval_wrong;
  for (MSF_Int fpm_pn_idx = (MSF_Int)fpm_pn_arr.count - 1; fpm_pn_idx >= 0; fpm_pn_idx -= 1) {
    MSF_PageNumber fpm_pn = fpm_pn_arr.v[fpm_pn_idx];
    []u32 fpm_data = msf_fpm_data_from_pn(page_data_list, page_size, fpm_pn);
    
    // we have to work around the fact that FPM bits are always alloced
    // and also there is a trail of unused FPM groups too
    u32 bit_idx = max_U32;
    for (MSF_Int i = fpm_page_count - 1; i >= 0; i -= 1) {
      u32 fpm_lo = i * fpm_interval_wrong + 3; // skip first page bit and FPM group bits
      u32 fpm_hi = i * fpm_interval_wrong + fpm_interval_wrong;
      bit_idx = bit_array_scan_right_to_left32(fpm_data, fpm_lo, fpm_hi, MSF_PAGE_STATE_ALLOC);
      if (bit_idx <= fpm_interval_correct) {
        break;
      }
    }
    
    // check first page bit
    if (bit_idx >= fpm_interval_correct) {
      bit_idx = bit_array_scan_left_to_right32(fpm_data, 0, 1, MSF_PAGE_STATE_ALLOC);
      if (bit_idx >= fpm_interval_correct) {
        continue;
      }
    }
    
    // compute max page number
    MSF_PageNumber pn = bit_idx + (MSF_UInt)fpm_pn_idx * fpm_interval_correct;
    max_pn = max(max_pn, pn);
    
    break;
  }
  
  return max_pn;
}

internal MSF_PageNumber
msf_find_max_pn(MSF_PageDataList page_data_list, MSF_UInt page_size)
{
  Temp scratch = scratch_begin(0, 0);
  MSF_PageNumberArray fpm0_pn_arr = msf_get_fpm_page_arr(scratch.arena, page_data_list, page_size, MSF_FPM0);
  MSF_PageNumberArray fpm1_pn_arr = msf_get_fpm_page_arr(scratch.arena, page_data_list, page_size, MSF_FPM1);
  MSF_PageNumber fpm0_max = msf_find_max_pn_(page_data_list, page_size, fpm0_pn_arr);
  MSF_PageNumber fpm1_max = msf_find_max_pn_(page_data_list, page_size, fpm1_pn_arr);
  MSF_PageNumber max_pn = max(fpm0_max, fpm1_max);
  scratch_end(scratch);
  return max_pn;
}

////////////////////////////////

internal b32
msf_write__(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNode **page_ptr, MSF_UInt *pos_ptr, void *buffer, MSF_UInt buffer_size)
{
  MSF_PageNode *start_page = *page_ptr;
  MSF_UInt start_pos = *pos_ptr;
  
  MSF_UInt buffer_pos = 0;
  while (*page_ptr) {
    MSF_UInt page_offset = *pos_ptr % page_size;
    
    // compute copy size
    MSF_UInt buffer_bytes_left = buffer_size - buffer_pos;
    MSF_UInt page_bytes_left   = page_size - page_offset;
    MSF_UInt copy_size         = min(buffer_bytes_left, page_bytes_left);

    // fetch page bytes
    MSF_PageNumber page_number = (*page_ptr)->pn;
    String8        page_bytes  = msf_data_from_pn(page_data_list, page_size, page_number);

    // copy bytes to buffer
    u8 *buffer_copy_ptr = (u8*)buffer + buffer_pos;
    u8 *page_bytes_ptr  = page_bytes.str + page_offset;
    MemoryCopy(page_bytes_ptr, buffer_copy_ptr, copy_size);

    // advance
    buffer_pos += copy_size;
    *pos_ptr   += copy_size;
    
    // have we used all bytes in this page?
    if (page_bytes_left <= copy_size) {
      *page_ptr = (*page_ptr)->next;
    }
    
    // have we copied all bytes?
    if (buffer_bytes_left <= copy_size) {
      break;
    }
  }
  
  b32 is_write_ok = (buffer_pos == buffer_size);
  
  // not enough bytes to perform write - restore positions
  if (!is_write_ok) {
    *page_ptr = start_page;
    *pos_ptr  = start_pos;
  }
  
  return is_write_ok;
}

internal MSF_UInt
msf_read__(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNode **page_ptr, MSF_UInt *pos_ptr, void *buffer, MSF_UInt buffer_size)
{
  MSF_UInt buffer_pos = 0;
  while (*page_ptr) {
    MSF_UInt page_offset = *pos_ptr % page_size;
    
    // compute copy size
    MSF_UInt buffer_bytes_left = buffer_size - buffer_pos;
    MSF_UInt page_bytes_left   = page_size - page_offset;
    MSF_UInt copy_size         = min(buffer_bytes_left, page_bytes_left);
    
    // fetch page bytes
    MSF_PageNumber page_number = (*page_ptr)->pn;
    String8        page_bytes  = msf_data_from_pn(page_data_list, page_size, page_number);
    
    // copy bytes to buffer
    u8 *buffer_ptr     = (u8*)buffer + buffer_pos;
    u8 *page_bytes_ptr = page_bytes.str + page_offset;
    MemoryCopy(buffer_ptr, page_bytes_ptr, copy_size);
    
    // advance
    buffer_pos  += copy_size;
    *pos_ptr    += copy_size;
    
    // no more bytes left in this page
    if (page_bytes_left <= copy_size) {
      *page_ptr = (*page_ptr)->next;
    }
    
    // have we copied all bytes?
    if (buffer_bytes_left <= copy_size) {
      break;
    }
  }
  
  MSF_UInt bytes_read = buffer_pos;
  //assert(bytes_read == buffer_size);
  
  return bytes_read;
}

internal b32
msf_write(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList page_list, MSF_UInt offset, void *buffer, MSF_UInt buffer_size)
{
  MSF_UInt page_idx = offset / page_size;
  MSF_PageNode *page = msf_page_from_index(page_list, page_idx);
  b32 is_write_ok = msf_write__(page_data_list, page_size, &page, &offset, buffer, buffer_size);
  return is_write_ok;
}

internal MSF_UInt
msf_read(MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList page_list, MSF_UInt offset, void *buffer, MSF_UInt buffer_size)
{
  MSF_UInt page_idx = offset / page_size;
  MSF_PageNode *page = msf_page_from_index(page_list, page_idx);
  MSF_UInt bytes_read = msf_read__(page_data_list, page_size, &page, &offset, buffer, buffer_size);
  return bytes_read;
}

////////////////////////////////

internal MSF_StreamNode * 
msf_stream_alloc_(Arena *arena, MSF_StreamList *list)
{
  assert(list->count < MSF_STREAM_NUMBER_MAX);
  MSF_UInt sn = list->count;
  MSF_StreamNode *stream_node = msf_stream_list_push(arena, list);
  MSF_Stream *stream = &stream_node->data;
  stream->sn = safe_cast_u16(sn);
  return stream_node;
}

internal MSF_StreamNumber
msf_stream_alloc_ex(MSF_Context *msf, MSF_UInt size)
{
  MSF_StreamNode *node = msf_stream_alloc_(msf->arena, &msf->sectab);
  MSF_Stream *stream = &node->data;
  msf_stream_resize_ex(msf, stream, size);
  return stream->sn;
}

internal MSF_StreamNumber
msf_stream_alloc(MSF_Context *msf)
{
  return msf_stream_alloc_ex(msf, 0);
}

internal b32
msf_stream_resize_ex(MSF_Context *msf, MSF_Stream *stream, MSF_UInt size)
{
  MSF_UInt new_page_count = msf_count_pages(msf->page_size, size);
  MSF_UInt cur_page_count = stream->page_list.count;
  
  if (new_page_count > cur_page_count) {
    MSF_UInt alloc_count = new_page_count - cur_page_count;
    MSF_PageList page_list = msf_alloc_pages(msf, alloc_count);
    msf_page_list_concat_in_place(&stream->page_list, &page_list);
  } else {
    MSF_PageList free_page_list = {0};
    for (MSF_UInt i = cur_page_count; i > new_page_count; i -= 1) {
      MSF_PageNode *page_node = msf_page_list_pop_last(&stream->page_list);
      msf_page_list_push_node(&free_page_list, page_node);
    }
    msf_free_pages(msf, &free_page_list);
  }
  
  // update stream
  stream->size = min(stream->size, stream->page_list.count * msf->page_size);
  stream->pos = min(stream->pos, stream->size);
  stream->pos_page = 0;

  return 1;
}

internal b32
msf_stream_resize(MSF_Context *msf, MSF_StreamNumber sn, MSF_UInt new_size)
{
  MSF_Stream *stream = msf_find_stream(msf, sn);
  b32 is_resized = 0;
  if (stream) {
    is_resized = msf_stream_resize_ex(msf, stream, new_size);
  }
  return is_resized;
}

internal b32
msf_stream_free(MSF_Context *msf, MSF_StreamNumber sn)
{
  b32 is_free_ok = 0;
  MSF_StreamNode *stream_node = msf_find_stream_node(msf, sn);
  if (stream_node) {
    msf_stream_list_remove(&msf->sectab, stream_node);
    msf_stream_resize_ex(msf, &stream_node->data, 0);
    stream_node->data.size = MSF_DELETED_STREAM_STAMP;
    is_free_ok = 1;
  }
  return is_free_ok;
}

internal void
msf_stream_set_size(MSF_Context *msf, MSF_StreamNumber sn, MSF_UInt size)
{
  MSF_Stream *stream = msf_find_stream(msf, sn);
  if (stream) {
    stream->size = min(size, stream->page_list.count * msf->page_size);
  } else {
    assert(!"invalid stream number");
  }
}

internal MSF_UInt
msf_stream_get_size(MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_UInt size = MSF_UINT_MAX;
  MSF_Stream *stream = msf_find_stream(msf, sn);
  if (stream) {
    size = stream->size;
  }
  return size;
}

internal MSF_UInt
msf_stream_get_cap__(MSF_Context *msf, MSF_Stream *stream)
{
  return stream->page_list.count * msf->page_size;
}

internal MSF_UInt
msf_stream_get_cap(MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_Stream *stream = msf_find_stream(msf, sn);
  MSF_UInt cap = 0;
  if (stream) {
    cap = msf_stream_get_cap__(msf, stream);
  }
  return cap;
}

internal MSF_UInt
msf_stream_get_pos__(MSF_Context *msf, MSF_Stream *stream)
{
  return stream->pos;
}

internal MSF_UInt
msf_stream_get_pos(MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_Stream *stream = msf_find_stream(msf, sn);
  MSF_UInt pos = MSF_UINT_MAX;
  if (stream) {
    pos = msf_stream_get_pos__(msf, stream);
  }
  return pos;
}

internal b32
msf_stream_seek__(MSF_Context *msf, MSF_Stream *stream, MSF_UInt new_pos) 
{ (void)msf;
  stream->pos = min(new_pos, stream->size);
  stream->pos_page = 0;
  return 1;
}

internal b32 
msf_stream_seek(MSF_Context *msf, MSF_StreamNumber sn, MSF_UInt new_pos)
{
  b32 is_seek_ok = 0;
  MSF_Stream *stream = msf_find_stream(msf, sn);
  if (stream) {
    is_seek_ok = msf_stream_seek__(msf, stream, new_pos);
  } else {
    assert(!"failed to stream seek");
  }
  return is_seek_ok;
}

internal b32
msf_stream_seek_start(MSF_Context *msf, MSF_StreamNumber sn)
{
  return msf_stream_seek(msf, sn, 0);
}

internal b32
msf_stream_seek_end(MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_UInt end = msf_stream_get_size(msf, sn);
  return msf_stream_seek(msf, sn, end);
}


internal b32 
msf_stream_write__(MSF_Context *msf, MSF_Stream *stream, void *buffer, MSF_UInt buffer_size)
{
  b32 is_write_ok = 0;

  // are we writing over limit?
  assert((u64)stream->pos + (u64)buffer_size <= (u64)MSF_UINT_MAX);
  
  // make sure we have enough space to write buffer
  MSF_UInt stream_cap = msf_stream_get_cap__(msf, stream);
  MSF_UInt stream_pos_opl = stream->pos + buffer_size;
  b32 grow_stream = stream_pos_opl > stream_cap;
  if (grow_stream) {
    b32 is_resize_ok = msf_stream_resize_ex(msf, stream, stream_pos_opl);
    if (!is_resize_ok) {
      goto exit;
    }
  }
  
  if (buffer) {
    // lookup page for current stream position
    if (!stream->pos_page) {
      MSF_UInt page_idx = stream->pos / msf->page_size;
      stream->pos_page = msf_page_from_index(stream->page_list, page_idx);
    }
  
    // make write
    is_write_ok = msf_write__(msf->page_data_list, msf->page_size, &stream->pos_page, &stream->pos, buffer, buffer_size);
  } else {
    stream->pos += buffer_size;
    stream->pos_page = 0;
    is_write_ok = 1;
  }
  
  // update stream size
  stream->size = max(stream->size, stream->pos);

exit:;
  assert(is_write_ok);
  return is_write_ok;
}

internal MSF_UInt
msf_stream_reserve__(MSF_Context *msf, MSF_Stream *stream, MSF_UInt res)
{
  ProfBeginV("MSF Reserve %m", res);
  
  b32 is_ok = 1;

  MSF_UInt cap = msf_stream_get_cap__(msf, stream);
  MSF_UInt pos = msf_stream_get_pos__(msf, stream);
  MSF_UInt cur = cap - pos;

  if (cur < res) {
    is_ok = msf_stream_write__(msf, stream, 0, res);
    ensure(is_ok);

    is_ok = msf_stream_seek__(msf, stream, pos);
    ensure(is_ok);
  }

  ProfEnd();
  return is_ok;
} 

internal b32
msf_stream_reserve(MSF_Context *msf, MSF_StreamNumber sn, MSF_UInt res)
{
  MSF_Stream *stream = msf_find_stream(msf, sn);
  b32 is_res_ok = 0;
  if (stream) {
    is_res_ok = msf_stream_reserve__(msf, stream, res);
  }
  return is_res_ok;
}

internal b32 
msf_stream_write(MSF_Context *msf, MSF_StreamNumber sn, void *buffer, MSF_UInt buffer_size)
{
  b32 is_write_ok = 0;
  MSF_Stream *stream = msf_find_stream(msf, sn);
  if (stream) {
    is_write_ok = msf_stream_write__(msf, stream, buffer, buffer_size);
  }
  return is_write_ok;
}

internal b32
msf_stream_write_string(MSF_Context *msf, MSF_StreamNumber sn, String8 string)
{
  return msf_stream_write(msf, sn, string.str, string.size);
}

internal b32
msf_stream_write_list(MSF_Context *msf, MSF_StreamNumber sn, String8List list)
{
  b32 is_write_ok = 0;
  MSF_Stream *stream = msf_find_stream(msf, sn);
  if (stream) {
    for (String8Node *node = list.first; node != 0; node = node->next) {
      is_write_ok = msf_stream_write__(msf, stream, node->string.str, node->string.size);
      if (!is_write_ok) {
        break;
      }
    }
  }
  return is_write_ok;
}

internal b32 
msf_stream_write_uint(MSF_Context *msf, MSF_StreamNumber sn, MSF_UInt value)
{
  return msf_stream_write_struct(msf, sn, &value);
}

internal b32
msf_stream_write_cstr(MSF_Context *msf, MSF_StreamNumber sn, String8 string)
{
  b32 is_string_written = msf_stream_write_string(msf, sn, string);
  b32 is_null_written = msf_stream_write(msf, sn, 0, 1);
  return is_string_written && is_null_written;
}

internal b32
msf_stream_write_u8(MSF_Context *msf, MSF_StreamNumber sn, u8 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_u16(MSF_Context *msf, MSF_StreamNumber sn, u16 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_u32(MSF_Context *msf, MSF_StreamNumber sn, u32 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_u64(MSF_Context *msf, MSF_StreamNumber sn, u64 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_s8(MSF_Context *msf, MSF_StreamNumber sn, i8 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_s16(MSF_Context *msf, MSF_StreamNumber sn, i16 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_s32(MSF_Context *msf, MSF_StreamNumber sn, i32 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal b32
msf_stream_write_s64(MSF_Context *msf, MSF_StreamNumber sn, i64 value)
{
  return msf_stream_write(msf, sn, &value, sizeof(value));
}

internal
THREAD_POOL_TASK_FUNC(msf_write_task)
{
  ProfBeginFunction();
  MSF_WriteTask *task = raw_task;

  Rng1U64  range    = task->range_arr[task_id];
  String8  data     = str8_substr(task->data, range);
  MSF_UInt data_pos = range.min + task->stream_pos;

  MSF_UInt      page_idx = data_pos / task->page_size;
  MSF_PageNode *page     = msf_page_from_index(task->page_list, page_idx);

  if (!msf_write__(task->page_data_list, task->page_size, &page, &data_pos, data.str, data.size)) {
    InvalidPath;
  }
  ProfEnd();
}

internal b32
msf_stream_write_parallel(TP_Context *tp, MSF_Context *msf, MSF_StreamNumber sn, void *buffer, MSF_UInt buffer_size)
{
  ProfBeginV("MSF Write Parallel %m", buffer_size);

  MSF_Stream *stream = msf_find_stream(msf, sn);

  b32 is_write_ok = msf_stream_reserve__(msf, stream, buffer_size);

  if (is_write_ok) {
    u64 expected_pos = stream->pos + buffer_size;

    u64 pre_size = min(AlignPadPow2(stream->pos, msf->page_size), buffer_size);
    u64 mid_size = AlignDownPow2(buffer_size - pre_size, msf->page_size);
    u64 end_size = buffer_size - (pre_size + mid_size);

    u8 *pre_ptr = (u8*)buffer;
    u8 *mid_ptr = (u8*)buffer + pre_size;
    u8 *end_ptr = (u8*)buffer + pre_size + mid_size;

    ProfBeginV("Write Buffer Pre %M", pre_size);
    b32 is_pre_written = msf_stream_write__(msf, stream, pre_ptr, pre_size);
    ensure(is_pre_written);
    ProfEnd();

    // write buffer mid
    if (mid_size > 0) {
	  Temp scratch = scratch_begin(0,0);
	  
      assert(stream->pos % msf->page_size == 0);
      assert(mid_size % msf->page_size == 0);

      MSF_WriteTask task;
      task.page_size      = msf->page_size;
      task.page_data_list = msf->page_data_list;
      task.page_list      = stream->page_list;
      task.stream_pos     = stream->pos;
      task.data           = str8(mid_ptr, mid_size);
      task.range_arr      = tp_divide_work(scratch.arena, mid_size, tp->worker_count);
      tp_for_parallel(tp, 0, tp->worker_count, msf_write_task, &task);

      // we rely on low-level msf_write__ to copy bytes which doesn't advance stream pos
      u64 after_mid = stream->pos + mid_size;
      b32 is_seek_ok = msf_stream_seek__(msf, stream, after_mid);
      ensure(is_seek_ok);
	  
      scratch_end(scratch);
    }

    ProfBeginV("Write Buffer End %M", end_size);
    b32 is_end_ok = msf_stream_write__(msf, stream, end_ptr, end_size);
    ensure(is_end_ok);
    ProfEnd();

    // did we write bytes correctly?
    ensure(stream->pos == expected_pos);
  }

  ProfEnd();
  return is_write_ok;
}

internal b32
msf_stream_write_string_parallel(TP_Context *tp, MSF_Context *msf, MSF_StreamNumber sn, String8 string)
{
  return msf_stream_write_parallel(tp, msf, sn, string.str, string.size);
}

////////////////////////////////

internal MSF_UInt
msf_stream_read__(MSF_Context *msf, MSF_Stream *stream, void *buffer, MSF_UInt buffer_size)
{
  // are we reading over limit?
  assert((u64)stream->pos + (u64)buffer_size <= (u64)MSF_UINT_MAX);
  
  // lookup page for current stream position
  if (!stream->pos_page) {
    MSF_UInt pos_page_idx = stream->pos / msf->page_size;
    stream->pos_page = msf_page_from_index(stream->page_list, pos_page_idx);
  }
  
  MSF_UInt bytes_read = msf_read__(msf->page_data_list, msf->page_size, &stream->pos_page, &stream->pos, buffer, buffer_size);
  return bytes_read;
}

internal MSF_UInt
msf_stream_read(MSF_Context *msf, MSF_StreamNumber sn, void *buffer, MSF_UInt buffer_size)
{
  MSF_Stream *stream = msf_find_stream(msf, sn);
  if (stream) {
    return msf_stream_read__(msf, stream, buffer, buffer_size);
  }
  return 0;
}

internal i8
msf_stream_read_s8(MSF_Context *msf, MSF_StreamNumber sn)
{
  i8 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal i16
msf_stream_read_s16(MSF_Context *msf, MSF_StreamNumber sn)
{
  i16 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal i32
msf_stream_read_s32(MSF_Context *msf, MSF_StreamNumber sn)
{
  i32 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal i64
msf_stream_read_s64(MSF_Context *msf, MSF_StreamNumber sn)
{
  i64 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal u8
msf_stream_read_u8(MSF_Context *msf, MSF_StreamNumber sn)
{
  u8 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal u16
msf_stream_read_u16(MSF_Context *msf, MSF_StreamNumber sn)
{
  u16 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal u32
msf_stream_read_u32(MSF_Context *msf, MSF_StreamNumber sn)
{
  u32 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal u64
msf_stream_read_u64(MSF_Context *msf, MSF_StreamNumber sn)
{
  u64 result = 0;
  msf_stream_read_struct(msf, sn, &result);
  return result;
}

internal String8
msf_stream_read_block(Arena *arena, MSF_Context *msf, MSF_StreamNumber sn, u64 block_size)
{
  u8 *block_buffer = push_array(arena, u8, block_size);
  MSF_UInt block_read = msf_stream_read(msf, sn, block_buffer, block_size);
  assert((u64)block_read == block_size);
  String8 block = str8(block_buffer, block_size);
  return block;
}

internal String8
msf_stream_read_string(Arena *arena, MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_UInt start_pos = msf_stream_get_pos(msf, sn);
  u64 size = 0;
  for (;; size += 1) {
    u8 cp = msf_stream_read_u8(msf, sn);
    if (cp == 0) {
      break;
    }
  }
  
  msf_stream_seek(msf, sn, start_pos);
  String8 string = msf_stream_read_block(arena, msf, sn, size);
  msf_stream_seek(msf, sn, start_pos + size + 1); // skip null

  return string;
}

internal void 
msf_stream_align(MSF_Context *msf, MSF_StreamNumber sn, MSF_UInt align)
{
  MSF_UInt pos = msf_stream_get_pos(msf, sn);
  MSF_UInt pos_aligned = AlignPow2(pos, align);
  msf_stream_seek(msf, sn, pos_aligned);
}

////////////////////////////////

internal MSF_Context * 
msf_alloc__(MSF_UInt page_size, MSF_PageNumber active_fpm)
{
  ProfBeginFunction();
  assert(active_fpm == MSF_FPM0 || active_fpm == MSF_FPM1);
  assert(IsPow2(page_size));
  
  Arena *arena = arena_alloc();
  
  MSF_Context *msf = push_array(arena, MSF_Context, 1);
  msf->arena = arena;
  msf->page_size = page_size;
  msf->active_fpm = active_fpm;
  
  ProfEnd();
  return msf;
}

internal MSF_Context *
msf_alloc(MSF_UInt page_size, MSF_UInt active_fpm)
{
  MSF_Context *msf = msf_alloc__(page_size, active_fpm);
  
  // reserve first page for header
  msf->header_page_list = msf_alloc_pages(msf, 1);
  assert(msf->header_page_list.count > 0);
  assert(msf->header_page_list.first->pn == 0);
  
  // reserve root page close to start of the file so we don't have to seek too far (not required)
  msf->root_page_list = msf_alloc_pages(msf, 1);
  assert(msf->root_page_list.count == 1);
  assert(msf->root_page_list.first->pn == 3);
  
  return msf;
}

internal MSF_StreamNode * 
msf_find_stream_node(MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_StreamNode *node;
  for (node = msf->sectab.first; node != 0; node = node->next) {
    if (node->data.sn == sn) {
      break;
    }
  }
  return node;
}

internal MSF_Stream *
msf_find_stream(MSF_Context *msf, MSF_StreamNumber sn)
{
  MSF_StreamNode *node = msf_find_stream_node(msf, sn);
  MSF_Stream *data = 0;
  if (node) {
    data = &node->data;
  }
  return data;
}

internal MSF_Error
msf_open_header(Arena *arena, MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList *page_list)
{
  ProfBeginFunction();
  msf_page_list_push_extant_page(arena, page_list, page_data_list, page_size, 0);
  ProfEnd();
  return MSF_Error_OK;
}

internal MSF_Error
msf_open_root(Arena *arena, MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageNumber root_pn, MSF_UInt stream_table_size, MSF_PageList *page_list)
{
  ProfBeginFunction();
  Temp scratch = scratch_begin(&arena, 1);
  MSF_PageNumber st_page_count = msf_count_pages(page_size, stream_table_size);
  MSF_UInt st_pn_size = sizeof(MSF_PageNumber) * st_page_count;
  MSF_PageNumber root_pn_count = msf_count_pages(page_size, st_pn_size);
  MSF_PageNumber *root_pn_arr = push_array(scratch.arena, MSF_PageNumber, root_pn_count);
  for (MSF_UInt i = 0; i < root_pn_count; i += 1) {
    root_pn_arr[i] = root_pn + i;
  }
  msf_page_list_push_extant_page_arr(arena, page_list, page_data_list, page_size, root_pn_arr, root_pn_count);
  scratch_end(scratch);
  ProfEnd();
  return MSF_Error_OK;
}

internal MSF_Error
msf_open_stream_table_page_list(Arena *arena, MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList root_page_list, MSF_UInt stream_table_size, MSF_PageList *page_list)
{
  ProfBeginFunction();
  Temp scratch = scratch_begin(&arena, 1);
  MSF_Error error = MSF_Error_OK; 
  MSF_UInt st_pn_count = msf_count_pages(page_size, stream_table_size);
  MSF_UInt st_pn_size = st_pn_count * sizeof(MSF_PageNumber);
  MSF_PageNumber *st_pn_arr = push_array(scratch.arena, MSF_PageNumber, st_pn_count);
  MSF_UInt st_pn_read_size = msf_read(page_data_list, page_size, root_page_list, 0, st_pn_arr, st_pn_size);
  if (st_pn_read_size == st_pn_size) {
    msf_page_list_push_extant_page_arr(arena, page_list, page_data_list, page_size, st_pn_arr, st_pn_count);
  } else {
    error = MSF_OpenError_UNABLE_TO_READ_STREAM_TABLE_PAGE_NUMBERS; 
  }
  scratch_end(scratch);
  ProfEnd();
  return error;
}

internal MSF_Error
msf_open_stream_table(Arena *arena, MSF_PageDataList page_data_list, MSF_UInt page_size, MSF_PageList st_page_list, MSF_UInt stream_table_size, MSF_StreamList *stream_list)
{
  ProfBeginFunction();
  Temp scratch = scratch_begin(&arena, 1);
  MSF_Error error = MSF_Error_OK;
  
  // read out entire stream table
  u8 *st_buffer = push_array(scratch.arena, u8, stream_table_size);
  MSF_UInt st_read_size = msf_read(page_data_list, page_size, st_page_list, 0, st_buffer, stream_table_size);
  if (st_read_size != stream_table_size) {
    error = MSF_OpenError_INVALID_STREAM_TABLE;
    goto exit;
  }
  
  // setup buffer reader
  String8 st_data = str8(st_buffer, st_read_size);
  u64 st_cursor = 0;
  
  MSF_UInt stream_count = 0;
  st_cursor += str8_deserial_read_struct(st_data, st_cursor, &stream_count);
  
  // stream count is a 32-bit but stream number is 16-bit?!
  if (stream_count > MSF_STREAM_NUMBER_MAX) {
    error = MSF_OpenError_STREAM_COUNT_OVERFLOW;
    goto exit;
  }
  
  // is there enoguh bytes to read streams sizes?
  u64 size_arr_end = st_cursor + (u64)stream_count * sizeof(MSF_UInt);
  if (size_arr_end > st_data.size) {
    error = MSF_OpenError_UNABLE_TO_READ_STREAM_SIZES;
    goto exit;
  }
  
  // make pointer to stream sizes array
  MSF_UInt *stream_size_arr = (MSF_UInt*)(st_buffer + st_cursor);
  st_cursor += sizeof(stream_size_arr[0]) * stream_count;
  
  u64 arena_pos_before_stream_allocations = arena_pos(arena);
  
  // open streams
  for (MSF_UInt stream_idx = 0; stream_idx < stream_count; stream_idx += 1) {
    MSF_UInt stream_size = stream_size_arr[stream_idx];
    b32 is_present = stream_size != MSF_DELETED_STREAM_STAMP;
    if (is_present) {
      MSF_PageNumber pn_count = msf_count_pages(page_size, stream_size);
      
      // is there enough bytes in buffer to build stream page list?
      MSF_UInt st_pn_end = st_cursor + pn_count * sizeof(MSF_PageNumber);
      if (st_pn_end > stream_table_size) {
        break;
      }
      
      // setup page number array
      MSF_PageNumber *pn_arr = (MSF_PageNumber*)(st_buffer + st_cursor);
      st_cursor += sizeof(pn_arr[0]) * pn_count;
      
      // build stream page list
      MSF_PageList page_list = {0};
      msf_page_list_push_extant_page_arr(arena, &page_list, page_data_list, page_size, pn_arr, pn_count);
      
      // alloc stream with opened pages
      MSF_StreamNode *stream_node = msf_stream_alloc_(arena, stream_list);
      stream_node->data.size = stream_size;
      stream_node->data.page_list = page_list;
    }
    // stream was deleted but slot was kept to be reused in subsequent allocations
    else {
      MSF_StreamNode *stream_node = msf_stream_alloc_(arena, stream_list);
      stream_node->data.size = stream_size;
    }
  }
  
  if (stream_list->count != stream_count) {
    arena_pop_to(arena, arena_pos_before_stream_allocations);
    error = MSF_OpenError_INVALID_STREAM_TABLE;
    goto exit;
  }
  
exit:;
  scratch_end(scratch);
  ProfEnd();
  return error;
}

internal MSF_Error
msf_open(String8 data, MSF_Context **msf_out)
{
  ProfBeginFunction();

  MSF_Error error = MSF_Error_OK;
  MSF_Context *msf = 0;
  MSF_PageDataList page_data_list = {0};

  // are there enough bytes for header?
  if (sizeof(MSF_Header70) > data.size) {
    error = MSF_OpenError_NOT_ENOUGH_BYTES_TO_READ_HEADER;
    goto exit;
  }
  
  // is this MSF 7.0?
  MSF_Header70 *header = (MSF_Header70*)data.str;
  if (MemoryCompare(header->magic, msf_msf70_magic, sizeof(msf_msf70_magic)) != 0) {
    error = MSF_OpenError_INVALID_MAGIC; 
    goto exit;
  }
  
  // validate page size
  if (!IsPow2(header->page_size)) {
    error = MSF_OpenError_PAGE_SIZE_IS_NOT_POW2;
    goto exit;
  }
  
  // validate page count
  MSF_UInt file_page_count = msf_count_pages(header->page_size, data.size);
  if (file_page_count != header->page_count) {
    error = MSF_OpenError_PAGE_COUNT_DOESNT_MATCH_DATA_SIZE;
    goto exit;
  }
  
  // validate FPM
  if (header->page_size < MSF_MIN_PAGE_SIZE) {
    error = MSF_OpenError_INVALID_PAGE_SIZE;
    goto exit;
  }
  if (header->page_size > MSF_MAX_PAGE_SIZE) {
    error = MSF_OpenError_INVALID_PAGE_SIZE;
    goto exit;
  }
  
  // is there enough bytes to initialize PDB?
  MSF_UInt check_size = header->page_size*3 + header->stream_table_size;
  if (check_size > data.size) { 
    error = MSF_OpenError_NOT_ENOUGH_PAGES_TO_INIT;
    goto exit;
  }
  
  // validate FPM
  if (header->active_fpm != MSF_FPM0 && header->active_fpm != MSF_FPM1) {
    error = MSF_OpenError_INVALID_ACTIVE_FPM;
    goto exit;
  }
  
  // is there enough bytes to initialize root stream?
  MSF_UInt root_pn_offset = OffsetOf(MSF_Header70, root_pn);
  if (root_pn_offset + header->stream_table_size > data.size) {
    error = MSF_OpenError_INVALID_ROOT_STREAM_PAGE_NUMBER;
    goto exit;
  }
  
  // validate root directory
  MSF_UInt root_directory_page_count = msf_count_pages(header->page_size, header->stream_table_size);
  MSF_UInt root_directory_max_page_count = header->page_size / sizeof(MSF_UInt);
  if (root_directory_page_count > root_directory_max_page_count) {
    error = MSF_Error_STREAM_TABLE_HAS_TOO_MANY_PAGES;
    goto exit;
  }

  // allocate MSF context and don't reserve special pages
  msf = msf_alloc__(header->page_size, header->active_fpm);
  
  // divide data into fixed size nodes (with 4KB page each node is 128MB)
  msf_set_page_data_list(msf->arena, &page_data_list, header->page_size, data);
  
  do {
    MSF_PageList header_page_list = {0};
    error = msf_open_header(msf->arena, page_data_list, header->page_size, &header_page_list);
    if (error != MSF_Error_OK) {
      break;
    }
    
    MSF_PageList root_page_list = {0};
    error = msf_open_root(msf->arena, page_data_list, header->page_size, header->root_pn, header->stream_table_size, &root_page_list);
    if (error != MSF_Error_OK) { 
      break;
    }
    
    MSF_PageList st_page_list = {0};
    error = msf_open_stream_table_page_list(msf->arena, page_data_list, header->page_size, root_page_list, header->stream_table_size, &st_page_list);
    if (error != MSF_Error_OK) {
      break;
    }
    
    MSF_StreamList stream_list = {0};
    error = msf_open_stream_table(msf->arena, page_data_list, header->page_size, st_page_list, header->stream_table_size, &stream_list);
    if (error != MSF_Error_OK) {
      break;
    }
    
    assert(msf->page_size == header->page_size);
    assert(msf->active_fpm == header->active_fpm);
    msf->page_count       = header->page_count;
    msf->page_data_list   = page_data_list;
    msf->header_page_list = header_page_list;
    msf->root_page_list   = root_page_list;
    msf->st_page_list     = st_page_list;
    msf->sectab               = stream_list;
    
    *msf_out = msf;
    
#if LNK_PARANOID
    msf_check_fpm_bits_for_page_list(page_data_list, msf->page_size, msf->active_fpm, header_page_list, MSF_PAGE_STATE_ALLOC);
    msf_check_fpm_bits_for_page_list(page_data_list, msf->page_size, msf->active_fpm, root_page_list, MSF_PAGE_STATE_ALLOC);
    msf_check_fpm_bits_for_page_list(page_data_list, msf->page_size, msf->active_fpm, st_page_list, MSF_PAGE_STATE_ALLOC);
    for (MSF_StreamNode *stream_node = stream_list.first; stream_node != 0; stream_node = stream_node->next) {
      msf_check_fpm_bits_for_page_list(page_data_list, msf->page_size, msf->active_fpm, stream_node->data.page_list, MSF_PAGE_STATE_ALLOC);
    }
#endif
  } while(0);
  
exit:;
  if (error != MSF_Error_OK) {
    if (msf) {
      msf_release(&msf);
    }
  }

  ProfEnd();
  return error;
}

internal void
msf_release(MSF_Context **msf_ptr)
{
  arena_release((*msf_ptr)->arena);
  *msf_ptr = 0;
}

internal String8List
msf_build_stream_table_data(Arena *arena, MSF_StreamList *sectab, MSF_UInt page_size, MSF_UInt page_count)
{
  ProfBeginFunction();
  
  MSF_UInt *stream_count_ptr = push_array(arena, MSF_UInt, 1);
  *stream_count_ptr = sectab->count;

  MSF_UInt *stream_size_arr = push_array(arena, MSF_UInt, sectab->count);
  MSF_UInt stream_page_count = 0;

  MSF_PageNumber *stream_pages_arr = push_array(arena, MSF_PageNumber, page_count);

  for (MSF_StreamNode *stream_node = sectab->first; stream_node != 0; stream_node = stream_node->next) {
    MSF_Stream *stream = &stream_node->data;
    
    // is page list correct?
    MSF_UInt expected_stream_page_count = msf_count_pages(page_size, stream->size);
    if (expected_stream_page_count > stream->page_list.count) {
      assert(!"invalid page list ");
    }
    
    // store stream sizes
    stream_size_arr[stream->sn] = stream->size;
    
    // store stream pages
    for (MSF_PageNode *page_node = stream->page_list.first; page_node != 0; page_node = page_node->next) {
      // first three pages are reserved for header, FPM0, and FPM1
      assert(page_node->pn > 2); 
      
      // it's not necessarily a bug to use interval FPM pages,
      // but for sake of correctness make sure there is no stream
      // aside from FPM that uses these pages
      //
      // also, actual FPM pages should be asserted on: pn % (msf->page_size * MSF_BITS_PER_CHAR)
      assert((page_node->pn % page_size) != 1);
      assert((page_node->pn % page_size) != 2);
      
      // is there a stream with too many page nodes?
      assert(stream_page_count < page_count);
      
      // is this page number allocated?
      //assert(msf_get_fpm_page_bit_state(msf, page_node->pn) == MSF_PAGE_STATE_ALLOC);
      
      stream_pages_arr[stream_page_count] = page_node->pn;
      stream_page_count += 1;
    }
  }
  
  // on disk stream table:
  //  MSF_UInt stream_count;
  //  MSF_UInt stream_size[stream_count];
  //  MSF_PageNumber pages[stream_count][*];
  String8List st_data_list = {0};
  str8_list_push(arena, &st_data_list, str8((u8*)stream_count_ptr, sizeof(*stream_count_ptr)));
  str8_list_push(arena, &st_data_list, str8((u8*)stream_size_arr, sizeof(*stream_size_arr) * (*stream_count_ptr)));
  str8_list_push(arena, &st_data_list, str8((u8*)stream_pages_arr, sizeof(*stream_pages_arr) * stream_page_count));
  
  ProfEnd();
  return st_data_list;
}

internal MSF_Error
msf_build_stream_table(MSF_Context *msf, MSF_UInt *stream_table_size_out)
{
  ProfBeginFunction();
  Temp scratch = scratch_begin(0, 0);

  MSF_Error error = MSF_Error_OK;
  
  String8List st_data_list = msf_build_stream_table_data(scratch.arena, &msf->sectab, msf->page_size, msf->page_count);
  
  MSF_UInt st_page_count = msf_count_pages(msf->page_size, st_data_list.total_size);
  msf_free_pages(msf, &msf->st_page_list); // TODO: page reuse
  msf->st_page_list = msf_alloc_pages(msf, st_page_count);
  
  MSF_UInt cursor = 0;
  for (String8Node *node = st_data_list.first; node != 0; node = node->next) {
    b32 is_data_written = msf_write(msf->page_data_list, msf->page_size, msf->st_page_list, cursor, node->string.str, node->string.size);
    if (!is_data_written) {
      error = MSF_BuildError_UNABLE_TO_WRITE_STREAM_TABLE;
      goto exit;
    }
    cursor += node->string.size;
  }
  
  *stream_table_size_out = st_data_list.total_size;
  
  exit:;
  scratch_end(scratch);
  ProfEnd();
  return error;
}

internal MSF_Error
msf_build_root_directory(MSF_Context *msf)
{
  ProfBeginFunction();
  Temp scratch = scratch_begin(0, 0);

  MSF_Error error = MSF_Error_OK;
  
  // MS impl doesn't handle root directory with page count above 1.
  MSF_UInt max_page_count_in_root_directory = msf->page_size / sizeof(MSF_PageNumber);
  if (msf->st_page_list.count > max_page_count_in_root_directory) {
    error = MSF_Error_STREAM_TABLE_HAS_TOO_MANY_PAGES;
    goto exit;
  }
  
  // collect stream table page numbers
  MSF_PageNumber *pn_arr = push_array(scratch.arena, MSF_PageNumber, msf->st_page_list.count);
  MSF_UInt pn_count = 0;
  for (MSF_PageNode *page = msf->st_page_list.first; page != 0; page = page->next) {
    pn_arr[pn_count++] = page->pn;
  }
  
  MSF_UInt root_page_count = msf_count_pages(msf->page_size, pn_count * sizeof(pn_arr[0]));
  assert(root_page_count == 1);
  
  msf_free_pages(msf, &msf->root_page_list); // TODO: page reuse
  msf->root_page_list = msf_alloc_pages(msf, root_page_count);
  b32 is_root_written = msf_write(msf->page_data_list, msf->page_size, msf->root_page_list, 0, pn_arr, sizeof(pn_arr[0]) * pn_count);
  if (!is_root_written) {
    error = MSF_BuildError_UNABLE_TO_WRITE_ROOT_DIRECTORY;
    goto exit;
  }
  
exit:;
  scratch_end(scratch);
  ProfEnd();
  return error;
}

internal MSF_Error
msf_build_header(MSF_Context *msf, MSF_UInt stream_table_size)
{
  ProfBeginFunction();
  MSF_Error error = MSF_Error_OK;
  
  MSF_Header70 header;
  MemoryCopy(&header.magic[0], &msf_msf70_magic[0], sizeof(msf_msf70_magic));
  header.page_size         = msf->page_size;
  header.active_fpm        = msf->active_fpm;
  header.page_count        = msf->page_count;
  header.stream_table_size = stream_table_size;
  header.unknown           = 0;
  header.root_pn           = msf->root_page_list.first->pn;
  
  b32 is_header_written = msf_write(msf->page_data_list, msf->page_size, msf->header_page_list, 0, &header, sizeof(header));
  if (!is_header_written) {
    error = MSF_BuildError_UNABLE_TO_WRITE_HEADER;
    goto exit;
  }
  
  exit:;
  ProfEnd();
  return error;
}

internal MSF_Error 
msf_build(MSF_Context *msf)
{
  ProfBeginFunction();
  
  MSF_Error err;
  do {
    MSF_UInt stream_table_size;
    err = msf_build_stream_table(msf, &stream_table_size);
    if (err != MSF_Error_OK) {
      break;
    }
    
    err = msf_build_root_directory(msf);
    if (err != MSF_Error_OK) {
      break;
    }
    
    err = msf_build_header(msf, stream_table_size);
    if (err != MSF_Error_OK) {
      break;
    }
  } while (0);

  ProfEnd();
  return err;
}

internal String8List
msf_get_page_data_nodes(Arena *arena, MSF_Context *msf)
{
  String8List list; MemoryZeroStruct(&list);

  u64 total_size = msf_get_save_size(msf);
  u64 bytes_left = total_size;
  u64 node_size = msf_get_data_node_size(msf->page_size);

  for (MSF_PageDataNode *data_node = msf->page_data_list.first; data_node != 0; data_node = data_node->next) {
    // compute byte count for the node
    u64 to_copy = min(bytes_left, node_size);
    bytes_left -= to_copy;

    String8 data = str8(data_node->data, to_copy);
    str8_list_push(arena, &list, data);
  }
  return list;
}

internal u64
msf_get_save_size(MSF_Context *msf)
{
#if 0
  MSF_PageNumber max_pn = msf_find_max_pn(msf->page_data_list, msf->page_size);
  u64 size = ((u64)max_pn + 1) * (u64)msf->page_size;
  assert(msf_count_pages(size, msf->page_size) == msf->page_count);
#else
  u64 size = (u64)msf->page_count * msf->page_size;
#endif
  return size;
}

internal b32
msf_save(MSF_Context *msf, void *buffer, u64 buffer_size)
{
  ProfBeginFunction();

  u64 node_size = msf_get_data_node_size(msf->page_size);
  u64 cursor = 0;

  for (MSF_PageDataNode *node = msf->page_data_list.first; node != 0; node = node->next) {
    // compute byte count for the copy
    u64 bytes_in_buffer = buffer_size - cursor;
    u64 to_copy = min(bytes_in_buffer, node_size);

    // copy MSF bytes to output buffer
    u8 *dst = (u8 *)buffer + cursor;
    u8 *src = node->data;
    MemoryCopy(dst, src, to_copy);

    // advance cursor
    cursor += to_copy;

    // is output buffer full?
    if (to_copy == 0) {
      break;
    }
  }

  b32 is_save_ok = (cursor == buffer_size);
  assert(is_save_ok);

  ProfEnd();
  return is_save_ok;
}

internal MSF_Error
msf_save_arena(Arena *arena, MSF_Context *msf, String8 *data_out)
{
  ProfBeginFunction();
  MSF_Error err = msf_build(msf);
  if (err == MSF_Error_OK) {
    u64 buffer_size = msf_get_save_size(msf);
    u8 *buffer = push_array(arena, u8, buffer_size);
    b32 is_saved = msf_save(msf, buffer, buffer_size);
    if (is_saved) {
      *data_out = str8(buffer, buffer_size);
    } else {
      arena_pop(arena, buffer_size);
    }
  }
  ProfEnd();
  return err;
}

internal char *
msf_error_to_string(MSF_Error code)
{
  char *str = "";
  switch (code) {
    case MSF_Error_OK: break;
    
    case MSF_Error_STREAM_TABLE_HAS_TOO_MANY_PAGES: str = "stream table exceeds page limit"; break;
    
    case MSF_OpenError_NOT_ENOUGH_BYTES_TO_READ_HEADER:           str = "input does not have enough bytes to read header";  break;
    case MSF_OpenError_INVALID_MAGIC:                             str = "magic value does not match";                       break;
    case MSF_OpenError_PAGE_SIZE_IS_NOT_POW2:                     str = "page size is not power of two";                    break;
    case MSF_OpenError_INVALID_PAGE_SIZE:                         str = "invalid page size";                                break;
    case MSF_OpenError_NOT_ENOUGH_PAGES_TO_INIT:                  str = "not enough pages to initialize MSF";               break;
    case MSF_OpenError_INVALID_ROOT_STREAM_PAGE_NUMBER:           str = "invalid root stream page number";                  break;
    case MSF_OpenError_UNABLE_TO_READ_STREAM_TABLE_PAGE_NUMBERS:  str = "unable to read stream table's page numbers";       break;
    case MSF_OpenError_STREAM_COUNT_OVERFLOW:                     str = "stream count is overflown";                        break;
    case MSF_OpenError_UNABLE_TO_READ_STREAM_SIZES:               str = "unable to read streams sizes";                     break;
    case MSF_OpenError_INVALID_STREAM_TABLE:                      str = "invalid stream table";                             break;
    case MSF_OpenError_INVALID_ACTIVE_FPM:                        str = "invalid active FPM";                               break;
    case MSF_OpenError_PAGE_COUNT_DOESNT_MATCH_DATA_SIZE:         str = "page count from MSF header does not match data page count"; break;
    
    case MSF_BuildError_UNABLE_TO_WRITE_STREAM_TABLE:                       str = "unable to write stream table";                       break;
    case MSF_BuildError_UNABLE_TO_WRITE_STREAM_TABLE_PAGE_NUMBER_DIRECTORY: str = "unable to write stream table page number directory"; break;
    case MSF_BuildError_UNABLE_TO_WRITE_ROOT_DIRECTORY:                     str = "unable to write root directory";                     break;
    case MSF_BuildError_UNABLE_TO_WRITE_HEADER:                             str = "unable to write header";                             break;
  }
  
  return str;
}

////////////////////////////////

/*
   Multi-Stream-Format is a database type of format for storing debug info
   but in principle can store anything you want. MSF divides file
   into fixed-sized pages (default page size is 4KiB) and puts them
   together into streams. A stream is made up from a non-contigous
   number of pages and supports following operations: alloc, free, open, write, read.
   Current MSF 7.0 allows creating up to 64K of streams, where each stream can potentially
   contain 2GiB of data (assuming default page size).

   Free Page Map assigns a bit to each page to indicate page alloc state. 0 = allocated and 1 = free.
   FPM is alloced at fixed intervals of 'page_size * MSF_BITS_PER_CHAR'. At the begining of interval
   two pages are reserved for status bits. The 'active_fpm' field in the MSF header tells which FPM page
   is in use. On commit time MSF alternates between two pages, this way they support atomic read and write.

   FPM Bug:
    Let's say you have a MSF file with page size 0x1000 bytes, you can represent 0x1000 * 8 = 0x8000 pages
    or 0x8000 * 0x1000 = 128MiB. And when file exceeds this size a new FPM group should be allocated
    at page numbers 0x8001 and 0x8002. However, in MS impl there is a bug where they don't multiply
    interval by 8 and each FPM group is allocated at intervals of page size or 0x1000, so each FPM group is placed
    at page numbers 0x1001, 0x1002, 0x2001, 0x2002, and so on. This way MSF files end up allocating 8 times more pages.

    Also, MS impl marks unused pages as allocated thus leaving them empty but LLVM repurposes them
    for regular allocations and things work out fine because of the fact that MS computes correct
    number of FPM pages when they save and load and the trailing pages aren't being touched: 
       https://github.com/microsoft/microsoft-pdb/blob/master/PDB/msf/msf.cpp#L2512

   Root directory is a single paged stored as a page number in 'MSF_Header70.root_pn'. 
   The directory contains an array of page numbers needed to read the stream table. This is a late
   addition introduced in version 7.0 that lets us have bigger stream tables. However, there is a limit
   if stream table exceeds root directory, MSF becomes invalid. MS impl isn't
   clear what should happen in this case, so we tried to contiguously allocate root pages
   but VS and LLVM error out. In practice you can double page size to work around the limit.

TODO: explain stream table

 */

#if 0

internal void
msf_bytedump_stream(char *file_name, MSF_Context *msf, MSF_StreamNumber sn, u64 start, u64 byte_count)
{
  Temp scratch = scratch_begin(0, 0);
  u64 pos = msf_stream_get_pos(msf, sn);
  msf_stream_seek(msf, sn, start);
  u64 buffer_size = byte_count;
  u8 *buffer = push_array(scratch.arena, u8, buffer_size);
  MSF_UInt read_size = msf_stream_read(msf, sn, buffer, buffer_size);
  os_write_file(str8_cstring(file_name), str8(buffer, read_size));
  msf_stream_seek(msf, sn, pos);
  scratch_end(scratch);
}

internal void
msf_hexdump_stream(FILE *file, MSF_Context *msf, MSF_StreamNumber sn, u64 start, u64 byte_count, u64 stride)
{
  Temp scratch = scratch_begin(0, 0);
  u8 *row_buffer = push_array(scratch.arena, u8, stride);
  u64 stream_size = msf_stream_get_size(msf, sn);
  u64 cursor = start;
  u64 end = min(start + byte_count, stream_size);
  while (cursor < stream_size) {
    MSF_UInt read_size = msf_stream_read(msf, sn, row_buffer, stride);
    
    // print offset
    fprintf(file, "%04llX", cursor);
    
    // print bytes
    fprintf(file, "    ");
    for (u64 i = 0; i < read_size; i += 1) {
      if (i > 0) {
        fprintf(file, " ");
      }
      fprintf(file, "%02X", row_buffer[i]);
    }
    
    // print ascii
    fprintf(file, "    ");
    for (u64 i = 0; i < read_size; i += 1) {
      u8 print_char = row_buffer[i];
      if (0x20 > print_char || print_char > 0x7E) {
        print_char = '.';
      }
      fprintf(file, "%c", print_char);
    }
    
    // row is done
    fprintf(file, "\n");
    
    cursor += stride;
  }
  
  scratch_end(scratch);
}

internal void
msf_hexdump_stream_to_file(char *name, MSF_Context *msf, MSF_StreamNumber sn, u64 start, u64 byte_count, u64 stride)
{
  FILE *f = fopen(name, "w");
  msf_hexdump_stream(f, msf, sn, start, byte_count, stride);
  fclose(f);
}

#endif

#if 0
internal void
test_msf_open_save(void)
{
  Temp scratch = scratch_begin(0, 0);
  
  u32 item0 = 123;
  u32 item1 = 321;
  
  MSF_StreamNumber stream;
  String8 data;
  {
    MSF_Context *msf = msf_alloc(MSF_DEFAULT_PAGE_SIZE, MSF_DEFAULT_FPM);
    stream = msf_stream_alloc(msf);
    msf_stream_write_u32(msf, stream, item0);
    msf_stream_write_u32(msf, stream, item1);
    data = msf_save_arena(scratch.arena, msf);
    msf_release(&msf);
  }
  
  String8 data1;
  {
    MSF_Context *msf = 0;
    MSF_Error err = msf_open(data, &msf);
    assert(err == MSF_Error_OK);
    u32 read0 = msf_stream_read_u32(msf, stream);
    assert(read0 == item0);
    u32 read1 = msf_stream_read_u32(msf, stream);
    assert(read1 == item1);
    data1 = msf_save_arena(scratch.arena, msf);
    msf_release(&msf);
  }
  
  {
    MSF_Context *msf = 0;
    MSF_Error err = msf_open(data, &msf);
    assert(err == MSF_Error_OK);
    u32 read0 = msf_stream_read_u32(msf, stream);
    assert(read0 == item0);
    u32 read1 = msf_stream_read_u32(msf, stream);
    assert(read1 == item1);
    msf_release(&msf);
  }
  
  scratch_end(scratch);
}

internal void
test_size_limit(void)
{
  Temp scratch = scratch_begin(0, 0);
  
  MSF_Context *msf = msf_alloc(8192, MSF_DEFAULT_FPM);
  assert(msf);
  
  u64 c = (1 * 1024 * 1024 * 1024) / msf->page_size;
  u64 stream_count = 8;
  
  u64 data_size = msf->page_size;
  u8 *data = push_array(scratch.arena, u8, data_size);
  
  for (u64 stream_idx = 0; stream_idx < stream_count; stream_idx += 1) {
    MSF_StreamNumber stream = msf_stream_alloc(msf);
    assert(stream != MSF_INVALID_STREAM_NUMBER);
    
    MemorySet(&data[0], 1 + stream_idx, data_size);
    
    msf_stream_resize(msf, stream, c * msf->page_size);
    
    for (u64 i = 0; i < c; i += 1) {
      b32 is_written = msf_stream_write(msf, stream, data, data_size);
      assert(is_written);
    }
  }
  
  //msf_grow(msf, MSF_PN_MAX);
  
  msf_stream_free(msf, 7);
  msf_stream_free(msf, 6);
  msf_stream_free(msf, 5);
  
  stream_count -= 3;
  
  String8 msf_data = msf_save_arena(scratch.arena, msf);
  assert(msf_data.size > 0);
  msf_release(&msf);
  
  //os_write_file(str8_lit("test.msf"), msf_data);
  
  MSF_Error err = msf_open(msf_data, &msf);
  assert(err == MSF_Error_OK);
  
#if 1
  u8 *buffer = push_array(scratch.arena, u8, data_size);
  for (u64 stream_idx = 0; stream_idx < stream_count; stream_idx += 1) {
    MSF_StreamNumber sn = (MSF_StreamNumber)stream_idx;
    
    MemorySet(&data[0], 1 + stream_idx, data_size);
    
    for (u64 i = 0; i < c; i += 1) {
      MSF_UInt read_size = msf_stream_read(msf, sn, buffer, data_size);
      assert(read_size == data_size);
      
      int cmp = MemoryCompare(buffer, data, data_size);
      assert(cmp == 0);
    }
  }
#endif
  
  msf_release(&msf);
  scratch_end(scratch);
}

internal void
test_msf(void)
{
  test_size_limit();
  test_msf_open_save();
}
#endif


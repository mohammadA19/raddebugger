// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Lexing/Parsing Data Tables

static read_only StringView e_multichar_symbol_strings[] =
{
  ("<<"),
  (">>"),
  ("."),
  ("<="),
  (">="),
  ("=="),
  ("!="),
  ("&&"),
  ("||"),
};

static read_only int64 e_max_precedence = 15;

////////////////////////////////
//~ rjf: Basic Map Functions

//- rjf: str . num

E_String2NumMap
e_string2num_map_make(Arena* arena, uint64 slot_count)
{
  E_String2NumMap map = {0};
  map.slots_count = slot_count;
  map.slots = push_array(arena, E_String2NumMapSlot, map.slots_count);
  return map;
}

void
e_string2num_map_insert(Arena* arena, E_String2NumMap* map, StringView str, uint64 num)
{
  uint64 hash = e_hash_from_string(5381, str);
  uint64 slot_idx = hash%map.slots_count;
  E_String2NumMapNode* existing_node = 0;
  for(E_String2NumMapNode* node = map.slots[slot_idx].first; node != 0; node = node.hash_next)
  {
    if(str8_match(node.str, str, 0) && node.num == num)
    {
      existing_node = node;
      break;
    }
  }
  if(existing_node == 0)
  {
    E_String2NumMapNode* node = push_array(arena, E_String2NumMapNode, 1);
    SLLQueuePush_N(map.slots[slot_idx].first, map.slots[slot_idx].last, node, hash_next);
    SLLQueuePush_N(map.first, map.last, node, order_next);
    node.str = push_str8_copy(arena, str);
    node.num = num;
    map.node_count += 1;
  }
}

uint64
e_num_from_string(E_String2NumMap* map, StringView str)
{
  uint64 num = 0;
  if(map.slots_count != 0)
  {
    uint64 hash = e_hash_from_string(5381, str);
    uint64 slot_idx = hash%map.slots_count;
    E_String2NumMapNode* existing_node = 0;
    for(E_String2NumMapNode* node = map.slots[slot_idx].first; node != 0; node = node.hash_next)
    {
      if(str8_match(node.str, str, 0))
      {
        existing_node = node;
        break;
      }
    }
    if(existing_node != 0)
    {
      num = existing_node.num;
    }
  }
  return num;
}

E_String2NumMapNodeArray
e_string2num_map_node_array_from_map(Arena* arena, E_String2NumMap* map)
{
  E_String2NumMapNodeArray result = {0};
  result.count = map.node_count;
  result.v = push_array(arena, E_String2NumMapNode *, result.count);
  uint64 idx = 0;
  for(E_String2NumMapNode* n = map.first; n != 0; n = n.order_next, idx += 1)
  {
    result.v[idx] = n;
  }
  return result;
}

int
e_string2num_map_node_qsort_compare__num_ascending(E_String2NumMapNode** a, E_String2NumMapNode** b)
{
  int result = 0;
  if(a[0].num < b[0].num)
  {
    result = -1;
  }
  else if(a[0].num > b[0].num)
  {
    result = +1;
  }
  return result;
}

void
e_string2num_map_node_array_sort__in_place(E_String2NumMapNodeArray* array)
{
  quick_sort(array.v, array.count, sizeof(array.v[0]), e_string2num_map_node_qsort_compare__num_ascending);
}

//- rjf: str . expr

E_String2ExprMap
e_string2expr_map_make(Arena* arena, uint64 slot_count)
{
  E_String2ExprMap map = {0};
  map.slots_count = slot_count;
  map.slots = push_array(arena, E_String2ExprMapSlot, map.slots_count);
  return map;
}

void
e_string2expr_map_insert(Arena* arena, E_String2ExprMap* map, StringView str, E_Expr* expr)
{
  uint64 hash = e_hash_from_string(5381, str);
  uint64 slot_idx = hash%map.slots_count;
  E_String2ExprMapNode* existing_node = 0;
  for(E_String2ExprMapNode* node = map.slots[slot_idx].first;
      node != 0;
      node = node.hash_next)
  {
    if(str8_match(node.str, str, 0))
    {
      existing_node = node;
      break;
    }
  }
  if(existing_node == 0)
  {
    E_String2ExprMapNode* node = push_array(arena, E_String2ExprMapNode, 1);
    SLLQueuePush_N(map.slots[slot_idx].first, map.slots[slot_idx].last, node, hash_next);
    node.str = push_str8_copy(arena, str);
    existing_node = node;
  }
  existing_node.expr = expr;
}

void
e_string2expr_map_inc_poison(E_String2ExprMap* map, StringView str)
{
  uint64 hash = e_hash_from_string(5381, str);
  uint64 slot_idx = hash%map.slots_count;
  for(E_String2ExprMapNode* node = map.slots[slot_idx].first;
      node != 0;
      node = node.hash_next)
  {
    if(str8_match(node.str, str, 0))
    {
      node.poison_count += 1;
      break;
    }
  }
}

void
e_string2expr_map_dec_poison(E_String2ExprMap* map, StringView str)
{
  uint64 hash = e_hash_from_string(5381, str);
  uint64 slot_idx = hash%map.slots_count;
  for(E_String2ExprMapNode* node = map.slots[slot_idx].first;
      node != 0;
      node = node.hash_next)
  {
    if(str8_match(node.str, str, 0) && node.poison_count > 0)
    {
      node.poison_count -= 1;
      break;
    }
  }
}

E_Expr *
e_string2expr_lookup(E_String2ExprMap* map, StringView str)
{
  E_Expr* expr = &e_expr_nil;
  if(map.slots_count != 0)
  {
    uint64 hash = e_hash_from_string(5381, str);
    uint64 slot_idx = hash%map.slots_count;
    E_String2ExprMapNode* existing_node = 0;
    for(E_String2ExprMapNode* node = map.slots[slot_idx].first; node != 0; node = node.hash_next)
    {
      if(str8_match(node.str, str, 0) && node.poison_count == 0)
      {
        existing_node = node;
        break;
      }
    }
    if(existing_node != 0)
    {
      expr = existing_node.expr;
    }
  }
  return expr;
}

////////////////////////////////
//~ rjf: Debug-Info-Driven Map Building Functions

E_String2NumMap *
e_push_locals_map_from_rdi_voff(Arena* arena, RDI_Parsed* rdi, uint64 voff)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  //- rjf: gather scopes to walk
  struct Task
  {
    Task* next;
    RDI_Scope* scope;
  };
  Task* first_task = 0;
  Task* last_task = 0;
  
  //- rjf: voff . tightest scope
  RDI_Scope* tightest_scope = 0;
  {
    uint64 scope_idx = rdi_vmap_idx_from_section_kind_voff(rdi, RDI_SectionKind_ScopeVMap, voff);
    RDI_Scope* scope = rdi_element_from_name_idx(rdi, Scopes, scope_idx);
    Task* task = push_array(scratch.arena, Task, 1);
    task.scope = scope;
    SLLQueuePush(first_task, last_task, task);
    tightest_scope = scope;
  }
  
  //- rjf: voff-1 . scope
  if(voff > 0)
  {
    uint64 scope_idx = rdi_vmap_idx_from_section_kind_voff(rdi, RDI_SectionKind_ScopeVMap, voff-1);
    RDI_Scope* scope = rdi_element_from_name_idx(rdi, Scopes, scope_idx);
    if(scope != tightest_scope)
    {
      Task* task = push_array(scratch.arena, Task, 1);
      task.scope = scope;
      SLLQueuePush(first_task, last_task, task);
    }
  }
  
  //- rjf: tightest scope . walk up the tree & build tasks for each parent scope
  if(tightest_scope != 0)
  {
    RDI_Scope* nil_scope = rdi_element_from_name_idx(rdi, Scopes, 0);
    for(RDI_Scope* scope = rdi_element_from_name_idx(rdi, Scopes, tightest_scope.parent_scope_idx);
        scope != 0 && scope != nil_scope;
        scope = rdi_element_from_name_idx(rdi, Scopes, scope.parent_scope_idx))
    {
      Task* task = push_array(scratch.arena, Task, 1);
      task.scope = scope;
      SLLQueuePush(first_task, last_task, task);
    }
  }
  
  //- rjf: build blank map
  E_String2NumMap* map = push_array(arena, E_String2NumMap, 1);
  *map = e_string2num_map_make(arena, 1024);
  
  //- rjf: accumulate locals for all tasks
  for(Task* task = first_task; task != 0; task = task.next)
  {
    RDI_Scope* scope = task.scope;
    if(scope != 0)
    {
      uint32 local_opl_idx = scope.local_first + scope.local_count;
      for(uint32 local_idx = scope.local_first; local_idx < local_opl_idx; local_idx += 1)
      {
        RDI_Local* local_var = rdi_element_from_name_idx(rdi, Locals, local_idx);
        uint64 local_name_size = 0;
        uint8* local_name_str = rdi_string_from_idx(rdi, local_var.name_string_idx, &local_name_size);
        StringView name = push_str8_copy(arena, StringView(local_name_str, local_name_size));
        e_string2num_map_insert(arena, map, name, (uint64)local_idx+1);
      }
    }
  }
  
  scratch_end(scratch);
  return map;
}

E_String2NumMap *
e_push_member_map_from_rdi_voff(Arena* arena, RDI_Parsed* rdi, uint64 voff)
{
  //- rjf: voff . tightest scope
  uint64 scope_idx = rdi_vmap_idx_from_section_kind_voff(rdi, RDI_SectionKind_ScopeVMap, voff);
  RDI_Scope* tightest_scope = rdi_element_from_name_idx(rdi, Scopes, scope_idx);
  
  //- rjf: tightest scope . procedure
  uint32 proc_idx = tightest_scope.proc_idx;
  RDI_Procedure* procedure = rdi_element_from_name_idx(rdi, Procedures, proc_idx);
  
  //- rjf: procedure . udt
  uint32 udt_idx = procedure.container_idx;
  RDI_UDT* udt = rdi_element_from_name_idx(rdi, UDTs, udt_idx);
  
  //- rjf: build blank map
  E_String2NumMap* map = push_array(arena, E_String2NumMap, 1);
  *map = e_string2num_map_make(arena, 64);
  
  //- rjf: udt . fill member map
  if(!(udt.flags & RDI_UDTFlag_EnumMembers))
  {
    uint64 data_member_num = 1;
    for(uint32 member_idx = udt.member_first;
        member_idx < udt.member_first+udt.member_count;
        member_idx += 1)
    {
      RDI_Member* m = rdi_element_from_name_idx(rdi, Members, member_idx);
      if(m.kind == RDI_MemberKind_DataField)
      {
        StringView name = {0};
        name.Ptr = rdi_string_from_idx(rdi, m.name_string_idx, &name.size);
        e_string2num_map_insert(arena, map, name, data_member_num);
        data_member_num += 1;
      }
    }
  }
  
  return map;
}

////////////////////////////////
//~ rjf: Tokenization Functions

E_Token
e_token_zero()
{
  E_Token t = {};
  return t;
}

void
e_token_chunk_list_push(Arena* arena, E_TokenChunkList* list, uint64 chunk_size, E_Token* token)
{
  E_TokenChunkNode* node = list.last;
  if(node == 0 || node.count >= node.cap)
  {
    node = push_array(arena, E_TokenChunkNode, 1);
    SLLQueuePush(list.first, list.last, node);
    node.cap = chunk_size;
    node.v = push_array_no_zero(arena, E_Token, node.cap);
    list.node_count += 1;
  }
  MemoryCopyStruct(&node.v[node.count], token);
  node.count += 1;
  list.total_count += 1;
}

E_TokenArray
e_token_array_from_chunk_list(Arena* arena, E_TokenChunkList* list)
{
  E_TokenArray array = {0};
  array.count = list.total_count;
  array.v = push_array_no_zero(arena, E_Token, array.count);
  uint64 idx = 0;
  for(E_TokenChunkNode* node = list.first; node != 0; node = node.next)
  {
    MemoryCopy(array.v+idx, node.v, sizeof(E_Token)*node.count);
  }
  return array;
}

E_TokenArray
e_token_array_from_text(Arena* arena, StringView text)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  //- rjf: lex loop
  E_TokenChunkList tokens = {0};
  uint64 active_token_start_idx = 0;
  E_TokenKind active_token_kind = E_TokenKind_Null;
  B32 active_token_kind_started_with_tick = 0;
  B32 escaped = 0;
  B32 exp = 0;
  for(uint64 idx = 0, advance = 0; idx <= text.size; idx += advance)
  {
    uint8 byte      = (idx+0 < text.size) ? text[idx+0] : 0;
    uint8 byte_next = (idx+1 < text.size) ? text[idx+1] : 0;
    uint8 byte_next2= (idx+2 < text.size) ? text[idx+2] : 0;
    advance = 1;
    B32 token_formed = 0;
    uint64 token_end_idx_pad = 0;
    switch(active_token_kind)
    {
      //- rjf: no active token . seek token starter
      default:
      {
        if(byte.IsLetter || byte == '_' || byte == '`' || byte == '$')
        {
          active_token_kind = E_TokenKind_Identifier;
          active_token_start_idx = idx;
          active_token_kind_started_with_tick = (byte == '`');
        }
        else if(char_is_digit(byte, 10) || (byte == '.' && char_is_digit(byte_next, 10)))
        {
          active_token_kind = E_TokenKind_Numeric;
          active_token_start_idx = idx;
        }
        else if(byte == '"')
        {
          active_token_kind = E_TokenKind_StringLiteral;
          active_token_start_idx = idx;
        }
        else if(byte == '\'')
        {
          active_token_kind = E_TokenKind_CharLiteral;
          active_token_start_idx = idx;
        }
        else if(byte == '~' || byte == '!' || byte == '%' || byte == '^' ||
                byte == '&' || byte == '*' || byte == '(' || byte == ')' ||
                byte == '-' || byte == '=' || byte == '+' || byte == '[' ||
                byte == ']' || byte == '{' || byte == '}' || byte == ':' ||
                byte == ';' || byte == ',' || byte == '.' || byte == '<' ||
                byte == '>' || byte == '/' || byte == '?' || byte == '|')
        {
          active_token_kind = E_TokenKind_Symbol;
          active_token_start_idx = idx;
        }
      }break;
      
      //- rjf: active tokens . seek enders
      case E_TokenKind_Identifier:
      {
        if(byte == ':' && byte_next == ':' && (byte_next2.IsLetter || byte_next2 == '_' || byte_next2 == '<'))
        {
          // NOTE(rjf): encountering C++-style namespaces - skip over scope resolution symbol
          // & keep going.
          advance = 2;
        }
        else if((byte == '\'' || byte == '`') && active_token_kind_started_with_tick)
        {
          // NOTE(rjf): encountering ` . ' or ` . ` style identifier escapes
          active_token_kind_started_with_tick = 0;
          advance = 1;
        }
        else if(byte == '<')
        {
          // NOTE(rjf): encountering C++-style templates - try to find ender. if no ender found,
          // assume this is an operator & just consume the identifier part.
          int64 nest = 1;
          for(uint64 idx2 = idx+1; idx2 <= text.size; idx2 += 1)
          {
            if(idx2 < text.size && text[idx2] == '<')
            {
              nest += 1;
            }
            else if(idx2 < text.size && text[idx2] == '>')
            {
              nest -= 1;
              if(nest == 0)
              {
                advance = (idx2+1-idx);
                break;
              }
            }
            else if(idx2 == text.size && nest != 0)
            {
              token_formed = 1;
              advance = 0;
              break;
            }
          }
        }
        else if(!byte.IsLetter && !char_is_digit(byte, 10) && byte != '_' && !active_token_kind_started_with_tick && byte != '@' && byte != '$')
        {
          advance = 0;
          token_formed = 1;
        }
      }break;
      case E_TokenKind_Numeric:
      {
        if(exp && (byte == '+' || byte == '-')){}
        else if(!byte.IsLetter && !char_is_digit(byte, 10) && byte != '.' && byte != ':')
        {
          advance = 0;
          token_formed = 1;
        }
        else
        {
          exp = 0;
          exp = (byte == 'e');
        }
      }break;
      case E_TokenKind_StringLiteral:
      {
        if(escaped == 0 && byte == '\\')
        {
          escaped = 1;
        }
        else if(escaped)
        {
          escaped = 0;
        }
        else if(escaped == 0 && byte == '"')
        {
          advance = 1;
          token_formed = 1;
          token_end_idx_pad = 1;
        }
      }break;
      case E_TokenKind_CharLiteral:
      {
        if(escaped == 0 && byte == '\\')
        {
          escaped = 1;
        }
        else if(escaped)
        {
          escaped = 0;
        }
        else if(escaped == 0 && byte == '\'')
        {
          advance = 1;
          token_formed = 1;
          token_end_idx_pad = 1;
        }
      }break;
      case E_TokenKind_Symbol:
      {
        if(byte != '~' && byte != '!' && byte != '%' && byte != '^' &&
           byte != '&' && byte != '*' && byte != '(' && byte != ')' &&
           byte != '-' && byte != '=' && byte != '+' && byte != '[' &&
           byte != ']' && byte != '{' && byte != '}' && byte != ':' &&
           byte != ';' && byte != ',' && byte != '.' && byte != '<' &&
           byte != '>' && byte != '/' && byte != '?' && byte != '|')
        {
          advance = 0;
          token_formed = 1;
        }
      }break;
    }
    
    //- rjf: token formed . push new formed token(s)
    if(token_formed)
    {
      // rjf: non-symbols* or* symbols of only 1-length can be immediately
      // pushed as a token
      if(active_token_kind != E_TokenKind_Symbol || idx==active_token_start_idx+1)
      {
        E_Token token = {active_token_kind, r1u64(active_token_start_idx, idx+token_end_idx_pad)};
        e_token_chunk_list_push(scratch.arena, &tokens, 256, &token);
      }
      
      // rjf: symbolic strings matching `--` mean the remainder of the str
      // is reserved for external usage. the rest of the stream should not
      // be tokenized.
      else if(idx == active_token_start_idx+2 && text[active_token_start_idx] == '-' && text[active_token_start_idx+1] == '-')
      {
        break;
      }
      
      // rjf: if we got a symbol str of N>1 characters, then we need to
      // apply the maximum-munch rule, and produce M<=N tokens, where each
      // formed token is the maximum size possible, given the legal
      // >1-length symbol strings.
      else
      {
        uint64 advance2 = 0;
        for(uint64 idx2 = active_token_start_idx; idx2 < idx; idx2 += advance2)
        {
          advance2 = 1;
          for(uint64 multichar_symbol_idx = 0;
              multichar_symbol_idx < ArrayCount(e_multichar_symbol_strings);
              multichar_symbol_idx += 1)
          {
            StringView multichar_symbol_string = e_multichar_symbol_strings[multichar_symbol_idx];
            StringView part_of_token = str8_substr(text, r1u64(idx2, idx2+multichar_symbol_string.size));
            if(str8_match(part_of_token, multichar_symbol_string, 0))
            {
              advance2 = multichar_symbol_string.size;
              break;
            }
          }
          E_Token token = {active_token_kind, r1u64(idx2, idx2+advance2)};
          e_token_chunk_list_push(scratch.arena, &tokens, 256, &token);
        }
      }
      
      // rjf: reset for subsequent tokens.
      active_token_kind = E_TokenKind_Null;
    }
  }
  
  //- rjf: chunk list . array & return
  E_TokenArray array = e_token_array_from_chunk_list(arena, &tokens);
  scratch_end(scratch);
  return array;
}

E_TokenArray
e_token_array_make_first_opl(E_Token* first, E_Token* opl)
{
  E_TokenArray array = {first, (uint64)(opl-first)};
  return array;
}

////////////////////////////////
//~ rjf: Context Selection Functions (Selection Required For All Subsequent APIs)

E_ParseCtx *
e_selected_parse_ctx()
{
  return e_parse_ctx;
}

void
e_select_parse_ctx(E_ParseCtx* ctx)
{
  e_parse_ctx = ctx;
}

uint32
e_parse_ctx_module_idx_from_rdi(RDI_Parsed* rdi)
{
  uint32 result = 0;
  for(uint64 idx = 0; idx < e_parse_ctx.modules_count; idx += 1)
  {
    if(e_parse_ctx.modules[idx].rdi == rdi)
    {
      result = (uint32)idx;
      break;
    }
  }
  return result;
}

////////////////////////////////
//~ rjf: Expression Tree Building Functions

E_Expr *
e_push_expr(Arena* arena, E_ExprKind kind, void* location)
{
  E_Expr* e = push_array(arena, E_Expr, 1);
  e.first = e.last = e.next = e.ref = &e_expr_nil;
  e.location = location;
  e.kind = kind;
  return e;
}

void
e_expr_insert_child(E_Expr* parent, E_Expr* prev, E_Expr* child)
{
  DLLInsert_NPZ(&e_expr_nil, parent.first, parent.last, prev, child, next, prev);
}

void
e_expr_push_child(E_Expr* parent, E_Expr* child)
{
  DLLPushBack_NPZ(&e_expr_nil, parent.first, parent.last, child, next, prev);
}

void
e_expr_remove_child(E_Expr* parent, E_Expr* child)
{
  DLLRemove_NPZ(&e_expr_nil, parent.first, parent.last, child, next, prev);
}

E_Expr *
e_expr_ref(Arena* arena, E_Expr* ref)
{
  E_Expr* expr = e_push_expr(arena, E_ExprKind_Ref, 0);
  expr.ref = ref;
  return expr;
}

E_Expr *
e_expr_ref_addr(Arena* arena, E_Expr* rhs)
{
  E_Expr* expr = e_push_expr(arena, E_ExprKind_Address, 0);
  E_Expr* rhs_ref = e_expr_ref(arena, rhs);
  e_expr_push_child(expr, rhs_ref);
  return expr;
}

E_Expr *
e_expr_ref_member_access(Arena* arena, E_Expr* lhs, StringView member_name)
{
  E_Expr* root = e_push_expr(arena, E_ExprKind_MemberAccess, 0);
  E_Expr* lhs_ref = e_expr_ref(arena, lhs);
  E_Expr* rhs = e_push_expr(arena, E_ExprKind_LeafMember, 0);
  rhs.str = push_str8_copy(arena, member_name);
  e_expr_push_child(root, lhs_ref);
  e_expr_push_child(root, rhs);
  return root;
}

E_Expr *
e_expr_ref_array_index(Arena* arena, E_Expr* lhs, uint64 index)
{
  E_Expr* root = e_push_expr(arena, E_ExprKind_ArrayIndex, 0);
  E_Expr* lhs_ref = e_expr_ref(arena, lhs);
  E_Expr* rhs = e_push_expr(arena, E_ExprKind_LeafU64, 0);
  rhs.value.u64 = index;
  e_expr_push_child(root, lhs_ref);
  e_expr_push_child(root, rhs);
  return root;
}

E_Expr *
e_expr_ref_deref(Arena* arena, E_Expr* rhs)
{
  E_Expr* root = e_push_expr(arena, E_ExprKind_Deref, 0);
  E_Expr* rhs_ref = e_expr_ref(arena, rhs);
  e_expr_push_child(root, rhs_ref);
  return root;
}

E_Expr *
e_expr_ref_cast(Arena* arena, E_TypeKey type_key, E_Expr* rhs)
{
  E_Expr* root = e_push_expr(arena, E_ExprKind_Cast, 0);
  E_Expr* lhs = e_push_expr(arena, E_ExprKind_TypeIdent, 0);
  lhs.type_key = type_key;
  E_Expr* rhs_ref = e_expr_ref(arena, rhs);
  e_expr_push_child(root, lhs);
  e_expr_push_child(root, rhs_ref);
  return root;
}

E_Expr *
e_expr_ref_bswap(Arena* arena, E_Expr* rhs)
{
  E_Expr* root = e_push_expr(arena, E_ExprKind_ByteSwap, 0);
  E_Expr* rhs_ref = e_expr_ref(arena, rhs);
  e_expr_push_child(root, rhs_ref);
  return root;
}

////////////////////////////////
//~ rjf: Expression Tree . String Conversions

void
e_append_strings_from_expr(Arena* arena, E_Expr* expr, String8List* out)
{
  switch(expr.kind)
  {
    default:
    {
      E_OpInfo* op_info = &e_expr_kind_op_info_table[expr.kind];
      StringView seps[] =
      {
        op_info.pre,
        op_info.sep,
        op_info.post,
      };
      uint64 idx = 0;
      for(E_Expr* child = expr.first;; child = child.next, idx += 1)
      {
        if(seps[idx].size != 0)
        {
          str8_list_push(arena, out, seps[idx]);
        }
        if(child == &e_expr_nil)
        {
          break;
        }
        E_OpInfo* child_op_info = &e_expr_kind_op_info_table[child.kind];
        B32 need_parens = (child_op_info.precedence > op_info.precedence);
        if(need_parens)
        {
          str8_list_pushf(arena, out, "(");
        }
        e_append_strings_from_expr(arena, child, out);
        if(need_parens)
        {
          str8_list_pushf(arena, out, ")");
        }
      }
    }break;
    case E_ExprKind_LeafBytecode:
    case E_ExprKind_LeafMember:
    case E_ExprKind_LeafIdent:
    {
      str8_list_push(arena, out, expr.str);
    }break;
    case E_ExprKind_LeafStringLiteral:
    {
      str8_list_pushf(arena, out, "\"%S\"", expr.str);
    }break;
    case E_ExprKind_LeafU64:
    case E_ExprKind_LeafBool:
    {
      str8_list_pushf(arena, out, "%I64u", expr.value.u64);
    }break;
    case E_ExprKind_LeafOffset:
    {
      str8_list_pushf(arena, out, "0x%I64x", expr.value.u64);
    }break;
    case E_ExprKind_LeafFilePath:
    {
      str8_list_pushf(arena, out, "file:\"%S\"", escaped_from_raw_str8(arena, expr.str));
    }break;
    case E_ExprKind_LeafF64:
    {
      str8_list_pushf(arena, out, "%f", expr.value.f64);
    }break;
    case E_ExprKind_LeafF32:
    {
      str8_list_pushf(arena, out, "%f", expr.value.f32);
    }break;
    case E_ExprKind_TypeIdent:
    {
      StringView type_string = e_type_string_from_key(arena, expr.type_key);
      str8_list_push(arena, out, type_string);
    }break;
    case E_ExprKind_Ref:
    {
      e_append_strings_from_expr(arena, expr.ref, out);
    }break;
  }
}

StringView
e_string_from_expr(Arena* arena, E_Expr* expr)
{
  String8List strings = {0};
  e_append_strings_from_expr(arena, expr, &strings);
  StringView result = str8_list_join(arena, &strings, 0);
  return result;
}

////////////////////////////////
//~ rjf: Parsing Functions

E_TypeKey
e_leaf_type_from_name(StringView name)
{
  E_TypeKey key = {};
  B32 found = 0;
  for(uint64 module_idx = 0; module_idx < e_parse_ctx.modules_count; module_idx += 1)
  {
    RDI_Parsed* rdi = e_parse_ctx.modules[module_idx].rdi;
    RDI_NameMap* name_map = rdi_element_from_name_idx(rdi, NameMaps, RDI_NameMapKind_Types);
    RDI_ParsedNameMap parsed_name_map = {0};
    rdi_parsed_from_name_map(rdi, name_map, &parsed_name_map);
    RDI_NameMapNode* node = rdi_name_map_lookup(rdi, &parsed_name_map, name.Ptr, name.size);
    if(node != 0)
    {
      uint32 match_count = 0;
      uint32* matches = rdi_matches_from_map_node(rdi, node, &match_count);
      if(match_count != 0)
      {
        RDI_TypeNode* type_node = rdi_element_from_name_idx(rdi, TypeNodes, matches[0]);
        found = (type_node.kind != RDI_TypeKind_NULL);
        key = e_type_key_ext(e_type_kind_from_rdi(type_node.kind), matches[0], module_idx);
        break;
      }
    }
  }
  if(!found)
  {
#define Case(str) (str8_match(name, (str), 0))
    if(0){}
    else if(Case("u8") || Case("uint8") || Case("uint8_t") || Case("uint8"))
    {
      key = e_type_key_basic(E_TypeKind_U8);
    }
    else if(Case("uchar8") || Case("uchar"))
    {
      key = e_type_key_basic(E_TypeKind_UChar8);
    }
    else if(Case("u16") || Case("uint16") || Case("uint16_t") || Case("uint16"))
    {
      key = e_type_key_basic(E_TypeKind_U16);
    }
    else if(Case("uchar16"))
    {
      key = e_type_key_basic(E_TypeKind_UChar16);
    }
    else if(Case("u32") || Case("uint32") || Case("uint32_t") || Case("uint32") || Case("uint"))
    {
      key = e_type_key_basic(E_TypeKind_U32);
    }
    else if(Case("uchar32"))
    {
      key = e_type_key_basic(E_TypeKind_UChar32);
    }
    else if(Case("u64") || Case("uint64") || Case("uint64_t") || Case("uint64") || Case("size_t"))
    {
      key = e_type_key_basic(E_TypeKind_U64);
    }
    else if(Case("s8") || Case("b8") || Case("B8") || Case("i8") || Case("int8") || Case("int8_t") || Case("int8"))
    {
      key = e_type_key_basic(E_TypeKind_S8);
    }
    else if(Case("char8") || Case("char"))
    {
      key = e_type_key_basic(E_TypeKind_Char8);
    }
    else if(Case("s16") || Case("b16") || Case("B16") || Case("i16") ||  Case("int16") || Case("int16_t") || Case("int16"))
    {
      key = e_type_key_basic(E_TypeKind_S16);
    }
    else if(Case("char16"))
    {
      key = e_type_key_basic(E_TypeKind_Char16);
    }
    else if(Case("s32") || Case("b32") || Case("B32") || Case("i32") || Case("int32") || Case("int32_t") || Case("char32") || Case("int32") || Case("int"))
    {
      key = e_type_key_basic(E_TypeKind_S32);
    }
    else if(Case("char32"))
    {
      key = e_type_key_basic(E_TypeKind_Char32);
    }
    else if(Case("s64") || Case("b64") || Case("B64") || Case("i64") || Case("int64") || Case("int64_t") || Case("int64") || Case("ssize_t"))
    {
      key = e_type_key_basic(E_TypeKind_S64);
    }
    else if(Case("void"))
    {
      key = e_type_key_basic(E_TypeKind_Void);
    }
    else if(Case("bool"))
    {
      key = e_type_key_basic(E_TypeKind_Bool);
    }
    else if(Case("float") || Case("f32") || Case("float") || Case("r32") || Case("R32"))
    {
      key = e_type_key_basic(E_TypeKind_F32);
    }
    else if(Case("double") || Case("f64") || Case("double") || Case("r64") || Case("R64"))
    {
      key = e_type_key_basic(E_TypeKind_F64);
    }
#undef Case
  }
  return key;
}

E_TypeKey
e_type_from_expr(E_Expr* expr)
{
  E_TypeKey result = {};
  E_ExprKind kind = expr.kind;
  switch(kind)
  {
    // TODO(rjf): do we support E_ExprKind_Func here?
    default:{}break;
    case E_ExprKind_TypeIdent:
    {
      result = expr.type_key;
    }break;
    case E_ExprKind_Ptr:
    {
      E_TypeKey direct_type_key = e_type_from_expr(expr.first);
      result = e_type_key_cons_ptr(e_parse_ctx.primary_module.arch, direct_type_key, 0);
    }break;
    case E_ExprKind_Array:
    {
      E_Expr* child_expr = expr.first;
      E_TypeKey direct_type_key = e_type_from_expr(child_expr);
      result = e_type_key_cons_array(direct_type_key, expr.value.u64);
    }break;
  }
  return result;
}

void
e_push_leaf_ident_exprs_from_expr__in_place(Arena* arena, E_String2ExprMap* map, E_Expr* expr)
{
  switch(expr.kind)
  {
    default:
    {
      for(E_Expr* child = expr.first; child != &e_expr_nil; child = child.next)
      {
        e_push_leaf_ident_exprs_from_expr__in_place(arena, map, child);
      }
    }break;
    case E_ExprKind_Define:
    {
      E_Expr* exprl = expr.first;
      E_Expr* exprr = exprl.next;
      if(exprl.kind == E_ExprKind_LeafIdent)
      {
        e_string2expr_map_insert(arena, map, exprl.str, exprr);
      }
    }break;
  }
}

E_Parse
e_parse_type_from_text_tokens(Arena* arena, StringView text, E_TokenArray* tokens)
{
  E_Parse parse = {0, &e_expr_nil};
  E_Token* token_it = tokens.v;
  
  //- rjf: parse unsigned marker
  B32 unsigned_marker = 0;
  {
    E_Token token = e_token_at_it(token_it, tokens);
    if(token.kind == E_TokenKind_Identifier)
    {
      StringView token_string = str8_substr(text, token.range);
      if(str8_match(token_string, ("unsigned"), 0))
      {
        token_it += 1;
        unsigned_marker = 1;
      }
    }
  }
  
  //- rjf: parse base type
  {
    E_Token token = e_token_at_it(token_it, tokens);
    if(token.kind == E_TokenKind_Identifier)
    {
      StringView token_string = str8_substr(text, token.range);
      if(token_string.size >= 2 &&
         token_string[0] == '`' &&
         token_string[token_string.size-1] == '`')
      {
        token_string = str8_substr(token_string, r1u64(1, token_string.size-1));
      }
      E_TypeKey type_key = e_leaf_type_from_name(token_string);
      if(!e_type_key_match(e_type_key_zero(), type_key))
      {
        token_it += 1;
        
        // rjf: apply unsigned marker to base type
        if(unsigned_marker) switch(e_type_kind_from_key(type_key))
        {
          default:{}break;
          case E_TypeKind_Char8: {type_key = e_type_key_basic(E_TypeKind_UChar8);}break;
          case E_TypeKind_Char16:{type_key = e_type_key_basic(E_TypeKind_UChar16);}break;
          case E_TypeKind_Char32:{type_key = e_type_key_basic(E_TypeKind_UChar32);}break;
          case E_TypeKind_S8:  {type_key = e_type_key_basic(E_TypeKind_U8);}break;
          case E_TypeKind_S16: {type_key = e_type_key_basic(E_TypeKind_U16);}break;
          case E_TypeKind_S32: {type_key = e_type_key_basic(E_TypeKind_U32);}break;
          case E_TypeKind_S64: {type_key = e_type_key_basic(E_TypeKind_U64);}break;
          case E_TypeKind_S128:{type_key = e_type_key_basic(E_TypeKind_U128);}break;
          case E_TypeKind_S256:{type_key = e_type_key_basic(E_TypeKind_U256);}break;
          case E_TypeKind_S512:{type_key = e_type_key_basic(E_TypeKind_U512);}break;
        }
        
        // rjf: construct leaf type
        parse.expr = e_push_expr(arena, E_ExprKind_TypeIdent, token_string.Ptr);
        parse.expr.type_key = type_key;
      }
    }
  }
  
  //- rjf: parse extensions
  if(parse.expr != &e_expr_nil)
  {
    for(;;)
    {
      E_Token token = e_token_at_it(token_it, tokens);
      if(token.kind != E_TokenKind_Symbol)
      {
        break;
      }
      StringView token_string = str8_substr(text, token.range);
      if(str8_match(token_string, ("*"), 0))
      {
        token_it += 1;
        E_Expr* ptee = parse.expr;
        parse.expr = e_push_expr(arena, E_ExprKind_Ptr, token_string.Ptr);
        e_expr_push_child(parse.expr, ptee);
      }
      else
      {
        break;
      }
    }
  }
  
  //- rjf: fill parse & end
  parse.last_token = token_it;
  return parse;
}

E_Parse
e_parse_expr_from_text_tokens__prec(Arena* arena, StringView text, E_TokenArray* tokens, int64 max_precedence)
{
  ProfBeginFunction();
  Temp scratch = scratch_begin(&arena, 1);
  E_Token* it = tokens.v;
  E_Token* it_opl = tokens.v + tokens.count;
  E_Parse result = {0, &e_expr_nil};
  
  //- rjf: parse prefix unaries
  struct PrefixUnaryNode
  {
    PrefixUnaryNode* next;
    E_ExprKind kind;
    E_Expr* cast_expr;
    void* location;
  };
  PrefixUnaryNode* first_prefix_unary = 0;
  PrefixUnaryNode* last_prefix_unary = 0;
  {
    for(;it < it_opl;)
    {
      E_Token* start_it = it;
      E_Token token = e_token_at_it(it, tokens);
      StringView token_string = str8_substr(text, token.range);
      int64 prefix_unary_precedence = 0;
      E_ExprKind prefix_unary_kind = 0;
      E_Expr* cast_expr = &e_expr_nil;
      void* location = 0;
      
      // rjf: try op table
      for EachNonZeroEnumVal(E_ExprKind, k)
      {
        E_OpInfo* op_info = &e_expr_kind_op_info_table[k];
        if(op_info.kind == E_OpKind_UnaryPrefix && str8_match(op_info.pre, token_string, 0))
        {
          prefix_unary_precedence = op_info.precedence;
          prefix_unary_kind = k;
          break;
        }
      }
      
      // rjf: consume valid op
      if(prefix_unary_precedence != 0)
      {
        location = token_string.Ptr;
        it += 1;
      }
      
      // rjf: try casting expression
      if(prefix_unary_precedence == 0 && str8_match(token_string, ("("), 0))
      {
        E_Token some_type_identifier_maybe = e_token_at_it(it+1, tokens);
        StringView some_type_identifier_maybe_string = str8_substr(text, some_type_identifier_maybe.range);
        if(some_type_identifier_maybe.kind == E_TokenKind_Identifier)
        {
          E_TypeKey type_key = e_leaf_type_from_name(some_type_identifier_maybe_string);
          if(!e_type_key_match(type_key, e_type_key_zero()) || str8_match(some_type_identifier_maybe_string, ("unsigned"), 0))
          {
            // rjf: move past open paren
            it += 1;
            
            // rjf: parse type expr
            E_TokenArray type_parse_tokens = e_token_array_make_first_opl(it, it_opl);
            E_Parse type_parse = e_parse_type_from_text_tokens(arena, text, &type_parse_tokens);
            E_Expr* type = type_parse.expr;
            e_msg_list_concat_in_place(&result.msgs, &type_parse.msgs);
            it = type_parse.last_token;
            location = token_string.Ptr;
            
            // rjf: expect )
            E_Token close_paren_maybe = e_token_at_it(it, tokens);
            StringView close_paren_maybe_string = str8_substr(text, close_paren_maybe.range);
            if(close_paren_maybe.kind != E_TokenKind_Symbol || !str8_match(close_paren_maybe_string, (")"), 0))
            {
              e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Missing `)`.");
            }
            
            // rjf: consume )
            else
            {
              it += 1;
            }
            
            // rjf: fill
            prefix_unary_precedence = 2;
            prefix_unary_kind = E_ExprKind_Cast;
            cast_expr = type;
          }
        }
      }
      
      // rjf: break if we got no operators
      if(prefix_unary_precedence == 0)
      {
        break;
      }
      
      // rjf: break if the token node iterator has not changed
      if(it == start_it)
      {
        break;
      }
      
      // rjf: push prefix unary if we got one
      {
        PrefixUnaryNode* op_n = push_array(scratch.arena, PrefixUnaryNode, 1);
        op_n.kind = prefix_unary_kind;
        op_n.cast_expr = cast_expr;
        op_n.location = location;
        SLLQueuePushFront(first_prefix_unary, last_prefix_unary, op_n);
      }
    }
  }
  
  //- rjf: parse atom
  E_Expr* atom = &e_expr_nil;
  StringView atom_implicit_member_name = {0};
  if(it < it_opl)
  {
    E_Token token = e_token_at_it(it, tokens);
    StringView token_string = str8_substr(text, token.range);
    
    //- rjf: gather resolution qualifier
    StringView resolution_qualifier = {0};
    if(token.kind == E_TokenKind_Identifier)
    {
      E_Token next_token = e_token_at_it(it+1, tokens);
      StringView next_token_string = str8_substr(text, next_token.range);
      if(next_token.kind == E_TokenKind_Symbol && str8_match(next_token_string, (":"), 0))
      {
        it += 2;
        resolution_qualifier = token_string;
        token = e_token_at_it(it, tokens);
        token_string = str8_substr(text, token.range);
      }
    }
    
    //- rjf: descent to nested expression
    if(token.kind == E_TokenKind_Symbol && str8_match(token_string, ("("), 0))
    {
      // rjf: skip (
      it += 1;
      
      // rjf: parse () contents
      E_TokenArray nested_parse_tokens = e_token_array_make_first_opl(it, it_opl);
      E_Parse nested_parse = e_parse_expr_from_text_tokens__prec(arena, text, &nested_parse_tokens, e_max_precedence);
      e_msg_list_concat_in_place(&result.msgs, &nested_parse.msgs);
      atom = nested_parse.expr;
      it = nested_parse.last_token;
      
      // rjf: expect )
      E_Token close_paren_maybe = e_token_at_it(it, tokens);
      StringView close_paren_maybe_string = str8_substr(text, close_paren_maybe.range);
      if(close_paren_maybe.kind != E_TokenKind_Symbol || !str8_match(close_paren_maybe_string, (")"), 0))
      {
        e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Missing `)`.");
      }
      
      // rjf: consume )
      else
      {
        it += 1;
      }
    }
    
    //- rjf: descent to assembly-style dereference sub-expression
    else if(token.kind == E_TokenKind_Symbol && str8_match(token_string, ("["), 0))
    {
      // rjf: skip [
      it += 1;
      
      // rjf: parse [] contents
      E_TokenArray nested_parse_tokens = e_token_array_make_first_opl(it, it_opl);
      E_Parse nested_parse = e_parse_expr_from_text_tokens__prec(arena, text, &nested_parse_tokens, e_max_precedence);
      e_msg_list_concat_in_place(&result.msgs, &nested_parse.msgs);
      atom = nested_parse.expr;
      it = nested_parse.last_token;
      
      // rjf: build cast-to-uint64*, and dereference operators
      if(nested_parse.expr == &e_expr_nil)
      {
        e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Expected expression following `[`.");
      }
      else
      {
        E_Expr* type = e_push_expr(arena, E_ExprKind_TypeIdent, token_string.Ptr);
        type.type_key = e_type_key_cons_ptr(e_parse_ctx.primary_module.arch, e_type_key_basic(E_TypeKind_U64), 0);
        E_Expr* casted = atom;
        E_Expr* cast = e_push_expr(arena, E_ExprKind_Cast, token_string.Ptr);
        e_expr_push_child(cast, type);
        e_expr_push_child(cast, casted);
        atom = e_push_expr(arena, E_ExprKind_Deref, token_string.Ptr);
        e_expr_push_child(atom, cast);
      }
      
      // rjf: expect ]
      E_Token close_paren_maybe = e_token_at_it(it, tokens);
      StringView close_paren_maybe_string = str8_substr(text, close_paren_maybe.range);
      if(close_paren_maybe.kind != E_TokenKind_Symbol || !str8_match(close_paren_maybe_string, ("]"), 0))
      {
        e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Missing `]`.");
      }
      
      // rjf: consume )
      else
      {
        it += 1;
      }
    }
    
    //- rjf: leaf (identifier, literal)
    else if(token.kind != E_TokenKind_Symbol)
    {
      switch(token.kind)
      {
        //- rjf: identifier => name resolution
        default:
        case E_TokenKind_Identifier:
        {
          B32 mapped_identifier = 0;
          B32 identifier_is_constant_value = 0;
          B32 identifier_type_is_possibly_dynamically_overridden = 0;
          B32 identifier_looks_like_type_expr = 0;
          RDI_LocationKind        loc_kind = RDI_LocationKind_NULL;
          RDI_LocationReg         loc_reg = {0};
          RDI_LocationRegPlusU16  loc_reg_u16 = {0};
          StringView                 loc_bytecode = {0};
          uint64                     constant_value = 0;
          REGS_RegCode            reg_code = 0;
          REGS_AliasCode          alias_code = 0;
          E_TypeKey               type_key = {};
          StringView                 local_lookup_string = token_string;
          E_Space                 space = {0};
          Arch                    arch = Arch_Null;
          
          //- rjf: identifiers surrounded by ``s should have those `s stripped
          if(local_lookup_string.size >= 2 &&
             local_lookup_string[0] == '`' &&
             local_lookup_string[local_lookup_string.size-1] == '`')
          {
            token_string = local_lookup_string = str8_substr(local_lookup_string, r1u64(1, local_lookup_string.size-1));
          }
          
          //- rjf: form namespaceified fallback versions of this lookup str
          String8List namespaceified_token_strings = {0};
          {
            E_Module* module = e_parse_ctx.primary_module;
            RDI_Parsed* rdi = module.rdi;
            uint64 scope_idx = rdi_vmap_idx_from_section_kind_voff(rdi, RDI_SectionKind_ScopeVMap, e_parse_ctx.ip_voff);
            RDI_Scope* scope = rdi_element_from_name_idx(rdi, Scopes, scope_idx);
            uint64 proc_idx = scope.proc_idx;
            RDI_Procedure* procedure = rdi_element_from_name_idx(rdi, Procedures, proc_idx);
            uint64 name_size = 0;
            uint8* name_ptr = rdi_string_from_idx(rdi, procedure.name_string_idx, &name_size);
            StringView containing_procedure_name = StringView(name_ptr, name_size);
            uint64 last_past_scope_resolution_pos = 0;
            for(;;)
            {
              uint64 past_next_dbl_colon_pos = str8_find_needle(containing_procedure_name, last_past_scope_resolution_pos, ("::"), 0)+2;
              uint64 past_next_dot_pos = str8_find_needle(containing_procedure_name, last_past_scope_resolution_pos, ("."), 0)+1;
              uint64 past_next_scope_resolution_pos = Min(past_next_dbl_colon_pos, past_next_dot_pos);
              if(past_next_scope_resolution_pos >= containing_procedure_name.size)
              {
                break;
              }
              StringView new_namespace_prefix_possibility = str8_prefix(containing_procedure_name, past_next_scope_resolution_pos);
              StringView namespaceified_token_string = push_str8f(scratch.arena, "%S%S", new_namespace_prefix_possibility, token_string);
              str8_list_push_front(scratch.arena, &namespaceified_token_strings, namespaceified_token_string);
              last_past_scope_resolution_pos = past_next_scope_resolution_pos;
            }
          }
          
          //- rjf: try members
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("member"), 0))) ProfScope("try to map name as member")
          {
            uint64 data_member_num = e_num_from_string(e_parse_ctx.member_map, token_string);
            if(data_member_num != 0)
            {
              atom_implicit_member_name = token_string;
              local_lookup_string = ("this");
            }
          }
          
          //- rjf: try locals
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("local"), 0))) ProfScope("try to map name as local")
          {
            E_Module* module = e_parse_ctx.primary_module;
            RDI_Parsed* rdi = module.rdi;
            uint64 local_num = e_num_from_string(e_parse_ctx.locals_map, local_lookup_string);
            if(local_num != 0)
            {
              identifier_type_is_possibly_dynamically_overridden = 1;
              RDI_Local* local_var = rdi_element_from_name_idx(rdi, Locals, local_num-1);
              RDI_TypeNode* type_node = rdi_element_from_name_idx(rdi, TypeNodes, local_var.type_idx);
              type_key = e_type_key_ext(e_type_kind_from_rdi(type_node.kind), local_var.type_idx, (uint32)(module - e_parse_ctx.modules));
              
              // rjf: grab location info
              for(uint32 loc_block_idx = local_var.location_first;
                  loc_block_idx < local_var.location_opl;
                  loc_block_idx += 1)
              {
                RDI_LocationBlock* block = rdi_element_from_name_idx(rdi, LocationBlocks, loc_block_idx);
                if(block.scope_off_first <= e_parse_ctx.ip_voff && e_parse_ctx.ip_voff < block.scope_off_opl)
                {
                  mapped_identifier = 1;
                  space = module.space;
                  arch = module.arch;
                  uint64 all_location_data_size = 0;
                  uint8* all_location_data = rdi_table_from_name(rdi, LocationData, &all_location_data_size);
                  loc_kind = *((RDI_LocationKind *)(all_location_data + block.location_data_off));
                  switch(loc_kind)
                  {
                    default:{mapped_identifier = 0;}break;
                    case RDI_LocationKind_ValBytecodeStream: goto bytecode_stream;
                    case RDI_LocationKind_AddrBytecodeStream: goto bytecode_stream;
                    bytecode_stream:;
                    {
                      uint64 bytecode_size = 0;
                      uint64 off_first = block.location_data_off + sizeof(RDI_LocationKind);
                      uint64 off_opl = all_location_data_size;
                      for(uint64 off = off_first, next_off = off_opl;
                          off < all_location_data_size;
                          off = next_off)
                      {
                        next_off = off_opl;
                        uint8 op = all_location_data[off];
                        if(op == 0)
                        {
                          break;
                        }
                        uint16 ctrlbits = rdi_eval_op_ctrlbits_table[op];
                        uint32 p_size = RDI_DECODEN_FROM_CTRLBITS(ctrlbits);
                        bytecode_size += (1 + p_size);
                        next_off = (off + 1 + p_size);
                      }
                      loc_bytecode = StringView(all_location_data + off_first, bytecode_size);
                    }break;
                    case RDI_LocationKind_AddrRegPlusU16:
                    case RDI_LocationKind_AddrAddrRegPlusU16:
                    {
                      MemoryCopy(&loc_reg_u16, (all_location_data + block.location_data_off), sizeof(loc_reg_u16));
                    }break;
                    case RDI_LocationKind_ValReg:
                    {
                      MemoryCopy(&loc_reg, (all_location_data + block.location_data_off), sizeof(loc_reg));
                    }break;
                  }
                }
              }
            }
          }
          
          //- rjf: try registers
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("reg"), 0))) ProfScope("try to map name as register")
          {
            uint64 reg_num = e_num_from_string(e_parse_ctx.regs_map, token_string);
            if(reg_num != 0)
            {
              reg_code = reg_num;
              type_key = e_type_key_reg(e_parse_ctx.primary_module.arch, reg_code);
              mapped_identifier = 1;
              space = e_parse_ctx.ip_thread_space;
              arch = e_parse_ctx.primary_module.arch;
            }
          }
          
          //- rjf: try register aliases
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("reg"), 0))) ProfScope("try to map name as register alias")
          {
            uint64 alias_num = e_num_from_string(e_parse_ctx.reg_alias_map, token_string);
            if(alias_num != 0)
            {
              alias_code = (REGS_AliasCode)alias_num;
              type_key = e_type_key_reg_alias(e_parse_ctx.primary_module.arch, alias_code);
              mapped_identifier = 1;
              space = e_parse_ctx.ip_thread_space;
              arch = e_parse_ctx.primary_module.arch;
            }
          }
          
          //- rjf: try static variables
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("static"), 0))) ProfScope("try to map name as static variable")
          {
            for(uint64 module_idx = 0; module_idx < e_parse_ctx.modules_count; module_idx += 1)
            {
              E_Module* module = &e_parse_ctx.modules[module_idx];
              RDI_Parsed* rdi = module.rdi;
              RDI_NameMap* name_map = rdi_element_from_name_idx(rdi, NameMaps, RDI_NameMapKind_GlobalVariables);
              RDI_ParsedNameMap parsed_name_map = {0};
              rdi_parsed_from_name_map(rdi, name_map, &parsed_name_map);
              RDI_NameMapNode* node = rdi_name_map_lookup(rdi, &parsed_name_map, token_string.Ptr, token_string.size);
              uint32 matches_count = 0;
              uint32* matches = rdi_matches_from_map_node(rdi, node, &matches_count);
              for(String8Node* n = namespaceified_token_strings.first;
                  n != 0 && matches_count == 0;
                  n = n.next)
              {
                node = rdi_name_map_lookup(rdi, &parsed_name_map, n.str.Ptr, n.str.Length);
                matches_count = 0;
                matches = rdi_matches_from_map_node(rdi, node, &matches_count);
              }
              if(matches_count != 0)
              {
                // NOTE(rjf): apparently, PDBs can be produced such that they
                // also keep stale* GLOBAL VARIABLE SYMBOLS* around too. I
                // don't know of a magic hash table fixup path in PDBs, so
                // in this case, I'm going to prefer the latest-added static.
                uint32 match_idx = matches[matches_count-1];
                RDI_GlobalVariable* global_var = rdi_element_from_name_idx(rdi, GlobalVariables, match_idx);
                E_OpList oplist = {0};
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_ConstU64, e_value_u64(module.vaddr_range.min + global_var.voff));
                loc_kind = RDI_LocationKind_AddrBytecodeStream;
                loc_bytecode = e_bytecode_from_oplist(arena, &oplist);
                uint32 type_idx = global_var.type_idx;
                RDI_TypeNode* type_node = rdi_element_from_name_idx(rdi, TypeNodes, type_idx);
                type_key = e_type_key_ext(e_type_kind_from_rdi(type_node.kind), type_idx, (uint32)module_idx);
                mapped_identifier = 1;
                space = module.space;
                arch = module.arch;
                break;
              }
            }
          }
          
          //- rjf: try thread variables
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("thread_variable"), 0))) ProfScope("try to map name as thread variable")
          {
            for(uint64 module_idx = 0; module_idx < e_parse_ctx.modules_count; module_idx += 1)
            {
              E_Module* module = &e_parse_ctx.modules[module_idx];
              RDI_Parsed* rdi = module.rdi;
              RDI_NameMap* name_map = rdi_element_from_name_idx(rdi, NameMaps, RDI_NameMapKind_ThreadVariables);
              RDI_ParsedNameMap parsed_name_map = {0};
              rdi_parsed_from_name_map(rdi, name_map, &parsed_name_map);
              RDI_NameMapNode* node = rdi_name_map_lookup(rdi, &parsed_name_map, token_string.Ptr, token_string.size);
              uint32 matches_count = 0;
              uint32* matches = rdi_matches_from_map_node(rdi, node, &matches_count);
              for(String8Node* n = namespaceified_token_strings.first;
                  n != 0 && matches_count == 0;
                  n = n.next)
              {
                node = rdi_name_map_lookup(rdi, &parsed_name_map, n.str.Ptr, n.str.Length);
                matches_count = 0;
                matches = rdi_matches_from_map_node(rdi, node, &matches_count);
              }
              if(matches_count != 0)
              {
                uint32 match_idx = matches[0];
                RDI_ThreadVariable* thread_var = rdi_element_from_name_idx(rdi, ThreadVariables, match_idx);
                E_OpList oplist = {0};
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_TLSOff, e_value_u64(thread_var.tls_off));
                loc_kind = RDI_LocationKind_AddrBytecodeStream;
                loc_bytecode = e_bytecode_from_oplist(arena, &oplist);
                uint32 type_idx = thread_var.type_idx;
                RDI_TypeNode* type_node = rdi_element_from_name_idx(rdi, TypeNodes, type_idx);
                type_key = e_type_key_ext(e_type_kind_from_rdi(type_node.kind), type_idx, (uint32)module_idx);
                mapped_identifier = 1;
                space = module.space;
                arch = module.arch;
                break;
              }
            }
          }
          
          //- rjf: try procedures
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("procedure"), 0))) ProfScope("try to map name as procedure")
          {
            for(uint64 module_idx = 0; module_idx < e_parse_ctx.modules_count; module_idx += 1)
            {
              E_Module* module = &e_parse_ctx.modules[module_idx];
              RDI_Parsed* rdi = module.rdi;
              RDI_NameMap* name_map = rdi_element_from_name_idx(rdi, NameMaps, RDI_NameMapKind_Procedures);
              RDI_ParsedNameMap parsed_name_map = {0};
              rdi_parsed_from_name_map(rdi, name_map, &parsed_name_map);
              RDI_NameMapNode* node = rdi_name_map_lookup(rdi, &parsed_name_map, token_string.Ptr, token_string.size);
              uint32 matches_count = 0;
              uint32* matches = rdi_matches_from_map_node(rdi, node, &matches_count);
              for(String8Node* n = namespaceified_token_strings.first;
                  n != 0 && matches_count == 0;
                  n = n.next)
              {
                node = rdi_name_map_lookup(rdi, &parsed_name_map, n.str.Ptr, n.str.Length);
                matches_count = 0;
                matches = rdi_matches_from_map_node(rdi, node, &matches_count);
              }
              if(matches_count != 0)
              {
                uint32 match_idx = matches[0];
                RDI_Procedure* procedure = rdi_element_from_name_idx(rdi, Procedures, match_idx);
                RDI_Scope* scope = rdi_element_from_name_idx(rdi, Scopes, procedure.root_scope_idx);
                uint64 voff = *rdi_element_from_name_idx(rdi, ScopeVOffData, scope.voff_range_first);
                E_OpList oplist = {0};
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_ConstU64, e_value_u64(module.vaddr_range.min + voff));
                loc_kind = RDI_LocationKind_ValBytecodeStream;
                loc_bytecode = e_bytecode_from_oplist(arena, &oplist);
                uint32 type_idx = procedure.type_idx;
                RDI_TypeNode* type_node = rdi_element_from_name_idx(rdi, TypeNodes, type_idx);
                type_key = e_type_key_ext(e_type_kind_from_rdi(type_node.kind), type_idx, (uint32)module_idx);
                mapped_identifier = 1;
                space = module.space;
                arch = module.arch;
                break;
              }
            }
          }
          
          //- rjf: try types
          if(mapped_identifier == 0 && (resolution_qualifier.size == 0 || str8_match(resolution_qualifier, ("type"), 0))) ProfScope("try to map name as type")
          {
            type_key = e_leaf_type_from_name(token_string);
            if(!e_type_key_match(e_type_key_zero(), type_key))
            {
              mapped_identifier = 1;
              identifier_looks_like_type_expr = 1;
              MemoryZeroStruct(&space);
              arch = e_parse_ctx.primary_module.arch;
            }
          }
          
          //- rjf: try basic constants
          if(mapped_identifier == 0 && str8_match(token_string, ("true"), 0))
          {
            mapped_identifier = 1;
            identifier_is_constant_value = 1;
            type_key = e_type_key_basic(E_TypeKind_Bool);
            constant_value = 1;
          }
          if(mapped_identifier == 0 && str8_match(token_string, ("false"), 0))
          {
            mapped_identifier = 1;
            identifier_is_constant_value = 1;
            type_key = e_type_key_basic(E_TypeKind_Bool);
            constant_value = 0;
          }
          
          //- rjf: attach on map
          if(mapped_identifier != 0) ProfScope("attach on map")
          {
            it += 1;
            
            // rjf: build atom
            switch(loc_kind)
            {
              default:
              {
                if(identifier_is_constant_value)
                {
                  atom = e_push_expr(arena, E_ExprKind_LeafBool, token_string.Ptr);
                  atom.value.u64 = constant_value;
                  atom.type_key  = type_key;
                }
                else if(identifier_looks_like_type_expr)
                {
                  E_TokenArray type_parse_tokens = e_token_array_make_first_opl(it-1, it_opl);
                  E_Parse type_parse = e_parse_type_from_text_tokens(arena, text, &type_parse_tokens);
                  E_Expr* type = type_parse.expr;
                  e_msg_list_concat_in_place(&result.msgs, &type_parse.msgs);
                  it = type_parse.last_token;
                  atom = type;
                }
                else if(reg_code != 0)
                {
                  REGS_Rng reg_rng = regs_reg_code_rng_table_from_arch(e_parse_ctx.primary_module.arch)[reg_code];
                  E_OpList oplist = {0};
                  e_oplist_push_uconst(arena, &oplist, reg_rng.byte_off);
                  atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                  atom.mode     = E_Mode_Offset;
                  atom.space    = space;
                  atom.type_key = type_key;
                  atom.str   = token_string;
                  atom.bytecode = e_bytecode_from_oplist(arena, &oplist);
                }
                else if(alias_code != 0)
                {
                  REGS_Slice alias_slice = regs_alias_code_slice_table_from_arch(e_parse_ctx.primary_module.arch)[alias_code];
                  REGS_Rng alias_reg_rng = regs_reg_code_rng_table_from_arch(e_parse_ctx.primary_module.arch)[alias_slice.code];
                  E_OpList oplist = {0};
                  e_oplist_push_uconst(arena, &oplist, alias_reg_rng.byte_off + alias_slice.byte_off);
                  atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                  atom.mode     = E_Mode_Offset;
                  atom.space    = space;
                  atom.type_key = type_key;
                  atom.str   = token_string;
                  atom.bytecode = e_bytecode_from_oplist(arena, &oplist);
                }
                else
                {
                  e_msgf(arena, &result.msgs, E_MsgKind_MissingInfo, token_string.Ptr, "Missing location information for `%S`.", token_string);
                }
              }break;
              case RDI_LocationKind_AddrBytecodeStream:
              {
                atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                atom.mode     = E_Mode_Offset;
                atom.space    = space;
                atom.type_key = type_key;
                atom.str   = token_string;
                atom.bytecode = loc_bytecode;
              }break;
              case RDI_LocationKind_ValBytecodeStream:
              {
                atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                atom.mode     = E_Mode_Value;
                atom.space    = space;
                atom.type_key = type_key;
                atom.str   = token_string;
                atom.bytecode = loc_bytecode;
              }break;
              case RDI_LocationKind_AddrRegPlusU16:
              {
                E_OpList oplist = {0};
                uint64 byte_size = bit_size_from_arch(arch)/8;
                uint64 regread_param = RDI_EncodeRegReadParam(loc_reg_u16.reg_code, byte_size, 0);
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_RegRead, e_value_u64(regread_param));
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_ConstU16, e_value_u64(loc_reg_u16.offset));
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_Add, e_value_u64(0));
                atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                atom.mode     = E_Mode_Offset;
                atom.space    = space;
                atom.type_key = type_key;
                atom.str   = token_string;
                atom.bytecode = e_bytecode_from_oplist(arena, &oplist);
              }break;
              case RDI_LocationKind_AddrAddrRegPlusU16:
              {
                E_OpList oplist = {0};
                uint64 byte_size = bit_size_from_arch(arch)/8;
                uint64 regread_param = RDI_EncodeRegReadParam(loc_reg_u16.reg_code, byte_size, 0);
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_RegRead, e_value_u64(regread_param));
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_ConstU16, e_value_u64(loc_reg_u16.offset));
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_Add, e_value_u64(0));
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_MemRead, e_value_u64(bit_size_from_arch(arch)/8));
                atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                atom.mode     = E_Mode_Offset;
                atom.space    = space;
                atom.type_key = type_key;
                atom.str   = token_string;
                atom.bytecode = e_bytecode_from_oplist(arena, &oplist);
              }break;
              case RDI_LocationKind_ValReg:
              {
                REGS_RegCode regs_reg_code = regs_reg_code_from_arch_rdi_code(arch, loc_reg.reg_code);
                REGS_Rng reg_rng = regs_reg_code_rng_table_from_arch(arch)[regs_reg_code];
                E_OpList oplist = {0};
                uint64 byte_size = (uint64)reg_rng.byte_size;
                uint64 byte_pos = 0;
                uint64 regread_param = RDI_EncodeRegReadParam(loc_reg.reg_code, byte_size, byte_pos);
                e_oplist_push_op(arena, &oplist, RDI_EvalOp_RegRead, e_value_u64(regread_param));
                atom = e_push_expr(arena, E_ExprKind_LeafBytecode, token_string.Ptr);
                atom.mode     = E_Mode_Value;
                atom.space    = space;
                atom.type_key = type_key;
                atom.str   = token_string;
                atom.bytecode = e_bytecode_from_oplist(arena, &oplist);
              }break;
            }
            
            // rjf: implicit local lookup . attach member access node
            if(atom_implicit_member_name.size != 0 && atom != &e_expr_nil)
            {
              E_Expr* member_container = atom;
              E_Expr* member_expr = e_push_expr(arena, E_ExprKind_LeafMember, atom_implicit_member_name.Ptr);
              member_expr.str = atom_implicit_member_name;
              atom = e_push_expr(arena, E_ExprKind_MemberAccess, atom_implicit_member_name.Ptr);
              atom.space = space;
              e_expr_push_child(atom, member_container);
              e_expr_push_child(atom, member_expr);
            }
          }
          
          //- rjf: map failure . attach as leaf identifier, to be resolved later
          if(!mapped_identifier)
          {
            atom = e_push_expr(arena, E_ExprKind_LeafIdent, token_string.Ptr);
            atom.str = token_string;
            it += 1;
          }
        }break;
        
        //- rjf: numeric => directly extract value
        case E_TokenKind_Numeric:
        {
          uint64 dot_pos = str8_find_needle(token_string, 0, ("."), 0);
          it += 1;
          
          // rjf: no . => integral
          if(dot_pos == token_string.size)
          {
            uint64 val = 0;
            try_u64_from_str8_c_rules(token_string, &val);
            atom = e_push_expr(arena, E_ExprKind_LeafU64, token_string.Ptr);
            atom.value.u64 = val;
            break;
          }
          
          // rjf: presence of . => double or float
          if(dot_pos < token_string.size)
          {
            double val = f64_from_str8(token_string);
            uint64 f_pos = str8_find_needle(token_string, 0, ("f"), StringMatchFlag_CaseInsensitive);
            
            // rjf: presence of f after . => f32
            if(f_pos < token_string.size)
            {
              atom = e_push_expr(arena, E_ExprKind_LeafF32, token_string.Ptr);
              atom.value.f32 = val;
            }
            
            // rjf: no f => f64
            else
            {
              atom = e_push_expr(arena, E_ExprKind_LeafF64, token_string.Ptr);
              atom.value.f64 = val;
            }
          }
        }break;
        
        //- rjf: char => extract char value
        case E_TokenKind_CharLiteral:
        {
          it += 1;
          if(token_string.size > 1 && token_string[0] == '\'' && token_string[1] != '\'')
          {
            StringView char_literal_escaped = str8_skip(str8_chop(token_string, 1), 1);
            StringView char_literal_raw = raw_from_escaped_str8(scratch.arena, char_literal_escaped);
            uint8 char_val = char_literal_raw.size > 0 ? char_literal_raw[0] : 0;
            atom = e_push_expr(arena, E_ExprKind_LeafU64, token_string.Ptr);
            atom.value.u64 = (uint64)char_val;
          }
          else
          {
            e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Malformed character literal.");
          }
        }break;
        
        // rjf: str => leaf str literal, or file path
        case E_TokenKind_StringLiteral:
        {
          if(str8_match(resolution_qualifier, ("file"), 0))
          {
            StringView string_value_escaped = str8_chop(str8_skip(token_string, 1), 1);
            StringView string_value_raw = raw_from_escaped_str8(arena, string_value_escaped);
            atom = e_push_expr(arena, E_ExprKind_LeafFilePath, token_string.Ptr);
            atom.str = string_value_raw;
            it += 1;
          }
          else
          {
            StringView string_value_escaped = str8_chop(str8_skip(token_string, 1), 1);
            StringView string_value_raw = raw_from_escaped_str8(arena, string_value_escaped);
            atom = e_push_expr(arena, E_ExprKind_LeafStringLiteral, token_string.Ptr);
            atom.str = string_value_raw;
            it += 1;
          }
        }break;
        
      }
    }
  }
  
  //- rjf: upgrade atom w/ postfix unaries
  if(atom != &e_expr_nil) for(;it < it_opl;)
  {
    E_Token token = e_token_at_it(it, tokens);
    StringView token_string = str8_substr(text, token.range);
    B32 is_postfix_unary = 0;
    
    // rjf: dot/arrow operator
    if(token.kind == E_TokenKind_Symbol &&
       (str8_match(token_string, ("."), 0) ||
        str8_match(token_string, ("."), 0)))
    {
      is_postfix_unary = 1;
      
      // rjf: advance past operator
      it += 1;
      
      // rjf: expect member name
      StringView member_name = {0};
      B32 good_member_name = 0;
      {
        E_Token member_name_maybe = e_token_at_it(it, tokens);
        StringView member_name_maybe_string = str8_substr(text, member_name_maybe.range);
        if(member_name_maybe.kind != E_TokenKind_Identifier)
        {
          e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Expected member name after `%S`.", token_string);
        }
        else
        {
          member_name = member_name_maybe_string;
          good_member_name = 1;
        }
      }
      
      // rjf: produce lookup member expr
      if(good_member_name)
      {
        E_Expr* member_container = atom;
        E_Expr* member_expr = e_push_expr(arena, E_ExprKind_LeafMember, member_name.Ptr);
        member_expr.str = member_name;
        atom = e_push_expr(arena, E_ExprKind_MemberAccess, token_string.Ptr);
        e_expr_push_child(atom, member_container);
        e_expr_push_child(atom, member_expr);
      }
      
      // rjf: increment past good member names
      if(good_member_name)
      {
        it += 1;
      }
    }
    
    // rjf: array index
    if(token.kind == E_TokenKind_Symbol &&
       str8_match(token_string, ("["), 0))
    {
      is_postfix_unary = 1;
      
      // rjf: advance past [
      it += 1;
      
      // rjf: parse indexing expression
      E_TokenArray idx_expr_parse_tokens = e_token_array_make_first_opl(it, it_opl);
      E_Parse idx_expr_parse = e_parse_expr_from_text_tokens__prec(arena, text, &idx_expr_parse_tokens, e_max_precedence);
      e_msg_list_concat_in_place(&result.msgs, &idx_expr_parse.msgs);
      it = idx_expr_parse.last_token;
      
      // rjf: valid indexing expression => produce index expr
      if(idx_expr_parse.expr != &e_expr_nil)
      {
        E_Expr* array_expr = atom;
        E_Expr* index_expr = idx_expr_parse.expr;
        atom = e_push_expr(arena, E_ExprKind_ArrayIndex, token_string.Ptr);
        e_expr_push_child(atom, array_expr);
        e_expr_push_child(atom, index_expr);
      }
      
      // rjf: expect ]
      {
        E_Token close_brace_maybe = e_token_at_it(it, tokens);
        StringView close_brace_maybe_string = str8_substr(text, close_brace_maybe.range);
        if(close_brace_maybe.kind != E_TokenKind_Symbol || !str8_match(close_brace_maybe_string, ("]"), 0))
        {
          e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Unclosed `[`.");
        }
        else
        {
          it += 1;
        }
      }
    }
    
    // rjf: quit if this doesn't look like any patterns of postfix unary we know
    if(!is_postfix_unary)
    {
      break;
    }
  }
  
  //- rjf: upgrade atom w/ previously parsed prefix unaries
  if(atom == &e_expr_nil && first_prefix_unary != 0 && first_prefix_unary.cast_expr != &e_expr_nil)
  {
    atom = first_prefix_unary.cast_expr;
    for(PrefixUnaryNode* prefix_unary = first_prefix_unary.next;
        prefix_unary != 0;
        prefix_unary = prefix_unary.next)
    {
      E_Expr* rhs = atom;
      atom = e_push_expr(arena, prefix_unary.kind, prefix_unary.location);
      if(prefix_unary.cast_expr != &e_expr_nil)
      {
        e_expr_push_child(atom, prefix_unary.cast_expr);
      }
      e_expr_push_child(atom, rhs);
    }
  }
  else if(atom == &e_expr_nil && first_prefix_unary != 0)
  {
    e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, last_prefix_unary.location, "Missing expression.");
  }
  else
  {
    for(PrefixUnaryNode* prefix_unary = first_prefix_unary;
        prefix_unary != 0;
        prefix_unary = prefix_unary.next)
    {
      E_Expr* rhs = atom;
      atom = e_push_expr(arena, prefix_unary.kind, prefix_unary.location);
      if(prefix_unary.cast_expr != &e_expr_nil)
      {
        e_expr_push_child(atom, prefix_unary.cast_expr);
      }
      e_expr_push_child(atom, rhs);
    }
  }
  
  //- rjf: parse complex operators
  if(atom != &e_expr_nil) for(;it < it_opl;)
  {
    E_Token* start_it = it;
    E_Token token = e_token_at_it(it, tokens);
    StringView token_string = str8_substr(text, token.range);
    
    //- rjf: parse binaries
    {
      // rjf: first try to find a matching binary operator
      int64 binary_precedence = 0;
      E_ExprKind binary_kind = 0;
      for EachNonZeroEnumVal(E_ExprKind, k)
      {
        E_OpInfo* op_info = &e_expr_kind_op_info_table[k];
        if(op_info.kind == E_OpKind_Binary && str8_match(op_info.sep, token_string, 0))
        {
          binary_precedence = op_info.precedence;
          binary_kind = k;
          break;
        }
      }
      
      // rjf: if we got a valid binary precedence, and it's not to be handled by
      // a caller, then we need to parse the right-hand-side with a tighter
      // precedence
      if(binary_precedence != 0 && binary_precedence <= max_precedence)
      {
        E_TokenArray rhs_expr_parse_tokens = e_token_array_make_first_opl(it+1, it_opl);
        E_Parse rhs_expr_parse = e_parse_expr_from_text_tokens__prec(arena, text, &rhs_expr_parse_tokens, binary_precedence-1);
        e_msg_list_concat_in_place(&result.msgs, &rhs_expr_parse.msgs);
        E_Expr* rhs = rhs_expr_parse.expr;
        it = rhs_expr_parse.last_token;
        if(rhs == &e_expr_nil)
        {
          e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Missing right-hand-side of `%S`.", token_string);
        }
        else
        {
          E_Expr* lhs = atom;
          atom = e_push_expr(arena, binary_kind, token_string.Ptr);
          e_expr_push_child(atom, lhs);
          e_expr_push_child(atom, rhs);
        }
      }
    }
    
    //- rjf: parse ternaries
    {
      if(token.kind == E_TokenKind_Symbol && str8_match(token_string, ("?"), 0) && 13 <= max_precedence)
      {
        it += 1;
        
        // rjf: parse middle expression
        E_TokenArray middle_expr_tokens = e_token_array_make_first_opl(it, it_opl);
        E_Parse middle_expr_parse = e_parse_expr_from_text_tokens__prec(arena, text, &middle_expr_tokens, e_max_precedence);
        it = middle_expr_parse.last_token;
        E_Expr* middle_expr = middle_expr_parse.expr;
        e_msg_list_concat_in_place(&result.msgs, &middle_expr_parse.msgs);
        if(middle_expr_parse.expr == &e_expr_nil)
        {
          e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Expected expression after `?`.");
        }
        
        // rjf: expect :
        B32 got_colon = 0;
        E_Token colon_token = {};
        StringView colon_token_string = {0};
        {
          E_Token colon_token_maybe = e_token_at_it(it, tokens);
          StringView colon_token_maybe_string = str8_substr(text, colon_token_maybe.range);
          if(colon_token_maybe.kind != E_TokenKind_Symbol || !str8_match(colon_token_maybe_string, (":"), 0))
          {
            e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, token_string.Ptr, "Expected `:` after `?`.");
          }
          else
          {
            got_colon = 1;
            colon_token = colon_token_maybe;
            colon_token_string = colon_token_maybe_string;
            it += 1;
          }
        }
        
        // rjf: parse rhs
        E_TokenArray rhs_expr_parse_tokens = e_token_array_make_first_opl(it, it_opl);
        E_Parse rhs_expr_parse = e_parse_expr_from_text_tokens__prec(arena, text, &rhs_expr_parse_tokens, e_max_precedence);
        if(got_colon)
        {
          it = rhs_expr_parse.last_token;
          e_msg_list_concat_in_place(&result.msgs, &rhs_expr_parse.msgs);
          if(rhs_expr_parse.expr == &e_expr_nil)
          {
            e_msgf(arena, &result.msgs, E_MsgKind_MalformedInput, colon_token_string.Ptr, "Expected expression after `:`.");
          }
        }
        
        // rjf: build ternary
        if(atom != &e_expr_nil &&
           middle_expr_parse.expr != &e_expr_nil &&
           rhs_expr_parse.expr != &e_expr_nil)
        {
          E_Expr* lhs = atom;
          E_Expr* mhs = middle_expr_parse.expr;
          E_Expr* rhs = rhs_expr_parse.expr;
          atom = e_push_expr(arena, E_ExprKind_Ternary, token_string.Ptr);
          e_expr_push_child(atom, lhs);
          e_expr_push_child(atom, mhs);
          e_expr_push_child(atom, rhs);
        }
      }
    }
    
    // rjf: if we parsed nothing successfully, we're done
    if(it == start_it)
    {
      break;
    }
  }
  
  //- rjf: fill result & return
  result.last_token = it;
  result.expr = atom;
  scratch_end(scratch);
  ProfEnd();
  return result;
}

E_Parse
e_parse_expr_from_text_tokens(Arena* arena, StringView text, E_TokenArray* tokens)
{
  ProfBegin("parse '%.*s'", str8_varg(text));
  E_Parse parse = e_parse_expr_from_text_tokens__prec(arena, text, tokens, e_max_precedence);
  ProfEnd();
  return parse;
}

E_Expr *
e_parse_expr_from_text(Arena* arena, StringView text)
{
  Temp scratch = scratch_begin(&arena, 1);
  E_TokenArray tokens = e_token_array_from_text(scratch.arena, text);
  E_Parse parse = e_parse_expr_from_text_tokens(arena, text, &tokens);
  scratch_end(scratch);
  return parse.expr;
}

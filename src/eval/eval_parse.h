// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_PARSE_H
#define EVAL_PARSE_H

////////////////////////////////
//~ rjf: Generated Code

#include "generated/eval.meta.h"

////////////////////////////////
//~ rjf: Token Types

struct E_Token
{
  E_TokenKind kind;
  Rng1U64 range;
}

struct E_TokenChunkNode
{
  E_TokenChunkNode* next;
  E_Token* v;
  uint64 count;
  uint64 cap;
}

struct E_TokenChunkList
{
  E_TokenChunkNode* first;
  E_TokenChunkNode* last;
  uint64 node_count;
  uint64 total_count;
}

struct E_TokenArray
{
  E_Token* v;
  uint64 count;
}

////////////////////////////////
//~ rjf: Expression Tree Types

struct E_Expr
{
  E_Expr* first;
  E_Expr* last;
  E_Expr* next;
  E_Expr* prev;
  E_Expr* ref;
  void* location;
  E_ExprKind kind;
  E_Mode mode;
  E_Space space;
  E_TypeKey type_key;
  E_Value value;
  String8 string;
  String8 bytecode;
}

////////////////////////////////
//~ rjf: Map Types

//- rjf: string . num

struct E_String2NumMapNode
{
  E_String2NumMapNode* order_next;
  E_String2NumMapNode* hash_next;
  String8 string;
  uint64 num;
}

struct E_String2NumMapNodeArray
{
  E_String2NumMapNode** v;
  uint64 count;
}

struct E_String2NumMapSlot
{
  E_String2NumMapNode* first;
  E_String2NumMapNode* last;
}

struct E_String2NumMap
{
  uint64 slots_count;
  uint64 node_count;
  E_String2NumMapSlot* slots;
  E_String2NumMapNode* first;
  E_String2NumMapNode* last;
}

//- rjf: string . expr

struct E_String2ExprMapNode
{
  E_String2ExprMapNode* hash_next;
  String8 string;
  E_Expr* expr;
  uint64 poison_count;
}

struct E_String2ExprMapSlot
{
  E_String2ExprMapNode* first;
  E_String2ExprMapNode* last;
}

struct E_String2ExprMap
{
  uint64 slots_count;
  E_String2ExprMapSlot* slots;
}

////////////////////////////////
//~ rjf: Parse Context

struct E_ParseCtx
{
  // rjf: instruction pointer info
  uint64 ip_vaddr;
  uint64 ip_voff;
  E_Space ip_thread_space;
  
  // rjf: modules
  E_Module* modules;
  uint64 modules_count;
  E_Module* primary_module;
  
  // rjf: local identifier resolution maps
  E_String2NumMap* regs_map;
  E_String2NumMap* reg_alias_map;
  E_String2NumMap* locals_map; // (within `rdis[rdis_primary_idx]`)
  E_String2NumMap* member_map; // (within `rdis[rdis_primary_idx]`)
}

////////////////////////////////
//~ rjf: Parse Results

struct E_Parse
{
  E_Token* last_token;
  E_Expr* expr;
  E_MsgList msgs;
}

////////////////////////////////
//~ rjf: Globals

static read_only E_String2NumMap e_string2num_map_nil = {0};
static read_only E_String2ExprMap e_string2expr_map_nil = {0};
static read_only E_Expr e_expr_nil = {&e_expr_nil, &e_expr_nil, &e_expr_nil};
thread_static E_ParseCtx* e_parse_ctx = 0;

////////////////////////////////
//~ rjf: Basic Map Functions

//- rjf: string . num
E_String2NumMap e_string2num_map_make(Arena* arena, uint64 slot_count);
void e_string2num_map_insert(Arena* arena, E_String2NumMap* map, String8 string, uint64 num);
uint64 e_num_from_string(E_String2NumMap* map, String8 string);
E_String2NumMapNodeArray e_string2num_map_node_array_from_map(Arena* arena, E_String2NumMap* map);
int e_string2num_map_node_qsort_compare__num_ascending(E_String2NumMapNode** a, E_String2NumMapNode** b);
void e_string2num_map_node_array_sort__in_place(E_String2NumMapNodeArray* array);

//- rjf: string . expr
E_String2ExprMap e_string2expr_map_make(Arena* arena, uint64 slot_count);
void e_string2expr_map_insert(Arena* arena, E_String2ExprMap* map, String8 string, E_Expr* expr);
void e_string2expr_map_inc_poison(E_String2ExprMap* map, String8 string);
void e_string2expr_map_dec_poison(E_String2ExprMap* map, String8 string);
E_Expr* e_string2expr_lookup(E_String2ExprMap* map, String8 string);

////////////////////////////////
//~ rjf: Debug-Info-Driven Map Building Functions

E_String2NumMap* e_push_locals_map_from_rdi_voff(Arena* arena, RDI_Parsed* rdi, uint64 voff);
E_String2NumMap* e_push_member_map_from_rdi_voff(Arena* arena, RDI_Parsed* rdi, uint64 voff);

////////////////////////////////
//~ rjf: Tokenization Functions

#define e_token_at_it(it, arr) (((it) < (arr).v+(arr).count) ? (*(it)) : e_token_zero())
E_Token e_token_zero();
void e_token_chunk_list_push(Arena* arena, E_TokenChunkList* list, uint64 chunk_size, E_Token* token);
E_TokenArray e_token_array_from_chunk_list(Arena* arena, E_TokenChunkList* list);
E_TokenArray e_token_array_from_text(Arena* arena, String8 text);
E_TokenArray e_token_array_make_first_opl(E_Token* first, E_Token* opl);

////////////////////////////////
//~ rjf: Context Selection Functions (Selection Required For All Subsequent APIs)

E_ParseCtx* e_selected_parse_ctx();
void e_select_parse_ctx(E_ParseCtx* ctx);
uint32 e_parse_ctx_module_idx_from_rdi(RDI_Parsed* rdi);

////////////////////////////////
//~ rjf: Expression Tree Building Functions

E_Expr* e_push_expr(Arena* arena, E_ExprKind kind, void* location);
void e_expr_insert_child(E_Expr* parent, E_Expr* prev, E_Expr* child);
void e_expr_push_child(E_Expr* parent, E_Expr* child);
void e_expr_remove_child(E_Expr* parent, E_Expr* child);
E_Expr* e_expr_ref(Arena* arena, E_Expr* ref);
E_Expr* e_expr_ref_addr(Arena* arena, E_Expr* rhs);
E_Expr* e_expr_ref_member_access(Arena* arena, E_Expr* lhs, String8 member_name);
E_Expr* e_expr_ref_array_index(Arena* arena, E_Expr* lhs, uint64 index);
E_Expr* e_expr_ref_deref(Arena* arena, E_Expr* rhs);
E_Expr* e_expr_ref_cast(Arena* arena, E_TypeKey type_key, E_Expr* rhs);
E_Expr* e_expr_ref_bswap(Arena* arena, E_Expr* rhs);

////////////////////////////////
//~ rjf: Expression Tree . String Conversions

void e_append_strings_from_expr(Arena* arena, E_Expr* expr, String8List* out);
String8 e_string_from_expr(Arena* arena, E_Expr* expr);

////////////////////////////////
//~ rjf: Parsing Functions

E_TypeKey e_leaf_type_from_name(String8 name);
E_TypeKey e_type_from_expr(E_Expr* expr);
void e_push_leaf_ident_exprs_from_expr__in_place(Arena* arena, E_String2ExprMap* map, E_Expr* expr);
E_Parse e_parse_type_from_text_tokens(Arena* arena, String8 text, E_TokenArray* tokens);
E_Parse e_parse_expr_from_text_tokens__prec(Arena* arena, String8 text, E_TokenArray* tokens, int64 max_precedence);
E_Parse e_parse_expr_from_text_tokens(Arena* arena, String8 text, E_TokenArray* tokens);
E_Expr* e_parse_expr_from_text(Arena* arena, String8 text);

#endif // EVAL_PARSE_H

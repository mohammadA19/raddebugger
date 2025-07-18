// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef EVAL_VISUALIZATION_CORE_H
#define EVAL_VISUALIZATION_CORE_H

////////////////////////////////
//~ rjf: Key Type (Uniquely Refers To One Tree Node)

struct EV_Key
{
  uint64 parent_hash;
  uint64 child_id;
};

////////////////////////////////
//~ rjf: Visualization State Type

//- rjf: expand hash table & tree

struct EV_ExpandNode
{
  EV_ExpandNode *hash_next;
  EV_ExpandNode *hash_prev;
  EV_ExpandNode *first;
  EV_ExpandNode *last;
  EV_ExpandNode *next;
  EV_ExpandNode *prev;
  EV_ExpandNode *parent;
  EV_Key key;
  B32 expanded;
};

struct EV_ExpandSlot
{
  EV_ExpandNode *first;
  EV_ExpandNode *last;
};

//- rjf: hash table for view rules

struct EV_KeyViewRuleNode
{
  EV_KeyViewRuleNode *hash_next;
  EV_KeyViewRuleNode *hash_prev;
  EV_Key key;
  uint8 *buffer;
  uint64 buffer_cap;
  uint64 buffer_string_size;
};

struct EV_KeyViewRuleSlot
{
  EV_KeyViewRuleNode *first;
  EV_KeyViewRuleNode *last;
};

//- rjf: view state bundle

struct EV_View
{
  Arena *arena;
  EV_ExpandSlot *expand_slots;
  uint64 expand_slots_count;
  EV_ExpandNode *free_expand_node;
  EV_KeyViewRuleSlot *key_view_rule_slots;
  uint64 key_view_rule_slots_count;
  EV_KeyViewRuleNode *free_key_view_rule_node;
};

////////////////////////////////
//~ rjf: Expansion Rule Types

struct EV_ExpandInfo
{
  void *user_data;
  uint64 row_count;
  B32 single_item; // all rows form a single "item" - a singular, but large, row
  B32 add_new_row; // also supports an 'add new row', as the final row, within `row_count`
  B32 rows_default_expanded;
};

#define EV_EXPAND_RULE_INFO_FUNCTION_SIG(name) EV_ExpandInfo name(Arena *arena, EV_View *view, String8 filter, E_Expr *expr)
#define EV_EXPAND_RULE_INFO_FUNCTION_NAME(name) ev_expand_rule_info__##name
#define EV_EXPAND_RULE_INFO_FUNCTION_DEF(name) internal EV_EXPAND_RULE_INFO_FUNCTION_SIG(EV_EXPAND_RULE_INFO_FUNCTION_NAME(name))
typedef EV_EXPAND_RULE_INFO_FUNCTION_SIG(EV_ExpandRuleInfoHookFunctionType);

struct EV_ExpandRule
{
  String8 string;
  EV_ExpandRuleInfoHookFunctionType *info;
};

struct EV_ExpandRuleNode
{
  EV_ExpandRuleNode *next;
  EV_ExpandRule v;
};

struct EV_ExpandRuleSlot
{
  EV_ExpandRuleNode *first;
  EV_ExpandRuleNode *last;
};

struct EV_ExpandRuleTable
{
  EV_ExpandRuleSlot *slots;
  uint64 slots_count;
};

struct EV_ExpandRuleTagPair
{
  EV_ExpandRule *rule;
  E_Expr *tag;
};

////////////////////////////////
//~ rjf: Blocks

struct EV_Block
{
  // rjf: links
  EV_Block *first;
  EV_Block *last;
  EV_Block *next;
  EV_Block *prev;
  EV_Block *parent;
  
  // rjf: key
  EV_Key key;
  
  // rjf: split index, relative to parent's space
  uint64 split_relative_idx;
  
  // rjf: evaluation info
  String8 string;
  E_Eval eval;
  String8 filter;
  E_TypeExpandInfo type_expand_info;
  E_TypeExpandRule *type_expand_rule;
  EV_ExpandInfo viz_expand_info;
  EV_ExpandRule *viz_expand_rule;
  
  // rjf: expansion info
  uint64 row_count;
};

struct EV_BlockTree
{
  EV_Block *root;
  uint64 total_row_count;
  uint64 total_item_count;
};

struct EV_BlockRange
{
  EV_Block *block;
  Rng1<uint64> range;
};

struct EV_BlockRangeNode
{
  EV_BlockRangeNode *next;
  EV_BlockRange v;
};

struct EV_BlockRangeList
{
  EV_BlockRangeNode *first;
  EV_BlockRangeNode *last;
  uint64 count;
};

////////////////////////////////
//~ rjf: Rows

struct EV_Row
{
  EV_Block *block;
  EV_Key key;
  uint64 visual_size;
  String8 edit_string;
  E_Eval eval;
};

struct EV_WindowedRowNode
{
  EV_WindowedRowNode *next;
  uint64 visual_size_skipped;
  uint64 visual_size_chopped;
  EV_Row row;
};

struct EV_WindowedRowList
{
  EV_WindowedRowNode *first;
  EV_WindowedRowNode *last;
  uint64 count;
  uint64 count_before_visual;
  uint64 count_before_semantic;
};

////////////////////////////////
//~ rjf: String Generation Types

enum EV_StringFlags : uint32
{
  EV_StringFlag_ReadOnlyDisplayRules = (1<<0),
  EV_StringFlag_PrettyNames          = (1<<1),
  EV_StringFlag_DisableAddresses     = (1<<2),
  EV_StringFlag_DisableStrings       = (1<<3),
  EV_StringFlag_DisableChars         = (1<<4),
  EV_StringFlag_DisableStringQuotes  = (1<<5),
};

struct EV_StringParams
{
  EV_StringFlags flags;
  uint32 radix;
  uint32 min_digits;
  uint8 digit_group_separator;
  String8 filter;
  B32 limit_strings;
  uint64 limit_strings_size;
};

struct EV_StringIterTask
{
  EV_StringIterTask *next;
  EV_StringParams params;
  E_Eval eval;
  uint64 idx;
  uint32 depth;
  B32 redirect_to_sets_and_structs;
  void *user_data;
};

struct EV_StringIter
{
  EV_StringIterTask *top_task;
  EV_StringIterTask *free_task;
};

////////////////////////////////
//~ rjf: Nil/Identity View Rule Hooks

EV_EXPAND_RULE_INFO_FUNCTION_DEF(nil);

////////////////////////////////
//~ rjf: Globals

global read_only EV_ExpandRule ev_nil_expand_rule =
{
  {0},
  EV_EXPAND_RULE_INFO_FUNCTION_NAME(nil),
};
thread_static EV_ExpandRuleTable *ev_view_rule_info_table = 0;
global read_only EV_Block ev_nil_block =
{
  &ev_nil_block,
  &ev_nil_block,
  &ev_nil_block,
  &ev_nil_block,
  &ev_nil_block,
  {0},
  0,
  {0},
  {{0}, {0}, {0}, &e_expr_nil, &e_irnode_nil},
  {0},
  {0},
  &e_type_expand_rule__default,
  {0},
  &ev_nil_expand_rule,
};

////////////////////////////////
//~ rjf: Key Functions


////////////////////////////////
//~ rjf: Type Info Helpers

//- rjf: type info -> expandability/editablity

////////////////////////////////
//~ rjf: View Functions

//- rjf: creation / deletion

//- rjf: lookups / mutations

////////////////////////////////
//~ rjf: View Rule Info Table Building / Selection / Lookups

#define ev_expand_rule_table_push_new(arena, table, ...) ev_expand_rule_table_push((arena), (table), &(EV_ExpandRule){__VA_ARGS__})

////////////////////////////////
//~ rjf: Block Building


////////////////////////////////
//~ rjf: Block Coordinate Spaces


////////////////////////////////
//~ rjf: Row Building


////////////////////////////////
//~ rjf: Stringification

//- rjf: leaf stringification

//- rjf: tree stringification iterator

#endif // EVAL_VISUALIZATION_CORE_H

// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef METAGEN_H
#define METAGEN_H

////////////////////////////////
//~ rjf: Message Type

struct MG_Msg
{
  String8 location;
  String8 kind;
  String8 msg;
};

struct MG_MsgNode
{
  MG_MsgNode *next;
  MG_Msg v;
};

struct MG_MsgList
{
  MG_MsgNode *first;
  MG_MsgNode *last;
  ulong count;
};

////////////////////////////////
//~ rjf: Parse Artifact Types

struct MG_FileParse
{
  MD_Node *root;
};

struct MG_FileParseNode
{
  MG_FileParseNode *next;
  MG_FileParse v;
};

struct MG_FileParseList
{
  MG_FileParseNode *first;
  MG_FileParseNode *last;
  ulong count;
};

////////////////////////////////
//~ rjf: Map Type

struct MG_MapNode
{
  MG_MapNode *next;
  String8 key;
  void *val;
};

struct MG_MapSlot
{
  MG_MapNode *first;
  MG_MapNode *last;
};

struct MG_Map
{
  MG_MapSlot *slots;
  ulong slots_count;
};

////////////////////////////////
//~ rjf: String Expression Types

enum MG_StrExprOpKind
{
  MG_StrExprOpKind_Null,
  MG_StrExprOpKind_Prefix,
  MG_StrExprOpKind_Postfix,
  MG_StrExprOpKind_Binary,
  MG_StrExprOpKind_COUNT
};

enum MG_StrExprOp
{
  MG_StrExprOp_Null,
  
#define MG_StrExprOp_FirstString MG_StrExprOp_Dot
  MG_StrExprOp_Dot,
  MG_StrExprOp_ExpandIfTrue,
  MG_StrExprOp_Concat,
  MG_StrExprOp_BumpToColumn,
#define MG_StrExprOp_LastString MG_StrExprOp_BumpToColumn
  
#define MG_StrExprOp_FirstNumeric MG_StrExprOp_Add
  MG_StrExprOp_Add,
  MG_StrExprOp_Subtract,
  MG_StrExprOp_Multiply,
  MG_StrExprOp_Divide,
  MG_StrExprOp_Modulo,
  MG_StrExprOp_LeftShift,
  MG_StrExprOp_RightShift,
  MG_StrExprOp_BitwiseAnd,
  MG_StrExprOp_BitwiseOr,
  MG_StrExprOp_BitwiseXor,
  MG_StrExprOp_BitwiseNegate,
  MG_StrExprOp_BooleanAnd,
  MG_StrExprOp_BooleanOr,
  MG_StrExprOp_BooleanNot,
  MG_StrExprOp_Equals,
  MG_StrExprOp_DoesNotEqual,
#define MG_StrExprOp_LastNumeric MG_StrExprOp_DoesNotEqual
  
  MG_StrExprOp_COUNT,
};

struct MG_StrExpr
{
  MG_StrExpr *parent;
  MG_StrExpr *left;
  MG_StrExpr *right;
  MG_StrExprOp op;
  MD_Node *node;
};

struct MG_StrExprParseResult
{
  MG_StrExpr *root;
  MD_MsgList msgs;
  MD_Node *next_node;
};

////////////////////////////////
//~ rjf: Table Generation Types

struct MG_NodeArray
{
  MD_Node **v;
  ulong count;
};

struct MG_NodeGrid
{
  ulong x_stride;
  ulong y_stride;
  MG_NodeArray cells;
  MG_NodeArray row_parents;
};

enum MG_ColumnKind
{
  MG_ColumnKind_DirectCell,
  MG_ColumnKind_CheckForTag,
  MG_ColumnKind_TagChild,
  MG_ColumnKind_COUNT
};

struct MG_ColumnDesc
{
  String8 name;
  MG_ColumnKind kind;
  String8 tag_name;
};

struct MG_ColumnDescArray
{
  ulong count;
  MG_ColumnDesc *v;
};

struct MG_TableExpandTask
{
  MG_TableExpandTask *next;
  String8 expansion_label;
  MG_NodeGrid *grid;
  MG_ColumnDescArray column_descs;
  ulong count;
  ulong idx;
};

struct MG_TableExpandInfo
{
  MG_TableExpandTask *first_expand_task;
  String8 missing_value_fallback;
};

////////////////////////////////
//~ rjf: Main Output Path Types

struct MG_Layer
{
  String8 key;
  B32 is_library;
  String8 gen_folder_name;
  String8 h_name_override;
  String8 c_name_override;
  String8List enums;
  String8List structs;
  String8List h_functions;
  String8List h_tables;
  String8List h_catchall;
  String8List h_header;
  String8List h_footer;
  String8List c_functions;
  String8List c_tables;
  String8List c_catchall;
  String8List c_header;
  String8List c_footer;
};

struct MG_LayerNode
{
  MG_LayerNode *next;
  MG_Layer v;
};

struct MG_LayerSlot
{
  MG_LayerNode *first;
  MG_LayerNode *last;
};

struct MG_State
{
  ulong slots_count;
  MG_LayerSlot *slots;
};

////////////////////////////////
//~ rjf: Globals

static Arena *mg_arena = 0;
static MG_State *mg_state = 0;
read_only static MG_StrExpr mg_str_expr_nil = {&mg_str_expr_nil, &mg_str_expr_nil, &mg_str_expr_nil};

////////////////////////////////
//~ rjf: Basic Helpers

ulong mg_hash_from_string(String8 string);
TxtPt mg_txt_pt_from_string_off(String8 string, ulong off);

////////////////////////////////
//~ rjf: Message Lists

void mg_msg_list_push(Arena *arena, MG_MsgList *msgs, MG_Msg *msg);

////////////////////////////////
//~ rjf: String Escaping

String8 mg_escaped_from_str8(Arena *arena, String8 string);

////////////////////////////////
//~ rjf: String Wrapping

String8List mg_wrapped_lines_from_string(Arena *arena, String8 string, ulong first_line_max_width, ulong max_width, ulong wrap_indent);

////////////////////////////////
//~ rjf: C-String-Izing

String8 mg_c_string_literal_from_multiline_string(String8 string);
String8 mg_c_array_literal_contents_from_data(String8 data);

////////////////////////////////
//~ rjf: Map Functions

MG_Map mg_push_map(Arena *arena, ulong slot_count);
void *mg_map_ptr_from_string(MG_Map *map, String8 string);
void mg_map_insert_ptr(Arena *arena, MG_Map *map, String8 string, void *val);

////////////////////////////////
//~ rjf: String Expression Parsing

MG_StrExpr *mg_push_str_expr(Arena *arena, MG_StrExprOp op, MD_Node *node);
MG_StrExprParseResult mg_str_expr_parse_from_first_opl__min_prec(Arena *arena, MD_Node *first, MD_Node *opl, sbyte min_prec);
MG_StrExprParseResult mg_str_expr_parse_from_first_opl(Arena *arena, MD_Node *first, MD_Node *opl);
MG_StrExprParseResult mg_str_expr_parse_from_root(Arena *arena, MD_Node *root);

////////////////////////////////
//~ rjf: Table Generation Functions

MG_NodeArray mg_node_array_make(Arena *arena, ulong count);
MG_NodeArray mg_child_array_from_node(Arena *arena, MD_Node *node);
MG_NodeGrid mg_node_grid_make_from_node(Arena *arena, MD_Node *root);
MG_NodeArray mg_row_from_index(MG_NodeGrid grid, ulong index);
MG_NodeArray mg_column_from_index(Arena *arena, MG_NodeGrid grid, ulong index);
MD_Node *mg_node_from_grid_xy(MG_NodeGrid grid, ulong x, ulong y);

MG_ColumnDescArray mg_column_desc_array_make(Arena *arena, ulong count, MG_ColumnDesc *descs);
MG_ColumnDescArray mg_column_desc_array_from_tag(Arena *arena, MD_Node *tag);
ulong mg_column_index_from_name(MG_ColumnDescArray descs, String8 name);
String8 mg_string_from_row_desc_idx(MD_Node *row_parent, MG_ColumnDescArray descs, ulong idx);

long mg_eval_table_expand_expr__numeric(MG_StrExpr *expr, MG_TableExpandInfo *info);
void mg_eval_table_expand_expr__string(Arena *arena, MG_StrExpr *expr, MG_TableExpandInfo *info, String8List *out);
void mg_loop_table_column_expansion(Arena *arena, String8 strexpr, MG_TableExpandInfo *info, MG_TableExpandTask *task, String8List *out);
String8List mg_string_list_from_table_gen(Arena *arena, MG_Map grid_name_map, MG_Map grid_column_desc_map, String8 fallback, MD_Node *gen);

////////////////////////////////
//~ rjf: Layer Lookup Functions

String8 mg_layer_key_from_path(String8 path);
MG_Layer *mg_layer_from_key(String8 key);

#endif //METAGEN_H

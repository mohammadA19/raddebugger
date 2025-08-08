// Copyright (c) Epic Games Tools
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
  U64 count;
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
  U64 count;
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
  U64 slots_count;
};

////////////////////////////////
//~ rjf: String Expression Types

typedef enum MG_StrExprOpKind
{
  MG_StrExprOpKind_Null,
  MG_StrExprOpKind_Prefix,
  MG_StrExprOpKind_Postfix,
  MG_StrExprOpKind_Binary,
  MG_StrExprOpKind_COUNT
}
MG_StrExprOpKind;

typedef enum MG_StrExprOp
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
}
MG_StrExprOp;

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
  U64 count;
};

struct MG_NodeGrid
{
  U64 x_stride;
  U64 y_stride;
  MG_NodeArray cells;
  MG_NodeArray row_parents;
};

typedef enum MG_ColumnKind
{
  MG_ColumnKind_DirectCell,
  MG_ColumnKind_CheckForTag,
  MG_ColumnKind_TagChild,
  MG_ColumnKind_COUNT
}
MG_ColumnKind;

struct MG_ColumnDesc
{
  String8 name;
  MG_ColumnKind kind;
  String8 tag_name;
};

struct MG_ColumnDescArray
{
  U64 count;
  MG_ColumnDesc *v;
};

struct MG_TableExpandTask
{
  MG_TableExpandTask *next;
  String8 expansion_label;
  MG_NodeGrid *grid;
  MG_ColumnDescArray column_descs;
  U64 count;
  U64 idx;
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
  U64 slots_count;
  MG_LayerSlot *slots;
};

////////////////////////////////
//~ rjf: Globals

global Arena *mg_arena = 0;
global MG_State *mg_state = 0;
read_only global MG_StrExpr mg_str_expr_nil = {&mg_str_expr_nil, &mg_str_expr_nil, &mg_str_expr_nil};

////////////////////////////////
//~ rjf: Basic Helpers


////////////////////////////////
//~ rjf: Message Lists


////////////////////////////////
//~ rjf: String Escaping


////////////////////////////////
//~ rjf: String Wrapping


////////////////////////////////
//~ rjf: C-String-Izing


////////////////////////////////
//~ rjf: Map Functions


////////////////////////////////
//~ rjf: String Expression Parsing


////////////////////////////////
//~ rjf: Table Generation Functions




////////////////////////////////
//~ rjf: Layer Lookup Functions


#endif //METAGEN_H

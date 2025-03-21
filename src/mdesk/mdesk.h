// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MDESK_H
#define MDESK_H

////////////////////////////////
//~ rjf: Messages

enum MD_MsgKind
{
  MD_MsgKind_Null,
  MD_MsgKind_Note,
  MD_MsgKind_Warning,
  MD_MsgKind_Error,
  MD_MsgKind_FatalError,
};

struct MD_Msg
{
  MD_Msg *next;
  struct MD_Node *node;
  MD_MsgKind kind;
  String8 string;
};

struct MD_MsgList
{
  MD_Msg *first;
  MD_Msg *last;
  ulong count;
  MD_MsgKind worst_message_kind;
};

////////////////////////////////
//~ rjf: Token Types

enum MD_TokenFlags : uint
{
  // rjf: base kind info
  MD_TokenFlag_Identifier          = (1<<0),
  MD_TokenFlag_Numeric             = (1<<1),
  MD_TokenFlag_StringLiteral       = (1<<2),
  MD_TokenFlag_Symbol              = (1<<3),
  MD_TokenFlag_Reserved            = (1<<4),
  MD_TokenFlag_Comment             = (1<<5),
  MD_TokenFlag_Whitespace          = (1<<6),
  MD_TokenFlag_Newline             = (1<<7),
  
  // rjf: decoration info
  MD_TokenFlag_StringSingleQuote   = (1<<8),
  MD_TokenFlag_StringDoubleQuote   = (1<<9),
  MD_TokenFlag_StringTick          = (1<<10),
  MD_TokenFlag_StringTriplet       = (1<<11),
  
  // rjf: error info
  MD_TokenFlag_BrokenComment       = (1<<12),
  MD_TokenFlag_BrokenStringLiteral = (1<<13),
  MD_TokenFlag_BadCharacter        = (1<<14),
};

enum MD_TokenGroups : uint
{
  MD_TokenGroup_Comment    = MD_TokenFlag_Comment,
  MD_TokenGroup_Whitespace = (MD_TokenFlag_Whitespace|
                              MD_TokenFlag_Newline),
  MD_TokenGroup_Irregular  = (MD_TokenGroup_Comment|
                              MD_TokenGroup_Whitespace),
  MD_TokenGroup_Regular    = ~MD_TokenGroup_Irregular,
  MD_TokenGroup_Label      = (MD_TokenFlag_Identifier|
                              MD_TokenFlag_Numeric|
                              MD_TokenFlag_StringLiteral|
                              MD_TokenFlag_Symbol),
  MD_TokenGroup_Error      = (MD_TokenFlag_BrokenComment|
                              MD_TokenFlag_BrokenStringLiteral|
                              MD_TokenFlag_BadCharacter),
};

struct MD_Token
{
  Rng1U64 range;
  MD_TokenFlags flags;
};

struct MD_TokenChunkNode
{
  MD_TokenChunkNode *next;
  MD_Token *v;
  ulong count;
  ulong cap;
};

struct MD_TokenChunkList
{
  MD_TokenChunkNode *first;
  MD_TokenChunkNode *last;
  ulong chunk_count;
  ulong total_token_count;
};

struct MD_TokenArray
{
  MD_Token *v;
  ulong count;
};

////////////////////////////////
//~ rjf: Node Types

enum MD_NodeKind
{
  MD_NodeKind_Nil,
  MD_NodeKind_File,
  MD_NodeKind_ErrorMarker,
  MD_NodeKind_Main,
  MD_NodeKind_Tag,
  MD_NodeKind_List,
  MD_NodeKind_Reference,
  MD_NodeKind_COUNT
};

enum MD_NodeFlags : uint
{
  MD_NodeFlag_MaskSetDelimiters          = (0x3F<<0),
  MD_NodeFlag_HasParenLeft               = (1<<0),
  MD_NodeFlag_HasParenRight              = (1<<1),
  MD_NodeFlag_HasBracketLeft             = (1<<2),
  MD_NodeFlag_HasBracketRight            = (1<<3),
  MD_NodeFlag_HasBraceLeft               = (1<<4),
  MD_NodeFlag_HasBraceRight              = (1<<5),
  
  MD_NodeFlag_MaskSeparators             = (0xF<<6),
  MD_NodeFlag_IsBeforeSemicolon          = (1<<6),
  MD_NodeFlag_IsAfterSemicolon           = (1<<7),
  MD_NodeFlag_IsBeforeComma              = (1<<8),
  MD_NodeFlag_IsAfterComma               = (1<<9),
  
  MD_NodeFlag_MaskStringDelimiters       = (0xF<<10),
  MD_NodeFlag_StringSingleQuote          = (1<<10),
  MD_NodeFlag_StringDoubleQuote          = (1<<11),
  MD_NodeFlag_StringTick                 = (1<<12),
  MD_NodeFlag_StringTriplet              = (1<<13),
  
  MD_NodeFlag_MaskLabelKind              = (0xF<<14),
  MD_NodeFlag_Numeric                    = (1<<14),
  MD_NodeFlag_Identifier                 = (1<<15),
  MD_NodeFlag_StringLiteral              = (1<<16),
  MD_NodeFlag_Symbol                     = (1<<17),
};
#define MD_NodeFlag_AfterFromBefore(f) ((f) << 1)

struct MD_Node
{
  // rjf: tree links
  MD_Node *next;
  MD_Node *prev;
  MD_Node *parent;
  MD_Node *first;
  MD_Node *last;
  
  // rjf: tag links
  MD_Node *first_tag;
  MD_Node *last_tag;
  
  // rjf: node info
  MD_NodeKind kind;
  MD_NodeFlags flags;
  String8 string;
  String8 raw_string;
  
  // rjf: source code info
  ulong src_offset;
  
  // rjf: user-controlled generation number
  //
  // (unused by mdesk layer, but can be used by usage code to use MD_Node trees
  // in a "retained mode" way, where stable generational handles can be formed
  // to nodes)
  ulong user_gen;
  
  // rjf: extra padding to 128 bytes
  ulong _unused_[2];
};

struct MD_NodeRec
{
  MD_Node *next;
  int push_count;
  int pop_count;
};

////////////////////////////////
//~ rjf: Text -> Tokens Types

struct MD_TokenizeResult
{
  MD_TokenArray tokens;
  MD_MsgList msgs;
};

////////////////////////////////
//~ rjf: Tokens -> Tree Types

struct MD_ParseResult
{
  MD_Node *root;
  MD_MsgList msgs;
};

////////////////////////////////
//~ rjf: Globals

static read_only MD_Node md_nil_node =
{
  &md_nil_node,
  &md_nil_node,
  &md_nil_node,
  &md_nil_node,
  &md_nil_node,
  &md_nil_node,
  &md_nil_node,
};

////////////////////////////////
//~ rjf: Message Type Functions

void md_msg_list_push(Arena *arena, MD_MsgList *msgs, MD_Node *node, MD_MsgKind kind, String8 string);
void md_msg_list_pushf(Arena *arena, MD_MsgList *msgs, MD_Node *node, MD_MsgKind kind, char *fmt, ...);
void md_msg_list_concat_in_place(MD_MsgList *dst, MD_MsgList *to_push);

////////////////////////////////
//~ rjf: Token Type Functions

MD_Token md_token_make(Rng1U64 range, MD_TokenFlags flags);
B32 md_token_match(MD_Token a, MD_Token b);
String8List md_string_list_from_token_flags(Arena *arena, MD_TokenFlags flags);
void md_token_chunk_list_push(Arena *arena, MD_TokenChunkList *list, ulong cap, MD_Token token);
MD_TokenArray md_token_array_from_chunk_list(Arena *arena, MD_TokenChunkList *chunks);
String8 md_content_string_from_token_flags_str8(MD_TokenFlags flags, String8 string);

////////////////////////////////
//~ rjf: Node Type Functions

//- rjf: flag conversions
MD_NodeFlags md_node_flags_from_token_flags(MD_TokenFlags flags);

//- rjf: nil
B32 md_node_is_nil(MD_Node *node);

//- rjf: iteration
#define MD_EachNode(it, first) (MD_Node *it = first; !md_node_is_nil(it); it = it->next)
MD_NodeRec md_node_rec_depth_first(MD_Node *node, MD_Node *subtree_root, ulong child_off, ulong sib_off);
#define md_node_rec_depth_first_pre(node, subtree_root) md_node_rec_depth_first((node), (subtree_root), OffsetOf(MD_Node, first), OffsetOf(MD_Node, next))
#define md_node_rec_depth_first_pre_rev(node, subtree_root) md_node_rec_depth_first((node), (subtree_root), OffsetOf(MD_Node, last), OffsetOf(MD_Node, prev))

//- rjf: tree building
MD_Node *md_push_node(Arena *arena, MD_NodeKind kind, MD_NodeFlags flags, String8 string, String8 raw_string, ulong src_offset);
void md_node_insert_child(MD_Node *parent, MD_Node *prev_child, MD_Node *node);
void md_node_insert_tag(MD_Node *parent, MD_Node *prev_child, MD_Node *node);
void md_node_push_child(MD_Node *parent, MD_Node *node);
void md_node_push_tag(MD_Node *parent, MD_Node *node);
void md_unhook(MD_Node *node);

//- rjf: tree introspection
MD_Node *  md_node_from_chain_string(MD_Node *first, MD_Node *opl, String8 string, StringMatchFlags flags);
MD_Node *  md_node_from_chain_index(MD_Node *first, MD_Node *opl, ulong index);
MD_Node *  md_node_from_chain_flags(MD_Node *first, MD_Node *opl, MD_NodeFlags flags);
ulong        md_index_from_node(MD_Node *node);
MD_Node *  md_root_from_node(MD_Node *node);
MD_Node *  md_child_from_string(MD_Node *node, String8 child_string, StringMatchFlags flags);
MD_Node *  md_tag_from_string(MD_Node *node, String8 tag_string, StringMatchFlags flags);
MD_Node *  md_child_from_index(MD_Node *node, ulong index);
MD_Node *  md_tag_from_index(MD_Node *node, ulong index);
MD_Node *  md_tag_arg_from_index(MD_Node *node, String8 tag_string, StringMatchFlags flags, ulong index);
MD_Node *  md_tag_arg_from_string(MD_Node *node, String8 tag_string, StringMatchFlags tag_str_flags, String8 arg_string, StringMatchFlags arg_str_flags);
B32        md_node_has_child(MD_Node *node, String8 string, StringMatchFlags flags);
B32        md_node_has_tag(MD_Node *node, String8 string, StringMatchFlags flags);
ulong        md_child_count_from_node(MD_Node *node);
ulong        md_tag_count_from_node(MD_Node *node);
String8    md_string_from_children(Arena *arena, MD_Node *root);

//- rjf: tree comparison
B32 md_tree_match(MD_Node *a, MD_Node *b, StringMatchFlags flags);
B32 md_node_match(MD_Node *a, MD_Node *b, StringMatchFlags flags);

//- rjf: tree duplication
MD_Node *md_tree_copy(Arena *arena, MD_Node *src_root);

////////////////////////////////
//~ rjf: Text -> Tokens Functions

MD_TokenizeResult md_tokenize_from_text(Arena *arena, String8 text);

////////////////////////////////
//~ rjf: Tokens -> Tree Functions

MD_ParseResult md_parse_from_text_tokens(Arena *arena, String8 filename, String8 text, MD_TokenArray tokens);

////////////////////////////////
//~ rjf: Bundled Text -> Tree Functions

MD_ParseResult md_parse_from_text(Arena *arena, String8 filename, String8 text);
#define md_tree_from_string(arena, string) (md_parse_from_text((arena), str8_zero(), (string)).root)

////////////////////////////////
//~ rjf: Tree -> Text Functions

String8List md_debug_string_list_from_tree(Arena *arena, MD_Node *root);

#endif // MDESK_H

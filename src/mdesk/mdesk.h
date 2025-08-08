// Copyright (c) Epic Games Tools
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
}
MD_MsgKind;

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
    U64 count;
    MD_MsgKind worst_message_kind;
};

////////////////////////////////
//~ rjf: Token Types

enum MD_TokenFlags : U32
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

enum MD_TokenGroups : U32
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
    U64 count;
    U64 cap;
};

struct MD_TokenChunkList
{
    MD_TokenChunkNode *first;
    MD_TokenChunkNode *last;
    U64 chunk_count;
    U64 total_token_count;
};

struct MD_TokenArray
{
    MD_Token *v;
    U64 count;
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
}
MD_NodeKind;

enum MD_NodeFlags : U32
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
    U64 src_offset;
    
    // rjf: user-controlled generation number
    //
    // (unused by mdesk layer, but can be used by usage code to use MD_Node trees
    // in a "retained mode" way, where stable generational handles can be formed
    // to nodes)
    U64 user_gen;
    
    // rjf: extra padding to 128 bytes
    U64 _unused_[2];
};

struct MD_NodeRec
{
    MD_Node *next;
    S32 push_count;
    S32 pop_count;
};

struct MD_NodePtrNode
{
    MD_NodePtrNode *next;
    MD_Node *v;
};

struct MD_NodePtrList
{
    MD_NodePtrNode *first;
    MD_NodePtrNode *last;
    U64 count;
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

global read_only MD_Node md_nil_node =
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


////////////////////////////////
//~ rjf: Token Type Functions


////////////////////////////////
//~ rjf: Node Type Functions

//- rjf: flag conversions

//- rjf: nil

//- rjf: iteration
#define MD_EachNode(it, first) (MD_Node *it = first; !md_node_is_nil(it); it = it->next)
#define md_node_rec_depth_first_pre(node, subtree_root) md_node_rec_depth_first((node), (subtree_root), OffsetOf(MD_Node, first), OffsetOf(MD_Node, next))
#define md_node_rec_depth_first_pre_rev(node, subtree_root) md_node_rec_depth_first((node), (subtree_root), OffsetOf(MD_Node, last), OffsetOf(MD_Node, prev))

//- rjf: tree building

//- rjf: tree introspection

//- rjf: tree comparison

//- rjf: tree duplication

////////////////////////////////
//~ rjf: Text -> Tokens Functions


////////////////////////////////
//~ rjf: Tokens -> Tree Functions


////////////////////////////////
//~ rjf: Bundled Text -> Tree Functions

#define md_tree_from_string(arena, string) (md_parse_from_text((arena), str8_zero(), (string)).root)

////////////////////////////////
//~ rjf: Tree -> Text Functions


////////////////////////////////
//~ rjf: Node Pointer List Functions


#endif // MDESK_H

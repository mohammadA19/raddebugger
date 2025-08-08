// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef TEXT_CACHE_H
#define TEXT_CACHE_H

////////////////////////////////
//~ rjf: Value Types

enum TXT_LineEndKind
{
    TXT_LineEndKind_Null,
    TXT_LineEndKind_LF,
    TXT_LineEndKind_CRLF,
    TXT_LineEndKind_COUNT
}
TXT_LineEndKind;

enum TXT_TokenKind
{
    TXT_TokenKind_Null,
    TXT_TokenKind_Error,
    TXT_TokenKind_Whitespace,
    TXT_TokenKind_Keyword,
    TXT_TokenKind_Identifier,
    TXT_TokenKind_Numeric,
    TXT_TokenKind_String,
    TXT_TokenKind_Symbol,
    TXT_TokenKind_Comment,
    TXT_TokenKind_Meta, // preprocessor, etc.
    TXT_TokenKind_COUNT
}
TXT_TokenKind;

struct TXT_Token
{
    TXT_TokenKind kind;
    Rng1U64 range;
};

struct TXT_TokenChunkNode
{
    TXT_TokenChunkNode *next;
    U64 count;
    U64 cap;
    TXT_Token *v;
};

struct TXT_TokenChunkList
{
    TXT_TokenChunkNode *first;
    TXT_TokenChunkNode *last;
    U64 chunk_count;
    U64 token_count;
};

struct TXT_TokenNode
{
    TXT_TokenNode *next;
    TXT_Token v;
};

struct TXT_TokenList
{
    TXT_TokenNode *first;
    TXT_TokenNode *last;
    U64 count;
};

struct TXT_TokenArray
{
    U64 count;
    TXT_Token *v;
};

struct TXT_TokenArrayArray
{
    U64 count;
    TXT_TokenArray *v;
};

struct TXT_ScopeNode
{
    U64 first_num;
    U64 last_num;
    U64 next_num;
    U64 parent_num;
    Rng1U64 token_idx_range;
};

struct TXT_ScopeNodeArray
{
    TXT_ScopeNode *v;
    U64 count;
};

struct TXT_ScopePt
{
    U64 token_idx;
    U64 scope_idx;
};

struct TXT_ScopePtArray
{
    TXT_ScopePt *v;
    U64 count;
};

struct TXT_TextInfo
{
    U64 lines_count;
    Rng1U64 *lines_ranges;
    U64 lines_max_size;
    TXT_LineEndKind line_end_kind;
    TXT_TokenArray tokens;
    TXT_ScopePtArray scope_pts;
    TXT_ScopeNodeArray scope_nodes;
    U64 bytes_processed;
    U64 bytes_to_process;
};

struct TXT_LineTokensSlice
{
    TXT_TokenArray *line_tokens;
};

////////////////////////////////
//~ rjf: Language Kind Types

enum TXT_LangKind
{
    TXT_LangKind_Null,
    TXT_LangKind_C,
    TXT_LangKind_CPlusPlus,
    TXT_LangKind_Odin,
    TXT_LangKind_Jai,
    TXT_LangKind_Zig,
    TXT_LangKind_DisasmX64Intel,
    TXT_LangKind_COUNT
}
TXT_LangKind;

typedef TXT_TokenArray TXT_LangLexFunctionType(Arena *arena, U64 *bytes_processed_counter, String8 string);

////////////////////////////////
//~ rjf: Cache Types

struct TXT_Node
{
    // rjf: links
    TXT_Node *next;
    TXT_Node *prev;
    
    // rjf: key
    U128 hash;
    TXT_LangKind lang;
    
    // rjf: artifacts
    Arena *arena;
    TXT_TextInfo info;
    
    // rjf: metadata
    B32 is_working;
    U64 scope_ref_count;
    U64 last_time_touched_us;
    U64 last_user_clock_idx_touched;
    U64 load_count;
};

struct TXT_Slot
{
    TXT_Node *first;
    TXT_Node *last;
};

struct TXT_Stripe
{
    Arena *arena;
    OS_Handle rw_mutex;
    OS_Handle cv;
};

////////////////////////////////
//~ rjf: Scoped Access

struct TXT_Touch
{
    TXT_Touch *next;
    U128 hash;
    TXT_LangKind lang;
};

struct TXT_Scope
{
    TXT_Scope *next;
    TXT_Touch *top_touch;
};

////////////////////////////////
//~ rjf: Thread Context

struct TXT_TCTX
{
    Arena *arena;
    TXT_Scope *free_scope;
    TXT_Touch *free_touch;
};

////////////////////////////////
//~ rjf: Shared State

struct TXT_Shared
{
    Arena *arena;
    
    // rjf: user clock
    U64 user_clock_idx;
    
    // rjf: cache
    U64 slots_count;
    U64 stripes_count;
    TXT_Slot *slots;
    TXT_Stripe *stripes;
    TXT_Node **stripes_free_nodes;
    
    // rjf: user -> parse thread
    U64 u2p_ring_size;
    U8 *u2p_ring_base;
    U64 u2p_ring_write_pos;
    U64 u2p_ring_read_pos;
    OS_Handle u2p_ring_cv;
    OS_Handle u2p_ring_mutex;
    
    // rjf: evictor thread
    OS_Handle evictor_thread;
};

////////////////////////////////
//~ rjf: Globals

read_only global TXT_ScopeNode txt_scope_node_nil = {0};
thread_static TXT_TCTX *txt_tctx = 0;
global TXT_Shared *txt_shared = 0;

////////////////////////////////
//~ rjf: Basic Helpers


////////////////////////////////
//~ rjf: Token Type Functions


////////////////////////////////
//~ rjf: Lexing Functions


////////////////////////////////
//~ rjf: Main Layer Initialization


////////////////////////////////
//~ rjf: Thread Context Initialization


////////////////////////////////
//~ rjf: Scoped Access


////////////////////////////////
//~ rjf: Cache Lookups


////////////////////////////////
//~ rjf: Text Info Extractor Helpers


////////////////////////////////
//~ rjf: Parse Threads

ASYNC_WORK_DEF(txt_parse_work);

////////////////////////////////
//~ rjf: Evictor Threads


#endif // TEXT_CACHE_H

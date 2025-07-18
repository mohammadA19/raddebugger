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

struct TXT_Token
{
  TXT_TokenKind kind;
  Rng1<uint64> range;
};

struct TXT_TokenChunkNode
{
  TXT_TokenChunkNode *next;
  uint64 count;
  uint64 cap;
  TXT_Token *v;
};

struct TXT_TokenChunkList
{
  TXT_TokenChunkNode *first;
  TXT_TokenChunkNode *last;
  uint64 chunk_count;
  uint64 token_count;
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
  uint64 count;
};

struct TXT_TokenArray
{
  uint64 count;
  TXT_Token *v;
};

struct TXT_TokenArrayArray
{
  uint64 count;
  TXT_TokenArray *v;
};

struct TXT_TextInfo
{
  uint64 lines_count;
  Rng1<uint64> *lines_ranges;
  uint64 lines_max_size;
  TXT_LineEndKind line_end_kind;
  TXT_TokenArray tokens;
  uint64 bytes_processed;
  uint64 bytes_to_process;
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

typedef TXT_TokenArray TXT_LangLexFunctionType(Arena *arena, uint64 *bytes_processed_counter, String8 string);

////////////////////////////////
//~ rjf: Cache Types

struct TXT_Node
{
  // rjf: links
  TXT_Node *next;
  TXT_Node *prev;
  
  // rjf: key
  uint128 hash;
  TXT_LangKind lang;
  
  // rjf: artifacts
  Arena *arena;
  TXT_TextInfo info;
  
  // rjf: metadata
  B32 is_working;
  uint64 scope_ref_count;
  uint64 last_time_touched_us;
  uint64 last_user_clock_idx_touched;
  uint64 load_count;
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
  uint128 hash;
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
  uint64 user_clock_idx;
  
  // rjf: cache
  uint64 slots_count;
  uint64 stripes_count;
  TXT_Slot *slots;
  TXT_Stripe *stripes;
  TXT_Node **stripes_free_nodes;
  
  // rjf: user -> parse thread
  uint64 u2p_ring_size;
  uint8 *u2p_ring_base;
  uint64 u2p_ring_write_pos;
  uint64 u2p_ring_read_pos;
  OS_Handle u2p_ring_cv;
  OS_Handle u2p_ring_mutex;
  
  // rjf: evictor thread
  OS_Handle evictor_thread;
};

////////////////////////////////
//~ rjf: Globals

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

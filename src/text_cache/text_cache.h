// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Value Types

public enum TXT_LineEndKind
{
  Null,
  LF,
  CRLF,
  COUNT
}

public enum TXT_TokenKind
{
  Null,
  Error,
  Whitespace,
  Keyword,
  Identifier,
  Numeric,
  String,
  Symbol,
  Comment,
  Meta, // preprocessor, etc.
  COUNT
}

public struct TXT_Token
{
  TXT_TokenKind kind;
  Rng1U64 range;
}

public struct TXT_TokenChunkNode
{
  TXT_TokenChunkNode* next;
  uint64 count;
  uint64 cap;
  TXT_Token* v;
}

public struct TXT_TokenChunkList
{
  TXT_TokenChunkNode* first;
  TXT_TokenChunkNode* last;
  uint64 chunk_count;
  uint64 token_count;
}

public struct TXT_TokenNode
{
  TXT_TokenNode* next;
  TXT_Token v;
}

public struct TXT_TokenList
{
  TXT_TokenNode* first;
  TXT_TokenNode* last;
  uint64 count;
}

public struct TXT_TokenArray
{
  uint64 count;
  TXT_Token* v;
}

public struct TXT_TokenArrayArray
{
  uint64 count;
  TXT_TokenArray* v;
}

public struct TXT_TextInfo
{
  uint64 lines_count;
  Rng1U64* lines_ranges;
  uint64 lines_max_size;
  TXT_LineEndKind line_end_kind;
  TXT_TokenArray tokens;
  uint64 bytes_processed;
  uint64 bytes_to_process;
}

public struct TXT_LineTokensSlice
{
  TXT_TokenArray* line_tokens;
}

////////////////////////////////
//~ rjf: Language Kind Types

public enum TXT_LangKind
{
  Null,
  C,
  CPlusPlus,
  Odin,
  Jai,
  Zig,
  DisasmX64Intel,
  COUNT
}

function TXT_TokenArray TXT_LangLexFunctionType(Arena* arena, uint64* bytes_processed_counter, String8 string);

////////////////////////////////
//~ rjf: Cache Types

public struct TXT_Node
{
  // rjf: links
  TXT_Node* next;
  TXT_Node* prev;
  
  // rjf: key
  U128 hash;
  TXT_LangKind lang;
  
  // rjf: artifacts
  Arena* arena;
  TXT_TextInfo info;
  
  // rjf: metadata
  B32 is_working;
  uint64 scope_ref_count;
  uint64 last_time_touched_us;
  uint64 last_user_clock_idx_touched;
  uint64 load_count;
}

public struct TXT_Slot
{
  TXT_Node* first;
  TXT_Node* last;
}

public struct TXT_Stripe
{
  Arena* arena;
  OS_Handle rw_mutex;
  OS_Handle cv;
}

////////////////////////////////
//~ rjf: Scoped Access

public struct TXT_Touch
{
  TXT_Touch* next;
  U128 hash;
  TXT_LangKind lang;
}

public struct TXT_Scope
{
  TXT_Scope* next;
  TXT_Touch* top_touch;
}

////////////////////////////////
//~ rjf: Thread Context

public struct TXT_TCTX
{
  Arena* arena;
  TXT_Scope* free_scope;
  TXT_Touch* free_touch;
}

////////////////////////////////
//~ rjf: Shared State

public struct TXT_Shared
{
  Arena* arena;
  
  // rjf: user clock
  uint64 user_clock_idx;
  
  // rjf: cache
  uint64 slots_count;
  uint64 stripes_count;
  TXT_Slot* slots;
  TXT_Stripe* stripes;
  TXT_Node** stripes_free_nodes;
  
  // rjf: user -> parse thread
  uint64 u2p_ring_size;
  uint8* u2p_ring_base;
  uint64 u2p_ring_write_pos;
  uint64 u2p_ring_read_pos;
  OS_Handle u2p_ring_cv;
  OS_Handle u2p_ring_mutex;
  
  // rjf: evictor thread
  OS_Handle evictor_thread;
}

////////////////////////////////
//~ rjf: Globals

[ThreadStatic]
public static TXT_TCTX* txt_tctx = null;

public static TXT_Shared* txt_shared = null;

////////////////////////////////
//~ rjf: Parse Threads

ASYNC_WORK_DEF(txt_parse_work);

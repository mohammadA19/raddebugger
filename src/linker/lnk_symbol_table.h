// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

typedef enum
{
  LNK_SymbolScopeIndex_Defined,
  LNK_SymbolScopeIndex_Internal, // symbols defined by linker
  LNK_SymbolScopeIndex_Weak,
  LNK_SymbolScopeIndex_Lib,
  LNK_SymbolScopeIndex_Count
} LNK_SymbolScopeIndex;

enum
{
  LNK_SymbolScopeFlag_Defined  = 1,
  LNK_SymbolScopeFlag_Internal = 2,
  LNK_SymbolScopeFlag_Weak     = 4,
  LNK_SymbolScopeFlag_Lib      = 8,
  
  LNK_SymbolScopeFlag_Main = LNK_SymbolScopeFlag_Defined | LNK_SymbolScopeFlag_Weak,
  LNK_SymbolScopeFlag_All  = LNK_SymbolScopeFlag_Defined | LNK_SymbolScopeFlag_Weak | LNK_SymbolScopeFlag_Lib | LNK_SymbolScopeFlag_Internal
};
typedef U64 LNK_SymbolScopeFlags;

typedef enum
{
  LNK_DefinedSymbolVisibility_Static,
  LNK_DefinedSymbolVisibility_Extern,
  LNK_DefinedSymbolVisibility_Internal,
} LNK_DefinedSymbolVisibility;

enum
{
  LNK_DefinedSymbolFlag_IsFunc  = (1 << 0),
  LNK_DefinedSymbolFlag_IsThunk = (1 << 1),
};
typedef U64 LNK_DefinedSymbolFlags;

typedef enum
{
  LNK_DefinedSymbolValue_Null,
  LNK_DefinedSymbolValue_Chunk,
  LNK_DefinedSymbolValue_VA
} LNK_DefinedSymbolValueType;

typedef struct LNK_DefinedSymbol
{
  LNK_DefinedSymbolFlags     flags;
  LNK_DefinedSymbolValueType value_type;
  union {
    struct {
      LNK_Chunk            *chunk;
      U64                   chunk_offset;
      U32                   check_sum;
      COFF_ComdatSelectType selection;
    };
    U64 va;
  } u;
} LNK_DefinedSymbol;

typedef struct LNK_WeakSymbol
{
  LNK_SymbolScopeFlags scope_flags;
  COFF_WeakExtType     lookup_type;
  struct LNK_Symbol   *fallback_symbol;
} LNK_WeakSymbol;

typedef struct LNK_UndefinedSymbol
{
  LNK_SymbolScopeFlags scope_flags;
} LNK_UndefinedSymbol;

typedef struct LNK_LazySymbol
{
  struct LNK_Lib *lib;
  U64             member_offset;
} LNK_LazySymbol;

#define LNK_Symbol_IsDefined(type) ((type) == LNK_Symbol_DefinedStatic || (type) == LNK_Symbol_DefinedExtern || (type) == LNK_Symbol_DefinedInternal)
typedef enum 
{
  LNK_Symbol_Null,
  LNK_Symbol_DefinedStatic,
  LNK_Symbol_DefinedExtern,
  LNK_Symbol_DefinedInternal,
  LNK_Symbol_Weak,
  LNK_Symbol_Lazy,
  LNK_Symbol_Undefined,
} LNK_SymbolType;

typedef struct LNK_Symbol
{
  String8         name;
  LNK_SymbolType  type;
  struct LNK_Obj *obj;
  union {
    LNK_DefinedSymbol   defined;
    LNK_WeakSymbol      weak;
    LNK_UndefinedSymbol undefined;
    LNK_LazySymbol      lazy;
  } u;
} LNK_Symbol;

typedef struct LNK_SymbolNode
{
  struct LNK_SymbolNode *next;
  LNK_Symbol            *data;
} LNK_SymbolNode;

typedef struct LNK_SymbolList
{
  U64             count;
  LNK_SymbolNode *first;
  LNK_SymbolNode *last;
} LNK_SymbolList;

typedef struct LNK_SymbolNodeArray
{
  U64              count;
  LNK_SymbolNode **v;
} LNK_SymbolNodeArray;

typedef struct LNK_SymbolArray
{
  U64         count;
  LNK_Symbol *v;
} LNK_SymbolArray;

typedef struct LNK_SymbolHashTrie
{
  String8                   *name;
  LNK_Symbol                *symbol;
  struct LNK_SymbolHashTrie *child[4];
} LNK_SymbolHashTrie;

typedef struct LNK_SymbolHashTrieChunk
{
  struct LNK_SymbolHashTrieChunk *next;
  U64                             count;
  U64                             cap;
  LNK_SymbolHashTrie             *v;
} LNK_SymbolHashTrieChunk;

typedef struct LNK_SymbolHashTrieChunkList
{
  U64                      count;
  LNK_SymbolHashTrieChunk *first;
  LNK_SymbolHashTrieChunk *last;
} LNK_SymbolHashTrieChunkList;

typedef struct LNK_SymbolTable
{
  TP_Arena                    *arena;
  LNK_SymbolHashTrie          *scopes[LNK_SymbolScopeIndex_Count];
  LNK_SymbolHashTrieChunkList *chunk_lists[LNK_SymbolScopeIndex_Count];
} LNK_SymbolTable;

////////////////////////////////
// parallel for wrappers

typedef struct
{
  LNK_SymbolTable *symtab;
  Rng1U64         *ranges;
  LNK_Symbol      *arr;
} LNK_LazySymbolInserter;

////////////////////////////////

global read_only LNK_Symbol   g_null_symbol     = { str8_lit_comp("NULL"), LNK_Symbol_DefinedStatic };
global read_only LNK_Symbol  *g_null_symbol_ptr = &g_null_symbol;

////////////////////////////////




////////////////////////////////


////////////////////////////////


////////////////////////////////





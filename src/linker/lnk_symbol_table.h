// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

// --- Symbol ------------------------------------------------------------------

enum
{
    LNK_SymbolScope_Defined,
    LNK_SymbolScope_Lib,
    LNK_SymbolScope_Count
} LNK_SymbolScope;

typedef struct LNK_SymbolDefined
{
    struct LNK_Obj *obj;
    U32             symbol_idx;
} LNK_SymbolDefined;

typedef struct LNK_SymbolLib
{
    struct LNK_Lib *lib;
    U64             member_offset;
} LNK_SymbolLib;

typedef struct LNK_SymbolUndefined
{
    struct LNK_Obj *obj;
} LNK_SymbolUndefined;

typedef struct LNK_Symbol
{
    String8 name;
    union {
        LNK_SymbolDefined   defined;
        LNK_SymbolLib       lib;
        LNK_SymbolUndefined undef;
    } u;
} LNK_Symbol;

// --- Symbol Containers -------------------------------------------------------

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

// --- Symbol Hash Trie --------------------------------------------------------

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

// --- Symbol Table ------------------------------------------------------------

typedef struct LNK_SymbolTable
{
    TP_Arena                    *arena;
    LNK_SymbolHashTrie          *scopes[LNK_SymbolScope_Count];
    LNK_SymbolHashTrieChunkList *chunk_lists[LNK_SymbolScope_Count];
} LNK_SymbolTable;

// --- Workers Contensts -------------------------------------------------------

typedef struct
{
    LNK_SymbolTable          *symtab;
    LNK_SymbolHashTrieChunk **chunks;
    LNK_SymbolList           *anti_dependency_symbols;
} LNK_FinalizeWeakSymbolsTask;

// --- Symbol Make -------------------------------------------------------------


// --- Symbol Containers ------------------------------------------------------


// --- Symbol Hash Trie --------------------------------------------------------


// --- Symbol Table ------------------------------------------------------------




// --- Symbol Contrib Helpers --------------------------------------------------




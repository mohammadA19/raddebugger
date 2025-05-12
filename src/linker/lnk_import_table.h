// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#define LNK_IMPORT_DLL_HASH_TABLE_BUCKET_COUNT  512
#define LNK_IMPORT_FUNC_HASH_TABLE_BUCKET_COUNT 2048

typedef struct LNK_ImportFunc
{
  LNK_ImportFunc* next;
  String8                name;
  String8                thunk_symbol_name;
  String8                iat_symbol_name;
} LNK_ImportFunc;

typedef struct LNK_ImportDLL
{
  LNK_ImportDLL*  next;
  LNK_ImportFunc* first_func;
  LNK_ImportFunc* last_func;
  LNK_Chunk*             dll_chunk;
  LNK_Chunk*             int_table_chunk;
  LNK_Chunk*             ilt_table_chunk;
  LNK_Chunk*             iat_table_chunk;
  LNK_Chunk*             biat_table_chunk;
  LNK_Chunk*             uiat_table_chunk;
  LNK_Chunk*             code_table_chunk;
  LNK_Symbol*            tail_merge_symbol;
  String8                name;
  COFF_MachineType       machine;
  HashTable*             func_ht;
} LNK_ImportDLL;

enum
{
  LNK_ImportTableFlag_EmitBiat = (1 << 0),
  LNK_ImportTableFlag_EmitUiat = (1 << 1),
}
typedef U32 LNK_ImportTableFlags;

typedef struct LNK_ImportTable
{
  Arena*                arena;
  COFF_MachineType      machine;
  LNK_ImportDLL*        first_dll;
  LNK_ImportDLL*        last_dll;
  LNK_Section*          data_sect;
  LNK_Section*          code_sect;
  LNK_Chunk*            dll_table_chunk;
  LNK_Chunk*            int_chunk;
  LNK_Chunk*            handle_table_chunk;
  LNK_Chunk*            iat_chunk;
  LNK_Chunk*            ilt_chunk;
  LNK_Chunk*            biat_chunk;
  LNK_Chunk*            uiat_chunk;
  LNK_Chunk*            code_chunk;
  LNK_ImportTableFlags  flags;
  HashTable*            dll_ht;
} LNK_ImportTable;






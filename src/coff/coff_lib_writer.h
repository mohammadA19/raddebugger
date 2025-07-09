// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




typedef struct COFF_LibWriterMember
{
  String8 name;
  String8 data;
} COFF_LibWriterMember;

typedef struct COFF_LibWriterMemberNode
{
  COFF_LibWriterMember             data;
  struct COFF_LibWriterMemberNode *next;
} COFF_LibWriterMemberNode;

typedef struct COFF_LibWriterMemberList
{
  U64                       count;
  COFF_LibWriterMemberNode *first;
  COFF_LibWriterMemberNode *last;
} COFF_LibWriterMemberList;

typedef struct COFF_LibWriterSymbol
{
  String8 name;
  U64     member_idx;
} COFF_LibWriterSymbol;

typedef struct COFF_LibWriterSymbolNode
{
  COFF_LibWriterSymbol             data;
  struct COFF_LibWriterSymbolNode *next;
} COFF_LibWriterSymbolNode;

typedef struct COFF_LibWriterSymbolList
{
  U64                       count;
  COFF_LibWriterSymbolNode *first;
  COFF_LibWriterSymbolNode *last;
} COFF_LibWriterSymbolList;

typedef struct COFF_LibWriter
{
  Arena                   *arena;
  COFF_LibWriterMemberList member_list;
  COFF_LibWriterSymbolList symbol_list;
} COFF_LibWriter;

////////////////////////////////




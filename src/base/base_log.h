// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Log Types

enum LogMsgKind
{
  LogMsgKind_Info,
  LogMsgKind_UserError,
  LogMsgKind_COUNT
}

struct LogScope
{
  LogScope* next;
  U64 pos;
  String8List[LogMsgKind_COUNT] strings;
}

struct LogScopeResult
{
  String8[LogMsgKind_COUNT] strings;
}

struct Log
{
  Arena* arena;
  LogScope* top_scope;
}

////////////////////////////////
//~ rjf: Log Building

#define log_info(s)               log_msg(LogMsgKind_Info, (s))
#define log_infof(fmt, ...)       log_msgf(LogMsgKind_Info, (fmt), __VA_ARGS__)
#define log_user_error(s)         log_msg(LogMsgKind_UserError, (s))
#define log_user_errorf(fmt, ...) log_msgf(LogMsgKind_UserError, (fmt), __VA_ARGS__)

#define LogInfoNamedBlock(s) DeferLoop(log_infof("%S:\n{\n", (s)), log_infof("}\n"))
#define LogInfoNamedBlockF(fmt, ...) DeferLoop((log_infof(fmt, __VA_ARGS__), log_infof(":\n{\n")), log_infof("}\n"))

////////////////////////////////
//~ rjf: Log Scopes



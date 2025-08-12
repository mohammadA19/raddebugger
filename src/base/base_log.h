// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)


////////////////////////////////
//~ rjf: Log Types

typedef enum LogMsgKind
{
  LogMsgKind_Info,
  LogMsgKind_UserError,
  LogMsgKind_COUNT
}
LogMsgKind;

typedef struct LogScope LogScope;
struct LogScope
{
  LogScope *next;
  U64 pos;
  String8List strings[LogMsgKind_COUNT];
};

typedef struct LogScopeResult LogScopeResult;
struct LogScopeResult
{
  String8 strings[LogMsgKind_COUNT];
};

typedef struct Log Log;
struct Log
{
  Arena *arena;
  LogScope *top_scope;
};

////////////////////////////////
//~ rjf: Log Creation/Selection


////////////////////////////////
//~ rjf: Log Building

#define log_info(s)          log_msg(LogMsgKind_Info, (s))
#define log_infof(...)       log_msgf(LogMsgKind_Info, __VA_ARGS__)
#define log_user_error(s)    log_msg(LogMsgKind_UserError, (s))
#define log_user_errorf(...) log_msgf(LogMsgKind_UserError, __VA_ARGS__)

#define LogInfoNamedBlock(s) DeferLoop(log_infof("%S:\n{\n", (s)), log_infof("}\n"))
#define LogInfoNamedBlockF(...) DeferLoop((log_infof(__VA_ARGS__), log_infof(":\n{\n")), log_infof("}\n"))

////////////////////////////////
//~ rjf: Log Scopes



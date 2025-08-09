// Copyright (c) 2025 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
// Build Options

#define BUILD_CONSOLE_INTERFACE 1
#define BUILD_TITLE "TORTURE"

////////////////////////////////

#include "third_party/xxHash/xxhash.c"
#include "third_party/xxHash/xxhash.h"
#include "third_party/radsort/radsort.h"

////////////////////////////////

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "coff/coff.h"
#include "coff/coff_parse.h"
#include "coff/coff_obj_writer.h"
#include "coff/coff_lib_writer.h"
#include "pe/pe.h"
#include "pe/pe_section_flags.h"
#include "linker/hash_table.h"

#include "base/base_inc.c"
#include "os/os_inc.c"
#include "coff/coff.c"
#include "coff/coff_parse.c"
#include "coff/coff_obj_writer.c"
#include "coff/coff_lib_writer.c"
#include "pe/pe.c"
#include "linker/hash_table.c"

#include "linker/lnk_cmd_line.h"
#include "linker/lnk_cmd_line.c"
#include "linker/lnk_error.h"

////////////////////////////////

typedef enum
{
  T_Result_Fail,
  T_Result_Crash,
  T_Result_Pass,
} T_Result;

typedef T_Result (*T_Run)(void);

internal char *
t_string_from_result(T_Result v)
{
  switch (v) {
  case T_Result_Fail:  return "FAIL";
  case T_Result_Crash: return "CRASH";
  case T_Result_Pass:  return "PASS";
  }
  return 0;
}

global string g_stdout_file_name = ("torture.out");
global U64     g_linker_time_out;
global string g_linker;
global string g_wdir;
global string g_out = ("torture");
global B32     g_verbose;
global B32     g_redirect_stdout = 1;

#define T_LINKER_TIME_OUT_EXIT_CODE 999999

typedef enum
{
  T_Linker_Null,
  T_Linker_RAD,
  T_Linker_MSVC,
  T_Linker_LLVM
} T_Linker;

internal T_Linker
t_ident_linker(void)
{
  string name = g_linker;
  name = str8_skip_last_slash(name);
  name = str8_chop_last_dot(name);
  if (str8_match(name, ("radlink"), StringMatchFlag_CaseInsensitive)) {
    return T_Linker_RAD;
  }
  if (str8_match(name, ("link"), StringMatchFlag_CaseInsensitive)) {
    return T_Linker_MSVC;
  }
  if (str8_match(name, ("lld-link"), StringMatchFlag_CaseInsensitive)) {
    return T_Linker_LLVM;
  }
  return T_Linker_Null;
}

internal int
t_invoke_linker_with_time_out(U64 time_out, string cmdline)
{
  Temp scratch = scratch_begin(0,0);

  OS_Handle output_redirect = {0};
  if (g_redirect_stdout) {
    output_redirect = os_file_open(OS_AccessFlag_Append|OS_AccessFlag_ShareRead|OS_AccessFlag_ShareWrite|OS_AccessFlag_Inherited, g_stdout_file_name);
  }

  //
  // Build Launch Options
  //
  OS_ProcessLaunchParams launch_opts = {0};
  launch_opts.path                   = g_wdir;
  launch_opts.inherit_env            = 1;
  launch_opts.stdout_file            = output_redirect;
  launch_opts.stderr_file            = output_redirect;
  str8_list_push(scratch.arena, &launch_opts.cmd_line, g_linker);
  str8_list_push(scratch.arena, &launch_opts.cmd_line, ("/nologo"));
  {
    String8List parsed_cmdline = lnk_arg_list_parse_windows_rules(scratch.arena, cmdline);
    str8_list_concat_in_place(&launch_opts.cmd_line, &parsed_cmdline);
  }

  //
  // Invoke Linker
  //
  int exit_code = -1;
  {
    if (g_verbose) {
      string full_cmd_line = str8_list_join(scratch.arena, &launch_opts.cmd_line, &(StringJoin){ .sep = (" ") });
      fprintf(stdout, "Command Line: %.*s\n", str8_varg(full_cmd_line));
      fprintf(stdout, "Working Dir:  %.*s\n", str8_varg(g_wdir));
    }

    OS_Handle linker_handle = os_process_launch(&launch_opts);
    if (os_handle_match(linker_handle, os_handle_zero())) {
      fprintf(stderr, "unable to start process: %.*s\n", str8_varg(g_linker));
    } else {
      B32 was_joined = os_process_join_exit_code(linker_handle, time_out, &exit_code);
      if (!was_joined) {
        os_process_kill(linker_handle);
        exit_code = T_LINKER_TIME_OUT_EXIT_CODE;
      }
      os_process_detach(linker_handle);
    }
  }

  if (g_redirect_stdout) {
    os_file_close(output_redirect);
  }

  scratch_end(scratch);
  return exit_code;
}

internal int
t_invoke_linker(string cmdline)
{
  return t_invoke_linker_with_time_out(max_U64, cmdline);
}

internal int
t_invoke_linkerf(char *fmt, ...)
{
  Temp scratch = scratch_begin(0,0);
  va_list args;
  va_start(args, fmt);
  string cmdline = push_str8fv(scratch.arena, fmt, args);
  int exit_code = t_invoke_linker(cmdline);
  va_end(args);
  scratch_end(scratch);
  return exit_code;
}

internal int
t_invoke_linker_with_time_outf(U64 time_out, char *fmt, ...)
{
  Temp scratch = scratch_begin(0,0);
  va_list args;
  va_start(args, fmt);
  string cmdline = push_str8fv(scratch.arena, fmt, args);
  int exit_code = t_invoke_linker_with_time_out(time_out, cmdline);
  va_end(args);
  scratch_end(scratch);
  return exit_code;
}

internal string
t_make_file_path(Arena *arena, string name)
{
  return push_str8f(arena, "%S\\%S", g_wdir, name);
}

internal B32
t_write_file_list(string name, String8List data)
{
  Temp scratch = scratch_begin(0,0);
  string path = t_make_file_path(scratch.arena, name);
  B32 is_written = os_write_data_list_to_file_path(path, data);
  scratch_end(scratch);
  return is_written;
}

internal B32
t_write_file(string name, string data)
{
  String8Node temp_node = {0};
  temp_node.string = data;

  String8List temp_list = {0};
  str8_list_push_node(&temp_list, &temp_node);

  return t_write_file_list(name, temp_list);
}

internal string
t_read_file(Arena *arena, string name)
{
  Temp scratch = scratch_begin(&arena,1);
  string path = t_make_file_path(scratch.arena, name);
  string data = os_data_from_file_path(arena, path);
  scratch_end(scratch);
  return data;
}

typedef struct
{
  T_Run run;
  T_Result result;
} T_RunCtx;

internal void
t_run_caller(void *raw_ctx)
{
  T_RunCtx *ctx = raw_ctx;
  ctx->result = ctx->run();
}

internal void
t_run_fail_handler(void *raw_ctx)
{
  T_RunCtx *ctx = raw_ctx;
  ctx->result = T_Result_Crash;
}

internal T_Result
t_run(T_Run run)
{
  T_RunCtx ctx = {0};
  ctx.run      = run;
  os_safe_call(t_run_caller, t_run_fail_handler, &ctx);
  return ctx.result;
}

internal COFF_SectionHeader *
t_coff_section_header_from_name(string string_table, COFF_SectionHeader *section_table, U64 section_count, string name)
{
  for (U64 sect_idx = 0; sect_idx < section_count; sect_idx += 1) {
    COFF_SectionHeader *section_header = &section_table[sect_idx];
    string             section_name   = coff_name_from_section_header(string_table, section_header);
    if (str8_match(section_name, name, 0)) {
      return section_header;
    }
  }
  return 0;
}

internal COFF_SectionHeaderArray
t_coff_section_header_array_from_name(Arena *arena, string string_table, COFF_SectionHeader *section_table, U64 section_count, string name)
{
  U64 match_count = 0;
  for (U64 sect_idx = 0; sect_idx < section_count; sect_idx += 1) {
    COFF_SectionHeader *section_header = &section_table[sect_idx];
    string             section_name   = coff_name_from_section_header(string_table, section_header);
    if (str8_match(section_name, name, 0)) {
      match_count += 1;
    }
  }

  COFF_SectionHeader *matches = push_array(arena, COFF_SectionHeader, match_count);
  for (U64 sect_idx = 0, match_idx = 0; sect_idx < section_count; sect_idx += 1) {
    COFF_SectionHeader *section_header = &section_table[sect_idx];
    string             section_name   = coff_name_from_section_header(string_table, section_header);
    if (str8_match(section_name, name, 0)) {
      matches[match_idx++] = *section_header;
    }
  }

  COFF_SectionHeaderArray result = {0};
  result.count = match_count;
  result.v     = matches;

  return result;
}

////////////////////////////////////////////////////////////////

typedef enum
{
  T_MsvcLinkExitCode_UnresolvedExternals                  = 1120,
  T_MsvcLinkExitCode_CorruptOrInvalidSymbolTable          = 1235,
  T_MsvcLinkExitCode_SectionsFoundWithDifferentAttributes = 4078,
} T_MsvcLinkExitCode;

internal COFF_ObjSection *
t_push_text_section(COFF_ObjWriter *obj_writer, string data)
{
  return coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS | COFF_SectionFlag_Align1Bytes, data);
}

internal COFF_ObjSection *
t_push_data_section(COFF_ObjWriter *obj_writer, string data)
{
  return coff_obj_writer_push_section(obj_writer, (".data"), PE_DATA_SECTION_FLAGS, data);
}

internal COFF_ObjSection *
t_push_rdata_section(COFF_ObjWriter *obj_writer, string data)
{
  return coff_obj_writer_push_section(obj_writer, (".rdata"), PE_RDATA_SECTION_FLAGS, data);
}

internal void
t_write_entry_obj(void)
{
  COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
  U8 text[] = { 0xc3 };
  COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
  coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, text_sect);
  string obj = coff_obj_writer_serialize(obj_writer->arena, obj_writer);
  if (!t_write_file(("entry.obj"), obj)) {
    AssertAlways(!"unable to write entry obj");
  }
  coff_obj_writer_release(&obj_writer);
}

////////////////////////////////

internal T_Result
t_machine_compat_check(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_Unknown);
    t_push_data_section(obj_writer, ("unknown"));
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("unknown.obj"), obj)) {
      goto exit;
    }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    t_push_data_section(obj_writer, ("x64"));
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("x64.obj"), obj)) {
      goto exit;
    }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_Arm64);
    t_push_data_section(obj_writer, ("arm64"));
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("arm64.obj"), obj)) {
      goto exit;
    }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int linker_exit_code;
  
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe entry.obj unknown.obj x64.obj");
  if (linker_exit_code != 0) {
    goto exit;
  }

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe entry.obj unknown.obj x64.obj arm64.obj");
  if (linker_exit_code == 0) {
    goto exit;
  }

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe /machine:amd64 arm64.obj entry.obj");
  if (linker_exit_code == 0) {
    goto exit;
  }

  result = T_Result_Pass;

  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_out_of_bounds_section_number(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *foo = coff_obj_writer_push_section(obj_writer, (".foo"), PE_DATA_SECTION_FLAGS, ("foo"));
    coff_obj_writer_push_symbol_extern(obj_writer, ("foo"), 0, foo);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    {
      COFF_FileHeaderInfo header = coff_file_header_info_from_data(obj);
      string string_table = str8_substr(obj, header.string_table_range);
      string symbol_table = str8_substr(obj, header.symbol_table_range);
      COFF_ParsedSymbol symbol = coff_parse_symbol(header, string_table, symbol_table, 0);
      COFF_Symbol16 *symbol16 = symbol.raw_symbol;
      symbol16->section_number = 123;
    }
    if (!t_write_file(("bad.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("foo"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe entry.obj bad.obj");
  if (linker_exit_code == 0) { goto exit; }
  if (t_ident_linker() == T_Linker_RAD && linker_exit_code != LNK_Error_IllData) { goto exit; }

  result = T_Result_Pass;
  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_merge(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_section(obj_writer, (".test"), PE_DATA_SECTION_FLAGS, ("hello, world"));
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("test.obj"), obj)) {
      goto exit;
    }
  }

  t_write_entry_obj();

  int linker_exit_code;

  // circular merge
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.test=.test entry.obj test.obj");
  if (linker_exit_code == 0) {
    goto exit;
  }
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_CircularMerge) {
      goto exit;
    }
  }

  // circular merge with extra link
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.test=.data /merge:.data=.test entry.obj test.obj");
  if (linker_exit_code == 0) {
    goto exit;
  }
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_CircularMerge) {
      goto exit;
    }
  }

  // merge with non-defined section
  {
    linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.test=.qwe entry.obj test.obj");
    if (linker_exit_code != 0) {
      goto exit;
    }

    // make sure linker created .qwe and merged .test into it
    string             exe           = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".qwe"));
    if (sect == 0) {
      goto exit;
    }
    if (sect->flags != PE_DATA_SECTION_FLAGS) {
      goto exit;
    }
    string qwe = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(qwe, ("hello, world"),0)) {
      goto exit;
    }
  }

  // illegal merge with .reloc
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.test=.reloc entry.obj test.obj");
  if (linker_exit_code == 0) {
    goto exit;
  }
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_IllegalSectionMerge) {
     goto exit;
    }
  }

  // illegal merge with .rsrc
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.test=.rsrc entry.obj test.obj");
  if (linker_exit_code == 0) {
    goto exit;
  }
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_IllegalSectionMerge) {
      goto exit;
    }
  }

  // merge non-defined section with defined section
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.qwe=.test entry.obj test.obj");
  if (linker_exit_code != 0) {
    goto exit;
  }

  // merge .test -> .qwe -> .data
  {
    linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /merge:.test=.qwe /merge:.qwe=.data entry.obj test.obj");
    if (linker_exit_code != 0) {
      goto exit;
    }

    // make sure linker merged .test into .data
    string             exe           = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".data"));
    if (sect == 0) {
      goto exit;
    }
    if (sect->flags != PE_DATA_SECTION_FLAGS) {
      goto exit;
    }
    string data = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("hello, world"),0)) {
      goto exit;
    }
  }
  
  result = T_Result_Pass;

  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_simple_link_test(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  U8 text_payload[] = { 0xC3 };

  string main_obj;
  {
    COFF_ObjWriter  *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect  = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text_payload));
    coff_obj_writer_push_section(obj_writer, (".data"), PE_DATA_SECTION_FLAGS, ("qwe"));
    coff_obj_writer_push_section(obj_writer, (".zero"), PE_BSS_SECTION_FLAGS, str8(0, 5));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    main_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string main_obj_name = ("main.obj");
  if (!t_write_file(main_obj_name, main_obj)) {
    goto exit;
  }

  int file_align = 512;
  int virt_align = 4096;
  string out_name = ("a.exe");
  int linker_exit_code = t_invoke_linkerf("/entry:my_entry /subsystem:console /fixed /filealign:%d /align:%d /out:%S %S", file_align, virt_align, out_name, main_obj_name);
  if (linker_exit_code != 0) {
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, out_name);
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  if (pe.is_pe32) {
    goto exit;
  }
  if (pe.section_count != 3) {
    goto exit;
  }
  if (pe.arch != Arch_x64) {
    goto exit;
  }
  if (pe.subsystem != PE_WindowsSubsystem_WINDOWS_CUI) {
    goto exit;
  }
  if (pe.virt_section_align != virt_align) {
    goto exit;
  }
  if (pe.file_section_align != file_align) {
    goto exit;
  }
  if (pe.symbol_count != 0) {
    goto exit;
  }
  if (pe.data_dir_count != PE_DataDirectoryIndex_COUNT) {
    goto exit;
  }

  // check section alignment
  for (U64 sect_idx = 0; sect_idx < pe.section_count; sect_idx += 1) {
    COFF_SectionHeader *sect_header = &section_table[sect_idx];
    if (AlignPadPow2(sect_header->fsize, file_align) != 0) {
      goto exit;
    }
    if (AlignPadPow2(sect_header->voff, virt_align) != 0) {
      goto exit;
    }
  }

  COFF_SectionHeader *text_section = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".text"));
  if (!text_section) {
    goto exit;
  }
  if (text_section->foff != file_align) { 
    goto exit;
  }
  if (pe.entry_point != text_section->voff) {
    goto exit;
  }

  COFF_SectionHeader *data_section = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
  if (data_section == 0) {
    goto exit;
  }

  COFF_SectionHeader *zero_section = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".zero"));
  if (zero_section == 0) {
    goto exit;
  }

  string text_data = str8_substr(exe, rng_1u64(text_section->foff, text_section->foff + text_section->vsize));
  if (!str8_match(text_data, str8_array_fixed(text_payload), 0)) {
    goto exit;
  }

  PE_OptionalHeader32Plus *opt = str8_deserial_get_raw_ptr(exe, pe.optional_header_off, sizeof(*opt));
  if (opt->sizeof_code != text_section->fsize) {
    goto exit;
  }
  if (opt->sizeof_inited_data != text_section->fsize + data_section->fsize) {
    goto exit;
  }
  if (opt->sizeof_uninited_data != 0x200) {
    goto exit;
  }
  if (opt->code_base != 0x1000) {
    goto exit;
  }
  if (opt->image_base != 0x140000000) {
    goto exit;
  }
  if (opt->major_os_ver != 6) {
    goto exit;
  }
  if (opt->minor_os_ver != 0) {
    goto exit;
  }
  if (opt->major_img_ver != 0) {
    goto exit;
  }
  if (opt->minor_img_ver != 0) {
    goto exit;
  }
  if (opt->major_subsystem_ver != 6) {
    goto exit;
  }
  if (opt->minor_subsystem_ver != 0) {
    goto exit;
  }
  if (opt->win32_version_value != 0) {
    goto exit;
  }
  if (opt->sizeof_image != 0x4000) {
    goto exit;
  }
  if (opt->sizeof_headers != 0x200) {
    goto exit;
  }
  if (opt->dll_characteristics != 0x8120) {
    goto exit;
  }
  if (opt->loader_flags != 0) {
    goto exit;
  }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_weak_vs_weak(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string a_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS, ("a"));
    COFF_ObjSymbol *sect_symbol = coff_obj_writer_push_symbol_static(obj_writer, ("_a"), 0, sect);
    coff_obj_writer_push_symbol_weak(obj_writer, ("w"), COFF_WeakExt_SearchLibrary, sect_symbol);
    a_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }
    
  string b_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".b"), PE_DATA_SECTION_FLAGS, ("b"));
    COFF_ObjSymbol *sect_symbol = coff_obj_writer_push_symbol_static(obj_writer, ("_b"), 0, sect);
    coff_obj_writer_push_symbol_weak(obj_writer, ("w"), COFF_WeakExt_SearchLibrary, sect_symbol);
    b_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }


  string entry_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("w"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  if (!t_write_file(("a.obj"), a_obj)) { goto exit; }
  if (!t_write_file(("b.obj"), b_obj)) { goto exit; }
  if (!t_write_file(("entry.obj"), entry_obj)) { goto exit; }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe a.obj b.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_weak_vs_common(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string weak_obj;
  {
    COFF_ObjWriter  *obj_writer  = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect        = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS, ("a"));
    COFF_ObjSymbol  *sect_symbol = coff_obj_writer_push_symbol_static(obj_writer, ("_a"), 0, sect);
    coff_obj_writer_push_symbol_weak(obj_writer, ("w"), COFF_WeakExt_SearchLibrary, sect_symbol);
    weak_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }
    
  string common_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_common(obj_writer, ("w"), 2);
    common_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string entry_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("w"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  if (!t_write_file(("weak.obj"), weak_obj))     { goto exit; }
  if (!t_write_file(("common.obj"), common_obj)) { goto exit; }
  if (!t_write_file(("entry.obj"), entry_obj))   { goto exit; }

  int linker_exit_code;

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe common.obj weak.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe weak.obj common.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *bss = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".bss"));
  if (!bss)            { goto exit; }
  if (bss->fsize != 0) { goto exit; }
  if (bss->vsize != 2) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_abs_vs_weak(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  U32 abs_value   = 0x123;
  U8  text_code[] = { 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC3 };

  string abs_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_abs(obj_writer, ("foo"), abs_value, COFF_SymStorageClass_External);
    abs_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string text_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    COFF_ObjSection *mydata = coff_obj_writer_push_section(obj_writer, (".mydata"), COFF_SectionFlag_CntCode|COFF_SectionFlag_MemRead|COFF_SectionFlag_MemExecute|COFF_SectionFlag_Align1Bytes, ("mydata"));
    COFF_ObjSymbol  *tag    = coff_obj_writer_push_symbol_extern(obj_writer, ("mydata"), 0, mydata);
    COFF_ObjSymbol  *foo    = coff_obj_writer_push_symbol_weak(obj_writer, ("foo"), COFF_WeakExt_NoLibrary, tag);

    COFF_ObjSection *text = coff_obj_writer_push_section(obj_writer, (".text"), COFF_SectionFlag_CntCode|COFF_SectionFlag_MemExecute|COFF_SectionFlag_MemRead|COFF_SectionFlag_Align1Bytes, str8_array_fixed(text_code));
    coff_obj_writer_section_push_reloc(obj_writer, text, 2, foo, COFF_Reloc_X64_Addr64);
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text);

    text_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  if (!t_write_file(("abs.obj"),  abs_obj))  { goto exit; }
  if (!t_write_file(("text.obj"), text_obj)) { goto exit; }

  int abs_vs_weak_exit_code = t_invoke_linker(("/subsystem:console /entry:my_entry /out:a.exe abs.obj text.obj"));
  if (abs_vs_weak_exit_code != 0) { goto exit; }

  int weak_vs_abs_exit_code = t_invoke_linker(("/subsystem:console /entry:my_entry /out:a.exe text.obj abs.obj"));
  if (weak_vs_abs_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *text_section = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".text"));
  if (text_section == 0) { goto exit; }

  string text_data = str8_substr(exe, rng_1u64(text_section->foff, text_section->foff + text_section->fsize));
  string inst      = str8_prefix(text_data, 2);
  if (!str8_match(inst, str8_array(text_code, 2), 0)) { goto exit; }

  string imm          = str8_prefix(str8_skip(text_data, 2), 8);
  U64     expected_imm = abs_value;
  if (!str8_match(imm, str8_struct(&expected_imm), 0)) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_abs_vs_regular(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  string shared_symbol_name = ("foo");

  U8 regular_payload[] = { 0xC0, 0xFF, 0xEE };
  string regular_obj_name = ("regular.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, str8_array_fixed(regular_payload));
    coff_obj_writer_push_symbol_extern(obj_writer, shared_symbol_name, 0, data_sect);
    string regular_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(regular_obj_name, regular_obj)) {
      goto exit;
    }
  }

  string abs_obj_name = ("abs.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_abs(obj_writer, shared_symbol_name, 0x1234, COFF_SymStorageClass_External);
    string abs_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(abs_obj_name, abs_obj)) {
      goto exit;
    }
  }

  U8 entry_text[] = { 
    0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, // mov rax, $imm
    0xC3 // ret
  };
  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    COFF_ObjSymbol *shared_symbol = coff_obj_writer_push_symbol_undef(obj_writer, shared_symbol_name);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 3, shared_symbol, COFF_Reloc_X64_Addr32Nb);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  // TODO: validate that linker issues multiply defined symbol error
  int abs_vs_regular_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe abs.obj regular.obj entry.obj");
  if (abs_vs_regular_exit_code == 0) {
    // linker should complain about multiply defined symbol
    goto exit;
  }

  int regular_vs_abs_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe regular.obj abs.obj entry.obj");
  if (regular_vs_abs_exit_code == 0) {
    // linker should complain even in case regular is before abs
    goto exit;
  }

  result = T_Result_Pass;
  
  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_abs_vs_common(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string shared_symbol_name = ("foo");

  string common_obj_name = ("common.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_common(obj_writer, shared_symbol_name, 321);
    string common_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(common_obj_name, common_obj)) {
      goto exit;
    }
  }

  string abs_obj_name = ("abs.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_abs(obj_writer, shared_symbol_name, 0x1234, COFF_SymStorageClass_External);
    string abs_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(abs_obj_name, abs_obj)) {
      goto exit;
    }
  }

  U8 entry_text[] = { 0xC3 };
  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe abs.obj common.obj entry.obj");
  if (linker_exit_code == 0) {
    // TODO: validate that linker issues multiply defined symbol error
    int common_vs_abs = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe common.obj abs.obj entry.obj");
    if (t_ident_linker() == T_Linker_RAD) {
      if (common_vs_abs == LNK_Error_MultiplyDefinedSymbol) {
        result = T_Result_Pass;
      }
    } else {
      if (common_vs_abs != 0) {
        result = T_Result_Pass;
      }
    }
  }

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_abs_vs_abs(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string a_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_abs(obj_writer, ("foo"), 'a', COFF_SymStorageClass_External);
    a_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string b_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_abs(obj_writer, ("foo"), 'b', COFF_SymStorageClass_External);
    b_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  if (!t_write_file(("a.obj"), a_obj)) { goto exit; }
  if (!t_write_file(("b.obj"), b_obj)) { goto exit; }
  t_write_entry_obj();

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe a.obj b.obj entry.obj");
  if (linker_exit_code != LNK_Error_MultiplyDefinedSymbol) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_undef_weak(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  string entry_symbol_name = ("my_entry");
  string shared_symbol_name = ("foo");

  U8 weak_payload[] = { 0xDE, 0xAD, 0xBE, 0xEF };
  string weak_obj_name = ("weak.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *weak_sect = t_push_data_section(obj_writer, str8_array_fixed(weak_payload));
    COFF_ObjSymbol *tag = coff_obj_writer_push_symbol_undef(obj_writer, ("ptr"));
    coff_obj_writer_push_symbol_weak(obj_writer, shared_symbol_name, COFF_WeakExt_SearchAlias, tag);
    string weak_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(weak_obj_name, weak_obj)) {
      goto exit;
    }
  }

  string ptr_obj_name = ("ptr.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSymbol *tag = coff_obj_writer_push_symbol_undef(obj_writer, entry_symbol_name);
    coff_obj_writer_push_symbol_weak(obj_writer, ("ptr"), COFF_WeakExt_SearchAlias, tag);
    string ptr_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(ptr_obj_name, ptr_obj)) {
      goto exit;
    }
  }

  U8 undef_obj_payload[] = { 0x00, 0x00, 0x00, 0x00 };
  string undef_obj_name = ("undef.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *undef_sect = t_push_data_section(obj_writer, str8_array_fixed(undef_obj_payload));
    COFF_ObjSymbol *undef_symbol = coff_obj_writer_push_symbol_undef(obj_writer, shared_symbol_name);
    coff_obj_writer_section_push_reloc(obj_writer, undef_sect, 0, undef_symbol, COFF_Reloc_X64_Addr32Nb);
    string undef_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(undef_obj_name, undef_obj)) {
      goto exit;
    }
  }

  U8 entry_payload[] = {0xC3};
  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_payload));
    coff_obj_writer_push_symbol_extern(obj_writer, entry_symbol_name, 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:%S /out:a.exe %S %S %S %S", entry_symbol_name, weak_obj_name, entry_obj_name, ptr_obj_name, undef_obj_name);

  T_Linker link_ident = t_ident_linker();
  if (linker_exit_code != 0) {
    goto exit;
  }

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_weak_cycle(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  string ab_obj_name = ("ab.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSymbol *b = coff_obj_writer_push_symbol_undef(obj_writer, ("B"));
    coff_obj_writer_push_symbol_weak(obj_writer, ("A"), COFF_WeakExt_SearchAlias, b);
    string ab_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(ab_obj_name, ab_obj)) {
      goto exit;
    }
  }

  string ba_obj_name = ("ba.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSymbol *a = coff_obj_writer_push_symbol_undef(obj_writer, ("A"));
    coff_obj_writer_push_symbol_weak(obj_writer, ("B"), COFF_WeakExt_SearchAlias, a);
    string ba_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(ba_obj_name, ba_obj)) {
      goto exit;
    }
  }

  string entry_obj_name = ("entry.obj");
  U8 entry_payload[] = { 0xC3 };
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_payload));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  U64 time_out = os_now_microseconds() + 3 * 1000 * 1000; // give a generous 3 seconds
  int linker_exit_code = t_invoke_linker_with_time_outf(time_out, "/subsystem:console /entry:my_entry %S %S %S", entry_obj_name, ab_obj_name, ba_obj_name);
  if (linker_exit_code != T_LINKER_TIME_OUT_EXIT_CODE) {
    if (t_ident_linker() == T_Linker_MSVC) {
      if (linker_exit_code == T_MsvcLinkExitCode_UnresolvedExternals) {
        result = T_Result_Pass;
      }
    } else {
      result = T_Result_Pass;
    }
  }

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_weak_tag(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  U32     weak_tag_expected_value = 0x12345678;
  string weak_tag_obj_name       = ("weak_tag.obj");
  {
    COFF_ObjWriter *obj_writer  = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSymbol *tag_symbol  = coff_obj_writer_push_symbol_abs(obj_writer, ("abs"), weak_tag_expected_value, COFF_SymStorageClass_Static);
    COFF_ObjSymbol *weak_first  = coff_obj_writer_push_symbol_weak(obj_writer, ("strong_first"), COFF_WeakExt_SearchAlias, tag_symbol);
    COFF_ObjSymbol *weak_second = coff_obj_writer_push_symbol_weak(obj_writer, ("strong_second"), COFF_WeakExt_SearchAlias, weak_first);

    U8 sect_data[4] = {0};
    COFF_ObjSection *sect = t_push_data_section(obj_writer, str8_array_fixed(sect_data));
    coff_obj_writer_section_push_reloc(obj_writer, sect, 0, weak_second, COFF_Reloc_X64_Addr32);

    string weak_tag_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(weak_tag_obj_name, weak_tag_obj)) { goto exit; }
  }

  string entry_name     = ("my_entry");
  U8      entry_text[]   = { 0xC3 };
  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter  *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect  = t_push_text_section(obj_writer, str8_array_fixed(entry_text));
    coff_obj_writer_push_symbol_extern(obj_writer, entry_name, 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe %S %S", weak_tag_obj_name, entry_obj_name);
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *data_section  = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
  string             data          = str8_substr(exe, rng_1u64(data_section->foff, data_section->foff + data_section->vsize));
  if (!data_section)                                               { goto exit; }
  if (data_section->vsize != 4)                                    { goto exit; }
  if (!str8_match(data, str8_struct(&weak_tag_expected_value), 0)) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal string
t_make_sec_defn_obj(Arena *arena, string payload)
{
  COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
  COFF_ObjSection *mysect_section = coff_obj_writer_push_section(obj_writer, (".mysect"), COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_MemRead|COFF_SectionFlag_Align1Bytes, payload);
  coff_obj_writer_push_symbol_secdef(obj_writer, mysect_section, COFF_ComdatSelect_Null);
  string obj = coff_obj_writer_serialize(arena, obj_writer);
  coff_obj_writer_release(&obj_writer);
  return obj;
}

internal T_Result
t_undef_section(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string main_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    U8 data[] = { 0, 0, 0, 0 };
    COFF_ObjSection *data_section = t_push_data_section(obj_writer, str8_array_fixed(data));
    COFF_ObjSymbol  *foo          = coff_obj_writer_push_symbol_undef_sect(obj_writer, (".mysect"), COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_MemRead);
    coff_obj_writer_section_push_reloc(obj_writer, data_section, 0, foo, COFF_Reloc_X64_Addr32Nb);

    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_section = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_section);

    main_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);

    coff_obj_writer_release(&obj_writer);
  }

  U8 payload[] = { 1, 2, 3 };
  string sec_defn_obj = t_make_sec_defn_obj(scratch.arena, str8_array_fixed(payload));

  t_write_file(("main.obj"), main_obj);
  t_write_file(("sec_defn.obj"), sec_defn_obj);

  int linker_exit_code = t_invoke_linker(("/subsystem:console /entry:my_entry /out:a.exe main.obj sec_defn.obj"));
  if (linker_exit_code == 0) {
    string             exe           = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);

    COFF_SectionHeader *data_section   = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
    COFF_SectionHeader *mysect_section = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".mysect"));
    if (data_section && mysect_section) {
      if (data_section->vsize == 4 && mysect_section->vsize == 3) {
        string addr32nb = str8_substr(exe, rng_1u64(data_section->foff, data_section->foff + data_section->vsize));
        string expected_voff = str8_struct(&mysect_section->voff);
        if (str8_match(addr32nb, expected_voff, 0)) {
          result = T_Result_Pass;
        }
      }
    }
  }

  scratch_end(scratch);
  return result;
}

internal T_Result
t_sect_symbol(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  string sect_payload = ("hello, world");
  string sect_obj_name = ("sect.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".mysect$1"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align1Bytes, sect_payload);
    coff_obj_writer_push_directive(obj_writer, ("/merge:.mysect=.data"));
    string sect_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(sect_obj_name, sect_obj)) {
      goto exit;
    }
  }

  string main_obj_name = ("main.obj");
  {
    U8 data[8] = {0};
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = t_push_data_section(obj_writer, str8_array_fixed(data));
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef_sect(obj_writer, (".mysect$2222"), PE_DATA_SECTION_FLAGS);
    coff_obj_writer_section_push_reloc_addr(obj_writer, sect, 0, symbol);

    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);

    string main_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);

    if (!t_write_file(main_obj_name, main_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe main.obj sect.obj");

  if (linker_exit_code != 0) { 
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *sect          = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));

  if (!sect) {
    goto exit;
  }

  string sect_data = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));

  string addr_data = str8_substr(sect_data, rng_1u64(0, sizeof(U64)));
  if (addr_data.size != sizeof(U64)) {
    goto exit;
  }
  U64 addr = *(U64 *)addr_data.str;
  if (addr - (pe.image_base + sect->voff) != 8) {
    goto exit;
  }

  string payload_got = str8_substr(sect_data, rng_1u64(8, sect_data.size));
  if (!str8_match(payload_got, sect_payload, 0)) {
    goto exit;
  }

  result = T_Result_Pass;

  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_undef_reloc_section(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string main_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_section = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    COFF_ObjSymbol *my_entry_symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_section);

    U8 data[8] = { 0 };
    COFF_ObjSection *data_section = t_push_data_section(obj_writer, str8_array_fixed(data));
    COFF_ObjSymbol  *foo          = coff_obj_writer_push_symbol_undef_sect(obj_writer, (".reloc"), PE_RELOC_SECTION_FLAGS);
    coff_obj_writer_section_push_reloc(obj_writer, data_section, 0, foo, COFF_Reloc_X64_Addr64);

    main_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);

    coff_obj_writer_release(&obj_writer);
  }

  U8 payload[] = { 1, 2, 3 };
  string sec_defn_obj = t_make_sec_defn_obj(scratch.arena, str8_array_fixed(payload));

  t_write_file(("main.obj"), main_obj);
  t_write_file(("sec_defn.obj"), sec_defn_obj);

  int linker_exit_code = t_invoke_linker(("/subsystem:console /entry:my_entry /out:a.exe main.obj sec_defn.obj"));
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_SectRefsDiscardedMemory) {
      goto exit;
    }
  } else if (linker_exit_code == 0) {
    goto exit;
  }

  result = T_Result_Pass;

  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_find_merged_pdata(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  U8 foobar_payload[] = {
    0x40, 0x57, 0x48, 0x81, 0xEC, 0x00, 0x02, 0x00, 0x00, 0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x48, 0x33, 0xC4, 0x48, 0x89, 0x84, 0x24, 0xF0, 0x01, 0x00, 0x00, 0x48, 0x8D, 0x04, 0x24, 0x48,
    0x8B, 0xF8, 0x33, 0xC0, 0xB9, 0xEC, 0x01, 0x00, 0x00, 0xF3, 0xAA, 0xB8, 0x04, 0x00, 0x00, 0x00,
    0x48, 0x6B, 0xC0, 0x02, 0x8B, 0x04, 0x04, 0x48, 0x8B, 0x8C, 0x24, 0xF0, 0x01, 0x00, 0x00, 0x48,
    0x33, 0xCC, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x81, 0xC4, 0x00, 0x02, 0x00, 0x00, 0x5F, 0xC3,
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
    0x48, 0x83, 0xEC, 0x28, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xC4, 0x28, 0xC3      
  };
  U8 xdata_payload[] = {
    0x19, 0x1B, 0x03, 0x00, 0x09, 0x01, 0x40, 0x00, 0x02, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x01, 0x00, 0x00, 0x01, 0x04, 0x01, 0x00, 0x04, 0x42, 0x00, 0x00
  };
  PE_IntelPdata intel_pdata = {0};
  U8 text_payload[]  = { 0xC3 };

  string main_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *xdata  = coff_obj_writer_push_section(obj_writer, (".xdata"),  COFF_SectionFlag_MemRead|COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_Align4Bytes, str8_array_fixed(xdata_payload));
    COFF_ObjSection *pdata  = coff_obj_writer_push_section(obj_writer, (".pdata"),  COFF_SectionFlag_MemRead|COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_Align4Bytes, str8_struct(&intel_pdata));
    COFF_ObjSection *foobar = coff_obj_writer_push_section(obj_writer, (".foobar"), COFF_SectionFlag_MemRead|COFF_SectionFlag_MemExecute|COFF_SectionFlag_CntCode|COFF_SectionFlag_Align1Bytes, str8_array_fixed(foobar_payload));
    COFF_ObjSection *text   = coff_obj_writer_push_section(obj_writer, (".text"),   COFF_SectionFlag_MemRead|COFF_SectionFlag_MemExecute|COFF_SectionFlag_CntCode|COFF_SectionFlag_Align1Bytes, str8_array_fixed(text_payload));

    COFF_ObjSymbol *foobar_symbol = coff_obj_writer_push_symbol_static(obj_writer, ("foobar"), 0, foobar);

    coff_obj_writer_push_symbol_secdef(obj_writer, xdata, COFF_ComdatSelect_Null);
    COFF_ObjSymbol *unwind_foobar = coff_obj_writer_push_symbol_static(obj_writer, ("$unwind$foobar"), 0, xdata);

    coff_obj_writer_push_symbol_secdef(obj_writer, pdata, COFF_ComdatSelect_Null);
    coff_obj_writer_push_symbol_static(obj_writer, ("$pdata$foobar"), 0, pdata);

    coff_obj_writer_section_push_reloc(obj_writer, pdata, OffsetOf(PE_IntelPdata, voff_unwind_info),   unwind_foobar, COFF_Reloc_X64_Addr32Nb);
    coff_obj_writer_section_push_reloc(obj_writer, pdata, OffsetOf(PE_IntelPdata, voff_first),         foobar_symbol, COFF_Reloc_X64_Addr32Nb);
    coff_obj_writer_section_push_reloc(obj_writer, pdata, OffsetOf(PE_IntelPdata, voff_one_past_last), foobar_symbol, COFF_Reloc_X64_Addr32Nb);

    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text);
    main_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  t_write_file(("main.obj"), main_obj);

  int linker_exit_code = t_invoke_linker(("/subsystem:console /entry:my_entry /out:a.exe main.obj /merge:.pdata=.rdata"));
  if (linker_exit_code == 0) {
    string    exe = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo pe  = pe_bin_info_from_data(scratch.arena, exe);
    if (dim_1u64(pe.data_dir_franges[PE_DataDirectoryIndex_EXCEPTIONS]) == 0xC) {
      result = T_Result_Pass;
    }
  }

  scratch_end(scratch);
  return result;
}

internal T_Result
t_section_sort(void)
{
  Temp scratch = scratch_begin(0,0);
  
  T_Result result = T_Result_Fail;

  string data_obj_name = ("data.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    COFF_SectionFlags data_flags = COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_MemRead|COFF_SectionFlag_MemRead|COFF_SectionFlag_Align1Bytes;
    coff_obj_writer_push_section(obj_writer, (".data$z"), data_flags, ("five"));
    coff_obj_writer_push_section(obj_writer, (".data$a"), data_flags, ("three"));
    coff_obj_writer_push_section(obj_writer, (".data$bbbbb"), data_flags, ("four"));
    coff_obj_writer_push_section(obj_writer, (".data$"), data_flags, ("two"));
    coff_obj_writer_push_section(obj_writer, (".data"), data_flags, ("one"));

    string data_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(data_obj_name, data_obj)) {
      goto exit;
    }
  }

  string entry_obj_name = ("entry.obj");
  U8 entry_text[] = { 0xC3 };
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe data.obj entry.obj");
  if (linker_exit_code != 0) {
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *data_section = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
  if (!data_section) {
    goto exit;
  }

  string data = str8_substr(exe, rng_1u64(data_section->foff, data_section->foff + data_section->vsize));
  string expected_data = ("onetwothreefourfive");
  if (!str8_match(data, expected_data, 0)) {
    goto exit;
  }

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_flag_conf(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  COFF_SectionFlags my_sect0_flags = COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_MemRead|COFF_SectionFlag_MemExecute;
  COFF_SectionFlags my_sect1_flags = COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_MemRead|COFF_SectionFlag_MemWrite;
  string conf_obj_name = ("conf.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *a_sect = coff_obj_writer_push_section(obj_writer, (".mysect"), my_sect0_flags, ("one"));
    COFF_ObjSection *b_sect = coff_obj_writer_push_section(obj_writer, (".mysect"), my_sect1_flags, ("two"));
    string conf_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(conf_obj_name, conf_obj)) {
      goto exit;
    }
  }

  U8 entry_text[] = { 0xC3 };
  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe conf.obj entry.obj");
  if (linker_exit_code != 0) {
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeaderArray my_sects = t_coff_section_header_array_from_name(scratch.arena, string_table, section_table, pe.section_count, (".mysect"));

  if (my_sects.count != 2) {
    goto exit;
  }

  COFF_SectionHeader *my_sect0 = &my_sects.v[0];
  COFF_SectionHeader *my_sect1 = &my_sects.v[1];
  if (my_sect0->flags != my_sect0_flags) {
    goto exit;
  }
  if (my_sect1->flags != my_sect1_flags) {
    goto exit;
  }

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_invalid_bss(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  COFF_SectionFlags bss_flags = COFF_SectionFlag_CntInitializedData|COFF_SectionFlag_MemRead;
  string bss_obj_name = ("bss.obj");
  string bss_data = ("Hello, World");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_section(obj_writer, (".bss"), bss_flags, bss_data);
    string bss_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(bss_obj_name, bss_obj)) {
      goto exit;
    }
  }

  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe bss.obj entry.obj");
  if (linker_exit_code != 0) {
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *bss_sect = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".bss"));
  if (bss_sect == 0) {
    goto exit;
  }
  if (bss_sect->vsize != 0xC) {
    goto exit;
  }
  if (bss_sect->flags != bss_flags) {
    goto exit;
  }
  string data = str8_substr(exe, rng_1u64(bss_sect->foff, bss_sect->foff + bss_sect->vsize));
  if (!str8_match(data, bss_data, 0)) {
    goto exit;
  }

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_common_block(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  string a_obj_name = ("a.obj");
  U8 a_data[6] = {0};
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_common(obj_writer, ("A"), 3);
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, str8_array_fixed(a_data));
    data_sect->flags |= COFF_SectionFlag_Align1Bytes;
    coff_obj_writer_push_section(obj_writer, (".bss"), PE_BSS_SECTION_FLAGS, str8(0, 1)); // shift common block's initial position
    coff_obj_writer_section_push_reloc(obj_writer, data_sect, 0, symbol, COFF_Reloc_X64_Addr32);
    string a_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(a_obj_name, a_obj)) { goto exit; }
  }

  string b_obj_name = ("b.obj");
  U8 b_data[9] = { 0 };
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, str8_array_fixed(b_data));
    data_sect->flags |= COFF_SectionFlag_Align1Bytes;
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_common(obj_writer, ("B"), 6);
    coff_obj_writer_section_push_reloc(obj_writer, data_sect, 0, symbol, COFF_Reloc_X64_Addr64);
    string b_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(b_obj_name, b_obj)) { goto exit; }
  }

  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe /fixed /largeaddressaware:no /merge:.bss=.comm a.obj b.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  COFF_SectionHeader *comm_sect     = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".comm"));
  COFF_SectionHeader *data_sect     = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
  if (comm_sect == 0) { goto exit; }
  if (data_sect == 0) { goto exit; }

  // blocks must be sorted in descending order to reduce alignment padding
  if (comm_sect->vsize != 0x13) { goto exit; }

  // ensure linker correctly patched addresses for symbols pointing into common block
  string             data      = str8_substr(exe, rng_1u64(data_sect->foff, data_sect->foff + data_sect->fsize));
  U32                *a_addr    = (U32 *)data.str;
  U64                *b_addr    = (U64 *)(data.str + sizeof(a_data));
  if (*a_addr != (pe.image_base + comm_sect->voff + 0x10)) { goto exit; }
  if (*b_addr != (pe.image_base + comm_sect->voff + 0x8)) { goto exit; }
  
  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_base_relocs(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  // main.obj
  string entry_name = ("my_entry");
  U64 mov_func_name64 = 2;
  U64 mov_func_name32 = 16;
  U8 main_text[] = {
    0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, func_name
    0xff, 0xd0,                                                  // call rax
    0x48, 0x31, 0xc0,                                            // xor rax, rax
    0xb8, 0x00, 0x00, 0x00, 0x00,                                // mov eax, func_name
    0xff, 0xd0,                                                  // call rax
    0xc3                                                         // ret
  };

  // func.obj
  string func_name   = ("foo");
  U8      func_text[] = { 0xc3 };

  string main_obj_name = ("main.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect  = t_push_text_section(obj_writer, str8_array_fixed(main_text));
    COFF_ObjSymbol  *func_undef = coff_obj_writer_push_symbol_undef(obj_writer, func_name);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, mov_func_name64, func_undef, COFF_Reloc_X64_Addr64);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, mov_func_name32, func_undef, COFF_Reloc_X64_Addr32);

    // linker must not produce base relocations for absolute symbol
    U8 data[4] = {0};
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, str8_array_fixed(data));
    COFF_ObjSymbol *abs_symbol = coff_obj_writer_push_symbol_abs(obj_writer, ("abs"), 0x12345678, COFF_SymStorageClass_Static);
    coff_obj_writer_section_push_reloc(obj_writer, data_sect, 0, abs_symbol, COFF_Reloc_X64_Addr32);

    coff_obj_writer_push_symbol_extern(obj_writer, entry_name, 0, text_sect);
    string main_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(main_obj_name, main_obj)) {
      goto exit;
    }
  }

  string func_obj_name = ("func.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(func_text));
    coff_obj_writer_push_symbol_extern(obj_writer, func_name, 0, text_sect);
    string func_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(func_obj_name, func_obj)) {
      goto exit;
    }
  }

  string out_name = ("a.exe");
  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /dynamicbase /largeaddressaware:no /out:a.exe main.obj func.obj");
  if (linker_exit_code != 0) {
    goto exit;
  }

  // it is illegal to merge .reloc with other sections
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /dynamicbase /largeaddressaware:no /out:a.exe /merge:.reloc=.rdata main.obj func.obj");
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_IllegalSectionMerge) {
      goto exit;
    }
  } else {
    if (linker_exit_code == 0) {
      goto exit;
    }
  }

  // the other way around is illegal too
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /dynamicbase /largeaddressaware:no /out:a.exe /merge:.rdata=.reloc main.obj func.obj");
  if (t_ident_linker() == T_Linker_RAD) {
    if (linker_exit_code != LNK_Error_IllegalSectionMerge) {
      goto exit;
    }
  } else {
    if (linker_exit_code == 0) {
      goto exit;
    }
  }

  result = T_Result_Pass;
  
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_simple_lib_test(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string test_payload = ("The quick brown fox jumps over the lazy dog");
  string test_obj = {0};
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0,COFF_MachineType_Unknown);
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, str8(test_payload.str, test_payload.size+1));
    coff_obj_writer_push_symbol_extern(obj_writer, ("test"), 0, data_sect);
    test_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string test_lib_name = ("test.lib");
  {
    COFF_LibWriter *lib_writer = coff_lib_writer_alloc();
    coff_lib_writer_push_obj(lib_writer, ("test.obj"), test_obj);
    String8List test_lib = coff_lib_writer_serialize(scratch.arena, lib_writer, 0, 0, 1);
    coff_lib_writer_release(&lib_writer);
    if (!t_write_file_list(test_lib_name, test_lib)) {
      goto exit;
    }
  }

  U8 entry_text[] = {
    0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,
    0xC3
  };
  string entry_obj_name = ("entry.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0,COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(entry_text));
    COFF_ObjSymbol *test_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("test"));
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 3, test_symbol, COFF_Reloc_X64_Addr32Nb);
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 7, text_sect);
    string entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(entry_obj_name, entry_obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe entry.obj test.lib");
  if (linker_exit_code != 0) {
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *text_sect = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".text"));
  COFF_SectionHeader *data_sect = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));

  string text_data = str8_substr(exe, rng_1u64(text_sect->foff, text_sect->foff + text_sect->fsize));
  string data_data = str8_substr(exe, rng_1u64(data_sect->foff, data_sect->foff + data_sect->fsize));

  // was test payload linked?
  string data_string = str8_cstring_capped(data_data.str, data_data.str + data_data.size);
  if (!str8_match(data_string, test_payload, 0)) {
    goto exit;
  }

  // do we have enough bytes to read text?
  if (text_data.size < sizeof(entry_text)) {
    goto exit;
  }

  // linker must pull-in test.obj and patch relocation for "test" symbol
  U32 *data_addr32nb = (U32 *)(text_data.str+3);
  if (*data_addr32nb != data_sect->voff) {
    goto exit;
  }

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_import_export(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  {
    string export_obj_name    = ("export.obj");
    string export_obj_payload = ("test");
    U8      export_text[]      = { 0xC3 };

    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, export_obj_payload);
    coff_obj_writer_push_symbol_extern(obj_writer, ("foo"), 0, data_sect);
    coff_obj_writer_push_symbol_extern(obj_writer, ("ord"), 0, data_sect);
    coff_obj_writer_push_symbol_extern(obj_writer, ("ord2"), 0, data_sect);
    coff_obj_writer_push_symbol_extern(obj_writer, ("ord3"), 0, data_sect);
    coff_obj_writer_push_symbol_extern(obj_writer, ("ord4"), 0, data_sect);
    //coff_obj_writer_push_directive(obj_writer, ("/export:foo=foo"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:bar=foo"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:ord,@5"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:ord2,@6,DATA"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:ord3,@7,NONAME,PRIVATE"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:ord4,@8,NONAME,DATA"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:baz=baz.qwe"));
    //coff_obj_writer_push_directive(obj_writer, ("/export:baf=baz.#1"));
    string export_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(export_obj_name, export_obj)) {
      goto exit;
    }
  }

  {
    string import_obj_name = ("import.obj");
    U8 import_payload[1024] = {0};

    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *data_sect = t_push_data_section(obj_writer, str8_array_fixed(import_payload));

    char *import_symbols[] = {
      "__imp_foo",
      "__imp_bar",
      "__imp_baz",
      "__imp_baf",
      "__imp_ord",
      //"__imp_ord2",
      //"__imp_ord4",
      "bar",
      //"baf",
      //"baz",
      "foo",
      "ord",
    };

    for (U64 i = 0; i < ArrayCount(import_symbols); ++i) {
      COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, str8_cstring(import_symbols[i]));
      coff_obj_writer_section_push_reloc_voff(obj_writer, data_sect, i * 4, symbol);
    }

    string import_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(import_obj_name, import_obj)) {
      goto exit;
    }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *s1 = coff_obj_writer_push_section(obj_writer, (".s1"), PE_DATA_SECTION_FLAGS, ("s1"));
    COFF_ObjSection *s2 = coff_obj_writer_push_section(obj_writer, (".s2"), PE_DATA_SECTION_FLAGS, ("s2"));
    COFF_ObjSection *text_sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed((U8[]){ 0xC3 }));
    coff_obj_writer_push_symbol_extern(obj_writer, ("_DllMainCRTStartup"), 0, text_sect);
    coff_obj_writer_push_symbol_extern(obj_writer, ("s1"), 0, s1);
    coff_obj_writer_push_symbol_extern(obj_writer, ("s2"), 0, s2);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("baz.obj"), obj)) {
      goto exit;
    }
  }

  t_write_entry_obj();

  //
  // link export.dll
  //
  int lib_link_exit_code = t_invoke_linkerf("/dll /out:export.dll libcmt.lib export.obj");
  if (lib_link_exit_code != 0) {
    goto exit;
  }

  //
  // link baz.dll
  //
  int baz_link_exit_code = t_invoke_linkerf("/dll /out:baz.dll /export:s1,@1,NONAME /export:qwe=s2 baz.obj");
  if (baz_link_exit_code != 0) {
    goto exit;
  }

  //
  // check export.dll export table
  //
  if (0) {
    string              dll           = t_read_file(scratch.arena, ("export.dll"));
    PE_BinInfo           pe            = pe_bin_info_from_data(scratch.arena, dll);
    COFF_SectionHeader  *section_table = (COFF_SectionHeader *)str8_substr(dll, pe.section_table_range).str;
    string              string_table  = str8_substr(dll, pe.string_table_range);
    PE_ParsedExportTable export_table  = pe_exports_from_data(scratch.arena, pe.section_count, section_table, dll, pe.data_dir_franges[PE_DataDirectoryIndex_EXPORT], pe.data_dir_vranges[PE_DataDirectoryIndex_EXPORT]);

    if (export_table.export_count != 6) {
      goto exit;
    }

    {
      string expected_symbols[] = {
        ("foo"),
        ("bar"),
        //("baz"),
        //("baf"),
        ("ord"),
        ("ord2")
      };
      U64 match_count = 0;
      for (U64 i = 0; i < export_table.export_count; i += 1) {
        for (U64 k = 0; k < ArrayCount(expected_symbols); k += 1) {
          if (str8_match(export_table.exports[i].name, expected_symbols[k], 0)) {
            match_count += 1;
          }
        }
      }
      if (match_count != ArrayCount(expected_symbols)) {
        goto exit;
      }
    }

    if (0) {
      string expected_forwarders[] = {
        ("baz.qwe"),
        ("baz.#1"),
      };
      U64 match_count = 0;
      for (U64 i = 0; i < export_table.export_count; i += 1) {
        for (U64 k = 0; k < ArrayCount(expected_forwarders); k += 1) {
          if (str8_match(export_table.exports[i].forwarder, expected_forwarders[k], 0)) {
            match_count += 1;
          }
        }
      }
      if (match_count != ArrayCount(expected_forwarders)) {
        goto exit;
      }
    }
  }

  #if OS_WINDOWS
  BOOL is_dir_set = SetDllDirectoryA(g_wdir.str);
  AssertAlways(is_dir_set);

  HANDLE export_dll = LoadLibrary("export.dll");
  DWORD last_error = GetLastError();
  if (export_dll == 0) {
    goto exit;
  }

  char *export_dll_procs[] = { "foo", "bar", "baz", "baf" };
  for (U64 i = 0; i < ArrayCount(export_dll_procs); ++i) {
    void *proc = GetProcAddress(export_dll, export_dll_procs[i]);
    if (proc == 0) {
      goto exit;
    }
  }

  U16 export_dll_ordinals[] = { 5 };
  for (U64 i = 0; i < ArrayCount(export_dll_ordinals); ++i) {
    void *proc = GetProcAddress(export_dll, MAKEINTRESOURCE(export_dll_ordinals[i]));
    if (proc == 0) {
      goto exit;
    }
  }
  #endif

  int import_link_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /delayload:export.dll /export:entry kernel32.Lib delayimp.lib libcmt.lib export.lib import.obj entry.obj");
  if (import_link_exit_code != 0) {
    goto exit;
  }

  // TODO: check import table

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_image_base(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  string obj_name = ("image_base.obj");
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = { 
      0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, // lea rcx, [__ImageBase]
      0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, __ImageBase
      0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, __ImageBase
      0xC3 // ret
    };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    COFF_ObjSymbol *image_base_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("__ImageBase"));
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 3, image_base_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 9, image_base_symbol, COFF_Reloc_X64_Addr64);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 18, image_base_symbol, COFF_Reloc_X64_Addr32Nb);
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect); 
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(obj_name, obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /base:0x2000000140000000 /out:a.exe image_base.obj");

  if (linker_exit_code != 0) {
    goto exit;
  }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *text_section  = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".text"));

  if (!text_section) {
    goto exit;
  }

  U8 expected_text[] = {
    0x48, 0x8D, 0x0D, 0xF9, 0xEF, 0xFF, 0xFF,
    0x48, 0xB8, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x20,
    0xB8, 0x00, 0x00, 0x00, 0x00,
    0xC3
  };
  string text_data = str8_substr(exe, rng_1u64(text_section->foff, text_section->foff + sizeof(expected_text)));
  if (!str8_match(text_data, str8_array_fixed(expected_text), 0)) {
    goto exit;
  }

  result = T_Result_Pass;

  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_any(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".test$mn"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("1"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Any);
    COFF_ObjSymbol *test = coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    test->type.u.msb = COFF_SymDType_Func;
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("1.obj"), obj)) {
      goto exit;
    }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".test$mn"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("2"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Any);
    COFF_ObjSymbol *test = coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("2.obj"), obj)) {
      goto exit;
    }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  {
    int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:1.exe 1.obj 2.obj entry.obj");
    if (linker_exit_code != 0) {
        goto exit;
    }
    string             exe           = t_read_file(scratch.arena, ("1.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".test"));
    string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("1"), 0)) {
      goto exit;
    }
  }

  {
    int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:2.exe 2.obj 1.obj entry.obj");
    if (linker_exit_code != 0) {
        goto exit;
    }
    string             exe           = t_read_file(scratch.arena, ("2.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".test"));
    string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("2"), 0)) {
      goto exit;
    }
  }

  result = T_Result_Pass;

  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_no_duplicates(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string test_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *test_sect = coff_obj_writer_push_section(obj_writer, (".test"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("a"));
    coff_obj_writer_push_symbol_secdef(obj_writer, test_sect, COFF_ComdatSelect_NoDuplicates);
    coff_obj_writer_push_symbol_extern(obj_writer, ("a"), 0, test_sect);
    test_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string entry_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("a"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  if (!t_write_file(("a.obj"),     test_obj))  { goto exit; }
  if (!t_write_file(("b.obj"),     test_obj))  { goto exit; }
  if (!t_write_file(("entry.obj"), entry_obj)) { goto exit; }

  int duplicates_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe a.obj b.obj entry.obj");
  if (duplicates_exit_code == 0) { goto exit; }
  if (t_ident_linker() == T_Linker_RAD && duplicates_exit_code != LNK_Error_MultiplyDefinedSymbol) { goto exit; }

  int good_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:b.exe a.obj entry.obj");
  if (good_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("b.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".test"));
  if (!sect) { goto exit; }
  string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
  if (!str8_match(data, ("a"), 0)) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_same_size(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("a"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_SameSize);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".b"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("b"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_SameSize);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".c"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("cc"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_SameSize);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("c.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int same_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe a.obj b.obj entry.obj");
  if (same_exit_code != 0) { goto exit; }

  {
    string             exe           = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".a"));
    if (sect == 0) { goto exit; }
    string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("a"), 0)) { goto exit; }
  }

  int not_same_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:b.exe a.obj b.obj c.obj entry.obj");
  if (not_same_exit_code == 0) { goto exit; }
  if (t_ident_linker() == T_Linker_RAD && not_same_exit_code != LNK_Error_MultiplyDefinedSymbol) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_exact_match(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("a"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_ExactMatch);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".a2"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("a"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_ExactMatch);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a2.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".b"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("b"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_ExactMatch);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int not_exact_match = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe entry.obj a.obj b.obj");
  if (not_exact_match == 0) { goto exit; }

  int exact_match = t_invoke_linkerf("/subsystem:console /entry:entry /out:b.exe entry.obj a2.obj a.obj");
  if (exact_match != 0) { goto exit; }

  {
    string             exe           = t_read_file(scratch.arena, ("b.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".a2"));
    if (sect == 0) { goto exit; }
    string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("a"), 0)) { goto exit; }
  }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_largest(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("a"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".b"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("bb"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".c"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("c"));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("c.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int largest_exit_code = t_invoke_linkerf("/subsystem:console /out:a.exe /entry:entry entry.obj a.obj b.obj");
  if (largest_exit_code != 0) { goto exit; }

  {
    string             exe           = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *discard_sect  = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".a"));
    if (discard_sect != 0) { goto exit; }
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".b"));
    if (sect == 0) { goto exit; }
    string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("bb"), 0)) { goto exit; }
  }

  int same_size_exit_code = t_invoke_linkerf("/subsystem:console /out:b.exe /entry:entry entry.obj c.obj a.obj");
  if (same_size_exit_code != 0) { goto exit; }

  {
    string             exe           = t_read_file(scratch.arena, ("b.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".c"));
    if (sect == 0) { goto exit; }
    string             data          = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));
    if (!str8_match(data, ("c"), 0)) { goto exit; }
  }

  result = T_Result_Pass;
exit:;
 scratch_end(scratch);
 return result;
}

internal T_Result
t_comdat_associative(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0,COFF_MachineType_X64);
    COFF_ObjSection *a = coff_obj_writer_push_section(obj_writer, ("a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("a"));
    COFF_ObjSection *aa = coff_obj_writer_push_section(obj_writer, ("aa"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("aa"));
    coff_obj_writer_push_symbol_secdef(obj_writer, a, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, a);
    coff_obj_writer_push_symbol_associative(obj_writer, aa, a);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0,COFF_MachineType_X64);
    COFF_ObjSection *bb = coff_obj_writer_push_section(obj_writer, ("bb"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("bb"));
    COFF_ObjSection *b = coff_obj_writer_push_section(obj_writer, ("b"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("b"));
    COFF_ObjSection *bbb = coff_obj_writer_push_section(obj_writer, ("bbb"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("bbb"));
    coff_obj_writer_push_symbol_secdef(obj_writer, bb, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_associative(obj_writer, b, bb);
    coff_obj_writer_push_symbol_associative(obj_writer, bbb, bb);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, bb);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe entry.obj a.obj b.obj");
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *a = t_coff_section_header_from_name(exe, section_table, pe.section_count, ("a"));
  COFF_SectionHeader *aa = t_coff_section_header_from_name(exe, section_table, pe.section_count, ("aa"));
  COFF_SectionHeader *b = t_coff_section_header_from_name(exe, section_table, pe.section_count, ("b"));
  COFF_SectionHeader *bb = t_coff_section_header_from_name(exe, section_table, pe.section_count, ("bb"));
  COFF_SectionHeader *bbb = t_coff_section_header_from_name(exe, section_table, pe.section_count, ("bbb"));
  if (a != 0) { goto exit; }
  if (aa != 0) { goto exit; }
  if (b == 0) { goto exit; }
  if (bb == 0) { goto exit; }
  if (bbb == 0) { goto exit; }
  string b_data = str8_substr(exe, rng_1u64(b->foff, b->foff + b->vsize));
  string bb_data = str8_substr(exe, rng_1u64(bb->foff, bb->foff + bb->vsize));
  string bbb_data = str8_substr(exe, rng_1u64(bbb->foff, bbb->foff + bbb->vsize));
  if (!str8_match(b_data, ("b"), 0)) { goto exit; }
  if (!str8_match(bb_data, ("bb"), 0)) { goto exit; }
  if (!str8_match(bbb_data, ("bbb"), 0)) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_associative_loop(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *aaaa = coff_obj_writer_push_section(obj_writer, (".aaaa"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("aaaa"));
    COFF_ObjSection *aa = coff_obj_writer_push_section(obj_writer, (".aa"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("aa"));
    COFF_ObjSection *a = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("a"));
    COFF_ObjSection *aaa = coff_obj_writer_push_section(obj_writer, (".aaa"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT|COFF_SectionFlag_Align1Bytes, ("aaa"));
    coff_obj_writer_push_symbol_associative(obj_writer, aaa, aa);
    coff_obj_writer_push_symbol_associative(obj_writer, aaaa, aaa);
    coff_obj_writer_push_symbol_associative(obj_writer, a, aa);
    coff_obj_writer_push_symbol_associative(obj_writer, aa, a);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("loop.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe loop.obj entry.obj");
  if (exit_code == 0) { goto exit; }
  if (t_ident_linker() == T_Linker_RAD && exit_code != LNK_Error_AssociativeLoop) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_associative_non_comdat(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *a = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS, ("a"));
    COFF_ObjSection *b = coff_obj_writer_push_section(obj_writer, (".b"), PE_DATA_SECTION_FLAGS, ("b"));
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, a);
    coff_obj_writer_push_symbol_associative(obj_writer, b, a);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("test.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe entry.obj test.obj");
  if (exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *a             = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".a"));
  COFF_SectionHeader *b             = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".b"));
  if (a == 0) { goto exit; }
  if (b == 0) { goto exit; }
  string             a_data        = str8_substr(exe, rng_1u64(a->foff, a->foff + a->vsize));
  string             b_data        = str8_substr(exe, rng_1u64(b->foff, b->foff + b->vsize));
  if (!str8_match(a_data, ("a"), 0)) { goto exit; }
  if (!str8_match(b_data, ("b"), 0)) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_associative_out_of_bounds(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0,COFF_MachineType_X64);
    COFF_ObjSection *a = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("a"));
    COFF_ObjSection *aa = coff_obj_writer_push_section(obj_writer, (".aa"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, ("aa"));
    coff_obj_writer_push_symbol_secdef(obj_writer, a, COFF_ComdatSelect_Any);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, a);
    coff_obj_writer_push_symbol_associative(obj_writer, aa, a);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    {
      COFF_FileHeaderInfo header = coff_file_header_info_from_data(obj);
      string string_table = str8_substr(obj, header.string_table_range);
      string symbol_table = str8_substr(obj, header.symbol_table_range);
      COFF_ParsedSymbol symbol = coff_parse_symbol(header, string_table, symbol_table, 3);
      AssertAlways(str8_match(symbol.name, (".aa"), 0));
      AssertAlways(symbol.aux_symbol_count == 1);
      COFF_Symbol16 *symbol16 = symbol.raw_symbol;
      COFF_SymbolSecDef *secdef = (COFF_SymbolSecDef *)(symbol16 + 1);
      secdef->number_lo = 321;
    }
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("bad.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe entry.obj bad.obj");
  if (linker_exit_code == 0) { goto exit; }
  if (t_ident_linker() == T_Linker_RAD && linker_exit_code != LNK_Error_IllData) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_comdat_with_offset(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 a[] = "1Hello, World!";
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".rdata"), PE_RDATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, str8_array_fixed(a));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 1, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 a[] = "Hello, World!";
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".rdata"), PE_RDATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, str8_array_fixed(a));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 1, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 3, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe a.obj b.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_reloc_against_removed_comdat(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 a[] = "1Hello, World!";
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".rdata"), PE_RDATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, str8_array_fixed(a));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 1, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 a[] = "H";
    COFF_ObjSection *comdat_sect = coff_obj_writer_push_section(obj_writer, (".rdata"), PE_RDATA_SECTION_FLAGS|COFF_SectionFlag_LnkCOMDAT, str8_array_fixed(a));
    coff_obj_writer_push_symbol_secdef(obj_writer, comdat_sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 1, comdat_sect);
    COFF_ObjSymbol *static_symbol = coff_obj_writer_push_symbol_static(obj_writer, ("STATIC"), 2, comdat_sect);

    U8 rdata[4] = {0};
    COFF_ObjSection *regular_sect = coff_obj_writer_push_section(obj_writer, (".rdata"), PE_RDATA_SECTION_FLAGS, str8_array_fixed(rdata));
    coff_obj_writer_section_push_reloc_voff(obj_writer, regular_sect, 0, static_symbol);

    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 3, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe a.obj c.obj entry.obj");
  if (linker_exit_code != LNK_Error_RelocationAgainstRemovedSection) { goto exit; }

exit:;
  result = T_Result_Pass;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_sect_align(void)
{
  Temp scratch = scratch_begin(0,0);

  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    COFF_ObjSection *sect_align_shift = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS, ("q"));
    COFF_ObjSection *sect_align_none = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS, ("abc"));
    COFF_ObjSection *sect_align_1 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align1Bytes, ("wr"));
    COFF_ObjSection *sect_align_2 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align2Bytes, ("e"));
    COFF_ObjSection *sect_align_4 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align4Bytes, ("ttttt"));
    COFF_ObjSection *sect_align_8 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align8Bytes, ("g"));
    COFF_ObjSection *sect_align_16 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align16Bytes, ("o"));
    COFF_ObjSection *sect_align_32 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align32Bytes, ("p"));
    COFF_ObjSection *sect_align_64 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align64Bytes, ("f"));
    COFF_ObjSection *sect_align_128 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align128Bytes, ("x"));
    COFF_ObjSection *sect_align_256 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align256Bytes, ("c"));
    COFF_ObjSection *sect_align_512 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align512Bytes, ("v"));
    COFF_ObjSection *sect_align_1024 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align1024Bytes, ("b"));
    COFF_ObjSection *sect_align_2048 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align2048Bytes, ("n"));
    COFF_ObjSection *sect_align_4096 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align4096Bytes, ("m"));
    COFF_ObjSection *sect_align_8192 = coff_obj_writer_push_section(obj_writer, (".a"), PE_DATA_SECTION_FLAGS|COFF_SectionFlag_Align8192Bytes, ("z"));

    U8 text[] = { 0xC3 };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("my_entry"), 0, text_sect);

    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("test.obj"), obj)) {
      goto exit;
    }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:my_entry /out:a.exe /align:8192 test.obj");

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  COFF_SectionHeader *sect = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".a"));
  if (!sect) {
    goto exit;
  }
  string sect_data = str8_substr(exe, rng_1u64(sect->foff, sect->foff + sect->vsize));

  string shift = str8_substr(sect_data, rng_1u64(0, 1));
  if (!str8_match(shift, ("q"), 0)) {
    goto exit;
  }
  string a_none = str8_substr(sect_data, rng_1u64(16, 16 + 3));
  if (!str8_match(a_none, ("abc"), 0)) {
    goto exit;
  }
  string a_1 = str8_substr(sect_data, rng_1u64(19, 21));
  if (!str8_match(a_1, ("wr"), 0)) {
    goto exit;
  }
  string a_2 = str8_substr(sect_data, rng_1u64(22, 23));
  if (!str8_match(a_2, ("e"), 0)) {
    goto exit;
  }
  string a_4 = str8_substr(sect_data, rng_1u64(24, 29));
  if (!str8_match(a_4, ("ttttt"), 0)) {
    goto exit;
  }
  string a_8 = str8_substr(sect_data, rng_1u64(32, 33));
  if (!str8_match(a_8, ("g"), 0)) {
    goto exit;
  }
  string a_16 = str8_substr(sect_data, rng_1u64(48, 49));
  if (!str8_match(a_16, ("o"), 0)) {
    goto exit;
  }
  string a_32 = str8_substr(sect_data, rng_1u64(64, 65));
  if (!str8_match(a_32, ("p"), 0)) {
    goto exit;
  }
  string a_64 = str8_substr(sect_data, rng_1u64(128, 129));
  if (!str8_match(a_64, ("f"), 0)) {
    goto exit;
  }
  string a_128 = str8_substr(sect_data, rng_1u64(256, 257));
  if (!str8_match(a_128, ("x"), 0)) {
    goto exit;
  }
  string a_256 = str8_substr(sect_data, rng_1u64(512, 513));
  if (!str8_match(a_256, ("c"), 0)) {
    goto exit;
  }
  string a_512 = str8_substr(sect_data, rng_1u64(1024, 1025));
  if (!str8_match(a_512, ("v"), 0)) {
    goto exit;
  }
  string a_1024 = str8_substr(sect_data, rng_1u64(2048, 2049));
  if (!str8_match(a_1024, ("b"), 0)) {
    goto exit;
  }
  string a_2048 = str8_substr(sect_data, rng_1u64(4096, 4097));
  if (!str8_match(a_2048, ("n"), 0)) {
    goto exit;
  }
  string a_4096 = str8_substr(sect_data, rng_1u64(8192, 8193));
  if (!str8_match(a_4096, ("m"), 0)) {
    goto exit;
  }
  string a_8192 = str8_substr(sect_data, rng_1u64(16384, 16385));
  if (!str8_match(a_8192, ("z"), 0)) {
    goto exit;
  }

  result = T_Result_Pass;

exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_alt_name(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = t_push_data_section(obj_writer, ("test"));
    coff_obj_writer_push_symbol_extern(obj_writer, ("test"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("test.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = t_push_data_section(obj_writer, ("foo"));
    coff_obj_writer_push_symbol_extern(obj_writer, ("foo"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("foo.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("foo"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) { goto exit; }
  }

  int linker_exit_code;

  // basic alternate name test
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /alternatename:foo=test test.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  // linker should not chase alt name links
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:b.exe /alternatename:foo=bar /alternatename:bar=test test.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // alt name conflict
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:c.exe /alternatename:foo=test /alternatename:foo=qwe test.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // syntax error
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:d.exe /alternatename:foo foo.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }
  
  // syntax error
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:e.exe /alternatename:foo-oof foo.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // syntax error
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /alternatename:foo=test=bar foo.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // syntax error
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /alternatename:foo= foo.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // syntax error
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /alternatename:= foo.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // syntax error
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /alternatename: foo.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  // TODO: check that RAD Linker prints these warnings

  // warn about alt name to self alt name?
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:f.exe /alternatename:foo=foo foo.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  // warn about alt name to unknown symbol?
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:g.exe /alternatename:qwe=ewq foo.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_include(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = t_push_data_section(obj_writer, ("foo"));
    coff_obj_writer_push_symbol_extern(obj_writer, ("foo"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("include.obj"), obj)) { goto exit; }

    COFF_LibWriter *lib_writer = coff_lib_writer_alloc();
    coff_lib_writer_push_obj(lib_writer, ("include.obj"), obj);
    String8List lib = coff_lib_writer_serialize(scratch.arena, lib_writer, 0, 0, 1);
    coff_lib_writer_release(&lib_writer);
    if (!t_write_file_list(("include.lib"), lib)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) { goto exit; }
  }

  int linker_exit_code;

  // simple include test
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /include:foo entry.obj include.lib");
  if (linker_exit_code != 0) { goto exit; }

  // validate that linker pulled-in include.obj
  {
    string             exe           = t_read_file(scratch.arena, ("a.exe"));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *foo_sect      = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
    if (foo_sect == 0) { goto exit; }
    string             foo_data      = str8_substr(exe, rng_1u64(foo_sect->foff, foo_sect->foff + foo_sect->vsize));
    if (!str8_match(foo_data, ("foo"), 0)) { goto exit; }
  }

  // test unresolved include
  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /include:ewq entry.obj");
  if (linker_exit_code == 0) { goto exit; }
  if (t_ident_linker() == T_Linker_RAD && linker_exit_code != LNK_Error_UnresolvedSymbol) { goto exit; }

  result = T_Result_Pass;
  exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_communal_var_vs_regular(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_common(obj_writer, ("TEST"), 1);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("communal.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = t_push_data_section(obj_writer, ("test"));
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("defn.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  // linker should replace communal TEST with .data TEST
  int linker_exit_code;

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe communal.obj defn.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:b.exe defn.obj communal.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  char *exes[] = { "a.exe", "b.exe" };
  for (U64 i = 0; i < ArrayCount(exes); i += 1) {
    string             exe           = t_read_file(scratch.arena, str8_cstring(exes[i]));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *data_sect     = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
    if (!data_sect) { goto exit; }
    string             data          = str8_substr(exe, rng_1u64(data_sect->foff, data_sect->foff + data_sect->vsize));
    if (!str8_match(data, ("test"), 0)) { goto exit; }
  }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_communal_var_vs_regular_comdat(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    coff_obj_writer_push_symbol_common(obj_writer, ("TEST"), 1);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("communal.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *sect = t_push_data_section(obj_writer, ("test"));
    sect->flags |= COFF_SectionFlag_LnkCOMDAT;
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("large.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    COFF_ObjSymbol *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) {
      goto exit;
    }
  }

  // linker should replace communal TEST with .data TEST
  int linker_exit_code;

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe communal.obj large.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:b.exe large.obj communal.obj entry.obj");
  if (linker_exit_code != 0) { goto exit; }

  char *exes[] = { "a.exe", "b.exe" };
  for (U64 i = 0; i < ArrayCount(exes); i += 1) {
    string             exe           = t_read_file(scratch.arena, str8_cstring(exes[i]));
    PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
    COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
    string             string_table  = str8_substr(exe, pe.string_table_range);
    COFF_SectionHeader *data_sect     = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".data"));
    if (!data_sect) { goto exit; }
    string             data          = str8_substr(exe, rng_1u64(data_sect->foff, data_sect->foff + data_sect->vsize));
    if (!str8_match(data, ("test"), 0)) { goto exit; }
  }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_import_kernel32(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 data[] = "test";
    U8 text[] = {
      0x48, 0x83, 0xec, 0x68,                               // sub  rsp,68h                        ; alloc space on stack
      0xc7, 0x44, 0x24, 0x48, 0x18, 0x00, 0x00, 0x00,       // mov  dword ptr [rsp+48h],18h        ; SECURITY_ATTRIBUTES.nLength
      0x48, 0xc7, 0x44, 0x24, 0x50, 0x00, 0x00, 0x00, 0x00, // mov  qword ptr [rsp+50h],0          ; SECURITY_ATTRIBUTES.lpSecurityDescriptor
      0xc7, 0x44, 0x24, 0x58, 0x00, 0x00, 0x00, 0x00,       // mov  dword ptr [rsp+58h],0          ; SECURITY_ATTRIBUTES.bInheritHandle
      0x48, 0xc7, 0x44, 0x24, 0x30, 0x00, 0x00, 0x00, 0x00, // mov  qword ptr [rsp+30h],0          ; hTemplateFile
      0xc7, 0x44, 0x24, 0x28, 0x80, 0x00, 0x00, 0x00,       // mov  dword ptr [rsp+28h],80h        ; dwFlagsAndAttributes
      0xc7, 0x44, 0x24, 0x20, 0x02, 0x00, 0x00, 0x00,       // mov  dword ptr [rsp+20h],2          ; dwCreationDisposition
      0x4c, 0x8d, 0x4c, 0x24, 0x48,                         // lea  r9,[rsp+48h]                   ; lpSecurityAttributes
      0x45, 0x33, 0xc0,                                     // xor  r8d,r8d                        ; dwShareMode
      0xba, 0x00, 0x00, 0x00, 0x40,                         // mov  edx,40000000h                  ; dwDesiredAccess
      0x48, 0x8d, 0x0d, 0x00, 0x00, 0x00, 0x00,             // lea  rcx,[test]                     ; lpFileName
      0xff, 0x15, 0x00, 0x00, 0x00, 0x00,                   // call qword ptr [__imp_CreateFileA]  ; call CreateFileA
      0x48, 0x89, 0xc1,                                     // mov  rcx,rax                        ; hObject
      0xff, 0x15, 0x00, 0x00, 0x00, 0x00,                   // call qword ptr [__imp_CloseHandle]  ; call CloseHandle
      0x33, 0xc0,                                           // xor  eax,eax                        ; clear result
      0x48, 0x83, 0xc4, 0x68,                               // add  rsp,68h                        ; dealloc stack
      0xc3                                                  // ret                                 ; return
    };
    COFF_ObjSection *data_sect = coff_obj_writer_push_section(obj_writer, (".data"), PE_DATA_SECTION_FLAGS, str8_array_fixed(data));
    COFF_ObjSection *text_sect = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    COFF_ObjSymbol *data_symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("test"), 0, data_sect);
    COFF_ObjSymbol *entry_symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, text_sect);
    COFF_ObjSymbol *create_file_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("__imp_CreateFileA"));
    COFF_ObjSymbol *close_handle_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("__imp_CloseHandle"));
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 70, data_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 76, create_file_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 85, close_handle_symbol, COFF_Reloc_X64_Rel32);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("import.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /fixed import.obj kernel32.lib");
  if (linker_exit_code != 0) { goto exit; }

#if OS_WINDOWS
  {
    string test_file_path = push_str8f(scratch.arena, "%S/test", g_wdir);
    os_delete_file_at_path(test_file_path);

    OS_ProcessLaunchParams launch_opts = {0};
    launch_opts.inherit_env = 0;
    launch_opts.path = g_wdir;
    str8_list_pushf(scratch.arena, &launch_opts.cmd_line, "%S/a.exe", g_wdir);
    OS_Handle handle = os_process_launch(&launch_opts);
    AssertAlways(!os_handle_match(handle, os_handle_zero()));
    int exit_code = -1;
    os_process_join_exit_code(handle, max_U64, &exit_code);
    os_process_detach(handle);
    if (exit_code != 0) { goto exit; }

    if (!os_file_path_exists(test_file_path)) { goto exit; }
  }
#endif

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_delay_import(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 return_0[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, // mov rax, 0
      0xc3 // ret
    };
    U8 return_1[] = {
      0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, // mov rax, 1
      0xc3 // ret
    };
    U8 return_2[] = {
      0x48, 0xC7, 0xC0, 0x02, 0x00, 0x00, 0x00, // mov rax, 2
      0xc3 // ret
    };
    COFF_ObjSection *return_0_sect = t_push_text_section(obj_writer, str8_array_fixed(return_0));
    COFF_ObjSection *return_1_sect = t_push_text_section(obj_writer, str8_array_fixed(return_1));
    COFF_ObjSection *return_2_sect = t_push_text_section(obj_writer, str8_array_fixed(return_2));
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("return_1"), 0, return_1_sect);
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("return_2"), 0, return_2_sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("a.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 return_0[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, // mov rax, 0
      0xc3 // ret
    };
    U8 return_123[] = {
      0x48, 0xC7, 0xC0, 0x7B, 0x00, 0x00, 0x00, // mov rax, 123
      0xc3 // ret
    };
    U8 return_321[] = {
      0x48, 0xC7, 0xC0, 0x41, 0x01, 0x00, 0x00, // mov rax, 321 
      0xc3 // ret
    };
    COFF_ObjSection *return_0_sect = t_push_text_section(obj_writer, str8_array_fixed(return_0));
    COFF_ObjSection *return_123_sect = t_push_text_section(obj_writer, str8_array_fixed(return_123));
    COFF_ObjSection *return_321_sect = t_push_text_section(obj_writer, str8_array_fixed(return_321));
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("return_123"), 0, return_123_sect);
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("return_321"), 0, return_321_sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("b.obj"), obj)) { goto exit; }
  }

  {
    U8 text[] = {
      0x56,                         // push  rsi
      0x57,                         // push  rdi
      0x48, 0x83, 0xEC, 0x28,       // sub   rsp,28h
      0xE8, 0x00, 0x00, 0x00, 0x00, // call  return_1
      0x89, 0xC6,                   // mov   esi,eax
      0xE8, 0x00, 0x00, 0x00, 0x00, // call  return_2
      0x89, 0xC7,                   // mov   edi,eax
      0x01, 0xF7,                   // add   edi,esi
      0xE8, 0x00, 0x00, 0x00, 0x00, // call  return_123
      0x89, 0xC6,                   // mov   esi,eax
      0xE8, 0x00, 0x00, 0x00, 0x00, // call  return_321
      0x01, 0xF0,                   // add   eax,esi
      0x01, 0xF8,                   // add   eax,edi
      0x48, 0x83, 0xC4, 0x28,       // add   rsp,28h
      0x5F,                         // pop   rdi
      0x5E,                         // pop   rsi
      0xC3,                         // ret
    };
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0,COFF_MachineType_X64);
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, text_sect);
    COFF_ObjSymbol *return_1_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("return_1"));
    COFF_ObjSymbol *return_2_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("return_2"));
    COFF_ObjSymbol *return_123_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("return_123"));
    COFF_ObjSymbol *return_321_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("return_321"));
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 7, return_1_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 14, return_2_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 23, return_123_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 30, return_321_symbol, COFF_Reloc_X64_Rel32);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("main.obj"), obj)) { goto exit; }
  }

  int linker_exit_code;

  linker_exit_code = t_invoke_linkerf("/dll /implib:a.lib /export:return_1 /export:return_2 a.obj libcmt.lib");
  if (linker_exit_code != 0) { goto exit; }

  linker_exit_code = t_invoke_linkerf("/dll /implib:b.lib /export:return_123 /export:return_321 b.obj libcmt.lib");
  if (linker_exit_code != 0) { goto exit; }

  linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe /fixed /debug:full main.obj a.lib b.lib kernel32.lib delayimp.lib libcmt.lib /delayload:a.dll /delayload:b.dll");
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);

  PE_ParsedDelayImportTable delay_import_table = pe_delay_imports_from_data(scratch.arena, pe.is_pe32, pe.section_count, section_table, exe, pe.data_dir_franges[PE_DataDirectoryIndex_DELAY_IMPORT]);

  PE_ParsedDelayDLLImport *a_import = &delay_import_table.v[0];
  if (a_import->attributes != 1)                         { goto exit; }
  if (!str8_match(a_import->name, ("a.dll"), 0)) { goto exit; }
  if (a_import->module_handle_voff == 0)                 { goto exit; }
  if (a_import->name_table_voff == 0)                    { goto exit; }
  if (a_import->bound_table_voff != 0)                   { goto exit; }
  if (a_import->unload_table_voff != 0)                  { goto exit; }
  if (a_import->time_stamp != 0)                         { goto exit; }
  if (a_import->bound_table_count != 0)                  { goto exit; }
  if (a_import->unload_table_count != 0)                 { goto exit; }
  if (a_import->import_count != 2)                       { goto exit; }

  PE_ParsedImport *return_1 = &a_import->imports[0];
  if (return_1->type != PE_ParsedImport_Name)                        { goto exit; }
  if (!str8_match(return_1->u.name.string, ("return_1"), 0)) { goto exit; }
  if (return_1->u.name.hint != 0)                                    { goto exit; }

  PE_ParsedImport *return_2 = &a_import->imports[1];
  if (return_2->type != PE_ParsedImport_Name)                        { goto exit; }
  if (!str8_match(return_2->u.name.string, ("return_2"), 0)) { goto exit; }
  if (return_2->u.name.hint != 1)                                    { goto exit; }

  PE_ParsedDelayDLLImport *b_import = &delay_import_table.v[1];
  if (b_import->attributes != 1)                         { goto exit; }
  if (!str8_match(b_import->name, ("b.dll"), 0)) { goto exit; }
  if (b_import->module_handle_voff == 0)                 { goto exit; }
  if (b_import->name_table_voff == 0)                    { goto exit; }
  if (b_import->bound_table_voff != 0)                   { goto exit; }
  if (b_import->unload_table_voff != 0)                  { goto exit; }
  if (b_import->time_stamp != 0)                         { goto exit; }
  if (b_import->bound_table_count != 0)                  { goto exit; }
  if (b_import->unload_table_count != 0)                 { goto exit; }
  if (b_import->import_count != 2)                       { goto exit; }

  PE_ParsedImport *return_123 = &b_import->imports[0];
  if (return_123->type != PE_ParsedImport_Name)                          { goto exit; }
  if (!str8_match(return_123->u.name.string, ("return_123"), 0)) { goto exit; }
  if (return_123->u.name.hint != 0)                                      { goto exit; }

  PE_ParsedImport *return_321 = &b_import->imports[1];
  if (return_321->type != PE_ParsedImport_Name)                          { goto exit; }
  if (!str8_match(return_321->u.name.string, ("return_321"), 0)) { goto exit; }
  if (return_321->u.name.hint != 1)                                      { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_delay_import_user32(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *data_sect = coff_obj_writer_push_section(obj_writer, (".str"), PE_DATA_SECTION_FLAGS, str8_zero());
    U64 msg_off = data_sect->data.total_size;
    str8_list_pushf(obj_writer->arena, &data_sect->data, "test\0");
    U64 caption_off = data_sect->data.total_size;
    str8_list_pushf(obj_writer->arena, &data_sect->data, "foo\0");
    COFF_ObjSymbol *msg_symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("msg"), msg_off, data_sect);
    COFF_ObjSymbol *caption_symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("caption"), caption_off, data_sect);

    U8 text[] = {
      0x48, 0x83, 0xEC, 0x28,                   // sub  rsp,28h
      0x45, 0x33, 0xC9,                         // xor  r9d,r9d
      0x4C, 0x8D, 0x05, 0x00, 0x00, 0x00, 0x00, // lea  r8,[msg]
      0x48, 0x8D, 0x15, 0x00, 0x00, 0x00, 0x00, // lea  rdx,[caption]
      0x33, 0xC9,                               // xor  ecx,ecx
      0xFF, 0x15, 0x00, 0x00, 0x00, 0x00,       // call qword ptr [__imp_MessageBoxA]
      0x33, 0xC0,                               // xor  eax,eax
      0x48, 0x83, 0xC4, 0x28,                   // add  rsp,28h
      0xC3,                                     // ret
    };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    COFF_ObjSymbol *text_symbol = coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, text_sect);
    COFF_ObjSymbol *message_box_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("__imp_MessageBoxA"));
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 10, msg_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 17, caption_symbol, COFF_Reloc_X64_Rel32);
    coff_obj_writer_section_push_reloc(obj_writer, text_sect, 25, message_box_symbol, COFF_Reloc_X64_Rel32);

    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("delay_import.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /out:a.exe /entry:entry /fixed /delayload:user32.dll kernel32.lib user32.lib libcmt.lib delayimp.lib delay_import.obj /debug:full");
  if (linker_exit_code != 0) { goto exit; }

  result = T_Result_Pass;
exit:;
 scratch_end(scratch);
 return result;
}

internal T_Result
t_empty_section(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    COFF_ObjSection *test = coff_obj_writer_push_section(obj_writer, (".test"), PE_TEXT_SECTION_FLAGS, str8(0,0));
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, test);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("empty_section.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    COFF_ObjSymbol *test_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, text_sect, 3, test_symbol); // relocation against removed section
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, text_sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe empty_section.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_removed_section(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 test_text[] = { 0xc3 };
    COFF_ObjSection *test_sect = coff_obj_writer_push_section(obj_writer, (".test"), PE_TEXT_SECTION_FLAGS | COFF_SectionFlag_LnkRemove, str8_array_fixed(test_text));
    coff_obj_writer_push_symbol_extern(obj_writer, ("TEST"), 0, test_sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("test.obj"), obj)) { goto exit; }
  }

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *text_sect = t_push_text_section(obj_writer, str8_array_fixed(text));
    COFF_ObjSymbol *test_symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("TEST"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, text_sect, 3, test_symbol); // relocation against removed section
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, text_sect);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("entry.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe test.obj entry.obj");
  if (linker_exit_code == 0) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_function_pad_min(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 ret[] = { 0xc3 };
    COFF_ObjSection *text_sect_0 = t_push_text_section(obj_writer, str8_array_fixed(ret));
    COFF_ObjSection *text_sect_1 = t_push_text_section(obj_writer, str8_array_fixed(ret));
    COFF_ObjSection *text_sect_2 = t_push_text_section(obj_writer, str8_array_fixed(ret));
    text_sect_0->flags |= COFF_SectionFlag_Align4Bytes;
    text_sect_1->flags |= COFF_SectionFlag_Align2Bytes;
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("A"), 0, text_sect_0);
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("B"), 0, text_sect_1);
    coff_obj_writer_push_symbol_extern_func(obj_writer, ("C"), 0, text_sect_2);
    string obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
    if (!t_write_file(("funcs.obj"), obj)) { goto exit; }
  }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:A /functionpadmin:1 /out:a.exe funcs.obj");
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *text_sect     = t_coff_section_header_from_name(string_table, section_table, pe.section_count, (".text"));
  if (text_sect == 0) { goto exit; }
  string             text_data     = str8_substr(exe, rng_1u64(text_sect->foff, text_sect->foff + text_sect->vsize));

  U8 expected_text[] = {
    0xcc, 0xcc, 0xcc, 0xcc, 0xc3, 
    0xcc, 0xcc, 0xcc, 0xc3, 
    0xcc, 0xc3, 
  };
  if (!str8_match(text_data, str8_array_fixed(expected_text), 0)) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

internal T_Result
t_opt_ref_dangling_section(void)
{
  Temp scratch = scratch_begin(0,0);
  T_Result result = T_Result_Fail;

  string entry_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);
    U8 text[] = {
      0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,  // mov rax, $imm
      0xC3 // ret
    };
    COFF_ObjSection *sect   = coff_obj_writer_push_section(obj_writer, (".text"), PE_TEXT_SECTION_FLAGS, str8_array_fixed(text));
    COFF_ObjSymbol  *symbol = coff_obj_writer_push_symbol_undef(obj_writer, ("f"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, symbol);
    coff_obj_writer_push_symbol_extern(obj_writer, ("entry"), 0, sect);
    entry_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string a_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    U8 data[] = "A0000";
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".data"), PE_DATA_SECTION_FLAGS | COFF_SectionFlag_LnkCOMDAT, str8_array_fixed(data));

    COFF_ObjSymbol *q = coff_obj_writer_push_symbol_undef(obj_writer, ("q"));
    coff_obj_writer_section_push_reloc_voff(obj_writer, sect, 0, q);

    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("f"), 0, sect);
    a_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  string b_obj;
  {
    COFF_ObjWriter *obj_writer = coff_obj_writer_alloc(0, COFF_MachineType_X64);

    U8 q[] = { 1,2,3,4};
    COFF_ObjSection *q_sect = coff_obj_writer_push_section(obj_writer, (".q"), PE_DATA_SECTION_FLAGS, str8_array_fixed(q));
    coff_obj_writer_push_symbol_extern(obj_writer, ("q"), 0, q_sect);

    U8 data[] = "BBBBBBBBBBBBBBB";
    COFF_ObjSection *sect = coff_obj_writer_push_section(obj_writer, (".data"), PE_DATA_SECTION_FLAGS | COFF_SectionFlag_LnkCOMDAT, str8_array_fixed(data));
    coff_obj_writer_push_symbol_secdef(obj_writer, sect, COFF_ComdatSelect_Largest);
    coff_obj_writer_push_symbol_extern(obj_writer, ("f"), 0, sect);

    b_obj = coff_obj_writer_serialize(scratch.arena, obj_writer);
    coff_obj_writer_release(&obj_writer);
  }

  String8List b_lib;
  {
    COFF_LibWriter *lib_writer = coff_lib_writer_alloc();
    coff_lib_writer_push_obj(lib_writer, ("b.obj"), b_obj);
    b_lib = coff_lib_writer_serialize(scratch.arena, lib_writer, 0, 0, 1);
    coff_lib_writer_release(&lib_writer);
  }

  if (!t_write_file(("entry.obj"), entry_obj)) { goto exit; }
  if (!t_write_file(("a.obj"), a_obj))         { goto exit; }
  if (!t_write_file_list(("b.lib"), b_lib))    { goto exit; }

  int linker_exit_code = t_invoke_linkerf("/subsystem:console /entry:entry /out:a.exe entry.obj a.obj b.lib");
  if (linker_exit_code != 0) { goto exit; }

  string             exe           = t_read_file(scratch.arena, ("a.exe"));
  PE_BinInfo          pe            = pe_bin_info_from_data(scratch.arena, exe);
  COFF_SectionHeader *section_table = (COFF_SectionHeader *)str8_substr(exe, pe.section_table_range).str;
  string             string_table  = str8_substr(exe, pe.string_table_range);
  COFF_SectionHeader *sect          = t_coff_section_header_from_name(exe, section_table, pe.section_count, (".q"));
  if (sect != 0) { goto exit; }

  result = T_Result_Pass;
exit:;
  scratch_end(scratch);
  return result;
}

////////////////////////////////////////////////////////////////

internal void
entry_point(CmdLine *cmdline)
{
  Temp scratch = scratch_begin(0,0);

  //
  // Test Targets
  //
  static struct {
    char *label; T_Result (*r)(void);
  } target_array[] = {
    { "simple_link_test",                 t_simple_link_test                 },
    { "machine_compat_check",             t_machine_compat_check             },
    { "out_of_bounds_section_number",     t_out_of_bounds_section_number     },
    { "merge",                            t_merge                            },
    { "undef_section",                    t_undef_section                    },
    { "undef_reloc_section",              t_undef_reloc_section              },
    { "weak_vs_weak",                     t_weak_vs_weak                     },
    { "weak_vs_common",                   t_weak_vs_common                   },
    { "abs_vs_weak",                      t_abs_vs_weak                      },
    { "abs_vs_regular",                   t_abs_vs_regular                   },
    { "abs_vs_common",                    t_abs_vs_common                    },
    { "abs_vs_abs",                       t_abs_vs_abs                       },
    { "undef_weak",                       t_undef_weak                       },
    { "sect_symbol",                      t_sect_symbol                      },
    { "weak_cycle",                       t_weak_cycle                       },
    { "weak_tag",                         t_weak_tag                         },
    { "find_merged_pdata",                t_find_merged_pdata                },
    { "section_sort",                     t_section_sort                     },
    { "flag_conf",                        t_flag_conf                        },
    { "invalid_bss",                      t_invalid_bss                      },
    { "common_block",                     t_common_block                     },
    { "base_relocs",                      t_base_relocs                      },
    { "simple_lib_test",                  t_simple_lib_test                  },
    { "sect_align",                       t_sect_align                       },
    { "image_base",                       t_image_base                       },
    { "comdat_any",                       t_comdat_any                       },
    { "comdat_no_duplicates",             t_comdat_no_duplicates             },
    { "comdat_same_size",                 t_comdat_same_size                 },
    { "comdat_exact_match",               t_comdat_exact_match               },
    { "comdat_largest",                   t_comdat_largest                   },
    { "comdat_associative",               t_comdat_associative               },
    { "comdat_associative_loop",          t_comdat_associative_loop          },
    { "comdat_associative_non_comdat",    t_comdat_associative_non_comdat    },
    { "comdat_associative_out_of_bounds", t_comdat_associative_out_of_bounds },
    { "comdat_with_offset",               t_comdat_with_offset               },
    { "reloc_against_removed_comdat",     t_reloc_against_removed_comdat     },
    { "alt_name",                         t_alt_name                         },
    { "include",                          t_include                          },
    { "communal_var_vs_regular_comdat",   t_communal_var_vs_regular_comdat   },
    { "communal_var_vs_regular",          t_communal_var_vs_regular          },
    { "import_kernel32",                  t_import_kernel32                  },
    { "delay_import_user32",              t_delay_import_user32              },
    { "delay_import",                     t_delay_import                     },
    { "empty_section",                    t_empty_section                    },
    { "removed_section",                  t_removed_section                  },
    { "function_pad_min",                 t_function_pad_min                 },
    //{ "opt_ref_dangling_section",         t_opt_ref_dangling_section         },
    //{ "import_export",        t_import_export        },
  };

  //
  // Handle -help
  //
  {
    B32 print_help = cmd_line_has_flag(cmdline, ("help")) ||
                     cmd_line_has_flag(cmdline, ("h"));
     if (print_help) {
      fprintf(stderr, "--- Help -----------------------------------------------------------------------\n");
      fprintf(stderr, " %s\n\n", BUILD_TITLE_STRING_LITERAL);
      fprintf(stderr, " Usage: torture [Options] [Files]\n\n");
      fprintf(stderr, " Options:\n");
      fprintf(stderr, "   -linker:{path}        Path to PE/COFF linker\n");
      fprintf(stderr, "   -target:{name[,name]} Selects targets to test\n");
      fprintf(stderr, "   -list                 Print available test targets and exit\n");
      fprintf(stderr, "   -out:{path}           Directory path for test outputs (default \"%.*s\")\n", str8_varg(g_out));
      fprintf(stderr, "   -verbose              Enable verbose mode\n");
      fprintf(stderr, "   -print_stdout         Print to console stdout and stderr of a run\n");
      fprintf(stderr, "   -help                 Print help menu and exit\n");
      os_abort(0);
    }
  }

  //
  // Handle -list
  //
  {
    if (cmd_line_has_flag(cmdline, ("list"))) {
      fprintf(stdout, "--- Targets --------------------------------------------------------------------\n");
      for (U64 i = 0; i < ArrayCount(target_array); i += 1) {
        fprintf(stdout, "  %s\n", target_array[i].label);
      }
      os_abort(0);
    }
  }


  //
  // Handle -linker
  //
  {
    CmdLineOpt *linker_opt = cmd_line_opt_from_string(cmdline, ("linker"));
    if (linker_opt == 0) {
      linker_opt = cmd_line_opt_from_string(cmdline, ("l"));
    }
    if (linker_opt) {
      if (linker_opt->value_strings.node_count == 1) {
        g_linker = linker_opt->value_string;
      } else {
        fprintf(stderr, "ERROR: -linker has invalid number of arguments\n");
        os_abort(1);
      }
    } else {
      // assume default linker
      g_linker = ("radlink");
    }
  }

  //
  // Handle optional -target
  //
  String8List target = cmdline->inputs;
  {
    CmdLineOpt *target_opt = cmd_line_opt_from_string(cmdline, ("target"));
    if (target_opt == 0) {
      target_opt = cmd_line_opt_from_string(cmdline, ("t"));
    }
    if (target_opt) {
      if (target_opt->value_strings.node_count > 0) {
        str8_list_concat_in_place(&target, &target_opt->value_strings);
      } else {
        fprintf(stderr, "ERROR: -target has invalid number of arguments\n");
      }
    }
  }

  //
  // Handle -out
  //
  {
    CmdLineOpt *out_opt = cmd_line_opt_from_string(cmdline, ("out"));
    if (out_opt) {
      if (out_opt->value_strings.node_count == 1) {
        g_out = out_opt->value_string;
      } else {
        fprintf(stderr, "ERROR: -out invalid number of arguments");
      }
    }
  }

  //
  // Handle -verbose
  //
  {
    g_verbose = cmd_line_has_flag(cmdline, ("verbose"));
  }

  //
  // Handle -print_stdout
  //
  {
    g_redirect_stdout = !cmd_line_has_flag(cmdline, ("print_stdout"));
  }

  //
  // Make Output Directory
  //
  os_make_directory(g_out);
  if (!os_folder_path_exists(g_out)) {
    fprintf(stderr, "ERROR: unable to create output directory \"%.*s\"\n", str8_varg(g_out));
    os_abort(1);
  }

  //
  // Clean up output from previous run
  //
  os_delete_file_at_path(g_stdout_file_name);

  //
  // Run Test Targets
  //
  {
    U64 max_label_size = 0;
    for (U64 i = 0; i < ArrayCount(target_array); i += 1) { max_label_size = Max(max_label_size, cstring8_length(target_array[i].label)); }

    U64 dots_min = 10;
    U64 dots_size = max_label_size+dots_min;
    U8 *dots      = push_array(scratch.arena, U8, dots_size);
    MemorySet(dots, '.', dots_size);

    U64  target_indices_count;
    U64 *target_indices;
    if (target.node_count == 0) {
      target_indices_count = ArrayCount(target_array);
      target_indices       = push_array(scratch.arena, U64, ArrayCount(target_array));
      for (U64 i = 0; i < target_indices_count; i += 1) { target_indices[i] = i; }
    } else {
      target_indices_count = 0;
      target_indices       = push_array(scratch.arena, U64, target.node_count);

      for (String8Node *target_n = target.first; target_n != 0; target_n = target_n->next) {
        B32 is_target_unknown = 1;
        for (U64 i = 0; i < ArrayCount(target_array); i += 1) {
          if (str8_match(str8_cstring(target_array[i].label), target_n->string, 0)) {
            target_indices[target_indices_count++] = i;
            is_target_unknown = 0;
            break;
          }
        }
        if (is_target_unknown) {
          fprintf(stderr, "ERROR: unknown target \"%.*s\"\n", str8_varg(target_n->string));
        }
      }
    }

    U64 pass_count  = 0;
    U64 fail_count  = 0;
    U64 crash_count = 0;
    for (U64 i = 0; i < target_indices_count; i += 1) {
      U64 target_idx = target_indices[i];

      g_wdir = push_str8f(scratch.arena, "%S\\%s", g_out, target_array[target_idx].label);
      g_wdir = os_full_path_from_path(scratch.arena, g_wdir);
      os_make_directory(g_wdir);
      if (!os_folder_path_exists(g_out)) {
        fprintf(stderr, "ERROR: unable to create output directory for test run %.*s\n", str8_varg(g_wdir));
        continue;
      }

      T_Result result = t_run(target_array[target_idx].r);

      U64 dots_count = (max_label_size - cstring8_length(target_array[target_idx].label)) + dots_min;
      string msg = push_str8f(scratch.arena, "%s%.*s%s", target_array[target_idx].label, dots_count, dots, t_string_from_result(result));

      // run progress
      fprintf(stdout, "[%2llu/%2llu] ", i+1, target_indices_count);

      if (result == T_Result_Pass) {
        fprintf(stdout, "\x1b[32m" "%.*s" "\x1b[0m" "\n", str8_varg(msg));
        pass_count += 1;
      } else if (result == T_Result_Fail) {
        fprintf(stdout, "\x1b[31m" "%.*s" "\x1b[0m" "\n", str8_varg(msg));
        fail_count += 1;
      } else if (result == T_Result_Crash) {
        fprintf(stdout, "\x1b[33m" "%.*s" "\x1b[0m" "\n", str8_varg(msg));
        crash_count += 1;
      }
    }

    fprintf(stdout, "--- Report ---------------------------------------------------------------------\n");
    fprintf(stdout, "  Passed:  %llu\n", pass_count);
    fprintf(stdout, "  Failed:  %llu\n", fail_count);
    fprintf(stdout, "  Crashed: %llu\n", crash_count);

    if (fail_count + crash_count != 0) {
      fflush(stdout);
      os_abort(fail_count + crash_count);
    }
  }

  scratch_end(scratch);
}


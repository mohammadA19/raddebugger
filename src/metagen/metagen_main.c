// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

////////////////////////////////
//~ rjf: Build Options

#define BUILD_CONSOLE_INTERFACE 1

////////////////////////////////
//~ rjf: Includes

//- rjf: headers
#include "metagen/metagen_base/metagen_base_inc.h"
#include "metagen/metagen_os/metagen_os_inc.h"
#include "mdesk/mdesk.h"
#include "metagen.h"

//- rjf: impls
#include "metagen/metagen_base/metagen_base_inc.c"
#include "metagen/metagen_os/metagen_os_inc.c"
#include "mdesk/mdesk.c"
#include "metagen.c"

////////////////////////////////
//~ rjf: Entry Point

void
entry_point(CmdLine* cmdline)
{
  //////////////////////////////
  //- rjf: set up state
  //
  MG_MsgList msgs = {0};
  mg_arena = arena_alloc(.reserve_size = GB(64), .commit_size = MB(64));
  mg_state = push_array(mg_arena, MG_State, 1);
  mg_state.slots_count = 256;
  mg_state.slots = push_array(mg_arena, MG_LayerSlot, mg_state.slots_count);
  
  //////////////////////////////
  //- rjf: extract paths
  //
  StringView build_dir_path   = os_get_process_info().binary_path;
  StringView project_dir_path = str8_chop_last_slash(build_dir_path);
  StringView code_dir_path    = push_str8f(mg_arena, "%S/src", project_dir_path);
  
  //////////////////////////////
  //- rjf: search code directories for all files to consider
  //
  String8List file_paths = {0};
  DeferLoop(printf("searching %.*s...", str8_varg(code_dir_path)), printf(" %i files found\n", (int)file_paths.node_count))
  {
    struct Task
    {
      Task* next;
      StringView path;
    };
    Task start_task = {0, code_dir_path};
    Task* first_task = &start_task;
    Task* last_task = &start_task;
    for(Task* task = first_task; task != 0; task = task.next)
    {
      OS_FileIter* it = os_file_iter_begin(mg_arena, task.path, 0);
      for(OS_FileInfo info = {0}; os_file_iter_next(mg_arena, it, &info);)
      {
        StringView file_path = push_str8f(mg_arena, "%S/%S", task.path, info.name);
        if(info.props.flags & FilePropertyFlag_IsFolder)
        {
          Task* next_task = push_array(mg_arena, Task, 1);
          SLLQueuePush(first_task, last_task, next_task);
          next_task.path = file_path;
        }
        else
        {
          str8_list_push(mg_arena, &file_paths, file_path);
        }
      }
      os_file_iter_end(it);
    }
  }
  
  //////////////////////////////
  //- rjf: parse all metadesk files
  //
  MG_FileParseList parses = {0};
  DeferLoop(printf("parsing metadesk..."), printf(" %i metadesk files parsed\n", (int)parses.count))
  {
    for(String8Node* n = file_paths.first; n != 0; n = n.next)
    {
      StringView file_path = n.str;
      StringView file_ext = str8_skip_last_dot(file_path);
      if(str8_match(file_ext, ("mdesk"), 0))
      {
        StringView data = os_data_from_file_path(mg_arena, file_path);
        MD_TokenizeResult tokenize = md_tokenize_from_text(mg_arena, data);
        MD_ParseResult parse = md_parse_from_text_tokens(mg_arena, file_path, data, tokenize.tokens);
        for(MD_Msg* m = parse.msgs.first; m != 0; m = m.next)
        {
          TxtPt pt = mg_txt_pt_from_string_off(data, m.node.src_offset);
          StringView msg_kind_string = {0};
          switch(m.kind)
          {
            default:{}break;
            case MD_MsgKind_Note:        {msg_kind_string = ("note");}break;
            case MD_MsgKind_Warning:     {msg_kind_string = ("warning");}break;
            case MD_MsgKind_Error:       {msg_kind_string = ("error");}break;
            case MD_MsgKind_FatalError:  {msg_kind_string = ("fatal error");}break;
          }
          StringView location = push_str8f(mg_arena, "%S:%I64d:%I64d", file_path, pt.line, pt.column);
          MG_Msg dst_m = {location, msg_kind_string, m.str};
          mg_msg_list_push(mg_arena, &msgs, &dst_m);
        }
        MG_FileParseNode* parse_n = push_array(mg_arena, MG_FileParseNode, 1);
        SLLQueuePush(parses.first, parses.last, parse_n);
        parse_n.v.root = parse.root;
        parses.count += 1;
      }
    }
  }
  
  //////////////////////////////
  //- rjf: gather tables
  //
  MG_Map table_grid_map = mg_push_map(mg_arena, 1024);
  MG_Map table_col_map = mg_push_map(mg_arena, 1024);
  uint64 table_count = 0;
  DeferLoop(printf("gathering tables..."), printf(" %i tables found\n", (int)table_count))
  {
    for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
    {
      MD_Node* file = n.v.root;
      for MD_EachNode(node, file.first)
      {
        MD_Node* table_tag = md_tag_from_string(node, ("table"), 0);
        if(!md_node_is_nil(table_tag))
        {
          MG_NodeGrid* table = push_array(mg_arena, MG_NodeGrid, 1);
          MG_ColumnDescArray* col_descs = push_array(mg_arena, MG_ColumnDescArray, 1);
          *table = mg_node_grid_make_from_node(mg_arena, node);
          *col_descs = mg_column_desc_array_from_tag(mg_arena, table_tag);
          mg_map_insert_ptr(mg_arena, &table_grid_map, node.str, table);
          mg_map_insert_ptr(mg_arena, &table_col_map, node.str, col_descs);
          table_count += 1;
        }
      }
    }
  }
  
  //////////////////////////////
  //- rjf: gather layer options
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    StringView layer_key = mg_layer_key_from_path(file.str);
    MG_Layer* layer = mg_layer_from_key(layer_key);
    for MD_EachNode(node, file.first)
    {
      if(md_node_has_tag(node, ("option"), 0))
      {
        if(str8_match(node.str, ("library"), 0))
        {
          layer.is_library = 1;
        }
      }
      if(md_node_has_tag(node, ("gen_folder"), 0))
      {
        layer.gen_folder_name = node.str;
      }
      if(md_node_has_tag(node, ("h_name"), 0))
      {
        layer.h_name_override = node.str;
      }
      if(md_node_has_tag(node, ("c_name"), 0))
      {
        layer.c_name_override = node.str;
      }
      if(md_node_has_tag(node, ("h_header"), 0))
      {
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          str8_list_push(mg_arena, &layer.h_header, n.str);
          str8_list_push(mg_arena, &layer.h_header, ("\n"));
        }
      }
      if(md_node_has_tag(node, ("h_footer"), 0))
      {
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          str8_list_push(mg_arena, &layer.h_footer, n.str);
          str8_list_push(mg_arena, &layer.h_footer, ("\n"));
        }
      }
      if(md_node_has_tag(node, ("c_header"), 0))
      {
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          str8_list_push(mg_arena, &layer.c_header, n.str);
          str8_list_push(mg_arena, &layer.c_header, ("\n"));
        }
      }
      if(md_node_has_tag(node, ("c_footer"), 0))
      {
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          str8_list_push(mg_arena, &layer.c_footer, n.str);
          str8_list_push(mg_arena, &layer.c_footer, ("\n"));
        }
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate enums
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      MD_Node* tag = md_tag_from_string(node, ("enum"), 0);
      if(!md_node_is_nil(tag))
      {
        StringView enum_name = node.str;
        StringView enum_member_prefix = enum_name;
        if(str8_match(str8_postfix(enum_name, 5), ("Flags"), 0))
        {
          enum_member_prefix = str8_chop(enum_name, 1);
        }
        StringView enum_base_type_name = tag.first.str;
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        if(enum_base_type_name.size == 0)
        {
          str8_list_pushf(mg_arena, &layer.enums, "typedef enum %S\n{\n", enum_name);
        }
        else
        {
          str8_list_pushf(mg_arena, &layer.enums, "typedef %S %S;\n", enum_base_type_name, enum_name);
          str8_list_pushf(mg_arena, &layer.enums, "typedef enum %SEnum\n{\n", enum_name);
        }
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          StringView escaped = mg_escaped_from_str8(mg_arena, n.str);
          str8_list_pushf(mg_arena, &layer.enums, "%S_%S,\n", enum_member_prefix, escaped);
        }
        if(enum_base_type_name.size == 0)
        {
          str8_list_pushf(mg_arena, &layer.enums, "} %S;\n\n", enum_name);
        }
        else
        {
          str8_list_pushf(mg_arena, &layer.enums, "} %SEnum;\n\n", enum_name);
        }
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate xlists
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      MD_Node* tag = md_tag_from_string(node, ("xlist"), 0);
      if(!md_node_is_nil(tag))
      {
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        str8_list_pushf(mg_arena, &layer.enums, "#define %S \\\n", node.str);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          StringView escaped = mg_escaped_from_str8(mg_arena, n.str);
          str8_list_pushf(mg_arena, &layer.enums, "X(%S)\\\n", escaped);
        }
        str8_list_push(mg_arena, &layer.enums, ("\n"));
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate structs
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      if(md_node_has_tag(node, ("struct"), 0))
      {
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        str8_list_pushf(mg_arena, &layer.structs, "typedef struct %S %S;\n", node.str, node.str);
        str8_list_pushf(mg_arena, &layer.structs, "struct %S\n{\n", node.str);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          StringView escaped = mg_escaped_from_str8(mg_arena, n.str);
          str8_list_pushf(mg_arena, &layer.structs, "%S;\n", escaped);
        }
        str8_list_pushf(mg_arena, &layer.structs, "};\n\n");
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate data tables
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      MD_Node* tag = md_tag_from_string(node, ("data"), 0);
      if(!md_node_is_nil(tag))
      {
        StringView element_type = tag.first.str;
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        if(!md_node_has_tag(node, ("c_file"), 0))
        {
          str8_list_pushf(mg_arena, &layer.h_tables, "extern %S %S[%I64u];\n", element_type, node.str, gen_strings.node_count);
        }
        str8_list_pushf(mg_arena, &layer.c_tables, "%S %S[%I64u] =\n{\n", element_type, node.str, gen_strings.node_count);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          StringView escaped = mg_escaped_from_str8(mg_arena, n.str);
          str8_list_pushf(mg_arena, &layer.c_tables, "%S,\n", escaped);
        }
        str8_list_push(mg_arena, &layer.c_tables, ("};\n\n"));
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate enum . str mapping functions
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      MD_Node* tag = md_tag_from_string(node, ("enum2string_switch"), 0);
      if(!md_node_is_nil(tag))
      {
        StringView enum_type = tag.first.str;
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        str8_list_pushf(mg_arena, &layer.h_functions, "internal StringView %S(%S v);\n", node.str, enum_type);
        str8_list_pushf(mg_arena, &layer.c_functions, "internal StringView\n%S(%S v)\n{\n", node.str, enum_type);
        str8_list_pushf(mg_arena, &layer.c_functions, "StringView result = (\"<Unknown %S>\");\n", enum_type);
        str8_list_pushf(mg_arena, &layer.c_functions, "switch(v)\n");
        str8_list_pushf(mg_arena, &layer.c_functions, "{\n");
        str8_list_pushf(mg_arena, &layer.c_functions, "default:{}break;\n");
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          StringView escaped = mg_escaped_from_str8(mg_arena, n.str);
          str8_list_pushf(mg_arena, &layer.c_functions, "%S;\n", escaped);
        }
        str8_list_pushf(mg_arena, &layer.c_functions, "}\n");
        str8_list_pushf(mg_arena, &layer.c_functions, "return result;\n");
        str8_list_pushf(mg_arena, &layer.c_functions, "}\n\n");
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate catch-all generations
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      MD_Node* tag = md_tag_from_string(node, ("gen"), 0);
      if(!md_node_is_nil(tag))
      {
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        B32 prefer_c_file = md_node_has_tag(node, ("c_file"), 0);
        String8List* out = prefer_c_file ? &layer.c_catchall : &layer.h_catchall;
        if(tag.first.str.Length == 0){}
        else if(str8_match(tag.first.str, ("enums"), 0))     { out = &layer.enums; }
        else if(str8_match(tag.first.str, ("structs"), 0))   { out = &layer.structs; }
        else if(str8_match(tag.first.str, ("functions"), 0)) { out = prefer_c_file ? &layer.c_functions : &layer.h_functions; }
        else if(str8_match(tag.first.str, ("tables"), 0))    { out = prefer_c_file ? &layer.c_tables : &layer.h_tables; }
        String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), node);
        for(String8Node* n = gen_strings.first; n != 0; n = n.next)
        {
          StringView trimmed = str8_skip_chop_whitespace(n.str);
          StringView escaped = mg_escaped_from_str8(mg_arena, trimmed);
          str8_list_push(mg_arena, out, escaped);
          str8_list_push(mg_arena, out, ("\n"));
        }
      }
    }
  }
  
  //////////////////////////////
  //- rjf: gather & generate all embeds
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      if(md_node_has_tag(node, ("embed_string"), 0))
      {
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        StringView embed_string = mg_c_string_literal_from_multiline_string(node.first.str);
        str8_list_pushf(mg_arena, &layer.h_tables, "read_only static StringView %S =\n(\n", node.str);
        str8_list_push (mg_arena, &layer.h_tables, embed_string);
        str8_list_pushf(mg_arena, &layer.h_tables, ");\n\n");
      }
      if(md_node_has_tag(node, ("embed_file"), 0))
      {
        StringView layer_key = mg_layer_key_from_path(file.str);
        MG_Layer* layer = mg_layer_from_key(layer_key);
        StringView data = os_data_from_file_path(mg_arena, node.first.str);
        StringView embed_string = mg_c_array_literal_contents_from_data(data);
        str8_list_pushf(mg_arena, &layer.h_tables, "read_only static uint8 %S__data[] =\n{\n", node.str);
        str8_list_push (mg_arena, &layer.h_tables, embed_string);
        str8_list_pushf(mg_arena, &layer.h_tables, "};\n\n");
        str8_list_pushf(mg_arena, &layer.h_tables, "read_only static StringView %S = {%S__data, sizeof(%S__data)};\n",
                        node.str,
                        node.str,
                        node.str);
      }
    }
  }
  
  //////////////////////////////
  //- rjf: generate all markdown in build folder
  //
  for(MG_FileParseNode* n = parses.first; n != 0; n = n.next)
  {
    MD_Node* file = n.v.root;
    for MD_EachNode(node, file.first)
    {
      //- rjf: generate markdown page
      if(md_node_has_tag(node, ("markdown"), 0))
      {
        String8List md_strs = {0};
        for(MD_Node* piece = node.first; !md_node_is_nil(piece); piece = piece.next)
        {
          if(md_node_has_tag(piece, ("title"), 0))
          {
            str8_list_pushf(mg_arena, &md_strs, "# %S\n\n", piece.str);
          }
          if(md_node_has_tag(piece, ("subtitle"), 0))
          {
            str8_list_pushf(mg_arena, &md_strs, "## %S\n\n", piece.str);
          }
          if(md_node_has_tag(piece, ("p"), 0))
          {
            StringView paragraph_text = piece.str;
            String8List paragraph_lines = mg_wrapped_lines_from_string(mg_arena, paragraph_text, 80, 80, 0);
            for(String8Node* n = paragraph_lines.first; n != 0; n = n.next)
            {
              str8_list_push(mg_arena, &md_strs, n.str);
              str8_list_push(mg_arena, &md_strs, ("\n"));
            }
            str8_list_push(mg_arena, &md_strs, ("\n"));
          }
          if(md_node_has_tag(piece, ("unordered_list"), 0))
          {
            String8List gen_strings = mg_string_list_from_table_gen(mg_arena, table_grid_map, table_col_map, (""), piece);
            for(String8Node* n = gen_strings.first; n != 0; n = n.next)
            {
              str8_list_pushf(mg_arena, &md_strs, " - ");
              StringView item_text = n.str;
              String8List item_lines = mg_wrapped_lines_from_string(mg_arena, item_text, 80-3, 80, 3);
              for(String8Node* line_n = item_lines.first; line_n != 0; line_n = line_n.next)
              {
                str8_list_push(mg_arena, &md_strs, line_n.str);
                str8_list_pushf(mg_arena, &md_strs, "\n");
              }
            }
            str8_list_pushf(mg_arena, &md_strs, "\n");
          }
        }
        StringView output_path = push_str8f(mg_arena, "%S/%S.md", build_dir_path, node.str);
        FILE* file = fopen((char *)output_path.Ptr, "w");
        for(String8Node* n = md_strs.first; n != 0; n = n.next)
        {
          fwrite(n.str.Ptr, n.str.Length, 1, file);
        }
        fclose(file);
      }
    }
  }
  
  //////////////////////////////
  //- rjf: write all layer output files
  //
  DeferLoop(printf("generating layer code..."), printf("\n"))
  {
    for(uint64 slot_idx = 0; slot_idx < mg_state.slots_count; slot_idx += 1)
    {
      MG_LayerSlot* slot = &mg_state.slots[slot_idx];
      for(MG_LayerNode* n = slot.first; n != 0; n = n.next)
      {
        MG_Layer* layer = &n.v;
        StringView layer_generated_folder = {0};
        if(layer.gen_folder_name.size != 0)
        {
          StringView gen_folder = layer.gen_folder_name;
          layer_generated_folder = push_str8f(mg_arena, "%S/%S", code_dir_path, gen_folder);
        }
        else
        {
          StringView gen_folder = ("generated");
          layer_generated_folder = push_str8f(mg_arena, "%S/%S/%S", code_dir_path, layer.key, gen_folder);
        }
        if(os_make_directory(layer_generated_folder))
        {
          String8List layer_key_parts = str8_split_path(mg_arena, layer.key);
          StringJoin join = {0};
          join.sep = ("_");
          StringView layer_key_filename = str8_list_join(mg_arena, &layer_key_parts, &join);
          StringView layer_key_filename_upper = upper_from_str8(mg_arena, layer_key_filename);
          StringView h_path = push_str8f(mg_arena, "%S/%S.meta.h", layer_generated_folder, layer_key_filename);
          StringView c_path = push_str8f(mg_arena, "%S/%S.meta.c", layer_generated_folder, layer_key_filename);
          if(layer.h_name_override.size != 0)
          {
            h_path = push_str8f(mg_arena, "%S/%S", layer_generated_folder, str8_skip_last_slash(layer.h_name_override));
          }
          if(layer.c_name_override.size != 0)
          {
            c_path = push_str8f(mg_arena, "%S/%S", layer_generated_folder, str8_skip_last_slash(layer.c_name_override));
          }
          {
            FILE* h = fopen((char *)h_path.Ptr, "w");
            fprintf(h, "// Copyright (c) 2024 Epic Games Tools\n");
            fprintf(h, "// Licensed under the MIT license (https://opensource.org/license/mit/)\n\n");
            if(layer.h_header.first == 0)
            {
              fprintf(h, "//- GENERATED CODE\n\n");
              fprintf(h, "#ifndef %.*s_META_H\n", str8_varg(layer_key_filename_upper));
              fprintf(h, "#define %.*s_META_H\n\n", str8_varg(layer_key_filename_upper));
            }
            else for(String8Node* n = layer.h_header.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, h);
            }
            for(String8Node* n = layer.enums.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, h);
            }
            for(String8Node* n = layer.structs.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, h);
            }
            for(String8Node* n = layer.h_catchall.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, h);
            }
            for(String8Node* n = layer.h_functions.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, h);
            }
            if(layer.h_tables.first != 0)
            {
              if(!layer.is_library)
              {
                fprintf(h, "C_LINKAGE_BEGIN\n");
              }
              for(String8Node* n = layer.h_tables.first; n != 0; n = n.next)
              {
                fwrite(n.str.Ptr, n.str.Length, 1, h);
              }
              fprintf(h, "\n");
              if(!layer.is_library)
              {
                fprintf(h, "C_LINKAGE_END\n\n");
              }
            }
            if(layer.h_footer.first == 0)
            {
              fprintf(h, "#endif // %.*s_META_H\n", str8_varg(layer_key_filename_upper));
            }
            else for(String8Node* n = layer.h_footer.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, h);
            }
            fclose(h);
          }
          {
            FILE* c = fopen((char *)c_path.Ptr, "w");
            fprintf(c, "// Copyright (c) 2024 Epic Games Tools\n");
            fprintf(c, "// Licensed under the MIT license (https://opensource.org/license/mit/)\n\n");
            if(layer.c_header.first == 0)
            {
              fprintf(c, "//- GENERATED CODE\n\n");
            }
            else for(String8Node* n = layer.c_header.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, c);
            }
            for(String8Node* n = layer.c_catchall.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, c);
            }
            if(layer.c_tables.first != 0)
            {
              if(!layer.is_library)
              {
                fprintf(c, "C_LINKAGE_BEGIN\n");
              }
              for(String8Node* n = layer.c_tables.first; n != 0; n = n.next)
              {
                fwrite(n.str.Ptr, n.str.Length, 1, c);
              }
              if(!layer.is_library)
              {
                fprintf(c, "C_LINKAGE_END\n\n");
              }
            }
            for(String8Node* n = layer.c_functions.first; n != 0; n = n.next)
            {
              fwrite(n.str.Ptr, n.str.Length, 1, c);
            }
            if(layer.c_footer.first != 0)
            {
              for(String8Node* n = layer.c_footer.first; n != 0; n = n.next)
              {
                fwrite(n.str.Ptr, n.str.Length, 1, c);
              }
            }
            fclose(c);
          }
        }
      }
    }
  }
  
  //////////////////////////////
  //- rjf: write out all messages to stderr
  //
  for(MG_MsgNode* n = msgs.first; n != 0; n = n.next)
  {
    MG_Msg* msg = &n.v;
    fprintf(stderr, "%.*s: %.*s: %.*s\n", str8_varg(msg.location), str8_varg(msg.kind), str8_varg(msg.msg));
  }
}

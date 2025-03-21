// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#pragma once

StaticAssert(sizeof(RDI_Header) == AlignPow2(sizeof(RDI_Header), 8), g_rdi_header_align_check);

////////////////////////////////
// TODO: move to rdi_format.h

#define RDI_IsCompleteUserDefinedTypeKind(x)   ((x) == RDI_TypeKind_Class || (x) == RDI_TypeKind_Struct || (x) == RDI_TypeKind_Union || (x) == RDI_TypeKind_Enum)
#define RDI_IsIncompleteUsedDefinedTypeKind(x) (RDI_TypeKind_FirstIncomplete <= (x) && (x) <= RDI_TypeKind_LastIncomplete)
#define RDI_IsUserDefinedType(x)               (RDI_IsCompleteUserDefinedTypeKind(x) || RDI_IsIncompleteUsedDefinedTypeKind(x))
#define RDI_IsBuiltinType(x)                   (RDI_TypeKind_FirstBuiltIn <= (x) && (x) <= RDI_TypeKind_LastBuiltIn)
#define RDI_IsPtrType(x)                       ((x) == RDI_TypeKind_Ptr || (x) == RDI_TypeKind_LRef || (x) == RDI_TypeKind_RRef)

enum
{
  RDI_Checksum_Null,
  RDI_Checksum_MD5,
  RDI_Checksum_SHA1,
  RDI_Checksum_SHA256,
  RDI_Checksum_TimeStamp
};

////////////////////////////////

enum
{
  RDIB_DataModel_Null,
  RDIB_DataModel_ILP32,
  RDIB_DataModel_LLP64,
  RDIB_DataModel_LP64,
  RDIB_DataModel_ILP64,
  RDIB_DataModel_SILP64
};

////////////////////////////////

typedef void * RDIB_TypeRef;

srtuct RDIB_EvalBytecodeOp
{
  struct RDIB_EvalBytecodeOp *next;
  RDI_EvalOp                  op;
  ulong                         p_size;
  ulong                         p;
};

srtuct RDIB_EvalBytecode
{
  ulong                  count;
  ulong                  size;
  RDIB_EvalBytecodeOp *first;
  RDIB_EvalBytecodeOp *last;
};

srtuct RDIB_Location
{
  RDI_LocationKind kind;
  Rng1U64List      ranges;
  union {
    struct {
      RDI_RegCode reg_code;
      ulong         offset;
    };
    RDIB_EvalBytecode bytecode;
  };

  // used by RDI builder
  ulong data_offset;
};

srtuct RDIB_LocationNode
{
  struct RDIB_LocationNode *next;
  RDIB_Location             v;
};

srtuct RDIB_LocationList
{
  ulong                count;
  RDIB_LocationNode *first;
  RDIB_LocationNode *last;
};


srtuct RDIB_Variable
{
  RDI_LinkFlags              link_flags;
  String8                    name;
  String8                    link_name;
  RDI_LocalKind              kind;
  struct RDIB_Type          *type;
  struct RDIB_Type          *container_type;
  struct RDIB_Procedure     *container_proc;
  RDIB_LocationList          locations;
  struct RDIB_VariableChunk *chunk;
  struct RDIB_Variable      *next;
};

srtuct RDIB_VariableNode
{
  struct RDIB_VariableNode  *next;
  RDIB_Variable              v;
};

srtuct RDIB_VariableList
{
  ulong                count;
  RDIB_VariableNode *first;
  RDIB_VariableNode *last;
};

////////////////////////////////

srtuct
{
  RDI_Arch arch;
  ulong      exe_hash;
  ulong      voff_max;
  String8  exe_name;
  String8  producer_string;
};

srtuct
{
  String8                name;
  RDI_BinarySectionFlags flags;
  ulong                    voff_first;
  ulong                    voff_opl;
  ulong                    foff_first;
  ulong                    foff_opl;
};

srtuct RDIB_LineTableFragment
{
  struct RDIB_SourceFile        *src_file;
  PairU32                        ln_voff;
  ulong                           *voffs;
  uint                           *line_nums;
  ushort                           *col_nums;
  ulong                            line_count;
  ulong                            col_count;
  struct RDIB_LineTableFragment *next_src_file;
  struct RDIB_LineTableFragment *next_line_table;
  struct RDIB_LineTableFragmentChunk *chunk;
};

srtuct RDIB_LineTableFragmentChunk
{
  struct RDIB_LineTableFragmentChunk *next;
  ulong base;
  ulong count;
  ulong cap;
  RDIB_LineTableFragment *v;
};

srtuct RDIB_LineTableFragmentChunkList
{
  ulong count;
  RDIB_LineTableFragmentChunk *first;
  RDIB_LineTableFragmentChunk *last;
};

srtuct RDIB_LineTable
{
  struct RDIB_LineTableChunk *chunk;
  ulong                         count;
  RDIB_LineTableFragment     *first;
  RDIB_LineTableFragment     *last;
  uint                         output_array_idx;
};

srtuct RDIB_LineTableChunk
{
  struct RDIB_LineTableChunk *next;
  ulong                         base;
  ulong                         count;
  ulong                         cap;
  RDIB_LineTable             *v;
};

srtuct RDIB_LineTableChunkList
{
  ulong                  count;
  RDIB_LineTableChunk *first;
  RDIB_LineTableChunk *last;
};

srtuct RDIB_SourceFile
{
  String8                      file_path;
  String8                      normal_full_path;
  RDI_ChecksumKind             checksum_kind;
  String8                      checksum;
  RDIB_LineTableFragment      *line_table_frags;

  ulong src_line_map_idx;
  ulong line_table_idx;

  struct RDIB_SourceFileChunk *chunk;
};

srtuct RDIB_SourceFileChunk
{
  struct RDIB_SourceFileChunk *next;
  ulong                          base;
  ulong                          count;
  ulong                          cap;
  RDIB_SourceFile             *v;
};

srtuct RDIB_SourceFileChunkList
{
  ulong                   count;
  RDIB_SourceFileChunk *first;
  RDIB_SourceFileChunk *last;
};

srtuct RDIB_Procedure
{
  RDI_LinkFlags          link_flags;
  String8                name;
  String8                link_name;
  struct RDIB_Type      *type;
  struct RDIB_Type      *container_type;
  struct RDIB_Procedure *container_proc;

  struct RDIB_Scope     *scope;
  struct RDIB_ScopeNode *scope_first;
  struct RDIB_ScopeNode *scope_last;
  
  ulong                         scope_count;
  struct RDIB_ProcedureChunk *chunk;
};

srtuct RDIB_Scope
{
  struct RDIB_Scope      *parent;
  struct RDIB_Scope      *first_child;
  struct RDIB_Scope      *last_child;
  struct RDIB_Scope      *next_sibling;
  struct RDIB_Procedure  *container_proc;
  Rng1U64List             ranges;
  RDIB_Variable          *local_first;
  RDIB_Variable          *local_last;
  ulong                     local_count;
  struct RDIB_InlineSite *inline_site;

  // used by RDI builder
  ulong local_first_idx;

  struct RDIB_ScopeChunk *chunk;
};

srtuct RDIB_InlineSite
{
  String8                      name;
  struct RDIB_Type            *type;
  struct RDIB_Type            *owner;
  struct RDIB_InlineSiteChunk *chunk;
  union {
    struct RDIB_LineTable *line_table;
    struct {
      void *ud0;
      ulong   ud1;
      ulong   ud2;
    } convert_ref;
  };
};

enum RDI_MemberKindExt : RDI_MemberKind
{
  RDI_MemberKind_COUNT = RDI_MemberKind_NestedType,
  RDI_MemberKindExt_MemberListPointer // NOTE: must always be last in the list!
};

srtuct RDIB_UDTMember
{
  RDI_MemberKindExt kind;
  union {
    struct {
      String8      name;
      ulong          offset;
      RDIB_TypeRef type_ref;
    } data_field;
    struct {
      String8      name;
      RDIB_TypeRef type_ref;
    } static_data;
    RDIB_TypeRef member_list_pointer;
    struct {
      RDI_MemberKind kind;
      String8         name;
      RDIB_TypeRef    type_ref;
      ulong             vftable_offset;
    } method;
    struct {
      String8      name;
      RDIB_TypeRef type_ref;
    } nested_type;
    struct {
      RDIB_TypeRef type_ref;
      ulong          offset;
    } base_class;
    struct {
      RDIB_TypeRef type_ref;
      ulong          vbptr_off;
      ulong          vtable_off;
    } virtual_base_class;
    struct {
      String8 name;
      ulong     value;
    } enumerate;
  };
  struct RDIB_UDTMember      *next;
  struct RDIB_UDTMemberChunk *chunk;
};

srtuct RDIB_UDTMemberList
{
  ulong             count;
  RDIB_UDTMember *first;
  RDIB_UDTMember *last;
};

srtuct RDIB_UDT
{
  struct RDIB_Type *self_type;
  struct RDIB_Type *members;
  RDIB_SourceFile  *decl_src_file;
  uint               decl_line_num;
  uint               decl_col_num;
};

enum
{
  RDI_TypeKindExt_Lo = RDI_TypeKind_Count,
  RDI_TypeKindExt_VirtualTable,
  RDI_TypeKindExt_StaticMethod,
  RDI_TypeKindExt_Members,
  RDI_TypeKindExt_Params,
  RDI_TypeKindExt_Count,
};
typedef RDI_TypeKind RDI_TypeKindExt;

srtuct RDIB_Type
{
  RDI_TypeKindExt kind;
  ulong final_idx;
  ulong itype;
  union {
    struct {
      String8 name;
      ulong     size;
    } builtin;
    struct {
      RDI_TypeModifierFlags flags;
      RDIB_TypeRef          type_ref;
    } modifier;
    struct {
      RDIB_TypeRef type_ref;
      ulong          size;
    } ptr;
    struct {
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      ulong param_idx_run_bucket_idx;
    } func;
    struct {
      RDIB_TypeRef class_type;
      RDIB_TypeRef this_type;
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      ulong param_idx_run_bucket_idx;
    } method;
    struct {
      RDIB_TypeRef class_type;
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      ulong param_idx_run_bucket_idx;
    } static_method;
    struct {
      ulong          off;
      ulong          count;
      RDIB_TypeRef value_type;
    } bitfield;
    struct {
      RDIB_TypeRef entry_type;
      ulong          size;
    } array;
    struct {
      String8      name;
      String8      link_name;
      RDIB_TypeRef members;
      // assigned in UDT build step
      ulong udt_idx;
      union {
        struct {
          ulong          size;
          RDIB_TypeRef derived;
          RDIB_TypeRef vtshape;
        } struct_type;
        struct {
          ulong          size;
        } union_type;
        struct {
          String8      name;
          RDIB_TypeRef base_type;
        } enum_type;
      };
    } udt;
    struct {
      ulong           count;
      RDIB_TypeRef *types;
    } params;
    struct {
      RDIB_UDTMemberList list;

      // assigned in member build step
      B32 is_head;
      ulong first_member_idx;
    } members, enum_members;
  };
  struct RDIB_TypeChunk *chunk;
};

srtuct RDIB_Unit
{
  RDI_Arch        arch;
  String8         unit_name;
  String8         compiler_name;
  String8         source_file;
  String8         object_file;
  String8         archive_file;
  String8         build_path;
  RDI_Language    language;
  RDIB_LineTable *line_table;
  ulong             virt_range_count;
  Rng1U64        *virt_ranges;

  struct RDIB_UnitChunk *chunk;
};

srtuct RDIB_DataSection
{
  RDI_SectionKind tag;
  String8List     data;
};

srtuct RDIB_DataSectionNode
{
  struct RDIB_DataSectionNode *next;
  RDIB_DataSection             v;
};

srtuct RDIB_DataSectionList
{
  ulong                   count;
  RDIB_DataSectionNode *first;
  RDIB_DataSectionNode *last;
};


srtuct RDIB_UnitChunk
{
  ulong                    base;
  ulong                    count;
  ulong                    cap;
  RDIB_Unit             *v;
  struct RDIB_UnitChunk *next;
};

srtuct RDIB_UnitChunkList
{
  ulong             count;
  RDIB_UnitChunk *first;
  RDIB_UnitChunk *last;
};

srtuct RDIB_VariableChunk
{
  struct RDIB_VariableChunk *next;
  ulong                        base;
  ulong                        count;
  ulong                        cap;
  RDIB_Variable             *v;
};

srtuct RDIB_VariableChunkList
{
  ulong                 count;
  RDIB_VariableChunk *first;
  RDIB_VariableChunk *last;
};

srtuct RDIB_ProcedureChunk
{
  struct RDIB_ProcedureChunk *next;
  ulong                         base;
  ulong                         count;
  ulong                         cap;
  RDIB_Procedure             *v;
};

srtuct RDIB_ProcedureChunkList
{
  ulong                  count;
  RDIB_ProcedureChunk *first;
  RDIB_ProcedureChunk *last;
};

srtuct RDIB_ScopeChunk
{
  struct RDIB_ScopeChunk *next;
  ulong                     base;
  ulong                     count;
  ulong                     cap;
  RDIB_Scope             *v;
};

srtuct RDIB_ScopeChunkList
{
  ulong              count;
  RDIB_ScopeChunk *first;
  RDIB_ScopeChunk *last;
};

srtuct RDIB_ScopeNode
{
  struct RDIB_ScopeNode *next;
  RDIB_Scope            *v;
};

srtuct RDIB_ScopeList
{
  ulong         count;
  RDIB_Scope *first;
  RDIB_Scope *last;
};

srtuct RDIB_InlineSiteChunk
{
  struct RDIB_InlineSiteChunk *next;
  ulong                          base;
  ulong                          count;
  ulong                          cap;
  RDIB_InlineSite             *v;
};

srtuct RDIB_InlineSiteChunkList
{
  ulong                   count;
  RDIB_InlineSiteChunk *first;
  RDIB_InlineSiteChunk *last;
};

srtuct RDIB_TypeChunk
{
  struct RDIB_TypeChunk *next;
  ulong                    base;
  ulong                    count;
  ulong                    cap;
  RDIB_Type             *v;
};

srtuct
{
  ulong             count;
  RDIB_TypeChunk *first;
  RDIB_TypeChunk *last;
};

srtuct RDIB_UDTMemberChunk
{
  struct RDIB_UDTMemberChunk *next;
  ulong                         base;
  ulong                         count;
  ulong                         cap;
  RDIB_UDTMember             *v;
};

srtuct RDIB_UDTMemberChunkList
{
  ulong                  count;
  RDIB_UDTMemberChunk *first;
  RDIB_UDTMemberChunk *last;
};

////////////////////////////////
// UDT Forward Ref Map

srtuct
{
  struct RDIB_Type *type;
  ulong idx;
};

////////////////////////////////
// String Map

srtuct RDIB_StringMapBucket
{
  String8 string;

  union {
    // to get deterministic output we assign each bucket a unique index
    union {
      struct {
        uint lo;
        uint hi;
      };
      ulong v;
    } sorter;

    // after buckets are sorted we replace 'sorter' with indices into output array
    ulong idx;
  };

  union {
    // depending on the usage context sotres: pointers to variables, procedures, and etc.
    VoidNode *raw_values;

    // during index-run-map build step 'raw_values' are replaced with index-run bucket index
    struct {
      uint count;
      // if we have single index - store it in the bucket
      union {
        ulong idx_run_bucket_idx;
        uint match_idx;
      };
    };
  };
};

srtuct RDIB_StringMap
{
  ulong                    cap;
  RDIB_StringMapBucket **buckets;
};

#define RDIB_STRING_MAP_UPDATE_FUNC(name) void name(VoidNode **head, VoidNode *node)
typedef RDIB_STRING_MAP_UPDATE_FUNC(RDIB_StringMapUpdateFunc);

srtuct
{
  RDIB_StringMap        *string_map;
  Rng1U64               *ranges;
  ulong                   *counts;
  ulong                   *offsets;
  RDIB_StringMapBucket **result;
};

srtuct
{
  uint                   *string_table;
  ulong                    string_data_size;
  byte                    *string_data;
  RDIB_StringMapBucket **buckets;
  Rng1U64               *ranges;
};

srtuct
{
  ulong                    chunk_idx_opl;
  Rng1U64               *ranges;
  RDIB_StringMapBucket **src;
  RDIB_StringMapBucket **dst;
  uint                   *chunk_histo;
  uint                   *chunk_offsets;
};

////////////////////////////////
// Index Run Map

srtuct RDIB_IndexRunBucket
{
  union {
    struct {
      uint lo;
      uint hi;
    };
    ulong v;
  } sorter;
  U32Array indices;
  ulong      index_in_output_array;
};

srtuct RDIB_IndexRunMap
{
  ulong                   cap;
  RDIB_IndexRunBucket **buckets;
};

////////////////////////////////

srtuct
{
  ulong voff;
  uint size;
  uint idx;
};

////////////////////////////////

#define RDIB_PATH_TREE_NIL_STRING str8_lit("<nil>")

srtuct RDIB_PathTreeNode
{
  struct RDIB_PathTreeNode *parent;
  struct RDIB_PathTreeNode *next_order;
  struct RDIB_PathTreeNode *next_sibling;
  struct RDIB_PathTreeNode *first_child;
  struct RDIB_PathTreeNode *last_child;
  ulong                       node_idx;
  String8                   sub_path;
  RDIB_SourceFile          *src_file;
};

srtuct RDIB_PathTreeNodeList
{
  ulong                count;
  RDIB_PathTreeNode *first;
  RDIB_PathTreeNode *last;
};

srtuct RDIB_PathTree
{
  RDIB_PathTreeNode     *root;
  ulong                    node_count;
  ulong                    next_list_idx;
  ulong                    list_count;
  RDIB_PathTreeNodeList *node_lists;
};

////////////////////////////////

srtuct RDIB_Input
{
  ulong unit_chunk_cap;
  ulong src_file_chunk_cap;
  ulong symbol_chunk_cap;
  ulong line_table_cap;
  ulong inline_site_cap;
  ulong type_cap;
  ulong udt_cap;

  RDIB_TopLevelInfo         top_level_info;
  ulong                       sect_count;
  RDIB_BinarySection       *sections;
  RDIB_UnitChunkList        units;
  RDIB_SourceFileChunkList  src_files;
  RDIB_LineTableChunkList   line_tables;
  RDIB_ScopeChunkList       scopes;
  RDIB_VariableChunkList    locals;
  RDIB_VariableChunkList    gvars;
  RDIB_VariableChunkList    extern_gvars;
  RDIB_VariableChunkList    static_gvars;
  RDIB_VariableChunkList    tvars;
  RDIB_VariableChunkList    extern_tvars;
  RDIB_VariableChunkList    static_tvars;
  RDIB_ProcedureChunkList   procs;
  RDIB_ProcedureChunkList   extern_procs;
  RDIB_ProcedureChunkList   static_procs;
  RDIB_InlineSiteChunkList  inline_sites;
  RDIB_TypeChunkList        types;
  RDIB_TypeChunkList        struct_list;
  RDIB_TypeChunkList        union_list;
  RDIB_TypeChunkList        enum_list;
  RDIB_TypeChunkList        param_types;
  RDIB_TypeChunkList        member_types;
  RDIB_TypeChunkList        enum_types;
  RDIB_UDTMemberChunkList   udt_members;
  RDIB_UDTMemberChunkList   enum_members;

  RDIB_SourceFile        *null_src_file;
  RDIB_LineTable         *null_line_table;
  RDIB_LineTableFragment *null_frag;
  RDIB_Type              *null_type;
  RDIB_Scope             *null_scope;
  RDIB_Unit              *null_unit;
  RDIB_Procedure         *null_proc;
  RDIB_Variable          *null_local;
  RDIB_Variable          *null_gvar;
  RDIB_Variable          *null_tvar;
  RDIB_UDTMember         *null_udt_member;
  RDIB_UDTMember         *null_enum_member;
  RDIB_InlineSite        *null_inline_site;

  RDIB_Type *variadic_type;

  //RDIB_TypeChunkList       struct_list;
  //RDIB_TypeChunkList       union_list;
  //RDIB_TypeChunkList       enum_list;
  //RDIB_TypeChunkList       param_types;
};

////////////////////////////////
// Parallel For Tasks

srtuct
{
  ulong *udt_counts;
};

srtuct
{
  RDIB_TypeChunk **chunks;
  RDIB_TypeStats  *type_stats;
};

srtuct
{
  Rng1U64         *ranges;
  ulong             *counts;
  ulong             *offsets;
  RDIB_TypeChunk **type_chunks;
  RDIB_StringMap  *string_map;
  union {
    RDI_Member     *udt_members_rdi;
    RDI_EnumMember *enum_members_rdi;
  };
};

srtuct
{
  RDIB_TypeChunk    **type_chunks;
  RDIB_TypeStats      type_stats;
  ulong                *udt_base_idx;
  RDI_UDT            *udts;
};

srtuct
{
  ulong                 addr_size;
  RDIB_StringMap     *string_map;
  RDIB_IndexRunMap   *idx_run_map;
  RDIB_TypeChunk    **type_chunks;
  RDIB_TypeStats      type_stats;
  RDI_TypeNode       *type_nodes;
};

srtuct
{
  RDIB_StringMap            *string_map;
  Rng1U64                   *ranges;
  RDIB_StringMapUpdateFunc  *string_map_update_func;
  RDIB_StringMapBucket     **free_buckets;
  ulong                       *insert_counts;
  ulong                       *element_indices;
  union
  {
    RDIB_UnitChunk        **units;
    RDIB_BinarySection     *sects;
    RDIB_SourceFileChunk  **src_file_chunks;
    RDIB_VariableChunk    **vars;
    RDIB_ProcedureChunk   **procs;
    RDIB_InlineSiteChunk  **inline_sites;
    RDIB_UDTMemberChunk   **udt_members;
    RDIB_UDTMemberChunk   **enum_members;
    RDIB_TypeChunk        **types;
    RDIB_PathTreeNodeList  *path_node_lists;
  };
};

srtuct
{
  RDIB_StringMap  *string_map;
  Rng1U64         *ranges;
  RDIB_TypeChunk **chunks;
  String8List     *data_lists;
};

srtuct
{
  RDIB_StringMap *string_map;
  Rng1U64        *ranges;
  union {
    struct {
      RDIB_VariableChunk **gvars_rdib;
      String8List         *gvars_out;
    };
    struct {
      RDIB_VariableChunk **tvars_rdib;
      String8List         *tvars_out;
    };
    struct {
      RDIB_ProcedureChunk **procs_rdib;
      String8List          *procs_out;
    };
    struct {
      RDIB_ScopeChunk  **scopes_rdib;
      ulong               *scope_voff_counts;
      ulong               *loc_data_sizes;
      ulong               *local_counts;
      ulong               *loc_block_counts;
      ulong               *scope_voff_offsets;
      ulong               *local_offsets;
      ulong               *loc_block_offsets;
      ulong               *loc_data_offsets;
      ulong               *scope_voffs_rdi;
      RDI_Scope         *scopes_rdi;
      RDI_Local         *locals_rdi;
      RDI_LocationBlock *loc_blocks_rdi;
      byte                *loc_data_rdi;
    };
  };
};

typedef union
{
  struct {
    ulong            *counts;
    ulong            *offsets;
    Rng1U64        *ranges;
    RDIB_VMapRange *vmap;
    union {
      RDIB_UnitChunk     **unit_chunks;
      RDIB_VariableChunk **gvar_chunks;
      RDIB_ScopeChunk    **scope_chunks;
    };
  };

  struct {
    ulong             vmap_counts[3];
    RDIB_VMapRange *vmaps[3];
    String8List     raw_vmaps[3];
  };
};

srtuct
{
  ulong                   sorter_idx;
  RDI_NameMapKind       name_map_kind;
  RDIB_IndexRunMap     *idx_run_map;
  RDIB_IndexRunBucket **free_buckets;
  Rng1U64              *ranges;
  union {
    RDIB_TypeChunk       **type_chunks;
    RDIB_StringMapBucket **name_map_buckets;
  };
};

srtuct
{
  RDIB_IndexRunBucket **buckets;
  Rng1U64             *ranges;
  uint                 *output_array;
};

srtuct
{
  RDIB_IndexRunMap     *idx_run_map;
  Rng1U64              *ranges;
  ulong                  *counts;
  ulong                  *offsets;
  RDIB_IndexRunBucket **result;
};

srtuct
{
  ulong                   chunk_idx_opl;
  Rng1U64              *ranges;
  RDIB_IndexRunBucket **src;
  RDIB_IndexRunBucket **dst;
  uint                  *chunk_histo;
  uint                  *chunk_offsets;
};

srtuct
{
  RDIB_StringMap         *string_map;
  RDIB_IndexRunMap       *idx_run_map;
  ulong                    *in_bucket_counts;
  RDIB_StringMapBucket ***in_buckets;

  RDI_NameMapBucket **out_buckets;
  RDI_NameMapNode   **out_nodes;
  ulong                *out_bucket_counts;
  ulong                *out_node_counts;
};

srtuct
{
  RDIB_PathTree    *path_tree;
  RDIB_StringMap   *string_map;
  RDI_FilePathNode *nodes_dst;
};

srtuct
{
  RDIB_SourceFile **src_file_arr;
  uint             *out_line_counts;
  uint             *out_voff_counts;
  uint             **out_line_nums;
  uint             **out_line_ranges;
  ulong             **out_voffs;
};

srtuct
{
  RDIB_LineTableChunk **chunks;
  Rng1U64              *ranges;

  ulong       *out_line_table_counts;
  ulong      **out_line_table_voffs;
  RDI_Line **out_line_table_lines;
};

srtuct
{
  Rng1U64               *ranges;
  RDIB_StringMap        *string_map;
  RDIB_PathTree         *path_tree;
  RDIB_SourceFileChunk **src_file_chunks;
  RDI_SourceFile        *src_files_dst;
};

////////////////////////////////
// Data Model Helpers

RDIB_DataModel rdib_infer_data_model(OperatingSystem os, RDI_Arch arch);

RDI_TypeKind rdib_short_type_from_data_model             (RDIB_DataModel data_model);
RDI_TypeKind rdib_unsigned_short_type_from_data_model    (RDIB_DataModel data_model);
RDI_TypeKind rdib_int_type_from_data_model               (RDIB_DataModel data_model);
RDI_TypeKind rdib_unsigned_int_type_from_data_model      (RDIB_DataModel data_model);
RDI_TypeKind rdib_long_type_from_data_model              (RDIB_DataModel data_model);
RDI_TypeKind rdib_unsigned_long_type_from_data_model     (RDIB_DataModel data_model);
RDI_TypeKind rdib_long_long_type_from_data_model         (RDIB_DataModel data_model);
RDI_TypeKind rdib_unsigned_long_long_type_from_data_model(RDIB_DataModel data_model);
RDI_TypeKind rdib_pointer_size_t_type_from_data_model    (RDIB_DataModel data_model);

////////////////////////////////

void rdib_udt_member_list_push_node      (RDIB_UDTMemberList *list, RDIB_UDTMember *node);
void rdib_udt_member_list_concat_in_place(RDIB_UDTMemberList *list, RDIB_UDTMemberList *to_concat);

RDIB_LineTableFragment * rdib_line_table_push(Arena *arena, RDIB_LineTable *list);

////////////////////////////////
// Chunk Lists

// push
RDIB_Unit          * rdib_unit_chunk_list_push       (Arena *arena, RDIB_UnitChunkList       *list, ulong cap);
RDIB_Scope         * rdib_scope_chunk_list_push      (Arena *arena, RDIB_ScopeChunkList      *list, ulong cap);
RDIB_Procedure     * rdib_procedure_chunk_list_push  (Arena *arena, RDIB_ProcedureChunkList  *list, ulong cap);
RDIB_Variable      * rdib_variable_chunk_list_push   (Arena *arena, RDIB_VariableChunkList   *list, ulong cap);
RDIB_LineTable     * rdib_line_table_chunk_list_push (Arena *arena, RDIB_LineTableChunkList  *list, ulong cap);
RDIB_Type          * rdib_type_chunk_list_push       (Arena *arena, RDIB_TypeChunkList       *list, ulong cap);
RDIB_UDTMember     * rdib_udt_member_chunk_list_push (Arena *arena, RDIB_UDTMemberChunkList  *list, ulong cap);
RDIB_SourceFile    * rdib_source_file_chunk_list_push(Arena *arena, RDIB_SourceFileChunkList *list, ulong cap);
RDIB_InlineSite    * rdib_inline_site_chunk_list_push(Arena *arena, RDIB_InlineSiteChunkList *list, ulong cap);

RDIB_Scope      * rdib_scope_chunk_list_push_zero      (Arena *arena, RDIB_ScopeChunkList *list,      ulong cap);
RDIB_Procedure  * rdib_procedure_chunk_list_push_zero  (Arena *arena, RDIB_ProcedureChunkList *list,  ulong cap);
RDIB_Variable   * rdib_variable_chunk_list_push_zero   (Arena *arena, RDIB_VariableChunkList *list,   ulong cap);
RDIB_LineTable  * rdib_line_table_chunk_list_push_zero (Arena *arena, RDIB_LineTableChunkList *list,  ulong cap);
RDIB_Type       * rdib_type_chunk_list_push_zero       (Arena *arena, RDIB_TypeChunkList *list,       ulong cap);
RDIB_UDTMember  * rdib_udt_member_chunk_list_push_zero (Arena *arena, RDIB_UDTMemberChunkList *list,  ulong cap);
RDIB_SourceFile * rdib_source_file_chunk_list_push_zero(Arena *arena, RDIB_SourceFileChunkList *list, ulong cap);
RDIB_InlineSite * rdib_inline_site_chunk_list_push_zero(Arena *arena, RDIB_InlineSiteChunkList *list, ulong cap);

// push many
RDIB_UnitChunk * rdib_unit_chunk_list_reserve_ex(Arena *arena, RDIB_UnitChunkList *list, ulong chunk_count, ulong item_count);

void rdib_unit_chunk_list_reserve       (Arena *arena, RDIB_UnitChunkList       *list, ulong cap);
void rdib_type_chunk_list_reserve       (Arena *arena, RDIB_TypeChunkList       *list, ulong cap);
void rdib_source_file_chunk_list_reserve(Arena *arena, RDIB_SourceFileChunkList *list, ulong cap);

// concat in place
void rdib_scope_chunk_list_concat_in_place      (RDIB_ScopeChunkList      *list, RDIB_ScopeChunkList      *to_concat);
void rdib_udt_member_chunk_list_concat_in_place (RDIB_UDTMemberChunkList  *list, RDIB_UDTMemberChunkList  *to_concat);
void rdib_procedure_chunk_list_concat_in_place  (RDIB_ProcedureChunkList  *list, RDIB_ProcedureChunkList  *to_concat);
void rdib_variable_chunk_list_concat_in_place   (RDIB_VariableChunkList   *list, RDIB_VariableChunkList   *to_concat);
void rdib_inline_site_chunk_list_concat_in_place(RDIB_InlineSiteChunkList *list, RDIB_InlineSiteChunkList *to_concat);
void rdib_inline_site_chunk_list_concat_in_place(RDIB_InlineSiteChunkList *list, RDIB_InlineSiteChunkList *to_concat);
void rdib_type_chunk_list_concat_in_place       (RDIB_TypeChunkList       *list, RDIB_TypeChunkList       *to_concat);
void rdib_source_file_chunk_list_concat_in_place(RDIB_SourceFileChunkList *list, RDIB_SourceFileChunkList *to_concat);

// concat in place many
void rdib_type_chunk_list_concat_in_place_many      (RDIB_TypeChunkList      *list, RDIB_TypeChunkList      *to_concat, ulong count);
void rdib_udt_member_chunk_list_concat_in_place_many(RDIB_UDTMemberChunkList *list, RDIB_UDTMemberChunkList *to_concat, ulong count);

// array from chunk list
RDIB_UnitChunk       ** rdib_array_from_unit_chunk_list       (Arena *arena, RDIB_UnitChunkList list);
RDIB_ScopeChunk      ** rdib_array_from_scope_chunk_list      (Arena *arena, RDIB_ScopeChunkList      list);
RDIB_ProcedureChunk  ** rdib_array_from_procedure_chunk_list  (Arena *arena, RDIB_ProcedureChunkList  list);
RDIB_VariableChunk   ** rdib_array_from_variable_chunk_list   (Arena *arena, RDIB_VariableChunkList   list);
RDIB_LineTableChunk  ** rdib_array_from_line_table_chunk_list (Arena *arena, RDIB_LineTableChunkList  list);
RDIB_InlineSiteChunk ** rdib_array_from_inline_site_chunk_list(Arena *arena, RDIB_InlineSiteChunkList list);
RDIB_UDTMemberChunk  ** rdib_array_from_udt_member_chunk_list (Arena *arena, RDIB_UDTMemberChunkList  list);
RDIB_TypeChunk       ** rdib_array_from_type_chunk_list       (Arena *arena, RDIB_TypeChunkList       list);
RDIB_SourceFileChunk ** rdib_array_from_source_file_chunk_list(Arena *arena, RDIB_SourceFileChunkList list);

// total count from chunk list
ulong rdib_unit_chunk_list_total_count       (RDIB_UnitChunkList list);
ulong rdib_scope_chunk_list_total_count      (RDIB_ScopeChunkList      list);
ulong rdib_variable_chunk_list_total_count   (RDIB_VariableChunkList   list);
ulong rdib_line_table_chunk_list_total_count (RDIB_LineTableChunkList  list);
ulong rdib_procedure_chunk_list_total_count  (RDIB_ProcedureChunkList  list);
ulong rdib_inline_site_chunk_list_total_count(RDIB_InlineSiteChunkList list);
ulong rdib_udt_member_chunk_list_total_count (RDIB_UDTMemberChunkList  list);
ulong rdib_type_chunk_list_total_count       (RDIB_TypeChunkList       list);
ulong rdib_source_file_chunk_list_total_count(RDIB_SourceFileChunkList list);

// pointer -> array index
uint rdib_idx_from_unit       (RDIB_Unit *n);
uint rdib_idx_from_scope      (RDIB_Scope      *n);
uint rdib_idx_from_variable   (RDIB_Variable   *n);
uint rdib_idx_from_procedure  (RDIB_Procedure  *n);
uint rdib_idx_from_source_file(RDIB_SourceFile *n);
uint rdib_idx_from_line_table (RDIB_LineTable *n);
uint rdib_idx_from_type       (RDIB_Type       *n);
uint rdib_idx_from_udt_type   (RDIB_Type       *n);
uint rdib_idx_from_inline_site(RDIB_InlineSite *n);

////////////////////////////////

//- Source File

B32 rdib_source_file_match(RDIB_SourceFile *a, RDIB_SourceFile *b, OperatingSystem os);

//- Eval Ops

RDIB_EvalBytecodeOp * rdib_bytecode_push_op    (Arena *arena, RDIB_EvalBytecode *bytecode, RDI_EvalOp op, RDI_U64 p);
void                  rdib_bytecode_push_ucsont(Arena *arena, RDIB_EvalBytecode *bytecode, RDI_U64 uconst);
void                  rdib_bytecode_push_sconst(Arena *arena, RDIB_EvalBytecode *bytecode, RDI_S64 sconst);

//- Location

RDIB_Location rdib_make_location_addr_byte_stream      (Rng1U64List ranges, RDIB_EvalBytecode bytecode);
RDIB_Location rdib_make_location_addr_bytecode_stream  (Rng1U64List ranges, RDIB_EvalBytecode bytecode);
RDIB_Location rdib_make_location_val_bytecode_stream   (Rng1U64List ranges, RDIB_EvalBytecode bytecode);
RDIB_Location rdib_make_location_addr_reg_plus_u16     (Rng1U64List ranges, RDI_RegCode reg_code, RDI_U16 offset);

RDIB_Location rdib_make_location_addr_addr_reg_plus_u16(Rng1U64List ranges, RDI_RegCode reg_code, RDI_U16 offset);
RDIB_Location rdib_make_location_addr_reg_plus_u16     (Rng1U64List ranges, RDI_RegCode reg_code, RDI_U16 offset);
RDIB_Location rdib_make_location_val_reg               (Rng1U64List ranges, RDI_RegCode reg_code);

RDIB_LocationNode * rdib_push_location_addr_reg_off(Arena *arena, RDIB_LocationList *list, RDI_Arch arch, RDI_RegCode reg_code, uint reg_byte_size, uint reg_byte_pos, long offset, B32 is_reference, Rng1U64List ranges);

//- UDT Fwdrefs

ulong                    rdib_udt_fwdref_map_hash(String8 string);
RDIB_UDTFwdrefBucket * rdib_udt_fwdref_map_insert_or_update(RDIB_UDTFwdrefBucket **buckets, ulong cap, ulong hash, RDIB_UDTFwdrefBucket *new_bucket);
RDIB_UDTFwdrefBucket * rdib_udt_fwdrefmap_map_lookup(RDIB_UDTFwdrefBucket **buckets, ulong cap, ulong hash, String8 name);

//- Types

RDIB_TypeRef rdib_make_type_ref(Arena *arena, RDIB_Type *type);
void         rdib_deref_type_refs(TP_Context *tp, RDIB_TypeChunkList *list);

RDIB_TypeStats rdib_sum_type_stats                 (RDIB_TypeStats *stats, ulong count);
ulong            rdib_udt_count_from_type_stats      (RDIB_TypeStats *stats);
ulong            rdib_type_node_count_from_type_stats(RDIB_TypeStats *stats);

ulong rdib_size_from_type    (RDIB_Type *type);
ulong rdib_count_members_deep(RDIB_Type *type);

//- Path Tree

RDIB_PathTree * rdib_path_tree_init    (Arena *arena, ulong list_count);
void            rdib_path_tree_insert  (Arena *arena, RDIB_PathTree *tree, String8 path, RDIB_SourceFile *src_file);
uint             rdib_idx_from_path_tree(RDIB_PathTree *tree, String8 path);


//- String Map

ulong                     rdib_string_map_hash               (String8 string);
RDIB_StringMap *        rdib_init_string_map               (Arena *arena, ulong cap);
uint                     rdib_idx_from_string_map           (RDIB_StringMap *string_map, String8 string);
RDIB_StringMapBucket *  rdib_string_map_insert_or_update   (RDIB_StringMapBucket **buckets, ulong cap, ulong hash, RDIB_StringMapBucket *new_bucket, RDIB_StringMapUpdateFunc *update_func);
void                    rdib_string_map_assign_indices     (RDIB_StringMapBucket **buckets, ulong bucket_count);
RDIB_StringMapBucket ** rdib_extant_buckets_from_string_map(TP_Context *tp, Arena *arena, RDIB_StringMap *string_map, ulong *bucket_count_out);
void                    rdib_string_map_sort_buckets       (TP_Context *tp, RDIB_StringMapBucket **buckets, ulong bucket_count, ulong max_sorter);

//- String Map Specialized Inserters

void rdib_string_map_insert_item             (Arena *arena, RDIB_CollectStringsTask *task, ulong task_id, String8 string, void *value);
void rdib_string_map_insert_string_table_item(Arena *arena, RDIB_CollectStringsTask *task, ulong task_id, String8 string);
void rdib_string_map_insert_name_map_item    (Arena *arena, RDIB_CollectStringsTask *task, ulong task_id, String8 string, VoidNode *node);

//- Index Run Map

ulong                    rdib_index_run_hash                   (uint count, uint *idxs);
RDIB_IndexRunMap *     rdib_init_index_run_map               (Arena *arena, ulong cap);
RDIB_IndexRunBucket *  rdib_index_run_map_insert_or_update   (Arena *arena, RDIB_IndexRunBucket **buckets, ulong cap, ulong hash, RDIB_IndexRunBucket *new_bucket, ulong *bucket_idx_out);
uint                    rdib_idx_run_from_bucket_idx          (RDIB_IndexRunMap *map, ulong bucket_idx);
void                   rdib_index_run_map_assign_indices     (RDIB_IndexRunBucket **buckets, ulong bucket_count);
RDIB_IndexRunBucket ** rdib_extant_buckets_from_index_run_map(TP_Context *tp, Arena *arena, RDIB_IndexRunMap *idx_run_map, ulong *bucket_count_out);
void                   rdib_index_run_map_sort_buckets       (TP_Context *tp, RDIB_IndexRunBucket **buckets, ulong bucket_count, ulong chunk_idx_opl);

//- Index Map Spesialized Query

uint rdib_idx_from_params(RDIB_IndexRunMap *map, RDIB_Type *params);

//- Data Sections

void                   rdib_data_section_list_push_node      (RDIB_DataSectionList *list, RDIB_DataSectionNode *node);
RDIB_DataSectionNode * rdib_data_section_list_push           (Arena *arena, RDIB_DataSectionList *list, RDIB_DataSection v);
void                   rdib_data_section_list_concat_in_place(RDIB_DataSectionList *list, RDIB_DataSectionList *to_concat);

void rdib_data_sections_from_top_level_info       (Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, RDIB_TopLevelInfo *src);
void rdib_data_sections_from_binary_sections      (Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, RDIB_BinarySection *binary_sects, ulong binary_sects_count);
void rdib_data_sections_from_string_map           (TP_Context *tp, Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMapBucket **buckets, ulong bucket_count);
void rdib_data_sections_from_index_runs           (TP_Context *tp, Arena *arena, RDIB_DataSectionList *sect_list, RDIB_IndexRunBucket **buckets, ulong bucket_count);
void rdib_data_sections_from_file_path_nodes      (TP_Context *tp, Arena *arena, RDIB_DataSectionList *sect_list,  RDIB_PathTree *tree);
void rdib_data_sections_from_source_files         (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, RDIB_PathTree *path_tree, ulong total_src_file_count, ulong src_file_chunk_count, RDIB_SourceFileChunk **src_file_chunks);
void rdib_data_sections_from_units                (Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, RDIB_PathTree *path_tree, ulong total_unit_count, ulong unit_chunk_count, RDIB_UnitChunk **unit_chunks);
void rdib_data_sections_from_string_map           (TP_Context *tp, Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMapBucket **buckets, ulong bucket_count);
void rdib_data_sections_from_types(TP_Context            *tp,
                                            Arena                 *arena,
                                            RDIB_DataSectionList  *sect_list,
                                            RDI_Arch               arch,
                                            RDIB_StringMap        *string_map,
                                            RDIB_IndexRunMap      *idx_run_map,
                                            ulong                    udt_member_chunk_count,
                                            RDIB_TypeChunk       **udt_member_type_chunks,
                                            ulong                    enum_member_chunk_count,
                                            RDIB_TypeChunk       **enum_member_type_chunks,
                                            ulong                    total_type_node_count,
                                            ulong                    type_chunk_count,
                                            RDIB_TypeChunk       **type_chunks,
                                            RDIB_TypeStats         type_stats);
void rdib_data_sections_from_global_variables     (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, ulong total_count, ulong chunk_count, RDIB_VariableChunk **chunks);
void rdib_data_sections_from_thread_variables     (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, ulong total_count, ulong chunk_count, RDIB_VariableChunk **chunks);
void rdib_data_sections_from_procedures           (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, ulong total_count, ulong chunk_count, RDIB_ProcedureChunk **chunks);
void rdib_data_sections_from_scopes               (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, ulong total_count, ulong chunk_count, RDIB_ScopeChunk **chunks);
void rdib_data_sections_from_name_maps            (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, RDIB_IndexRunMap *idx_run_map, RDIB_StringMapBucket **src_name_maps[RDI_NameMapKind_COUNT], ulong src_name_map_counts[RDI_NameMapKind_COUNT]);
void rdib_data_sections_from_source_line_maps 	   (TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, ulong total_src_file_count, ulong src_file_chunk_count, RDIB_SourceFileChunk **src_fille_chunks);
void rdib_data_sections_from_unit_gvar_scope_vmaps(TP_Context *tp, TP_Arena *arena, RDIB_DataSectionList *sect_list, ulong unit_chunk_count, RDIB_UnitChunk **unit_chunks, ulong gvar_chunk_count, RDIB_VariableChunk **gvar_chunks, ulong scope_chunk_count, RDIB_ScopeChunk **scope_chunks);
void rdib_data_sections_from_inline_sites         (TP_Context *tp, Arena *arena, RDIB_DataSectionList *sect_list, RDIB_StringMap *string_map, ulong total_inline_site_count, ulong inline_site_chunk_count, RDIB_InlineSiteChunk **inline_site_chunks);
void rdib_data_sections_from_checksums            (TP_Context *tp, Arena *arena, RDIB_DataSectionList *sect_list);

RDIB_Input  rdib_init_input(Arena *arena);
String8List rdib_finish(TP_Context *tp, TP_Arena *arena, RDIB_Input *input);


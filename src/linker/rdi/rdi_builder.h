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
} RDI_ChecksumKind;

////////////////////////////////

enum
{
  RDIB_DataModel_Null,
  RDIB_DataModel_ILP32,
  RDIB_DataModel_LLP64,
  RDIB_DataModel_LP64,
  RDIB_DataModel_ILP64,
  RDIB_DataModel_SILP64
} RDIB_DataModel;

////////////////////////////////

typedef void * RDIB_TypeRef;

typedef struct RDIB_EvalBytecodeOp
{
  RDIB_EvalBytecodeOp* next;
  RDI_EvalOp                  op;
  U64                         p_size;
  U64                         p;
} RDIB_EvalBytecodeOp;

typedef struct RDIB_EvalBytecode
{
  U64                  count;
  U64                  size;
  RDIB_EvalBytecodeOp* first;
  RDIB_EvalBytecodeOp* last;
} RDIB_EvalBytecode;

typedef struct RDIB_Location
{
  RDI_LocationKind kind;
  Rng1U64List      ranges;
  union {
    struct {
      RDI_RegCode reg_code;
      U64         offset;
    }
    RDIB_EvalBytecode bytecode;
  }

  // used by RDI builder
  U64 data_offset;
} RDIB_Location;

typedef struct RDIB_LocationNode
{
  RDIB_LocationNode* next;
  RDIB_Location             v;
} RDIB_LocationNode;

typedef struct RDIB_LocationList
{
  U64                count;
  RDIB_LocationNode* first;
  RDIB_LocationNode* last;
} RDIB_LocationList;


typedef struct RDIB_Variable
{
  RDI_LinkFlags              link_flags;
  String8                    name;
  String8                    link_name;
  RDI_LocalKind              kind;
  RDIB_Type*          type;
  RDIB_Type*          container_type;
  RDIB_Procedure*     container_proc;
  RDIB_LocationList          locations;
  RDIB_VariableChunk* chunk;
  RDIB_Variable*      next;
} RDIB_Variable;

typedef struct RDIB_VariableNode
{
  RDIB_VariableNode*  next;
  RDIB_Variable              v;
} RDIB_VariableNode;

typedef struct RDIB_VariableList
{
  U64                count;
  RDIB_VariableNode* first;
  RDIB_VariableNode* last;
} RDIB_VariableList;

////////////////////////////////

typedef struct
{
  RDI_Arch arch;
  U64      exe_hash;
  U64      voff_max;
  String8  exe_name;
  String8  producer_string;
} RDIB_TopLevelInfo;

typedef struct
{
  String8                name;
  RDI_BinarySectionFlags flags;
  U64                    voff_first;
  U64                    voff_opl;
  U64                    foff_first;
  U64                    foff_opl;
} RDIB_BinarySection;

typedef struct RDIB_LineTableFragment
{
  RDIB_SourceFile*        src_file;
  PairU32                        ln_voff;
  U64*                           voffs;
  U32*                           line_nums;
  U16*                           col_nums;
  U64                            line_count;
  U64                            col_count;
  RDIB_LineTableFragment* next_src_file;
  RDIB_LineTableFragment* next_line_table;
  RDIB_LineTableFragmentChunk* chunk;
} RDIB_LineTableFragment;

typedef struct RDIB_LineTableFragmentChunk
{
  RDIB_LineTableFragmentChunk* next;
  U64 base;
  U64 count;
  U64 cap;
  RDIB_LineTableFragment* v;
} RDIB_LineTableFragmentChunk;

typedef struct RDIB_LineTableFragmentChunkList
{
  U64 count;
  RDIB_LineTableFragmentChunk* first;
  RDIB_LineTableFragmentChunk* last;
} RDIB_LineTableFragmentChunkList;

typedef struct RDIB_LineTable
{
  RDIB_LineTableChunk* chunk;
  U64                         count;
  RDIB_LineTableFragment*     first;
  RDIB_LineTableFragment*     last;
  U32                         output_array_idx;
} RDIB_LineTable;

typedef struct RDIB_LineTableChunk
{
  RDIB_LineTableChunk* next;
  U64                         base;
  U64                         count;
  U64                         cap;
  RDIB_LineTable*             v;
} RDIB_LineTableChunk;

typedef struct RDIB_LineTableChunkList
{
  U64                  count;
  RDIB_LineTableChunk* first;
  RDIB_LineTableChunk* last;
} RDIB_LineTableChunkList;

typedef struct RDIB_SourceFile
{
  String8                      file_path;
  String8                      normal_full_path;
  RDI_ChecksumKind             checksum_kind;
  String8                      checksum;
  RDIB_LineTableFragment*      line_table_frags;

  U64 src_line_map_idx;
  U64 line_table_idx;

  RDIB_SourceFileChunk* chunk;
} RDIB_SourceFile;

typedef struct RDIB_SourceFileChunk
{
  RDIB_SourceFileChunk* next;
  U64                          base;
  U64                          count;
  U64                          cap;
  RDIB_SourceFile*             v;
} RDIB_SourceFileChunk;

typedef struct RDIB_SourceFileChunkList
{
  U64                   count;
  RDIB_SourceFileChunk* first;
  RDIB_SourceFileChunk* last;
} RDIB_SourceFileChunkList;

typedef struct RDIB_Procedure
{
  RDI_LinkFlags          link_flags;
  String8                name;
  String8                link_name;
  RDIB_Type*      type;
  RDIB_Type*      container_type;
  RDIB_Procedure* container_proc;

  RDIB_Scope*     scope;
  RDIB_ScopeNode* scope_first;
  RDIB_ScopeNode* scope_last;
  
  U64                         scope_count;
  RDIB_ProcedureChunk* chunk;
} RDIB_Procedure;

typedef struct RDIB_Scope
{
  RDIB_Scope*      parent;
  RDIB_Scope*      first_child;
  RDIB_Scope*      last_child;
  RDIB_Scope*      next_sibling;
  RDIB_Procedure*  container_proc;
  Rng1U64List             ranges;
  RDIB_Variable*          local_first;
  RDIB_Variable*          local_last;
  U64                     local_count;
  RDIB_InlineSite* inline_site;

  // used by RDI builder
  U64 local_first_idx;

  RDIB_ScopeChunk* chunk;
} RDIB_Scope;

typedef struct RDIB_InlineSite
{
  String8                      name;
  RDIB_Type*            type;
  RDIB_Type*            owner;
  RDIB_InlineSiteChunk* chunk;
  union {
    RDIB_LineTable* line_table;
    struct {
      void* ud0;
      U64   ud1;
      U64   ud2;
    } convert_ref;
  }
} RDIB_InlineSite;

enum RDI_MemberKindExt : RDI_MemberKind
{
  RDI_MemberKind_COUNT = RDI_MemberKind_NestedType,
  RDI_MemberKindExt_MemberListPointer // NOTE: must always be last in the list!
}

typedef struct RDIB_UDTMember
{
  RDI_MemberKindExt kind;
  union {
    struct {
      String8      name;
      U64          offset;
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
      U64             vftable_offset;
    } method;
    struct {
      String8      name;
      RDIB_TypeRef type_ref;
    } nested_type;
    struct {
      RDIB_TypeRef type_ref;
      U64          offset;
    } base_class;
    struct {
      RDIB_TypeRef type_ref;
      U64          vbptr_off;
      U64          vtable_off;
    } virtual_base_class;
    struct {
      String8 name;
      U64     value;
    } enumerate;
  }
  RDIB_UDTMember*      next;
  RDIB_UDTMemberChunk* chunk;
} RDIB_UDTMember;

typedef struct RDIB_UDTMemberList
{
  U64             count;
  RDIB_UDTMember* first;
  RDIB_UDTMember* last;
} RDIB_UDTMemberList;

typedef struct RDIB_UDT
{
  RDIB_Type* self_type;
  RDIB_Type* members;
  RDIB_SourceFile*  decl_src_file;
  U32               decl_line_num;
  U32               decl_col_num;
} RDIB_UDT;

enum
{
  RDI_TypeKindExt_Lo = RDI_TypeKind_Count,
  RDI_TypeKindExt_VirtualTable,
  RDI_TypeKindExt_StaticMethod,
  RDI_TypeKindExt_Members,
  RDI_TypeKindExt_Params,
  RDI_TypeKindExt_Count,
}
typedef RDI_TypeKind RDI_TypeKindExt;

typedef struct RDIB_Type
{
  RDI_TypeKindExt kind;
  U64 final_idx;
  U64 itype;
  union {
    struct {
      String8 name;
      U64     size;
    } builtin;
    struct {
      RDI_TypeModifierFlags flags;
      RDIB_TypeRef          type_ref;
    } modifier;
    struct {
      RDIB_TypeRef type_ref;
      U64          size;
    } ptr;
    struct {
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      U64 param_idx_run_bucket_idx;
    } func;
    struct {
      RDIB_TypeRef class_type;
      RDIB_TypeRef this_type;
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      U64 param_idx_run_bucket_idx;
    } method;
    struct {
      RDIB_TypeRef class_type;
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      U64 param_idx_run_bucket_idx;
    } static_method;
    struct {
      U64          off;
      U64          count;
      RDIB_TypeRef value_type;
    } bitfield;
    struct {
      RDIB_TypeRef entry_type;
      U64          size;
    } array;
    struct {
      String8      name;
      String8      link_name;
      RDIB_TypeRef members;
      // assigned in UDT build step
      U64 udt_idx;
      union {
        struct {
          U64          size;
          RDIB_TypeRef derived;
          RDIB_TypeRef vtshape;
        } struct_type;
        struct {
          U64          size;
        } union_type;
        struct {
          String8      name;
          RDIB_TypeRef base_type;
        } enum_type;
      }
    } udt;
    struct {
      U64           count;
      RDIB_TypeRef* types;
    } params;
    struct {
      RDIB_UDTMemberList list;

      // assigned in member build step
      B32 is_head;
      U64 first_member_idx;
    } members, enum_members;
  }
  RDIB_TypeChunk* chunk;
} RDIB_Type;

typedef struct RDIB_Unit
{
  RDI_Arch        arch;
  String8         unit_name;
  String8         compiler_name;
  String8         source_file;
  String8         object_file;
  String8         archive_file;
  String8         build_path;
  RDI_Language    language;
  RDIB_LineTable* line_table;
  U64             virt_range_count;
  Rng1U64*        virt_ranges;

  RDIB_UnitChunk* chunk;
} RDIB_Unit;

typedef struct RDIB_DataSection
{
  RDI_SectionKind tag;
  String8List     data;
} RDIB_DataSection;

typedef struct RDIB_DataSectionNode
{
  RDIB_DataSectionNode* next;
  RDIB_DataSection             v;
} RDIB_DataSectionNode;

typedef struct RDIB_DataSectionList
{
  U64                   count;
  RDIB_DataSectionNode* first;
  RDIB_DataSectionNode* last;
} RDIB_DataSectionList;


typedef struct RDIB_UnitChunk
{
  U64                    base;
  U64                    count;
  U64                    cap;
  RDIB_Unit*             v;
  RDIB_UnitChunk* next;
} RDIB_UnitChunk;

typedef struct RDIB_UnitChunkList
{
  U64             count;
  RDIB_UnitChunk* first;
  RDIB_UnitChunk* last;
} RDIB_UnitChunkList;

typedef struct RDIB_VariableChunk
{
  RDIB_VariableChunk* next;
  U64                        base;
  U64                        count;
  U64                        cap;
  RDIB_Variable*             v;
} RDIB_VariableChunk;

typedef struct RDIB_VariableChunkList
{
  U64                 count;
  RDIB_VariableChunk* first;
  RDIB_VariableChunk* last;
} RDIB_VariableChunkList;

typedef struct RDIB_ProcedureChunk
{
  RDIB_ProcedureChunk* next;
  U64                         base;
  U64                         count;
  U64                         cap;
  RDIB_Procedure*             v;
} RDIB_ProcedureChunk;

typedef struct RDIB_ProcedureChunkList
{
  U64                  count;
  RDIB_ProcedureChunk* first;
  RDIB_ProcedureChunk* last;
} RDIB_ProcedureChunkList;

typedef struct RDIB_ScopeChunk
{
  RDIB_ScopeChunk* next;
  U64                     base;
  U64                     count;
  U64                     cap;
  RDIB_Scope*             v;
} RDIB_ScopeChunk;

typedef struct RDIB_ScopeChunkList
{
  U64              count;
  RDIB_ScopeChunk* first;
  RDIB_ScopeChunk* last;
} RDIB_ScopeChunkList;

typedef struct RDIB_ScopeNode
{
  RDIB_ScopeNode* next;
  RDIB_Scope*            v;
} RDIB_ScopeNode;

typedef struct RDIB_ScopeList
{
  U64         count;
  RDIB_Scope* first;
  RDIB_Scope* last;
} RDIB_ScopeList;

typedef struct RDIB_InlineSiteChunk
{
  RDIB_InlineSiteChunk* next;
  U64                          base;
  U64                          count;
  U64                          cap;
  RDIB_InlineSite*             v;
} RDIB_InlineSiteChunk;

typedef struct RDIB_InlineSiteChunkList
{
  U64                   count;
  RDIB_InlineSiteChunk* first;
  RDIB_InlineSiteChunk* last;
} RDIB_InlineSiteChunkList;

typedef struct RDIB_TypeChunk
{
  RDIB_TypeChunk* next;
  U64                    base;
  U64                    count;
  U64                    cap;
  RDIB_Type*             v;
} RDIB_TypeChunk;

typedef struct
{
  U64             count;
  RDIB_TypeChunk* first;
  RDIB_TypeChunk* last;
} RDIB_TypeChunkList;

typedef struct RDIB_UDTMemberChunk
{
  RDIB_UDTMemberChunk* next;
  U64                         base;
  U64                         count;
  U64                         cap;
  RDIB_UDTMember*             v;
} RDIB_UDTMemberChunk;

typedef struct RDIB_UDTMemberChunkList
{
  U64                  count;
  RDIB_UDTMemberChunk* first;
  RDIB_UDTMemberChunk* last;
} RDIB_UDTMemberChunkList;

////////////////////////////////
// UDT Forward Ref Map

typedef struct
{
  RDIB_Type* type;
  U64 idx;
} RDIB_UDTFwdrefBucket;

////////////////////////////////
// String Map

typedef struct RDIB_StringMapBucket
{
  String8 string;

  union {
    // to get deterministic output we assign each bucket a unique index
    union {
      struct {
        U32 lo;
        U32 hi;
      }
      U64 v;
    } sorter;

    // after buckets are sorted we replace 'sorter' with indices into output array
    U64 idx;
  }

  union {
    // depending on the usage context sotres: pointers to variables, procedures, and etc.
    VoidNode* raw_values;

    // during index-run-map build step 'raw_values' are replaced with index-run bucket index
    struct {
      U32 count;
      // if we have single index - store it in the bucket
      union {
        U64 idx_run_bucket_idx;
        U32 match_idx;
      }
    }
  }
} RDIB_StringMapBucket;

typedef struct RDIB_StringMap
{
  U64                    cap;
  RDIB_StringMapBucket** buckets;
} RDIB_StringMap;

#define RDIB_STRING_MAP_UPDATE_FUNC(name) void name(VoidNode **head, VoidNode *node)
typedef RDIB_STRING_MAP_UPDATE_FUNC(RDIB_StringMapUpdateFunc);

typedef struct
{
  RDIB_StringMap*        string_map;
  Rng1U64*               ranges;
  U64*                   counts;
  U64*                   offsets;
  RDIB_StringMapBucket** result;
} RDIB_GetExtantBucketsStringMapTask;

typedef struct
{
  U32*                   string_table;
  U64                    string_data_size;
  U8*                    string_data;
  RDIB_StringMapBucket** buckets;
  Rng1U64*               ranges;
} RDIB_CopyStringDataTask;

typedef struct
{
  U64                    chunk_idx_opl;
  Rng1U64*               ranges;
  RDIB_StringMapBucket** src;
  RDIB_StringMapBucket** dst;
  U32*                   chunk_histo;
  U32*                   chunk_offsets;
} RDIB_StringMapRadixSort;

////////////////////////////////
// Index Run Map

typedef struct RDIB_IndexRunBucket
{
  union {
    struct {
      U32 lo;
      U32 hi;
    }
    U64 v;
  } sorter;
  U32Array indices;
  U64      index_in_output_array;
} RDIB_IndexRunBucket;

typedef struct RDIB_IndexRunMap
{
  U64                   cap;
  RDIB_IndexRunBucket** buckets;
} RDIB_IndexRunMap;

////////////////////////////////

typedef struct
{
  U64 voff;
  U32 size;
  U32 idx;
} RDIB_VMapRange;

////////////////////////////////

#define RDIB_PATH_TREE_NIL_STRING str8_lit("<nil>")

typedef struct RDIB_PathTreeNode
{
  RDIB_PathTreeNode* parent;
  RDIB_PathTreeNode* next_order;
  RDIB_PathTreeNode* next_sibling;
  RDIB_PathTreeNode* first_child;
  RDIB_PathTreeNode* last_child;
  U64                       node_idx;
  String8                   sub_path;
  RDIB_SourceFile*          src_file;
} RDIB_PathTreeNode;

typedef struct RDIB_PathTreeNodeList
{
  U64                count;
  RDIB_PathTreeNode* first;
  RDIB_PathTreeNode* last;
} RDIB_PathTreeNodeList;

typedef struct RDIB_PathTree
{
  RDIB_PathTreeNode*     root;
  U64                    node_count;
  U64                    next_list_idx;
  U64                    list_count;
  RDIB_PathTreeNodeList* node_lists;
} RDIB_PathTree;

////////////////////////////////

typedef struct RDIB_Input
{
  U64 unit_chunk_cap;
  U64 src_file_chunk_cap;
  U64 symbol_chunk_cap;
  U64 line_table_cap;
  U64 inline_site_cap;
  U64 type_cap;
  U64 udt_cap;

  RDIB_TopLevelInfo         top_level_info;
  U64                       sect_count;
  RDIB_BinarySection*       sections;
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

  RDIB_SourceFile*        null_src_file;
  RDIB_LineTable*         null_line_table;
  RDIB_LineTableFragment* null_frag;
  RDIB_Type*              null_type;
  RDIB_Scope*             null_scope;
  RDIB_Unit*              null_unit;
  RDIB_Procedure*         null_proc;
  RDIB_Variable*          null_local;
  RDIB_Variable*          null_gvar;
  RDIB_Variable*          null_tvar;
  RDIB_UDTMember*         null_udt_member;
  RDIB_UDTMember*         null_enum_member;
  RDIB_InlineSite*        null_inline_site;

  RDIB_Type* variadic_type;

  //RDIB_TypeChunkList       struct_list;
  //RDIB_TypeChunkList       union_list;
  //RDIB_TypeChunkList       enum_list;
  //RDIB_TypeChunkList       param_types;
} RDIB_Input;

////////////////////////////////
// Parallel For Tasks

typedef struct
{
  U64* udt_counts;
} RDIB_TypeStats;

typedef struct
{
  RDIB_TypeChunk** chunks;
  RDIB_TypeStats*  type_stats;
} RDIB_TypeStatsTask;

typedef struct
{
  Rng1U64*         ranges;
  U64*             counts;
  U64*             offsets;
  RDIB_TypeChunk** type_chunks;
  RDIB_StringMap*  string_map;
  union {
    RDI_Member*     udt_members_rdi;
    RDI_EnumMember* enum_members_rdi;
  }
} RDIB_MembersTask;

typedef struct
{
  RDIB_TypeChunk**    type_chunks;
  RDIB_TypeStats      type_stats;
  U64*                udt_base_idx;
  RDI_UDT*            udts;
} RDIB_UserDefinesTask;

typedef struct
{
  U64                 addr_size;
  RDIB_StringMap*     string_map;
  RDIB_IndexRunMap*   idx_run_map;
  RDIB_TypeChunk**    type_chunks;
  RDIB_TypeStats      type_stats;
  RDI_TypeNode*       type_nodes;
} RDIB_TypeNodesTask;

typedef struct
{
  RDIB_StringMap*            string_map;
  Rng1U64*                   ranges;
  RDIB_StringMapUpdateFunc*  string_map_update_func;
  RDIB_StringMapBucket**     free_buckets;
  U64*                       insert_counts;
  U64*                       element_indices;
  union
  {
    RDIB_UnitChunk**        units;
    RDIB_BinarySection*     sects;
    RDIB_SourceFileChunk**  src_file_chunks;
    RDIB_VariableChunk**    vars;
    RDIB_ProcedureChunk**   procs;
    RDIB_InlineSiteChunk**  inline_sites;
    RDIB_UDTMemberChunk**   udt_members;
    RDIB_UDTMemberChunk**   enum_members;
    RDIB_TypeChunk**        types;
    RDIB_PathTreeNodeList*  path_node_lists;
  }
} RDIB_CollectStringsTask;

typedef struct
{
  RDIB_StringMap*  string_map;
  Rng1U64*         ranges;
  RDIB_TypeChunk** chunks;
  String8List*     data_lists;
} RDIB_BuildTypeDataTask;

typedef struct
{
  RDIB_StringMap* string_map;
  Rng1U64*        ranges;
  union {
    struct {
      RDIB_VariableChunk** gvars_rdib;
      String8List*         gvars_out;
    }
    struct {
      RDIB_VariableChunk** tvars_rdib;
      String8List*         tvars_out;
    }
    struct {
      RDIB_ProcedureChunk** procs_rdib;
      String8List*          procs_out;
    }
    struct {
      RDIB_ScopeChunk**  scopes_rdib;
      U64*               scope_voff_counts;
      U64*               loc_data_sizes;
      U64*               local_counts;
      U64*               loc_block_counts;
      U64*               scope_voff_offsets;
      U64*               local_offsets;
      U64*               loc_block_offsets;
      U64*               loc_data_offsets;
      U64*               scope_voffs_rdi;
      RDI_Scope*         scopes_rdi;
      RDI_Local*         locals_rdi;
      RDI_LocationBlock* loc_blocks_rdi;
      U8*                loc_data_rdi;
    }
  }
} RDIB_BuildSymbolSectionTask;

typedef union
{
  struct {
    U64*            counts;
    U64*            offsets;
    Rng1U64*        ranges;
    RDIB_VMapRange* vmap;
    union {
      RDIB_UnitChunk**     unit_chunks;
      RDIB_VariableChunk** gvar_chunks;
      RDIB_ScopeChunk**    scope_chunks;
    }
  }

  struct {
    U64[3]             vmap_counts;
    RDIB_VMapRange *vmaps[3];
    String8List[3]     raw_vmaps;
  }
} RDIB_VMapBuilderTask;

typedef struct
{
  U64                   sorter_idx;
  RDI_NameMapKind       name_map_kind;
  RDIB_IndexRunMap*     idx_run_map;
  RDIB_IndexRunBucket** free_buckets;
  Rng1U64*              ranges;
  union {
    RDIB_TypeChunk**       type_chunks;
    RDIB_StringMapBucket** name_map_buckets;
  }
} RDIB_BuildIndexRunsTask;

typedef struct
{
  RDIB_IndexRunBucket** buckets;
  Rng1U64*             ranges;
  U32*                 output_array;
} RDIB_IdxRunCopyTask;

typedef struct
{
  RDIB_IndexRunMap*     idx_run_map;
  Rng1U64*              ranges;
  U64*                  counts;
  U64*                  offsets;
  RDIB_IndexRunBucket** result;
} RDIB_GetExtantBucketsIndexRunMapTask;

typedef struct
{
  U64                   chunk_idx_opl;
  Rng1U64*              ranges;
  RDIB_IndexRunBucket** src;
  RDIB_IndexRunBucket** dst;
  U32*                  chunk_histo;
  U32*                  chunk_offsets;
} RDIB_IndexRunMapRadixSort;

typedef struct
{
  RDIB_StringMap*         string_map;
  RDIB_IndexRunMap*       idx_run_map;
  U64*                    in_bucket_counts;
  RDIB_StringMapBucket ***in_buckets;

  RDI_NameMapBucket** out_buckets;
  RDI_NameMapNode**   out_nodes;
  U64*                out_bucket_counts;
  U64*                out_node_counts;
} RDIB_NameMapBuilderTask;

typedef struct
{
  RDIB_PathTree*    path_tree;
  RDIB_StringMap*   string_map;
  RDI_FilePathNode* nodes_dst;
} RDIB_BuildFilePathNodesTask;

typedef struct
{
  RDIB_SourceFile** src_file_arr;
  U32*             out_line_counts;
  U32*             out_voff_counts;
  U32**             out_line_nums;
  U32**             out_line_ranges;
  U64**             out_voffs;
} RDIB_SrcLineMapsTask;

typedef struct
{
  RDIB_LineTableChunk** chunks;
  Rng1U64*              ranges;

  U64*       out_line_table_counts;
  U64**      out_line_table_voffs;
  RDI_Line** out_line_table_lines;
} RDIB_BuildLineTablesTask;

typedef struct
{
  Rng1U64*               ranges;
  RDIB_StringMap*        string_map;
  RDIB_PathTree*         path_tree;
  RDIB_SourceFileChunk** src_file_chunks;
  RDI_SourceFile*        src_files_dst;
} RDIB_FillSourceFilesTask;

////////////////////////////////
// Data Model Helpers



////////////////////////////////



////////////////////////////////
// Chunk Lists

// push


// push many


// concat in place

// concat in place many

// array from chunk list

// total count from chunk list

// pointer -> array index

////////////////////////////////

//- Data Sections



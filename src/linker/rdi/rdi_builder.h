// Copyright (c) 2025 Epic Games Tools
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

typedef enum
{
  RDI_Checksum_Null,
  RDI_Checksum_MD5,
  RDI_Checksum_SHA1,
  RDI_Checksum_SHA256,
  RDI_Checksum_TimeStamp
} RDI_ChecksumKind;

////////////////////////////////

typedef enum
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
  struct RDIB_EvalBytecodeOp *next;
  RDI_EvalOp                  op;
  uint64                         p_size;
  uint64                         p;
} RDIB_EvalBytecodeOp;

typedef struct RDIB_EvalBytecode
{
  uint64                  count;
  uint64                  size;
  RDIB_EvalBytecodeOp *first;
  RDIB_EvalBytecodeOp *last;
} RDIB_EvalBytecode;

typedef struct RDIB_Location
{
  RDI_LocationKind kind;
  Rng1<uint64List>      ranges;
  union {
    struct {
      RDI_RegCode reg_code;
      uint64         offset;
    };
    RDIB_EvalBytecode bytecode;
  };

  // used by RDI builder
  uint64 data_offset;
} RDIB_Location;

typedef struct RDIB_LocationNode
{
  struct RDIB_LocationNode *next;
  RDIB_Location             v;
} RDIB_LocationNode;

typedef struct RDIB_LocationList
{
  uint64                count;
  RDIB_LocationNode *first;
  RDIB_LocationNode *last;
} RDIB_LocationList;


typedef struct RDIB_Variable
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
} RDIB_Variable;

typedef struct RDIB_VariableNode
{
  struct RDIB_VariableNode  *next;
  RDIB_Variable              v;
} RDIB_VariableNode;

typedef struct RDIB_VariableList
{
  uint64                count;
  RDIB_VariableNode *first;
  RDIB_VariableNode *last;
} RDIB_VariableList;

////////////////////////////////

typedef struct
{
  RDI_Arch arch;
  uint64      exe_hash;
  uint64      voff_max;
  String8  exe_name;
  String8  producer_string;
} RDIB_TopLevelInfo;

typedef struct
{
  String8                name;
  RDI_BinarySectionFlags flags;
  uint64                    voff_first;
  uint64                    voff_opl;
  uint64                    foff_first;
  uint64                    foff_opl;
} RDIB_BinarySection;

typedef struct RDIB_LineTableFragment
{
  struct RDIB_SourceFile        *src_file;
  Pairuint32                        ln_voff;
  uint64                           *voffs;
  uint32                           *line_nums;
  uint16                           *col_nums;
  uint64                            line_count;
  uint64                            col_count;
  struct RDIB_LineTableFragment *next_src_file;
  struct RDIB_LineTableFragment *next_line_table;
  struct RDIB_LineTableFragmentChunk *chunk;
} RDIB_LineTableFragment;

typedef struct RDIB_LineTableFragmentChunk
{
  struct RDIB_LineTableFragmentChunk *next;
  uint64 base;
  uint64 count;
  uint64 cap;
  RDIB_LineTableFragment *v;
} RDIB_LineTableFragmentChunk;

typedef struct RDIB_LineTableFragmentChunkList
{
  uint64 count;
  RDIB_LineTableFragmentChunk *first;
  RDIB_LineTableFragmentChunk *last;
} RDIB_LineTableFragmentChunkList;

typedef struct RDIB_LineTable
{
  struct RDIB_LineTableChunk *chunk;
  uint64                         count;
  RDIB_LineTableFragment     *first;
  RDIB_LineTableFragment     *last;
  uint32                         output_array_idx;
} RDIB_LineTable;

typedef struct RDIB_LineTableChunk
{
  struct RDIB_LineTableChunk *next;
  uint64                         base;
  uint64                         count;
  uint64                         cap;
  RDIB_LineTable             *v;
} RDIB_LineTableChunk;

typedef struct RDIB_LineTableChunkList
{
  uint64                  count;
  RDIB_LineTableChunk *first;
  RDIB_LineTableChunk *last;
} RDIB_LineTableChunkList;

typedef struct RDIB_SourceFile
{
  String8                      file_path;
  String8                      normal_full_path;
  RDI_ChecksumKind             checksum_kind;
  String8                      checksum;
  RDIB_LineTableFragment      *line_table_frags;

  uint64 src_line_map_idx;
  uint64 line_table_idx;

  struct RDIB_SourceFileChunk *chunk;
} RDIB_SourceFile;

typedef struct RDIB_SourceFileChunk
{
  struct RDIB_SourceFileChunk *next;
  uint64                          base;
  uint64                          count;
  uint64                          cap;
  RDIB_SourceFile             *v;
} RDIB_SourceFileChunk;

typedef struct RDIB_SourceFileChunkList
{
  uint64                   count;
  RDIB_SourceFileChunk *first;
  RDIB_SourceFileChunk *last;
} RDIB_SourceFileChunkList;

typedef struct RDIB_Procedure
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
  
  uint64                         scope_count;
  struct RDIB_ProcedureChunk *chunk;
} RDIB_Procedure;

typedef struct RDIB_Scope
{
  struct RDIB_Scope      *parent;
  struct RDIB_Scope      *first_child;
  struct RDIB_Scope      *last_child;
  struct RDIB_Scope      *next_sibling;
  struct RDIB_Procedure  *container_proc;
  Rng1<uint64List>             ranges;
  RDIB_Variable          *local_first;
  RDIB_Variable          *local_last;
  uint64                     local_count;
  struct RDIB_InlineSite *inline_site;

  // used by RDI builder
  uint64 local_first_idx;

  struct RDIB_ScopeChunk *chunk;
} RDIB_Scope;

typedef struct RDIB_InlineSite
{
  String8                      name;
  struct RDIB_Type            *type;
  struct RDIB_Type            *owner;
  struct RDIB_InlineSiteChunk *chunk;
  union {
    struct RDIB_LineTable *line_table;
    struct {
      void *ud0;
      uint64   ud1;
      uint64   ud2;
    } convert_ref;
  };
} RDIB_InlineSite;

enum RDI_MemberKindExt : RDI_MemberKind
{
  RDI_MemberKind_COUNT = RDI_MemberKind_NestedType,
  RDI_MemberKindExt_MemberListPointer // NOTE: must always be last in the list!
};

typedef struct RDIB_UDTMember
{
  RDI_MemberKindExt kind;
  union {
    struct {
      String8      name;
      uint64          offset;
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
      uint64             vftable_offset;
    } method;
    struct {
      String8      name;
      RDIB_TypeRef type_ref;
    } nested_type;
    struct {
      RDIB_TypeRef type_ref;
      uint64          offset;
    } base_class;
    struct {
      RDIB_TypeRef type_ref;
      uint64          vbptr_off;
      uint64          vtable_off;
    } virtual_base_class;
    struct {
      String8 name;
      uint64     value;
    } enumerate;
  };
  struct RDIB_UDTMember      *next;
  struct RDIB_UDTMemberChunk *chunk;
} RDIB_UDTMember;

typedef struct RDIB_UDTMemberList
{
  uint64             count;
  RDIB_UDTMember *first;
  RDIB_UDTMember *last;
} RDIB_UDTMemberList;

typedef struct RDIB_UDT
{
  struct RDIB_Type *self_type;
  struct RDIB_Type *members;
  RDIB_SourceFile  *decl_src_file;
  uint32               decl_line_num;
  uint32               decl_col_num;
} RDIB_UDT;

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

typedef struct RDIB_Type
{
  RDI_TypeKindExt kind;
  uint64 final_idx;
  uint64 itype;
  union {
    struct {
      String8 name;
      uint64     size;
    } builtin;
    struct {
      RDI_TypeModifierFlags flags;
      RDIB_TypeRef          type_ref;
    } modifier;
    struct {
      RDIB_TypeRef type_ref;
      uint64          size;
    } ptr;
    struct {
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      uint64 param_idx_run_bucket_idx;
    } func;
    struct {
      RDIB_TypeRef class_type;
      RDIB_TypeRef this_type;
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      uint64 param_idx_run_bucket_idx;
    } method;
    struct {
      RDIB_TypeRef class_type;
      RDIB_TypeRef return_type;
      RDIB_TypeRef params_type;

      uint64 param_idx_run_bucket_idx;
    } static_method;
    struct {
      uint64          off;
      uint64          count;
      RDIB_TypeRef value_type;
    } bitfield;
    struct {
      RDIB_TypeRef entry_type;
      uint64          size;
    } array;
    struct {
      String8      name;
      String8      link_name;
      RDIB_TypeRef members;
      // assigned in UDT build step
      uint64 udt_idx;
      union {
        struct {
          uint64          size;
          RDIB_TypeRef derived;
          RDIB_TypeRef vtshape;
        } struct_type;
        struct {
          uint64          size;
        } union_type;
        struct {
          String8      name;
          RDIB_TypeRef base_type;
        } enum_type;
      };
    } udt;
    struct {
      uint64           count;
      RDIB_TypeRef *types;
    } params;
    struct {
      RDIB_UDTMemberList list;

      // assigned in member build step
      B32 is_head;
      uint64 first_member_idx;
    } members, enum_members;
  };
  struct RDIB_TypeChunk *chunk;
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
  RDIB_LineTable *line_table;
  uint64             virt_range_count;
  Rng1<uint64>        *virt_ranges;

  struct RDIB_UnitChunk *chunk;
} RDIB_Unit;

typedef struct RDIB_DataSection
{
  RDI_SectionKind tag;
  String8List     data;
} RDIB_DataSection;

typedef struct RDIB_DataSectionNode
{
  struct RDIB_DataSectionNode *next;
  RDIB_DataSection             v;
} RDIB_DataSectionNode;

typedef struct RDIB_DataSectionList
{
  uint64                   count;
  RDIB_DataSectionNode *first;
  RDIB_DataSectionNode *last;
} RDIB_DataSectionList;


typedef struct RDIB_UnitChunk
{
  uint64                    base;
  uint64                    count;
  uint64                    cap;
  RDIB_Unit             *v;
  struct RDIB_UnitChunk *next;
} RDIB_UnitChunk;

typedef struct RDIB_UnitChunkList
{
  uint64             count;
  RDIB_UnitChunk *first;
  RDIB_UnitChunk *last;
} RDIB_UnitChunkList;

typedef struct RDIB_VariableChunk
{
  struct RDIB_VariableChunk *next;
  uint64                        base;
  uint64                        count;
  uint64                        cap;
  RDIB_Variable             *v;
} RDIB_VariableChunk;

typedef struct RDIB_VariableChunkList
{
  uint64                 count;
  RDIB_VariableChunk *first;
  RDIB_VariableChunk *last;
} RDIB_VariableChunkList;

typedef struct RDIB_ProcedureChunk
{
  struct RDIB_ProcedureChunk *next;
  uint64                         base;
  uint64                         count;
  uint64                         cap;
  RDIB_Procedure             *v;
} RDIB_ProcedureChunk;

typedef struct RDIB_ProcedureChunkList
{
  uint64                  count;
  RDIB_ProcedureChunk *first;
  RDIB_ProcedureChunk *last;
} RDIB_ProcedureChunkList;

typedef struct RDIB_ScopeChunk
{
  struct RDIB_ScopeChunk *next;
  uint64                     base;
  uint64                     count;
  uint64                     cap;
  RDIB_Scope             *v;
} RDIB_ScopeChunk;

typedef struct RDIB_ScopeChunkList
{
  uint64              count;
  RDIB_ScopeChunk *first;
  RDIB_ScopeChunk *last;
} RDIB_ScopeChunkList;

typedef struct RDIB_ScopeNode
{
  struct RDIB_ScopeNode *next;
  RDIB_Scope            *v;
} RDIB_ScopeNode;

typedef struct RDIB_ScopeList
{
  uint64         count;
  RDIB_Scope *first;
  RDIB_Scope *last;
} RDIB_ScopeList;

typedef struct RDIB_InlineSiteChunk
{
  struct RDIB_InlineSiteChunk *next;
  uint64                          base;
  uint64                          count;
  uint64                          cap;
  RDIB_InlineSite             *v;
} RDIB_InlineSiteChunk;

typedef struct RDIB_InlineSiteChunkList
{
  uint64                   count;
  RDIB_InlineSiteChunk *first;
  RDIB_InlineSiteChunk *last;
} RDIB_InlineSiteChunkList;

typedef struct RDIB_TypeChunk
{
  struct RDIB_TypeChunk *next;
  uint64                    base;
  uint64                    count;
  uint64                    cap;
  RDIB_Type             *v;
} RDIB_TypeChunk;

typedef struct
{
  uint64             count;
  RDIB_TypeChunk *first;
  RDIB_TypeChunk *last;
} RDIB_TypeChunkList;

typedef struct RDIB_UDTMemberChunk
{
  struct RDIB_UDTMemberChunk *next;
  uint64                         base;
  uint64                         count;
  uint64                         cap;
  RDIB_UDTMember             *v;
} RDIB_UDTMemberChunk;

typedef struct RDIB_UDTMemberChunkList
{
  uint64                  count;
  RDIB_UDTMemberChunk *first;
  RDIB_UDTMemberChunk *last;
} RDIB_UDTMemberChunkList;

////////////////////////////////
// UDT Forward Ref Map

typedef struct
{
  struct RDIB_Type *type;
  uint64 idx;
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
        uint32 lo;
        uint32 hi;
      };
      uint64 v;
    } sorter;

    // after buckets are sorted we replace 'sorter' with indices into output array
    uint64 idx;
  };

  union {
    // depending on the usage context sotres: pointers to variables, procedures, and etc.
    VoidNode *raw_values;

    // during index-run-map build step 'raw_values' are replaced with index-run bucket index
    struct {
      uint32 count;
      // if we have single index - store it in the bucket
      union {
        uint64 idx_run_bucket_idx;
        uint32 match_idx;
      };
    };
  };
} RDIB_StringMapBucket;

typedef struct RDIB_StringMap
{
  uint64                    cap;
  RDIB_StringMapBucket **buckets;
} RDIB_StringMap;

#define RDIB_STRING_MAP_UPDATE_FUNC(name) void name(VoidNode **head, VoidNode *node)
typedef RDIB_STRING_MAP_UPDATE_FUNC(RDIB_StringMapUpdateFunc);

typedef struct
{
  RDIB_StringMap        *string_map;
  Rng1<uint64>               *ranges;
  uint64                   *counts;
  uint64                   *offsets;
  RDIB_StringMapBucket **result;
} RDIB_GetExtantBucketsStringMapTask;

typedef struct
{
  uint32                   *string_table;
  uint64                    string_data_size;
  uint8                    *string_data;
  RDIB_StringMapBucket **buckets;
  Rng1<uint64>               *ranges;
} RDIB_CopyStringDataTask;

typedef struct
{
  uint64                    chunk_idx_opl;
  Rng1<uint64>               *ranges;
  RDIB_StringMapBucket **src;
  RDIB_StringMapBucket **dst;
  uint32                   *chunk_histo;
  uint32                   *chunk_offsets;
} RDIB_StringMapRadixSort;

////////////////////////////////
// Index Run Map

typedef struct RDIB_IndexRunBucket
{
  union {
    struct {
      uint32 lo;
      uint32 hi;
    };
    uint64 v;
  } sorter;
  Sapn<uint32> indices;
  uint64      index_in_output_array;
} RDIB_IndexRunBucket;

typedef struct RDIB_IndexRunMap
{
  uint64                   cap;
  RDIB_IndexRunBucket **buckets;
} RDIB_IndexRunMap;

////////////////////////////////

typedef struct
{
  uint64 voff;
  uint32 size;
  uint32 idx;
} RDIB_VMapRange;

////////////////////////////////

#define RDIB_PATH_TREE_NIL_STRING str8_lit("<nil>")

typedef struct RDIB_PathTreeNode
{
  struct RDIB_PathTreeNode *parent;
  struct RDIB_PathTreeNode *next_order;
  struct RDIB_PathTreeNode *next_sibling;
  struct RDIB_PathTreeNode *first_child;
  struct RDIB_PathTreeNode *last_child;
  uint64                       node_idx;
  String8                   sub_path;
  RDIB_SourceFile          *src_file;
} RDIB_PathTreeNode;

typedef struct RDIB_PathTreeNodeList
{
  uint64                count;
  RDIB_PathTreeNode *first;
  RDIB_PathTreeNode *last;
} RDIB_PathTreeNodeList;

typedef struct RDIB_PathTree
{
  RDIB_PathTreeNode     *root;
  uint64                    node_count;
  uint64                    next_list_idx;
  uint64                    list_count;
  RDIB_PathTreeNodeList *node_lists;
} RDIB_PathTree;

////////////////////////////////

typedef struct RDIB_Input
{
  uint64 unit_chunk_cap;
  uint64 src_file_chunk_cap;
  uint64 symbol_chunk_cap;
  uint64 line_table_cap;
  uint64 inline_site_cap;
  uint64 type_cap;
  uint64 udt_cap;

  RDIB_TopLevelInfo         top_level_info;
  uint64                       sect_count;
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
} RDIB_Input;

////////////////////////////////
// Parallel For Tasks

typedef struct
{
  uint64 *udt_counts;
} RDIB_TypeStats;

typedef struct
{
  RDIB_TypeChunk **chunks;
  RDIB_TypeStats  *type_stats;
} RDIB_TypeStatsTask;

typedef struct
{
  Rng1<uint64>         *ranges;
  uint64             *counts;
  uint64             *offsets;
  RDIB_TypeChunk **type_chunks;
  RDIB_StringMap  *string_map;
  union {
    RDI_Member     *udt_members_rdi;
    RDI_EnumMember *enum_members_rdi;
  };
} RDIB_MembersTask;

typedef struct
{
  RDIB_TypeChunk    **type_chunks;
  RDIB_TypeStats      type_stats;
  uint64                *udt_base_idx;
  RDI_UDT            *udts;
} RDIB_UserDefinesTask;

typedef struct
{
  uint64                 addr_size;
  RDIB_StringMap     *string_map;
  RDIB_IndexRunMap   *idx_run_map;
  RDIB_TypeChunk    **type_chunks;
  RDIB_TypeStats      type_stats;
  RDI_TypeNode       *type_nodes;
} RDIB_TypeNodesTask;

typedef struct
{
  RDIB_StringMap            *string_map;
  Rng1<uint64>                   *ranges;
  RDIB_StringMapUpdateFunc  *string_map_update_func;
  RDIB_StringMapBucket     **free_buckets;
  uint64                       *insert_counts;
  uint64                       *element_indices;
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
} RDIB_CollectStringsTask;

typedef struct
{
  RDIB_StringMap  *string_map;
  Rng1<uint64>         *ranges;
  RDIB_TypeChunk **chunks;
  String8List     *data_lists;
} RDIB_BuildTypeDataTask;

typedef struct
{
  RDIB_StringMap *string_map;
  Rng1<uint64>        *ranges;
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
      uint64               *scope_voff_counts;
      uint64               *loc_data_sizes;
      uint64               *local_counts;
      uint64               *loc_block_counts;
      uint64               *scope_voff_offsets;
      uint64               *local_offsets;
      uint64               *loc_block_offsets;
      uint64               *loc_data_offsets;
      uint64               *scope_voffs_rdi;
      RDI_Scope         *scopes_rdi;
      RDI_Local         *locals_rdi;
      RDI_LocationBlock *loc_blocks_rdi;
      uint8                *loc_data_rdi;
    };
  };
} RDIB_BuildSymbolSectionTask;

typedef union
{
  struct {
    uint64            *counts;
    uint64            *offsets;
    Rng1<uint64>        *ranges;
    RDIB_VMapRange *vmap;
    union {
      RDIB_UnitChunk     **unit_chunks;
      RDIB_VariableChunk **gvar_chunks;
      RDIB_ScopeChunk    **scope_chunks;
    };
  };

  struct {
    uint64             vmap_counts[3];
    RDIB_VMapRange *vmaps[3];
    String8List     raw_vmaps[3];
  };
} RDIB_VMapBuilderTask;

typedef struct
{
  uint64                   sorter_idx;
  RDI_NameMapKind       name_map_kind;
  RDIB_IndexRunMap     *idx_run_map;
  RDIB_IndexRunBucket **free_buckets;
  Rng1<uint64>              *ranges;
  union {
    RDIB_TypeChunk       **type_chunks;
    RDIB_StringMapBucket **name_map_buckets;
  };
} RDIB_BuildIndexRunsTask;

typedef struct
{
  RDIB_IndexRunBucket **buckets;
  Rng1<uint64>             *ranges;
  uint32                 *output_array;
} RDIB_IdxRunCopyTask;

typedef struct
{
  RDIB_IndexRunMap     *idx_run_map;
  Rng1<uint64>              *ranges;
  uint64                  *counts;
  uint64                  *offsets;
  RDIB_IndexRunBucket **result;
} RDIB_GetExtantBucketsIndexRunMapTask;

typedef struct
{
  uint64                   chunk_idx_opl;
  Rng1<uint64>              *ranges;
  RDIB_IndexRunBucket **src;
  RDIB_IndexRunBucket **dst;
  uint32                  *chunk_histo;
  uint32                  *chunk_offsets;
} RDIB_IndexRunMapRadixSort;

typedef struct
{
  RDIB_StringMap         *string_map;
  RDIB_IndexRunMap       *idx_run_map;
  uint64                    *in_bucket_counts;
  RDIB_StringMapBucket ***in_buckets;

  RDI_NameMapBucket **out_buckets;
  RDI_NameMapNode   **out_nodes;
  uint64                *out_bucket_counts;
  uint64                *out_node_counts;
} RDIB_NameMapBuilderTask;

typedef struct
{
  RDIB_PathTree    *path_tree;
  RDIB_StringMap   *string_map;
  RDI_FilePathNode *nodes_dst;
} RDIB_BuildFilePathNodesTask;

typedef struct
{
  RDIB_SourceFile **src_file_arr;
  uint32             *out_line_counts;
  uint32             *out_voff_counts;
  uint32             **out_line_nums;
  uint32             **out_line_ranges;
  uint64             **out_voffs;
} RDIB_SrcLineMapsTask;

typedef struct
{
  RDIB_LineTableChunk **chunks;
  Rng1<uint64>              *ranges;

  uint64       *out_line_table_counts;
  uint64      **out_line_table_voffs;
  RDI_Line **out_line_table_lines;
} RDIB_BuildLineTablesTask;

typedef struct
{
  Rng1<uint64>               *ranges;
  RDIB_StringMap        *string_map;
  RDIB_PathTree         *path_tree;
  RDIB_SourceFileChunk **src_file_chunks;
  RDI_SourceFile        *src_files_dst;
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

//- Source File


//- Eval Ops


//- Location




//- UDT Fwdrefs


//- Types




//- Path Tree



//- String Map


//- String Map Specialized Inserters


//- Index Run Map


//- Index Map Spesialized Query


//- Data Sections


internal void rdib_data_sections_from_types(TP_Context            *tp,
                                            Arena                 *arena,
                                            RDIB_DataSectionList  *sect_list,
                                            RDI_Arch               arch,
                                            RDIB_StringMap        *string_map,
                                            RDIB_IndexRunMap      *idx_run_map,
                                            uint64                    udt_member_chunk_count,
                                            RDIB_TypeChunk       **udt_member_type_chunks,
                                            uint64                    enum_member_chunk_count,
                                            RDIB_TypeChunk       **enum_member_type_chunks,
                                            uint64                    total_type_node_count,
                                            uint64                    type_chunk_count,
                                            RDIB_TypeChunk       **type_chunks,
                                            RDIB_TypeStats         type_stats);



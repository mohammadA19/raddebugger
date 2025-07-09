// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)




////////////////////////////////
//~ rjf: PE Format-Defined Types/Constants

#pragma pack(push,1)



struct PE_DosHeader
{
  u16 magic;
  u16 last_page_size;
  u16 page_count;
  u16 reloc_count;
  u16 paragraph_header_size;
  u16 min_paragraph;
  u16 max_paragraph;
  u16 init_ss;
  u16 init_sp;
  u16 checksum;
  u16 init_ip;
  u16 init_cs;
  u16 reloc_table_file_off;
  u16 overlay_number;
  u16 reserved[4];
  u16 oem_id;
  u16 oem_info;
  u16 reserved2[10];
  u32 coff_file_offset;
};

typedef u16 PE_WindowsSubsystem;
enum
{
  PE_WindowsSubsystem_UNKNOWN                  = 0,
  PE_WindowsSubsystem_NATIVE                   = 1,
  PE_WindowsSubsystem_WINDOWS_GUI              = 2,
  PE_WindowsSubsystem_WINDOWS_CUI              = 3,
  PE_WindowsSubsystem_OS2_CUI                  = 5,
  PE_WindowsSubsystem_POSIX_CUI                = 7,
  PE_WindowsSubsystem_NATIVE_WINDOWS           = 8,
  PE_WindowsSubsystem_WINDOWS_CE_GUI           = 9,
  PE_WindowsSubsystem_EFI_APPLICATION          = 10,
  PE_WindowsSubsystem_EFI_BOOT_SERVICE_DRIVER  = 11,
  PE_WindowsSubsystem_EFI_RUNTIME_DRIVER       = 12,
  PE_WindowsSubsystem_EFI_ROM                  = 13,
  PE_WindowsSubsystem_XBOX                     = 14,
  PE_WindowsSubsystem_WINDOWS_BOOT_APPLICATION = 16,
  PE_WindowsSubsystem_COUNT                    = 14
};

typedef u16 PE_ImageFileCharacteristics;
enum
{
  PE_ImageFileCharacteristic_STRIPPED                     = (1 << 0),
  PE_ImageFileCharacteristic_EXE                          = (1 << 1),
  PE_ImageFileCharacteristic_NUMS_STRIPPED                = (1 << 2),
  PE_ImageFileCharacteristic_PE_STRIPPED                  = (1 << 3),
  PE_ImageFileCharacteristic_AGGRESIVE_WS_TRIM            = (1 << 4),
  PE_ImageFileCharacteristic_LARGE_ADDRESS_AWARE          = (1 << 5),
  PE_ImageFileCharacteristic_UNUSED1                      = (1 << 6),
  PE_ImageFileCharacteristic_BYTES_RESERVED_LO            = (1 << 7),
  PE_ImageFileCharacteristic_32BIT_MACHINE                = (1 << 8),
  PE_ImageFileCharacteristic_DEBUG_STRIPPED               = (1 << 9),
  PE_ImageFileCharacteristic_FILE_REMOVABLE_RUN_FROM_SWAP = (1 << 10),
  PE_ImageFileCharacteristic_NET_RUN_FROM_SWAP            = (1 << 11),
  PE_ImageFileCharacteristic_FILE_SYSTEM                  = (1 << 12),
  PE_ImageFileCharacteristic_FILE_DLL                     = (1 << 13),
  PE_ImageFileCharacteristic_FILE_UP_SYSTEM_ONLY          = (1 << 14),
  PE_ImageFileCharacteristic_BYTES_RESERVED_HI            = (1 << 15),
};

typedef u16 PE_DllCharacteristics;
enum
{
  PE_DllCharacteristic_HIGH_ENTROPY_VA       = (1 << 5),
  PE_DllCharacteristic_DYNAMIC_BASE          = (1 << 6),
  PE_DllCharacteristic_FORCE_INTEGRITY       = (1 << 7),
  PE_DllCharacteristic_NX_COMPAT             = (1 << 8),
  PE_DllCharacteristic_NO_ISOLATION          = (1 << 9),
  PE_DllCharacteristic_NO_SEH                = (1 << 10),
  PE_DllCharacteristic_NO_BIND               = (1 << 11),
  PE_DllCharacteristic_APPCONTAINER          = (1 << 12),
  PE_DllCharacteristic_WDM_DRIVER            = (1 << 13),
  PE_DllCharacteristic_GUARD_CF              = (1 << 14),
  PE_DllCharacteristic_TERMINAL_SERVER_AWARE = (1 << 15),
};



struct PE_OptionalHeader32
{
  u16                   magic;
  u8                    major_linker_version;
  u8                    minor_linker_version;
  u32                   sizeof_code;
  u32                   sizeof_inited_data;
  u32                   sizeof_uninited_data;
  u32                   entry_point_va;
  u32                   code_base;
  u32                   data_base;
  u32                   image_base;
  u32                   section_alignment;
  u32                   file_alignment;
  u16                   major_os_ver;
  u16                   minor_os_ver;
  u16                   major_img_ver;
  u16                   minor_img_ver;
  u16                   major_subsystem_ver;
  u16                   minor_subsystem_ver;
  u32                   win32_version_value;
  u32                   sizeof_image;
  u32                   sizeof_headers;
  u32                   check_sum;
  PE_WindowsSubsystem   subsystem;
  PE_DllCharacteristics dll_characteristics;
  u32                   sizeof_stack_reserve;
  u32                   sizeof_stack_commit;
  u32                   sizeof_heap_reserve;
  u32                   sizeof_heap_commit;
  u32                   loader_flags;
  u32                   data_dir_count;
};



struct PE_OptionalHeader32Plus
{
  u16                   magic;
  u8                    major_linker_version;
  u8                    minor_linker_version;
  u32                   sizeof_code;
  u32                   sizeof_inited_data;
  u32                   sizeof_uninited_data;
  u32                   entry_point_va;
  u32                   code_base;
  u64                   image_base;
  u32                   section_alignment;
  u32                   file_alignment;
  u16                   major_os_ver;
  u16                   minor_os_ver;
  u16                   major_img_ver;
  u16                   minor_img_ver;
  u16                   major_subsystem_ver;
  u16                   minor_subsystem_ver;
  u32                   win32_version_value;
  u32                   sizeof_image;
  u32                   sizeof_headers;
  u32                   check_sum;
  PE_WindowsSubsystem   subsystem;
  PE_DllCharacteristics dll_characteristics;
  u64                   sizeof_stack_reserve;
  u64                   sizeof_stack_commit;
  u64                   sizeof_heap_reserve;
  u64                   sizeof_heap_commit;
  u32                   loader_flags;
  u32                   data_dir_count;
};

typedef enum PE_DataDirectoryIndex
{
  PE_DataDirectoryIndex_EXPORT,
  PE_DataDirectoryIndex_IMPORT,
  PE_DataDirectoryIndex_RESOURCES,
  PE_DataDirectoryIndex_EXCEPTIONS,
  PE_DataDirectoryIndex_CERT,
  PE_DataDirectoryIndex_BASE_RELOC,
  PE_DataDirectoryIndex_DEBUG,
  PE_DataDirectoryIndex_ARCH,
  PE_DataDirectoryIndex_GLOBAL_PTR,
  PE_DataDirectoryIndex_TLS,
  PE_DataDirectoryIndex_LOAD_CONFIG,
  PE_DataDirectoryIndex_BOUND_IMPORT,
  PE_DataDirectoryIndex_IMPORT_ADDR,
  PE_DataDirectoryIndex_DELAY_IMPORT,
  PE_DataDirectoryIndex_COM_DESCRIPTOR,
  PE_DataDirectoryIndex_RESERVED,
  PE_DataDirectoryIndex_COUNT = 16
}
PE_DataDirectoryIndex;



struct PE_DataDirectory
{
  u32 virt_off;
  u32 virt_size;
};

typedef u32 PE_DebugDirectoryType;
enum
{
  PE_DebugDirectoryType_UNKNOWN               = 0,
  PE_DebugDirectoryType_COFF                  = 1,
  PE_DebugDirectoryType_CODEVIEW              = 2,
  PE_DebugDirectoryType_FPO                   = 3,
  PE_DebugDirectoryType_MISC                  = 4,
  PE_DebugDirectoryType_EXCEPTION             = 5,
  PE_DebugDirectoryType_FIXUP                 = 6,
  PE_DebugDirectoryType_OMAP_TO_SRC           = 7,
  PE_DebugDirectoryType_OMAP_FROM_SRC         = 8,
  PE_DebugDirectoryType_BORLAND               = 9,
  PE_DebugDirectoryType_RESERVED10            = 10,
  PE_DebugDirectoryType_CLSID                 = 11,
  PE_DebugDirectoryType_VC_FEATURE            = 12,
  PE_DebugDirectoryType_COFF_GROUP            = 13,
  PE_DebugDirectoryType_ILTCG                 = 14,
  PE_DebugDirectoryType_MPX                   = 15,
  PE_DebugDirectoryType_REPRO                 = 16,
  PE_DebugDirectoryType_EX_DLLCHARACTERISTICS = 20,
  PE_DebugDirectoryType_COUNT                 = 18
};

typedef u8 PE_FPOFlags;
enum
{
  PE_FPOFlags_HAS_SEH    = 0x800,
  PE_FPOFlags_USE_BP_REG = 0x1000,
  PE_FPOFlags_RESERVED   = 0x2000,
  PE_FPOFlags_COUNT      = 3
};

typedef u16 PE_FPOEncoded;
enum
{
  PE_FPOEncoded_PROLOG_SIZE_SHIFT     = 0,  PE_FPOEncoded_PROLOG_SIZE_MASK     = 0xff,
  PE_FPOEncoded_SAVED_REGS_SIZE_SHIFT = 8,  PE_FPOEncoded_SAVED_REGS_SIZE_MASK = 0x7,
  PE_FPOEncoded_FLAGS_SHIFT           = 11, PE_FPOEncoded_FLAGS_MASK           = 0x7,
  PE_FPOEncoded_FRAME_TYPE_SHIFT      = 14, PE_FPOEncoded_FRAME_TYPE_MASK      = 0x3,
};
#define PE_FPOEncoded_Extract_PROLOG_SIZE(f)     (u8)(((f) >> PE_FPOEncoded_PROLOG_SIZE_SHIFT)     & PE_FPOEncoded_PROLOG_SIZE_MASK)
#define PE_FPOEncoded_Extract_SAVED_REGS_SIZE(f) (u8)(((f) >> PE_FPOEncoded_SAVED_REGS_SIZE_SHIFT) & PE_FPOEncoded_SAVED_REGS_SIZE_MASK)
#define PE_FPOEncoded_Extract_FLAGS(f)           (u8)(((f) >> PE_FPOEncoded_FLAGS_SHIFT)           & PE_FPOEncoded_FLAGS_MASK)
#define PE_FPOEncoded_Extract_FRAME_TYPE(f)      (u8)(((f) >> PE_FPOEncoded_FRAME_TYPE_SHIFT)      & PE_FPOEncoded_FRAME_TYPE_MASK)

typedef u8 PE_FPOType;
enum
{
  PE_FPOType_FPO   = 0,
  PE_FPOType_TRAP  = 1,
  PE_FPOType_TSS   = 2,
  PE_FPOType_NOFPO = 3,
  PE_FPOType_COUNT = 4
};

// winnt.h: FPO_DATA


struct PE_DebugFPO
{
  u32 func_code_off;
  u32 func_size;
  u32 locals_size;
  u16 params_size;
  u16 flags;
};

typedef u32 PE_DebugMiscType;
enum
{
  PE_DebugMiscType_NULL,
  PE_DebugMiscType_EXE_NAME,
  PE_DebugMiscType_COUNT = 2
};

// winnt.h: IMAGE_DEBUG_MISC


struct PE_DebugMisc
{
  PE_DebugMiscType data_type;
  u32              size;
  u8               unicode;
  u8               pad[3];
  //char name[];
};

// winnt.h: IMAGE_COFF_SYMBOLS_HEADER


struct PE_DebugCoff
{
  u32 symbol_count;
  u32 lva_to_first_symbol;
  u32 line_number_count;
  u32 lva_to_first_line_number;
  u32 virt_off_to_first_byte_of_code;
  u32 virt_off_to_last_byte_of_code;
  u32 virt_off_to_first_byte_of_data;
  u32 virt_off_to_last_byte_of_data;
};



struct PE_DebugDirectory
{
  u32                   characteristics;
  COFF_TimeStamp        time_stamp;
  u16                   major_ver;
  u16                   minor_ver;
  PE_DebugDirectoryType type;
  u32                   size;
  u32                   voff;
  u32                   foff;
};

typedef u32 PE_GlobalFlags;
enum
{
  PE_GlobalFlags_STOP_ON_EXCEPTION          = (1 << 0),
  PE_GlobalFlags_SHOW_LDR_SNAPS             = (1 << 1),
  PE_GlobalFlags_DEBUG_INITIAL_COMMAND      = (1 << 2),
  PE_GlobalFlags_STOP_ON_HUNG_GUI           = (1 << 3),
  PE_GlobalFlags_HEAP_ENABLE_TAIL_CHECK     = (1 << 4),
  PE_GlobalFlags_HEAP_ENABLE_FREE_CHECK     = (1 << 5),
  PE_GlobalFlags_HEAP_VALIDATE_PARAMETERS   = (1 << 6),
  PE_GlobalFlags_HEAP_VALIDATE_ALL          = (1 << 7),
  PE_GlobalFlags_APPLICATION_VERIFIER       = (1 << 8),
  PE_GlobalFlags_POOL_ENABLE_TAGGING        = (1 << 10),
  PE_GlobalFlags_HEAP_ENABLE_TAGGING        = (1 << 11),
  PE_GlobalFlags_STACK_TRACE_DB             = (1 << 12),
  PE_GlobalFlags_KERNEL_STACK_TRACE_DB      = (1 << 13),
  PE_GlobalFlags_MAINTAIN_OBJECT_TYPELIST   = (1 << 14),
  PE_GlobalFlags_HEAP_ENABLE_TAG_BY_DLL     = (1 << 15),
  PE_GlobalFlags_DISABLE_STACK_EXTENSION    = (1 << 16),
  PE_GlobalFlags_ENABLE_CSRDEBUG            = (1 << 17),
  PE_GlobalFlags_ENABLE_KDEBUG_SYMBOL_LOAD  = (1 << 18),
  PE_GlobalFlags_DISABLE_PAGE_KERNEL_STACKS = (1 << 19),
  PE_GlobalFlags_ENABLE_SYSTEM_CRIT_BREAKS  = (1 << 20),
  PE_GlobalFlags_HEAP_DISABLE_COALESCING    = (1 << 21),
  PE_GlobalFlags_ENABLE_CLOSE_EXCEPTIONS    = (1 << 22),
  PE_GlobalFlags_ENABLE_EXCEPTION_LOGGING   = (1 << 23),
  PE_GlobalFlags_ENABLE_HANDLE_TYPE_TAGGING = (1 << 24),
  PE_GlobalFlags_HEAP_PAGE_ALLOCS           = (1 << 25),
  PE_GlobalFlags_DEBUG_INITIAL_COMMAND_EX 	= (1 << 26),
  PE_GlobalFlags_DISABLE_DBGPRINT           = (1 << 27),
  PE_GlobalFlags_CRITSEC_EVENT_CREATION     = (1 << 28),
  PE_GlobalFlags_LDR_TOP_DOWN               = (1 << 29),
  PE_GlobalFlags_ENABLE_HANDLE_EXCEPTIONS   = (1 << 30),
  PE_GlobalFlags_DISABLE_PROTDLLS           = (1 << 31),
};

typedef u32 PE_LoadConfigGuardFlags;
enum
{
  PE_LoadConfigGuardFlags_CF_INSTRUMENTED                    = (1 << 8),
  PE_LoadConfigGuardFlags_CFW_INSTRUMENTED                   = (1 << 9),
  PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_PRESENT          = (1 << 10),
  PE_LoadConfigGuardFlags_SECURITY_COOKIE_UNUSED             = (1 << 11),
  PE_LoadConfigGuardFlags_PROTECT_DELAYLOAD_IAT              = (1 << 12),
  PE_LoadConfigGuardFlags_DELAYLOAD_IAT_IN_ITS_OWN_SECTION   = (1 << 13),
  PE_LoadConfigGuardFlags_CF_EXPORT_SUPPRESSION_INFO_PRESENT = (1 << 14),
  PE_LoadConfigGuardFlags_CF_ENABLE_EXPORT_SUPPRESSION       = (1 << 15),
  PE_LoadConfigGuardFlags_CF_LONGJUMP_TABLE_PRESENT          = (1 << 16),
  PE_LoadConfigGuardFlags_EH_CONTINUATION_TABLE_PRESENT      = (1 << 22),
  PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_SIZE_SHIFT       = 28, PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_SIZE_MASK = 0xf,
};
#define PE_LoadConfigGuardFlags_Extract_CF_FUNCTION_TABLE_SIZE(f) (u32)(((f) >> PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_SIZE_SHIFT) & PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_SIZE_MASK)



struct PE_LoadConfigCodeIntegrity
{
  u16 flags;
  u16 catalog;
  u32 catalog_offset;
  u32 reserved;
};



struct PE_LoadConfig32
{
  u32            size;
  COFF_TimeStamp time_stamp;
  u16            major_version;
  u16            minor_version;
  u32            global_flag_clear;
  u32            global_flag_set;
  u32            critical_section_timeout;
  u32            decommit_free_block_threshold;
  u32            decommit_total_free_threshold;
  u32            lock_prefix_table;
  u32            maximum_allocation_size;
  u32            virtual_memory_threshold;
  u32            process_affinity_mask;
  u32            process_heap_flags;
  u16            csd_version;
  u16            reserved;
  u32            edit_list;
  u32            security_cookie;
  u32            seh_handler_table;
  u32            seh_handler_count;
  
  // msvc 2015
  u32 guard_cf_check_func_ptr;
  u32 guard_cf_dispatch_func_ptr;
  u32 guard_cf_func_table;
  u32 guard_cf_func_count;
  u32 guard_flags;
  
  // msvc 2017
  PE_LoadConfigCodeIntegrity code_integrity;
  u32                        guard_address_taken_iat_entry_table;
  u32                        guard_address_taken_iat_entry_count;
  u32                        guard_long_jump_target_table;
  u32                        guard_long_jump_target_count;
  u32                        dynamic_value_reloc_table;
  u32                        chpe_metadata_ptr;
  u32                        guard_rf_failure_routine;
  u32                        guard_rf_failure_routine_func_ptr;
  u32                        dynamic_value_reloc_table_offset;
  u16                        dynamic_value_reloc_table_section;
  u16                        reserved2;
  u32                        guard_rf_verify_stack_pointer_func_ptr;
  u32                        hot_patch_table_offset;
  
  // msvc 2019
  u32 reserved3;
  u32 enclave_config_ptr;
  u32 volatile_metadata_ptr;
  u32 guard_eh_continue_table;
  u32 guard_eh_continue_count;
  u32 guard_xfg_check_func_ptr;
  u32 guard_xfg_dispatch_func_ptr;
  u32 guard_xfg_table_dispatch_func_ptr;
  u32 cast_guard_os_determined_failure_mode;
};



struct PE_LoadConfig64
{
  u32            size;
  COFF_TimeStamp time_stamp;
  u16            major_version;
  u16            minor_version;
  u32            global_flag_clear;
  u32            global_flag_set;
  u32            critical_section_timeout;
  u64            decommit_free_block_threshold;
  u64            decommit_total_free_threshold;
  u64            lock_prefix_table;
  u64            maximum_allocation_size;
  u64            virtual_memory_threshold;
  u64            process_affinity_mask;
  u32            process_heap_flags;
  u16            csd_version;
  u16            reserved;
  u64            edit_list;
  u64            security_cookie;
  u64            seh_handler_table;
  u64            seh_handler_count;
  
  // msvc 2015
  u64 guard_cf_check_func_ptr;
  u64 guard_cf_dispatch_func_ptr;
  u64 guard_cf_func_table;
  u64 guard_cf_func_count;
  u32 guard_flags;
  
  // msvc 2017
  PE_LoadConfigCodeIntegrity code_integrity;
  u64                        guard_address_taken_iat_entry_table;
  u64                        guard_address_taken_iat_entry_count;
  u64                        guard_long_jump_target_table;
  u64                        guard_long_jump_target_count;
  u64                        dynamic_value_reloc_table;
  u64                        chpe_metadata_ptr;
  u64                        guard_rf_failure_routine;
  u64                        guard_rf_failure_routine_func_ptr;
  u32                        dynamic_value_reloc_table_offset;
  u16                        dynamic_value_reloc_table_section;
  u16                        reserved2;
  u64                        guard_rf_verify_stack_pointer_func_ptr;
  u32                        hot_patch_table_offset;
  
  // msvc 2019
  u32 reserved3;
  u64 enclave_config_ptr;
  u64 volatile_metadata_ptr;
  u64 guard_eh_continue_table;
  u64 guard_eh_continue_count;
  u64 guard_xfg_check_func_ptr;
  u64 guard_xfg_dispatch_func_ptr;
  u64 guard_xfg_table_dispatch_func_ptr;
  u64 cast_guard_os_determined_failure_mode;
};

// this is the "MZ" as a 16-bit short
#define PE_DOS_MAGIC      0x5a4d
#define PE_MAGIC          0x00004550u
#define PE_PE32_MAGIC     0x010bu
#define PE_PE32PLUS_MAGIC 0x020bu



struct PE_MipsPdata
{
  u32 voff_first;
  u32 voff_one_past_last;
  u32 voff_exception_handler;
  u32 voff_exception_handler_data;
  u32 voff_one_past_prolog;
};



struct PE_ArmPdata
{
  u32 voff_first;
  // NOTE(allen):
  // bits    | meaning
  // [0:7]   | prolog length
  // [8:29]  | function length
  // [30:30] | instructions_are_32bits (otherwise they are 16 bits)
  // [31:31] | has_exception_handler
  u32 combined;
};



struct PE_IntelPdata
{
  u32 voff_first;
  u32 voff_one_past_last;
  u32 voff_unwind_info;
};

#define PE_CODEVIEW_PDB20_MAGIC 0x3031424e
#define PE_CODEVIEW_PDB70_MAGIC 0x53445352
#define PE_CODEVIEW_RDI_MAGIC   '0IDR' 



struct PE_CvHeaderPDB20
{
  u32            magic;
  u32            offset;
  COFF_TimeStamp time_stamp;
  u32            age;
  // file name packed after struct
};



struct PE_CvHeaderPDB70
{
  u32  magic;
  Guid guid;
  u32  age;
  // file name packed after struct
};



struct PE_CvHeaderRDI
{
  u32  magic;
  Guid guid;
  // file name packed after struct
};



struct PE_ImportEntry
{
  u32            lookup_table_voff;
  COFF_TimeStamp time_stamp;
  u32            forwarder_chain;
  u32            name_voff;
  u32            import_addr_table_voff;
};



struct PE_DelayedImportEntry
{
  // According to COFF/PE spec this field is unused and should be set zero,
  // but when I compile mule with MSVC 2019 this is set to 1.
  u32            attributes;
  u32            name_voff;          // Name of the DLL
  u32            module_handle_voff; // Place where module handle from LoadLibrary is stored
  u32            iat_voff;
  u32            name_table_voff;    // Array of hint/name or oridnals
  u32            bound_table_voff;   // (Optional) Points to an array of PE_ThunkData
  u32            unload_table_voff;  // (Optional) Copy of iat_voff
  //  0 not bound
  // -1 if bound and real timestamp located in bounded import directory
  // Otherwise time when dll was bound
  COFF_TimeStamp time_stamp;
};



struct PE_ExportTableHeader
{
  u32            flags;                       // must be zero
  COFF_TimeStamp time_stamp;                  // time and date when export table was created
  u16            major_ver;                   // table version, user can change major and minor version
  u16            minor_ver;
  u32            name_voff;                   // ASCII name of the dll
  u32            ordinal_base;                // Starting oridnal number
  u32            export_address_table_count;
  u32            name_pointer_table_count;
  u32            export_address_table_voff;
  u32            name_pointer_table_voff;
  u32            ordinal_table_voff;
};



struct PE_TLSHeader32
{
  u32               raw_data_start;    // Range of initialized data that is copied for each thread from the image.
  u32               raw_data_end;      // (Typically points to .tls section)
  u32               index_address;     // Address where image loader places TLS slot index.
  u32               callbacks_address; // Zero terminated list of callbacks used for initializing data with constructors.
  u32               zero_fill_size;    // Amount of memory to fill with zeroes in TLS.
  COFF_SectionFlags characteristics;   // COFF_SectionFlags but only align flags are used.
};



struct PE_TLSHeader64
{
  u64               raw_data_start;    // Range of initialized data that is copied for each thread from the image.
  u64               raw_data_end;      // (Typically points to .tls section)
  u64               index_address;     // Address where image loader places TLS slot index.
  u64               callbacks_address; // Zero terminated list of callbacks used for initializing data with constructors.
  u32               zero_fill_size;    // Amount of memory to fill with zeroes in TLS.
  COFF_SectionFlags characteristics;   // COFF_SectionFlags but only align flags are used.
};

global @(rodata) u8 PE_RES_MAGIC[] =
{
  0x00, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0x00, 0x00,
  0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

typedef u32 PE_ResourceKind;
enum
{
  PE_ResourceKind_CURSOR       = 0x1,
  PE_ResourceKind_BITMAP       = 0x2,
  PE_ResourceKind_ICON         = 0x3,
  PE_ResourceKind_MENU         = 0x4,
  PE_ResourceKind_DIALOG       = 0x5,
  PE_ResourceKind_STRING       = 0x6,
  PE_ResourceKind_FONTDIR      = 0x7,
  PE_ResourceKind_FONT         = 0x8,
  PE_ResourceKind_ACCELERATOR  = 0x9,
  PE_ResourceKind_RCDATA       = 0xA,
  PE_ResourceKind_MESSAGETABLE = 0xB,
  PE_ResourceKind_GROUP_CURSOR = 0xC,
  PE_ResourceKind_GROUP_ICON   = 0xE,
  PE_ResourceKind_VERSION      = 0x10,
  PE_ResourceKind_DLGINCLUDE   = 0x11,
  PE_ResourceKind_PLUGPLAY     = 0x13,
  PE_ResourceKind_VXD          = 0x14,
  PE_ResourceKind_ANICURSOR    = 0x15,
  PE_ResourceKind_ANIICON      = 0x16,
  PE_ResourceKind_HTML         = 0x17,
  PE_ResourceKind_MANIFEST     = 0x18,
  PE_ResourceKind_BITMAP_NEW   = 0x2002,
  PE_ResourceKind_MENU_NEW     = 0x2004,
  PE_ResourceKind_DIALOG_NEW   = 0x2005,
};

typedef enum PE_ResDataKind
{
  PE_ResDataKind_NULL,
  PE_ResDataKind_DIR,
  PE_ResDataKind_COFF_LEAF,
  PE_ResDataKind_COFF_RESOURCE,
}
PE_ResDataKind;



struct PE_ResourceHeader
{
  COFF_ResourceHeaderPrefix prefix;
  u16                       type;
  u16                       pad0;
  u16                       name;
  u16                       pad1;
  u32                       data_version;
  COFF_ResourceMemoryFlags  memory_flags;
  u16                       language_id;
  u32                       version;
  u32                       characteristics;
};

typedef u16 PE_BaseRelocKind;
enum
{
  PE_BaseRelocKind_ABSOLUTE            = 0, // No reallocation is applied. Can be used as padding.
  PE_BaseRelocKind_HIGH                = 1,
  PE_BaseRelocKind_LOW                 = 2,
  PE_BaseRelocKind_HIGHLOW             = 3,
  PE_BaseRelocKind_HIGHADJ             = 4,
  PE_BaseRelocKind_MIPS_JMPADDR        = 5,
  PE_BaseRelocKind_ARM_MOV32           = 5,
  PE_BaseRelocKind_RISCV_HIGH20        = 5,
  // 6 is reserved
  PE_BaseRelocKind_THUMB_MOV32         = 7,
  PE_BaseRelocKind_RISCV_LOW12I        = 7,
  PE_BaseRelocKind_RISCV_LOW12S        = 8,
  PE_BaseRelocKind_LOONGARCH32_MARK_LA = 8,
  PE_BaseRelocKind_LOONGARCH64_MARK_LA = 8,
  PE_BaseRelocKind_MIPS_JMPADDR16      = 9,
  PE_BaseRelocKind_DIR64               = 10,
};
#define PE_BaseRelocOffsetFromEntry(x) ((x) & 0x1fff)
#define PE_BaseRelocKindFromEntry(x)   (((x) >> 12) & 0xf)
#define PE_BaseRelocMake(k, off)       ((((u16)(k) & 0xf) << 12) | (u16)((off) & 0x1fff))

typedef u32 PE_UnwindOpCode;
enum
{
  PE_UnwindOpCode_PUSH_NONVOL      = 0,
  PE_UnwindOpCode_ALLOC_LARGE      = 1,
  PE_UnwindOpCode_ALLOC_SMALL      = 2,
  PE_UnwindOpCode_SET_FPREG        = 3,
  PE_UnwindOpCode_SAVE_NONVOL      = 4,
  PE_UnwindOpCode_SAVE_NONVOL_FAR  = 5,
  PE_UnwindOpCode_EPILOG           = 6,
  PE_UnwindOpCode_SPARE_CODE       = 7,
  PE_UnwindOpCode_SAVE_XMM128      = 8,
  PE_UnwindOpCode_SAVE_XMM128_FAR  = 9,
  PE_UnwindOpCode_PUSH_MACHFRAME   = 10,
};

typedef u8 PE_UnwindGprRegX64;
enum
{
  PE_UnwindGprRegX64_RAX = 0,
  PE_UnwindGprRegX64_RCX = 1,
  PE_UnwindGprRegX64_RDX = 2,
  PE_UnwindGprRegX64_RBX = 3,
  PE_UnwindGprRegX64_RSP = 4,
  PE_UnwindGprRegX64_RBP = 5,
  PE_UnwindGprRegX64_RSI = 6,
  PE_UnwindGprRegX64_RDI = 7,
  PE_UnwindGprRegX64_R8  = 8,
  PE_UnwindGprRegX64_R9  = 9,
  PE_UnwindGprRegX64_R10 = 10,
  PE_UnwindGprRegX64_R11 = 11,
  PE_UnwindGprRegX64_R12 = 12,
  PE_UnwindGprRegX64_R13 = 13,
  PE_UnwindGprRegX64_R14 = 14,
  PE_UnwindGprRegX64_R15 = 15,
};

typedef u8 PE_UnwindInfoFlags;
enum
{
  PE_UnwindInfoFlag_EHANDLER = (1<<0),
  PE_UnwindInfoFlag_UHANDLER = (1<<1),
  PE_UnwindInfoFlag_FHANDLER = 3,
  PE_UnwindInfoFlag_CHAINED  = (1<<2),
};

#define PE_UNWIND_OPCODE_FROM_FLAGS(f) ((f)&0xF)
#define PE_UNWIND_INFO_FROM_FLAGS(f) (((f) >> 4)&0xF)


union PE_UnwindCode
{
  struct
  {
    u8 off_in_prolog;
    u8 flags;
  };
  u16 u16;
};

#define PE_UNWIND_INFO_VERSION_FROM_HDR(x) ((x)&0x7)
#define PE_UNWIND_INFO_FLAGS_FROM_HDR(x)   (((x) >> 3)&0x1F)
#define PE_UNWIND_INFO_REG_FROM_FRAME(x)   ((x)&0xF)
#define PE_UNWIND_INFO_OFF_FROM_FRAME(x)   (((x) >> 4)&0xF)
#define PE_UNWIND_INFO_GET_CODE_COUNT(x)   (((x)+1) & ~1)



struct PE_UnwindInfo
{
  u8 header;
  u8 prolog_size;
  u8 codes_num;
  u8 frame;
};

#pragma pack(pop)

////////////////////////////////
//~ rjf: DOS Program

// generated from pe/dos_program.asm
@(rodata) global u8 pe_dos_program_data[] =
{
  0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
  0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
  0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
  0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x24, 0x00, 0x00
};
@(rodata) global String8 pe_dos_program = {pe_dos_program_data, sizeof(pe_dos_program_data)};

////////////////////////////////
//~ rjf: Parsed Info Types

//- rjf: relocation blocks



struct PE_BaseRelocBlock
{
  u64  page_virt_off;
  u64  entry_count;
  u16 *entries;
};



struct PE_BaseRelocBlockNode
{
  PE_BaseRelocBlockNode *next;
  PE_BaseRelocBlock      v;
};



struct PE_BaseRelocBlockList
{
  PE_BaseRelocBlockNode *first;
  PE_BaseRelocBlockNode *last;
  u64                    count;
};

//- rjf: resources



struct PE_Resource
{
  COFF_ResourceID id;
  PE_ResDataKind  kind;
  union
  {
    COFF_ResourceDataEntry leaf;
    struct PE_ResourceDir *dir;
    struct
    {
      COFF_ResourceID          type;
      u32                      data_version;
      u32                      version;
      COFF_ResourceMemoryFlags memory_flags;
      String8                  data;
    } coff_res;
  } u;
};



struct PE_ResourceNode
{
  PE_ResourceNode *next;
  PE_Resource      data;
};



struct PE_ResourceList
{
  PE_ResourceNode *first;
  PE_ResourceNode *last;
  u64              count;
};



struct PE_ResourceArray
{
  PE_Resource *v;
  u64          count;
};



struct PE_ResourceDir
{
  u32             characteristics;
  COFF_TimeStamp  time_stamp;
  u16             major_version;
  u16             minor_version;
  PE_ResourceList named_list;
  PE_ResourceList id_list;
};

//- exports & imports



struct PE_ParsedExport
{
  String8 forwarder;
  String8 name;
  u64     voff;
  u64     ordinal;
};



struct PE_ParsedExportTable
{
  u32              flags;
  COFF_TimeStamp   time_stamp;
  u16              major_ver;
  u16              minor_ver;
  u64              ordinal_base;
  u64              export_count;
  PE_ParsedExport *exports;
};

typedef u32 PE_ParsedImportType;
enum PE_ParsedImportTypeEnum
{
  PE_ParsedImport_Null,
  PE_ParsedImport_Ordinal,
  PE_ParsedImport_Name,
};



struct PE_ParsedImport
{
  PE_ParsedImportType type;
  union
  {
    u16 ordinal;
    struct
    {
      u64     hint;
      String8 string;
    } name;
  } u;
};



struct PE_ParsedStaticDLLImport
{
  String8          name;
  u64              import_address_table_voff;
  u64              import_name_table_voff;
  COFF_TimeStamp   time_stamp;
  u64              forwarder_chain;
  u64              import_count;
  PE_ParsedImport *imports;
};



struct PE_ParsedStaticImportTable
{
  u64                       count;
  PE_ParsedStaticDLLImport *v;
};



struct PE_ParsedDelayDLLImport
{
  u32              attributes;
  String8          name;
  u64              module_handle_voff;
  u64              iat_voff;
  u64              name_table_voff;
  u64              bound_table_voff;
  u64              unload_table_voff;
  COFF_TimeStamp   time_stamp;
  u64              bound_table_count;
  u64             *bound_table;
  u64              unload_table_count;
  u64             *unload_table;
  u64              import_count;
  PE_ParsedImport *imports;
};



struct PE_ParsedDelayImportTable
{
  u64                      count;
  PE_ParsedDelayDLLImport *v;
};



struct PE_ParsedTLS
{
  PE_TLSHeader64 header;
  u64            callback_count;
  u64           *callback_addrs;
};

////////////////////////////////
// SEH Scope Table



struct PE_HandlerScope
{
  u32 begin;
  u32 end;
  u32 handler;
  u32 target;
};

//- rjf: bundle



struct PE_BinInfo
{
  Arch                 arch;
  u64                  image_base;
  u64                  entry_point;
  b32                  is_pe32;
  PE_WindowsSubsystem  subsystem;
  u32                 *check_sum;
  u64                  virt_section_align;
  u64                  file_section_align;
  u64                  section_count;
  u64                  symbol_count;
  u64                  optional_header_off;
  Rng1U64              section_table_range;
  Rng1U64              symbol_table_range;
  Rng1U64              string_table_range;
  Rng1U64              data_dir_range;
  Rng1U64             *data_dir_franges;
  Rng1U64             *data_dir_vranges;
  u32                  data_dir_count;
  PE_TLSHeader64       tls_header;
};

typedef struct PE_DebugInfo
{
  PE_DebugDirectory header;
  u32 cv_magic;
  PE_CvHeaderPDB20 cv_pdb20_header;
  PE_CvHeaderPDB70 cv_pdb70_header;
  PE_CvHeaderRDI cv_rdi_header;
  String8 path;
}
PE_DebugInfo;

typedef struct PE_DebugInfoNode
{
  struct PE_DebugInfoNode *next;
  PE_DebugInfo             v;
} PE_DebugInfoNode;

typedef struct PE_DebugInfoList
{
  PE_DebugInfoNode *first;
  PE_DebugInfoNode *last;
  u64               count;
} PE_DebugInfoList;

////////////////////////////////
//~ rjf: Basic Enum Functions

















////////////////////////////////
//~ rjf: Parser Functions












////////////////////////////////
//~ rjf: Helpers







////////////////////////////////
//~ Resource Helpers














////////////////////////////////
//~ Debug Directory




////////////////////////////////
//~ Image Checksum



////////////////////////////////





// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef PE_H
#define PE_H

////////////////////////////////
//~ rjf: PE Format-Defined Types/Constants

#pragma pack(push,1)

struct PE_DosHeader
{
  ushort magic;
  ushort last_page_size;
  ushort page_count;
  ushort reloc_count;
  ushort paragraph_header_size;
  ushort min_paragraph;
  ushort max_paragraph;
  ushort init_ss;
  ushort init_sp;
  ushort checksum;
  ushort init_ip;
  ushort init_cs;
  ushort reloc_table_file_off;
  ushort overlay_number;
  ushort reserved[4];
  ushort oem_id;
  ushort oem_info;
  ushort reserved2[10];
  uint coff_file_offset;
};

enum PE_WindowsSubsystem : ushort
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

enum PE_ImageFileCharacteristics : ushort
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

enum PE_DllCharacteristics : ushort
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
  ushort                   magic;
  byte                    major_linker_version;
  byte                    minor_linker_version;
  uint                   sizeof_code;
  uint                   sizeof_inited_data;
  uint                   sizeof_uninited_data;
  uint                   entry_point_va;
  uint                   code_base;
  uint                   data_base;
  uint                   image_base;
  uint                   section_alignment;
  uint                   file_alignment;
  ushort                   major_os_ver;
  ushort                   minor_os_ver;
  ushort                   major_img_ver;
  ushort                   minor_img_ver;
  ushort                   major_subsystem_ver;
  ushort                   minor_subsystem_ver;
  uint                   win32_version_value;
  uint                   sizeof_image;
  uint                   sizeof_headers;
  uint                   check_sum;
  PE_WindowsSubsystem   subsystem;
  PE_DllCharacteristics dll_characteristics;
  uint                   sizeof_stack_reserve;
  uint                   sizeof_stack_commit;
  uint                   sizeof_heap_reserve;
  uint                   sizeof_heap_commit;
  uint                   loader_flags;
  uint                   data_dir_count;
};

struct PE_OptionalHeader32Plus
{
  ushort                   magic;
  byte                    major_linker_version;
  byte                    minor_linker_version;
  uint                   sizeof_code;
  uint                   sizeof_inited_data;
  uint                   sizeof_uninited_data;
  uint                   entry_point_va;
  uint                   code_base;
  ulong                   image_base;
  uint                   section_alignment;
  uint                   file_alignment;
  ushort                   major_os_ver;
  ushort                   minor_os_ver;
  ushort                   major_img_ver;
  ushort                   minor_img_ver;
  ushort                   major_subsystem_ver;
  ushort                   minor_subsystem_ver;
  uint                   win32_version_value;
  uint                   sizeof_image;
  uint                   sizeof_headers;
  uint                   check_sum;
  PE_WindowsSubsystem   subsystem;
  PE_DllCharacteristics dll_characteristics;
  ulong                   sizeof_stack_reserve;
  ulong                   sizeof_stack_commit;
  ulong                   sizeof_heap_reserve;
  ulong                   sizeof_heap_commit;
  uint                   loader_flags;
  uint                   data_dir_count;
};

enum PE_DataDirectoryIndex
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
};

struct PE_DataDirectory
{
  uint virt_off;
  uint virt_size;
};

enum PE_DebugDirectoryType : uint
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

enum PE_FPOFlags : byte
{
  PE_FPOFlags_HAS_SEH    = 0x800,
  PE_FPOFlags_USE_BP_REG = 0x1000,
  PE_FPOFlags_RESERVED   = 0x2000,
  PE_FPOFlags_COUNT      = 3
};

enum PE_FPOEncoded : ushort
{
  PE_FPOEncoded_PROLOG_SIZE_SHIFT     = 0,  PE_FPOEncoded_PROLOG_SIZE_MASK     = 0xff,
  PE_FPOEncoded_SAVED_REGS_SIZE_SHIFT = 8,  PE_FPOEncoded_SAVED_REGS_SIZE_MASK = 0x7,
  PE_FPOEncoded_FLAGS_SHIFT           = 11, PE_FPOEncoded_FLAGS_MASK           = 0x7,
  PE_FPOEncoded_FRAME_TYPE_SHIFT      = 14, PE_FPOEncoded_FRAME_TYPE_MASK      = 0x3,
};
#define PE_FPOEncoded_Extract_PROLOG_SIZE(f)     (byte)(((f) >> PE_FPOEncoded_PROLOG_SIZE_SHIFT)     & PE_FPOEncoded_PROLOG_SIZE_MASK)
#define PE_FPOEncoded_Extract_SAVED_REGS_SIZE(f) (byte)(((f) >> PE_FPOEncoded_SAVED_REGS_SIZE_SHIFT) & PE_FPOEncoded_SAVED_REGS_SIZE_MASK)
#define PE_FPOEncoded_Extract_FLAGS(f)           (byte)(((f) >> PE_FPOEncoded_FLAGS_SHIFT)           & PE_FPOEncoded_FLAGS_MASK)
#define PE_FPOEncoded_Extract_FRAME_TYPE(f)      (byte)(((f) >> PE_FPOEncoded_FRAME_TYPE_SHIFT)      & PE_FPOEncoded_FRAME_TYPE_MASK)

enum PE_FPOType : byte
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
  uint func_code_off;
  uint func_size;
  uint locals_size;
  ushort params_size;
  ushort flags;
};

enum PE_DebugMiscType : uint
{
  PE_DebugMiscType_NULL,
  PE_DebugMiscType_EXE_NAME,
  PE_DebugMiscType_COUNT = 2
};

// winnt.h: IMAGE_DEBUG_MISC
struct PE_DebugMisc
{
  PE_DebugMiscType data_type;
  uint              size;
  byte               unicode;
  byte               pad[3];
  //char name[];
};

// winnt.h: IMAGE_COFF_SYMBOLS_HEADER
struct PE_DebugCoff
{
  uint symbol_count;
  uint lva_to_first_symbol;
  uint line_number_count;
  uint lva_to_first_line_number;
  uint virt_off_to_first_byte_of_code;
  uint virt_off_to_last_byte_of_code;
  uint virt_off_to_first_byte_of_data;
  uint virt_off_to_last_byte_of_data;
};

struct PE_DebugDirectory
{
  uint                   characteristics;
  COFF_TimeStamp        time_stamp;
  ushort                   major_ver;
  ushort                   minor_ver;
  PE_DebugDirectoryType type;
  uint                   size;
  uint                   voff;
  uint                   foff;
};

enum PE_GlobalFlags : uint
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

enum PE_LoadConfigGuardFlags : uint
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
#define PE_LoadConfigGuardFlags_Extract_CF_FUNCTION_TABLE_SIZE(f) (uint)(((f) >> PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_SIZE_SHIFT) & PE_LoadConfigGuardFlags_CF_FUNCTION_TABLE_SIZE_MASK)

struct PE_LoadConfigCodeIntegrity
{
  ushort flags;
  ushort catalog;
  uint catalog_offset;
  uint reserved;
};

struct PE_LoadConfig32
{
  uint            size;
  COFF_TimeStamp time_stamp;
  ushort            major_version;
  ushort            minor_version;
  uint            global_flag_clear;
  uint            global_flag_set;
  uint            critical_section_timeout;
  uint            decommit_free_block_threshold;
  uint            decommit_total_free_threshold;
  uint            lock_prefix_table;
  uint            maximum_allocation_size;
  uint            virtual_memory_threshold;
  uint            process_affinity_mask;
  uint            process_heap_flags;
  ushort            csd_version;
  ushort            reserved;
  uint            edit_list;
  uint            security_cookie;
  uint            seh_handler_table;
  uint            seh_handler_count;

  // msvc 2015
  uint guard_cf_check_func_ptr;
  uint guard_cf_dispatch_func_ptr;
  uint guard_cf_func_table;
  uint guard_cf_func_count;
  uint guard_flags;

  // msvc 2017
  PE_LoadConfigCodeIntegrity code_integrity;
  uint                        guard_address_taken_iat_entry_table;
  uint                        guard_address_taken_iat_entry_count;
  uint                        guard_long_jump_target_table;
  uint                        guard_long_jump_target_count;
  uint                        dynamic_value_reloc_table;
  uint                        chpe_metadata_ptr;
  uint                        guard_rf_failure_routine;
  uint                        guard_rf_failure_routine_func_ptr;
  uint                        dynamic_value_reloc_table_offset;
  ushort                        dynamic_value_reloc_table_section;
  ushort                        reserved2;
  uint                        guard_rf_verify_stack_pointer_func_ptr;
  uint                        hot_patch_table_offset;

  // msvc 2019
  uint reserved3;
  uint enclave_config_ptr;
  uint volatile_metadata_ptr;
  uint guard_eh_continue_table;
  uint guard_eh_continue_count;
  uint guard_xfg_check_func_ptr;
  uint guard_xfg_dispatch_func_ptr;
  uint guard_xfg_table_dispatch_func_ptr;
  uint cast_guard_os_determined_failure_mode;
};

struct PE_LoadConfig64
{
  uint            size;
  COFF_TimeStamp time_stamp;
  ushort            major_version;
  ushort            minor_version;
  uint            global_flag_clear;
  uint            global_flag_set;
  uint            critical_section_timeout;
  ulong            decommit_free_block_threshold;
  ulong            decommit_total_free_threshold;
  ulong            lock_prefix_table;
  ulong            maximum_allocation_size;
  ulong            virtual_memory_threshold;
  ulong            process_affinity_mask;
  uint            process_heap_flags;
  ushort            csd_version;
  ushort            reserved;
  ulong            edit_list;
  ulong            security_cookie;
  ulong            seh_handler_table;
  ulong            seh_handler_count;

  // msvc 2015
  ulong guard_cf_check_func_ptr;
  ulong guard_cf_dispatch_func_ptr;
  ulong guard_cf_func_table;
  ulong guard_cf_func_count;
  uint guard_flags;

  // msvc 2017
  PE_LoadConfigCodeIntegrity code_integrity;
  ulong                        guard_address_taken_iat_entry_table;
  ulong                        guard_address_taken_iat_entry_count;
  ulong                        guard_long_jump_target_table;
  ulong                        guard_long_jump_target_count;
  ulong                        dynamic_value_reloc_table;
  ulong                        chpe_metadata_ptr;
  ulong                        guard_rf_failure_routine;
  ulong                        guard_rf_failure_routine_func_ptr;
  uint                        dynamic_value_reloc_table_offset;
  ushort                        dynamic_value_reloc_table_section;
  ushort                        reserved2;
  ulong                        guard_rf_verify_stack_pointer_func_ptr;
  uint                        hot_patch_table_offset;

  // msvc 2019
  uint reserved3;
  ulong enclave_config_ptr;
  ulong volatile_metadata_ptr;
  ulong guard_eh_continue_table;
  ulong guard_eh_continue_count;
  ulong guard_xfg_check_func_ptr;
  ulong guard_xfg_dispatch_func_ptr;
  ulong guard_xfg_table_dispatch_func_ptr;
  ulong cast_guard_os_determined_failure_mode;
};

// this is the "MZ" as a 16-bit short
#define PE_DOS_MAGIC      0x5a4d
#define PE_MAGIC          0x00004550u
#define PE_PE32_MAGIC     0x010bu
#define PE_PE32PLUS_MAGIC 0x020bu

struct PE_MipsPdata
{
  uint voff_first;
  uint voff_one_past_last;
  uint voff_exception_handler;
  uint voff_exception_handler_data;
  uint voff_one_past_prolog;
};

struct PE_ArmPdata
{
  uint voff_first;
  // NOTE(allen):
  // bits    | meaning
  // [0:7]   | prolog length
  // [8:29]  | function length
  // [30:30] | instructions_are_32bits (otherwise they are 16 bits)
  // [31:31] | has_exception_handler
  uint combined;
};

struct PE_IntelPdata
{
  uint voff_first;
  uint voff_one_past_last;
  uint voff_unwind_info;
};

#define PE_CODEVIEW_PDB20_MAGIC 0x3031424e
#define PE_CODEVIEW_PDB70_MAGIC 0x53445352
#define PE_CODEVIEW_RDI_MAGIC   '0IDR' 

struct PE_CvHeaderPDB20
{
  uint            magic;
  uint            offset;
  COFF_TimeStamp time_stamp;
  uint            age;
  // file name packed after struct
};

struct PE_CvHeaderPDB70
{
  uint  magic;
  Guid guid;
  uint  age;
  // file name packed after struct
};

struct PE_CvHeaderRDI
{
  uint  magic;
  Guid guid;
  // file name packed after struct
};

struct PE_ImportEntry
{
  uint            lookup_table_voff;
  COFF_TimeStamp time_stamp;
  uint            forwarder_chain;
  uint            name_voff;
  uint            import_addr_table_voff;
};

struct PE_DelayedImportEntry
{
  // According to COFF/PE spec this field is unused and should be set zero,
  // but when I compile mule with MSVC 2019 this is set to 1.
  uint            attributes;
  uint            name_voff;          // Name of the DLL
  uint            module_handle_voff; // Place where module handle from LoadLibrary is stored
  uint            iat_voff;
  uint            name_table_voff;    // Array of hint/name or oridnals
  uint            bound_table_voff;   // (Optional) Points to an array of PE_ThunkData
  uint            unload_table_voff;  // (Optional) Copy of iat_voff
  //  0 not bound
  // -1 if bound and real timestamp located in bounded import directory
  // Otherwise time when dll was bound
  COFF_TimeStamp time_stamp;
};

struct PE_ExportTableHeader
{
  uint            flags;                       // must be zero
  COFF_TimeStamp time_stamp;                  // time and date when export table was created
  ushort            major_ver;                   // table version, user can change major and minor version
  ushort            minor_ver;
  uint            name_voff;                   // ASCII name of the dll
  uint            ordinal_base;                // Starting oridnal number
  uint            export_address_table_count;
  uint            name_pointer_table_count;
  uint            export_address_table_voff;
  uint            name_pointer_table_voff;
  uint            ordinal_table_voff;
};

struct PE_TLSHeader32
{
  uint               raw_data_start;    // Range of initialized data that is copied for each thread from the image.
  uint               raw_data_end;      // (Typically points to .tls section)
  uint               index_address;     // Address where image loader places TLS slot index.
  uint               callbacks_address; // Zero terminated list of callbacks used for initializing data with constructors.
  uint               zero_fill_size;    // Amount of memory to fill with zeroes in TLS.
  COFF_SectionFlags characteristics;   // COFF_SectionFlags but only align flags are used.
};

struct PE_TLSHeader64
{
  ulong               raw_data_start;    // Range of initialized data that is copied for each thread from the image.
  ulong               raw_data_end;      // (Typically points to .tls section)
  ulong               index_address;     // Address where image loader places TLS slot index.
  ulong               callbacks_address; // Zero terminated list of callbacks used for initializing data with constructors.
  uint               zero_fill_size;    // Amount of memory to fill with zeroes in TLS.
  COFF_SectionFlags characteristics;   // COFF_SectionFlags but only align flags are used.
};

static read_only byte PE_RES_MAGIC[] =
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

enum PE_ResourceKind : uint
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

enum PE_ResDataKind
{
  PE_ResDataKind_NULL,
  PE_ResDataKind_DIR,
  PE_ResDataKind_COFF_LEAF,
  PE_ResDataKind_COFF_RESOURCE,
};

struct PE_ResourceHeader
{
  COFF_ResourceHeaderPrefix prefix;
  ushort                       type;
  ushort                       pad0;
  ushort                       name;
  ushort                       pad1;
  uint                       data_version;
  COFF_ResourceMemoryFlags  memory_flags;
  ushort                       language_id;
  uint                       version;
  uint                       characteristics;
};

enum PE_BaseRelocKind : ushort
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
#define PE_BaseRelocMake(k, off)       ((((ushort)(k) & 0xf) << 12) | (ushort)((off) & 0x1fff))

enum PE_UnwindOpCode : uint
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

enum PE_UnwindGprRegX64 : byte
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

enum PE_UnwindInfoFlags : byte
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
    byte off_in_prolog;
    byte flags;
  };
  ushort u16;
};

#define PE_UNWIND_INFO_VERSION_FROM_HDR(x) ((x)&0x7)
#define PE_UNWIND_INFO_FLAGS_FROM_HDR(x)   (((x) >> 3)&0x1F)
#define PE_UNWIND_INFO_REG_FROM_FRAME(x)   ((x)&0xF)
#define PE_UNWIND_INFO_OFF_FROM_FRAME(x)   (((x) >> 4)&0xF)
#define PE_UNWIND_INFO_GET_CODE_COUNT(x)   (((x)+1) & ~1)

struct PE_UnwindInfo
{
  byte header;
  byte prolog_size;
  byte codes_num;
  byte frame;
};

#pragma pack(pop)

////////////////////////////////
//~ rjf: DOS Program

// generated from pe/dos_program.asm
read_only static byte pe_dos_program_data[] =
{
  0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
  0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
  0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
  0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x24, 0x00, 0x00
};
read_only static String8 pe_dos_program = {pe_dos_program_data, sizeof(pe_dos_program_data)};

////////////////////////////////
//~ rjf: Parsed Info Types

//- rjf: relocation blocks

struct PE_BaseRelocBlock
{
  ulong  page_virt_off;
  ulong  entry_count;
  ushort *entries;
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
  ulong                    count;
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
      uint                      data_version;
      uint                      version;
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
  ulong              count;
};

struct PE_ResourceArray
{
  PE_Resource *v;
  ulong          count;
};

struct PE_ResourceDir
{
  uint             characteristics;
  COFF_TimeStamp  time_stamp;
  ushort             major_version;
  ushort             minor_version;
  PE_ResourceList named_list;
  PE_ResourceList id_list;
};

//- exports & imports

struct PE_ParsedExport
{
  String8 forwarder;
  String8 name;
  ulong     voff;
  ulong     ordinal;
};

struct PE_ParsedExportTable
{
  uint              flags;
  COFF_TimeStamp   time_stamp;
  ushort              major_ver;
  ushort              minor_ver;
  ulong              ordinal_base;
  ulong              export_count;
  PE_ParsedExport *exports;
};

enum PE_ParsedImportType : uint PE_ParsedImportTypeEnum
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
    ushort ordinal;
    struct
    {
      ulong     hint;
      String8 string;
    } name;
  } u;
};

struct PE_ParsedStaticDLLImport
{
  String8          name;
  ulong              import_address_table_voff;
  ulong              import_name_table_voff;
  COFF_TimeStamp   time_stamp;
  ulong              forwarder_chain;
  ulong              import_count;
  PE_ParsedImport *imports;
};

struct PE_ParsedStaticImportTable
{
  ulong                       count;
  PE_ParsedStaticDLLImport *v;
};

struct PE_ParsedDelayDLLImport
{
  uint              attributes;
  String8          name;
  ulong              module_handle_voff;
  ulong              iat_voff;
  ulong              name_table_voff;
  ulong              bound_table_voff;
  ulong              unload_table_voff;
  COFF_TimeStamp   time_stamp;
  ulong              bound_table_count;
  ulong             *bound_table;
  ulong              unload_table_count;
  ulong             *unload_table;
  ulong              import_count;
  PE_ParsedImport *imports;
};

struct PE_ParsedDelayImportTable
{
  ulong                      count;
  PE_ParsedDelayDLLImport *v;
};

struct PE_ParsedTLS
{
  PE_TLSHeader64 header;
  ulong            callback_count;
  ulong           *callback_addrs;
};

////////////////////////////////
// SEH Scope Table

struct PE_HandlerScope
{
  uint begin;
  uint end;
  uint handler;
  uint target;
};

//- rjf: bundle

struct PE_BinInfo
{
  ulong             image_base;
  ulong             entry_point;
  B32             is_pe32;
  ulong             virt_section_align;
  ulong             file_section_align;
  ulong             section_array_off;
  ulong             section_count;
  ulong             symbol_array_off;
  ulong             symbol_count;
  ulong             string_table_off;
  Arch            arch;
  Rng1U64        *data_dir_franges;
  uint             data_dir_count;
  PE_TLSHeader64  tls_header;
};

srtuct PE_DebugInfo
{
  PE_DebugDirectory header;
  union
  {
    union
    {
      uint magic;
      struct
      {
        PE_CvHeaderPDB20 header;
        String8          path;
      } pdb20;
      struct
      {
        PE_CvHeaderPDB70 header;
        String8          path;
      } pdb70;
      struct
      {
        PE_CvHeaderRDI header;
        String8        path;
      } rdi;
    } codeview;
    String8 raw_data;
  } u;
};

srtuct PE_DebugInfoNode
{
  struct PE_DebugInfoNode *next;
  PE_DebugInfo             v;
};

srtuct PE_DebugInfoList
{
  PE_DebugInfoNode *first;
  PE_DebugInfoNode *last;
  ulong               count;
};

////////////////////////////////
//~ rjf: Basic Enum Functions

uint                 pe_slot_count_from_unwind_op_code(PE_UnwindOpCode opcode);
PE_WindowsSubsystem pe_subsystem_from_string(String8 string);

String8 pe_string_from_subsystem(PE_WindowsSubsystem x);
String8 pe_string_from_unwind_gpr_x64(PE_UnwindGprRegX64 x);
String8 pe_string_from_data_directory_index(PE_DataDirectoryIndex x);
String8 pe_string_from_debug_directory_type(PE_DebugDirectoryType x);
String8 pe_string_from_fpo_type(PE_FPOType x);
String8 pe_string_from_misc_type(PE_DebugMiscType x);
String8 pe_resource_kind_to_string(PE_ResourceKind x);

String8 pe_string_from_fpo_flags(Arena *arena, PE_FPOFlags flags);
String8 pe_string_from_global_flags(Arena *arena, PE_GlobalFlags flags);
String8 pe_string_from_load_config_guard_flags(Arena *arena, PE_LoadConfigGuardFlags flags);
String8 pe_string_from_dll_characteristics(Arena *arena, PE_DllCharacteristics dll_chars);

////////////////////////////////
//~ rjf: Parser Functions

PE_BinInfo pe_bin_info_from_data(Arena *arena, String8 data);

PE_DebugInfoList           pe_parse_debug_directory(Arena *arena, String8 raw_image, String8 raw_debug_dir);
PE_ParsedStaticImportTable pe_static_imports_from_data(Arena *arena, B32 is_pe32, ulong section_count, COFF_SectionHeader *sections, String8 raw_data, Rng1U64 dir_file_range);
PE_ParsedDelayImportTable  pe_delay_imports_from_data(Arena *arena, B32 is_pe32, ulong section_count, COFF_SectionHeader *sections, String8 raw_data, Rng1U64 dir_file_range);
PE_ParsedExportTable       pe_exports_from_data(Arena *arena, ulong section_count, COFF_SectionHeader *sections, String8 raw_data, Rng1U64 dir_file_range, Rng1U64 dir_virt_range);
PE_ParsedTLS               pe_tls_from_data(Arena *arena, COFF_MachineType machine, ulong image_base, ulong section_count, COFF_SectionHeader *sections, String8 raw_data, Rng1U64 tls_frange);

////////////////////////////////
//~ rjf: Helpers

ulong                   pe_pdata_off_from_voff__binary_search_x8664(String8 raw_data, ulong voff);
void *                pe_ptr_from_voff(String8 data, PE_BinInfo *bin, ulong voff);
ulong                   pe_section_num_from_voff(String8 data, PE_BinInfo *bin, ulong voff);
void *                pe_ptr_from_section_num(String8 data, PE_BinInfo *bin, ulong n);
ulong                   pe_foff_from_voff(String8 data, PE_BinInfo *bin, ulong voff);
PE_BaseRelocBlockList pe_base_reloc_block_list_from_data(Arena *arena, String8 raw_relocs);
Rng1U64               pe_tls_rng_from_bin_base_vaddr(String8 data, PE_BinInfo *bin, ulong base_vaddr);
String8Array          pe_get_entry_point_names(COFF_MachineType machine, PE_WindowsSubsystem subsystem, PE_ImageFileCharacteristics file_characteristics);

////////////////////////////////
//~ Resource Helpers

B32               pe_is_res(String8 data);
void              pe_resource_dir_push_res_file(Arena *arena, PE_ResourceDir *root_dir, String8 res_file);
PE_ResourceNode * pe_resource_dir_push_dir_node(Arena *arena, PE_ResourceDir *dir, COFF_ResourceID id, uint characteristics, COFF_TimeStamp time_stamp, ushort major_version, ushort minor_version);
PE_ResourceNode * pe_resource_dir_push_entry_node(Arena *arena, PE_ResourceDir *dir, COFF_ResourceID id, COFF_ResourceID type, uint data_version, uint version, COFF_ResourceMemoryFlags memory_flags, String8 data);
PE_Resource *     pe_resource_dir_push_entry(Arena *arena, PE_ResourceDir *dir, COFF_ResourceID id, COFF_ResourceID type, uint data_version, uint version, COFF_ResourceMemoryFlags memory_flags, String8 data);
PE_Resource *     pe_resource_dir_push_dir(Arena *arena, PE_ResourceDir *dir, COFF_ResourceID id, uint characteristics, COFF_TimeStamp time_stamp, ushort major_version, ushort minor_version);
PE_ResourceNode * pe_resource_dir_search_node(PE_ResourceDir *dir, COFF_ResourceID id);
PE_Resource *     pe_resource_dir_search(PE_ResourceDir *dir, COFF_ResourceID id);
PE_ResourceArray  pe_resource_list_to_array(Arena *arena, PE_ResourceList *list);
PE_ResourceDir *  pe_resource_table_from_directory_data(Arena *arena, String8 data);

String8 pe_make_manifest_resource(Arena *arena, uint resource_id, String8 manifest_data);

////////////////////////////////
//~ Debug Directory

String8 pe_make_debug_header_pdb70(Arena *arena, Guid guid, uint age, String8 pdb_path);
String8 pe_make_debug_header_rdi(Arena *arena, Guid guid, String8 rdi_path);

////////////////////////////////
//~ Image Checksum

uint pe_compute_checksum(byte *buffer, ulong buffer_size);

#endif // PE_H

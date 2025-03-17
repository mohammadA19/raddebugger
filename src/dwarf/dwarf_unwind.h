// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef DWARF_UNWIND_H
#define DWARF_UNWIND_H

struct DW_UnwindResult
{
  B32 is_invalid;
  B32 missed_read;
  uint64 missed_read_addr;
  uint64 stack_pointer;
}

// EH: Exception Frames

enum DW_EhPtrEnc : uint8
{
  DW_EhPtrEnc_TypeMask = 0x0F,
  DW_EhPtrEnc_Ptr       = 0x00, // Pointer sized unsigned value
  DW_EhPtrEnc_ULEB128   = 0x01, // Unsigned LE base-128 value
  DW_EhPtrEnc_UData2    = 0x02, // Unsigned 16-bit value
  DW_EhPtrEnc_UData4    = 0x03, // Unsigned 32-bit value
  DW_EhPtrEnc_UData8    = 0x04, // Unsigned 64-bit value
  DW_EhPtrEnc_Signed    = 0x08, // Signed pointer
  DW_EhPtrEnc_SLEB128   = 0x09, // Signed LE base-128 value
  DW_EhPtrEnc_SData2    = 0x0A, // Signed 16-bit value
  DW_EhPtrEnc_SData4    = 0x0B, // Signed 32-bit value
  DW_EhPtrEnc_SData8    = 0x0C, // Signed 64-bit value
}

enum
{
  DW_EhPtrEnc_ModifyMask = 0x70,
  DW_EhPtrEnc_PcRel      = 0x10, // Value is relative to the current program counter.
  DW_EhPtrEnc_TextRel    = 0x20, // Value is relative to the .text section.
  DW_EhPtrEnc_DataRel    = 0x30, // Value is relative to the .got or .eh_frame_hdr section.
  DW_EhPtrEnc_FuncRel    = 0x40, // Value is relative to the function.
  DW_EhPtrEnc_Aligned    = 0x50, // Value is aligned to an address unit sized boundary.
}

enum
{
  DW_EhPtrEnc_Indirect = 0x80, // This flag indicates that value is stored in virtual memory.
  DW_EhPtrEnc_Omit     = 0xFF,
}

struct DW_EhPtrCtx
{
  uint64 raw_base_vaddr; // address where pointer is being read
  uint64 text_vaddr;     // base address of section with instructions (used for encoding pointer on SH and IA64)
  uint64 data_vaddr;     // base address of data section (used for encoding pointer on x86-64)
  uint64 func_vaddr;     // base address of function where IP is located
}

// CIE: Common Information Entry
struct DW_CIEUnpacked
{
  uint8          version;
  DW_EhPtrEnc lsda_encoding;
  DW_EhPtrEnc addr_encoding;
  
  B32     has_augmentation_size;
  uint64     augmentation_size;
  StringView augmentation;
  
  uint64 code_align_factor;
  int64 data_align_factor;
  uint64 ret_addr_reg;
  
  uint64 handler_ip;
  
  Rng1U64 cfi_range;
}

struct DW_CIEUnpackedNode
{
  struct DW_CIEUnpackedNode* next;
  DW_CIEUnpacked             cie;
  uint64                        offset;
}

// FDE: Frame Description Entry
struct DW_FDEUnpacked
{
  Rng1U64 ip_voff_range;
  uint64     lsda_ip;
  Rng1U64 cfi_range;
}

// CFI: Call Frame Information
struct DW_CFIRecords
{
  B32            valid;
  DW_CIEUnpacked cie;
  DW_FDEUnpacked fde;
}

enum DW_CFICFARule{
  DW_CFI_CFA_Rule_RegOff,
  DW_CFI_CFA_Rule_Expr,
}

struct DW_CFICFACell
{
  DW_CFICFARule rule;
  [Union]
  struct {
    struct {
      uint64 reg_idx;
      int64 offset;
    };
    Rng1U64 expr;
  };
}

enum DW_CFIRegisterRule
{
  DW_CFIRegisterRule_SameValue,
  DW_CFIRegisterRule_Undefined,
  DW_CFIRegisterRule_Offset,
  DW_CFIRegisterRule_ValOffset,
  DW_CFIRegisterRule_Register,
  DW_CFIRegisterRule_Expression,
  DW_CFIRegisterRule_ValExpression,
}

struct DW_CFICell
{
  DW_CFIRegisterRule rule;
  [Union]
  struct {
    int64 n;
    Rng1U64 expr;
  };
}

struct DW_CFIRow
{
  struct DW_CFIRow* next;
  DW_CFICell*       cells;
  DW_CFICFACell     cfa_cell;
}

struct DW_CFIMachine
{
  uint64             cells_per_row;
  DW_CIEUnpacked* cie;
  DW_EhPtrCtx*    ptr_ctx;
  DW_CFIRow*      initial_row;
  uint64             fde_ip;
}

enum DW_CFADecode : uint8
{
  DW_CFADecode_Nop     = 0x0,
  // 1,2,4,8 reserved for literal byte sizes
  DW_CFADecode_Address = 0x9,
  DW_CFADecode_ULEB128 = 0xA,
  DW_CFADecode_SLEB128 = 0xB,
}

enum DW_CFAControlBits : uint16
{
  DW_CFAControlBits_Dec1Mask = 0x00F,
  DW_CFAControlBits_Dec2Mask = 0x0F0,
  DW_CFAControlBits_IsReg0   = 0x100,
  DW_CFAControlBits_IsReg1   = 0x200,
  DW_CFAControlBits_IsReg2   = 0x400,
  DW_CFAControlBits_NewRow   = 0x800,
}

static read_only DW_CFAControlBits dw_unwind__cfa_control_bits_kind1[DW_CFA_OplKind1 + 1];
static read_only DW_CFAControlBits dw_unwind__cfa_control_bits_kind2[DW_CFA_OplKind2 + 1];

// register codes for unwinding match the DW_RegX64 register codes
#define DW_UNWIND_X64__REG_SLOT_COUNT 17

////////////////////////////////
// x64 Unwind Function

DW_UnwindResult
dw_unwind_x64(StringView           raw_text,
              StringView           raw_eh_frame,
              StringView           raw_eh_frame_header,
              Rng1U64           text_vrange,
              Rng1U64           eh_frame_vrange,
              Rng1U64           eh_frame_header_vrange,
              uint64               default_image_base,
              uint64               image_base,
              uint64               stack_pointer,
              DW_RegsX64*       regs,
              DW_ReadMemorySig* read_memory,
              void*             read_memory_ud);

DW_UnwindResult dw_unwind_x64__apply_frame_rules(StringView raw_eh_frame, DW_CFIRow* row, uint64 text_base_vaddr, DW_ReadMemorySig* read_memory, void* read_memory_ud, uint64 stack_pointer, DW_RegsX64* regs);

////////////////////////////////
// x64 Unwind Helper Functions

void dw_unwind_init_x64();
uint64  dw_unwind_parse_pointer_x64(void* base, Rng1U64 range, DW_EhPtrCtx* ptr_ctx, DW_EhPtrEnc ptr_enc, uint64 off, uint64* ptr_out);

//- eh_frame parsing
void dw_unwind_parse_cie_x64(void* base,Rng1U64 range,DW_EhPtrCtx* ptr_ctx, uint64 off, DW_CIEUnpacked* cie_out);
void dw_unwind_parse_fde_x64(void* base,Rng1U64 range,DW_EhPtrCtx* ptr_ctx, DW_CIEUnpacked* parent_cie, uint64 off, DW_FDEUnpacked* fde_out);
DW_CFIRecords dw_unwind_eh_frame_cfi_from_ip_slow_x64(StringView raw_eh_frame, DW_EhPtrCtx* ptr_ctx, uint64 ip_voff);
DW_CFIRecords dw_unwind_eh_frame_hdr_from_ip_fast_x64(StringView raw_eh_frame, StringView raw_eh_frame_hdr, DW_EhPtrCtx* ptr_ctx, uint64 ip_voff);

//- cfi machine

DW_CFIMachine dw_unwind_make_machine_x64(uint64 cells_per_row, DW_CIEUnpacked* cie, DW_EhPtrCtx* ptr_ctx);
void          dw_unwind_machine_equip_initial_row_x64(DW_CFIMachine* machine, DW_CFIRow* initial_row);
void          dw_unwind_machine_equip_fde_ip_x64(DW_CFIMachine* machine, uint64 fde_ip);

DW_CFIRow* dw_unwind_row_alloc_x64(Arena* arena, uint64 cells_per_row);
void       dw_unwind_row_zero_x64(DW_CFIRow* row, uint64 cells_per_row);
void       dw_unwind_row_copy_x64(DW_CFIRow* dst, DW_CFIRow* src, uint64 cells_per_row);

B32 dw_unwind_machine_run_to_ip_x64(void* base, Rng1U64 range, DW_CFIMachine* machine, uint64 target_ip, DW_CFIRow* row_out);

#endif // DWARF_UNWIND_H


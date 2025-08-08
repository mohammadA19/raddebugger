// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef RADDBG_WIDGETS_H
#define RADDBG_WIDGETS_H

////////////////////////////////
//~ rjf: Cell Types

enum RD_CellFlags : U32
{
    //- rjf: expander
    RD_CellFlag_Expander            = (1<<0),
    RD_CellFlag_ExpanderSpace       = (1<<1),
    RD_CellFlag_ExpanderPlaceholder = (1<<2),
    
    //- rjf: toggle switch extension
    RD_CellFlag_ToggleSwitch        = (1<<3),
    
    //- rjf: slider extension
    RD_CellFlag_Slider              = (1<<4),
    
    //- rjf: bindings extension
    RD_CellFlag_Bindings            = (1<<5),
    
    //- rjf: extra button extensions
    RD_CellFlag_EmptyEditButton     = (1<<6),
    RD_CellFlag_RevertButton        = (1<<7),
    
    //- rjf: behavior
    RD_CellFlag_DisableEdit         = (1<<8),
    RD_CellFlag_KeyboardClickable   = (1<<9),
    RD_CellFlag_SingleClickActivate = (1<<10),
    
    //- rjf: contents description
    RD_CellFlag_CodeContents        = (1<<11),
    
    //- rjf: appearance
    RD_CellFlag_Border              = (1<<12),
    RD_CellFlag_NoBackground        = (1<<13),
    RD_CellFlag_Button              = (1<<14),
};

struct RD_CellParams
{
    //- rjf: catchall parameters
    RD_CellFlags flags;
    S32 depth;
    String8 pre_edit_value;
    DR_FStrList meta_fstrs;
    DR_FStrList value_fstrs;
    String8 search_needle;
    String8 description;
    
    //- rjf: expander r/w info
    B32 *expanded_out;
    
    //- rjf: toggle-switch r/w info
    B32 *toggled_out;
    
    //- rjf: slider info r/w info
    F32 *slider_value_out;
    
    //- rjf: bindings name w info
    String8 bindings_name;
    
    //- rjf: revert out
    B32 *revert_out;
    
    //- rjf: text editing r/w info
    TxtPt *cursor;
    TxtPt *mark;
    U8 *edit_buffer;
    U64 edit_buffer_size;
    U64 *edit_string_size_out;
    UI_Key *line_edit_key_out;
};

////////////////////////////////
//~ rjf: Code Slice Types

enum RD_CodeSliceFlags : U32
{
    RD_CodeSliceFlag_Clickable         = (1<<0),
    RD_CodeSliceFlag_PriorityMargin    = (1<<1),
    RD_CodeSliceFlag_CatchallMargin    = (1<<2),
    RD_CodeSliceFlag_LineNums          = (1<<3),
};

struct RD_CodeSliceParams
{
    // rjf: content
    RD_CodeSliceFlags flags;
    Rng1S64 line_num_range;
    String8 *line_text;
    Rng1U64 *line_ranges;
    TXT_TokenArray *line_tokens;
    RD_CfgList *line_bps;
    CTRL_EntityList *line_ips;
    RD_CfgList *line_pins;
    U64 *line_vaddrs;
    D_LineList *line_infos;
    DI_KeyList relevant_dbgi_keys;
    TXT_TextInfo *text_info;
    
    // rjf: visual parameters
    FNT_Tag font;
    F32 font_size;
    F32 tab_size;
    String8 search_query;
    F32 line_height_px;
    F32 priority_margin_width_px;
    F32 catchall_margin_width_px;
    F32 line_num_width_px;
    F32 line_text_max_width_px;
    F32 margin_float_off_px;
};

struct RD_CodeSliceSignal
{
    UI_Signal base;
    TxtPt mouse_pt;
    TxtRng mouse_expr_rng;
};

////////////////////////////////
//~ rjf: UI Building Helpers

#define RD_Font(slot) UI_Font(rd_font_from_slot(slot)) UI_TextRasterFlags(rd_raster_flags_from_slot((slot)))

////////////////////////////////
//~ rjf: UI Widgets: Fancy Title Strings


////////////////////////////////
//~ rjf: UI Widgets: Loading Overlay


////////////////////////////////
//~ rjf: UI Widgets: Fancy Buttons


////////////////////////////////
//~ rjf: UI Widgets: Text View



////////////////////////////////
//~ rjf: UI Widgets: Fancy Labels


////////////////////////////////
//~ rjf: UI Widgets: Line Edit


#endif // RADDBG_WIDGETS_H

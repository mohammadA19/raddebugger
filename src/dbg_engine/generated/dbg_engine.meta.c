// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//- GENERATED CODE

C_LINKAGE_BEGIN
D_ViewRuleSpecInfo d_core_view_rule_spec_info_table[21] =
{
{("default"), ("Default"), (""), (""), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("array"), ("Array"), ("x:{expr}"), ("Specifies that a pointer points to N elements, rather than only 1."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*1)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("slice"), ("Slice"), (""), ("Specifies that a pointer within a struct, also containing an integer, points to the number of elements encoded by the integer."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*1)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("list"), ("List"), ("x:{member}"), ("Specifies that some struct, union, or class forms the top of a linked list, and the member which points at the following element in the list."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("bswap"), ("Byte Swap"), (""), ("Specifies that all integral evaluations should be byte-swapped, such that their endianness is reversed."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*1)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("cast"), ("Cast"), ("x:{type}"), ("Specifies that the expression to which the view rule is applied should be casted to the provided type."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*1)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("dec"), ("Decimal Base (Base 10)"), (""), ("Specifies that all integral evaluations should appear in base-10 form."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("bin"), ("Binary Base (Base 2)"), (""), ("Specifies that all integral evaluations should appear in base-2 form."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("oct"), ("Octal Base (Base 8)"), (""), ("Specifies that all integral evaluations should appear in base-8 form."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("hex"), ("Hexadecimal Base (Base 16)"), (""), ("Specifies that all integral evaluations should appear in base-16 form."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("only"), ("Only Specified Members"), ("x:{member}"), ("Specifies that only the specified members should appear in struct, union, or class evaluations."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("omit"), ("Omit Specified Members"), ("x:{member}"), ("Omits a list of member names from appearing in struct, union, or class evaluations."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("no_addr"), ("Disable Address Values"), (""), ("Displays only what pointers point to, if possible, without the pointer's address value."), (D_ViewRuleSpecInfoFlag_Inherited*1)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("checkbox"), ("Checkbox"), (""), ("Displays simple integer values as checkboxes, encoding zero or nonzero values."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*0)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*0), },
{("color_rgba"), ("Color (RGBA)"), (""), ("Displays as a color, interpreting the data as encoding R, G, B, and A values."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("text"), ("Text"), ("x:{'lang':lang, 'size':expr}"), ("Displays as text."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("disasm"), ("Disassembly"), ("x:{'arch':arch, 'size':expr}"), ("Displays as disassembled instructions, interpreting the data as raw machine code."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("memory"), ("Memory"), ("x:{'size':expr}"), ("Displays as a raw memory grid."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("graph"), ("Graph"), (""), ("Displays as a pointer graph, visualizing nodes and edges formed by pointers directly."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("bitmap"), ("Bitmap"), ("x:{'w':expr, 'h':expr, 'fmt':tex2dformat}"), ("Displays as a bitmap, interpreting the data as raw pixel data."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
{("geo3d"), ("Geometry (3D)"), ("x:{'count':expr, 'vtx':expr, 'vtx_size':expr}"), ("Displays as geometry, interpreting the data as index or vertex data."), (D_ViewRuleSpecInfoFlag_Inherited*0)|(D_ViewRuleSpecInfoFlag_Expandable*1)|(D_ViewRuleSpecInfoFlag_ExprResolution*0)|(D_ViewRuleSpecInfoFlag_VizBlockProd*1), },
};

C_LINKAGE_END


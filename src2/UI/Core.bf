
public struct UI_Key : uint64;

public struct UI_Box
{
    //- rjf: per-build equipment
    UI_Key Key;
    UI_BoxFlags Flags;
    StringView String;
    UI_TextAlign TextAlign;
    Vec2<float> FixedPosition, FixedSize;
    (UI_Size x, UI_Size y) PrefSize;
    Axis2 ChildLayoutAxis;
    OS_Cursor HoverCursor;
    char32 FastpathCodepoint;
    UI_Key GroupKey;
    // draw_bucket 
    UI_BoxCustomDrawFunctionType CustomDraw;
    void* CustomDrawUserData;
    // palette
    FNT_Tag Font;
    float FontSize, TabSize;
    FNT_RasterFlags TextRasterFlags;
    float[4] CornerRadii;
    float BlurSize, Transparency, Squish, TextPadding;

    //- rjf: per-build artifacts
    
}
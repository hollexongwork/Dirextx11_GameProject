float2 SubUV(float2 TexCoord, int Frame, int WidthDivision, int HeightDivision)
{
    if (WidthDivision <= 0 || HeightDivision <= 0)
    {
        return float2(0.0f, 0.0f);
    }
 
    float w = 1.0f / WidthDivision;
    float h = 1.0f / HeightDivision;
    
    float2 UV;
    UV.x = (Frame % WidthDivision) * w;
    UV.y = (Frame / HeightDivision) * h;
    
    UV += TexCoord * float2(1.0f / WidthDivision, 1.0f / HeightDivision);

    return UV;
}
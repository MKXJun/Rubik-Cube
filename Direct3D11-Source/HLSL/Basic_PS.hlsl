#include "Basic.hlsli"

// ÏñËØ×ÅÉ«Æ÷
float4 PS(VertexPosHTex pIn) : SV_Target
{
    float4 texColor = gTexArray.Sample(gSam, float3(pIn.Tex, gTexIndex));
    return texColor;
}

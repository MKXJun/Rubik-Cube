#include "Basic.hlsli"

// ¶¥µã×ÅÉ«Æ÷
VertexPosHTex VS(VertexPosTex pIn)
{
    VertexPosHTex pOut;
    
    matrix worldViewProj = mul(gWorld, mul(gView, gProj));
   
    pOut.PosH = mul(float4(pIn.PosL, 1.0f), worldViewProj);
    pOut.Tex = pIn.Tex;
    return pOut;
}
Texture2DArray gTexArray : register(t0);
SamplerState gSam : register(s0);

cbuffer CBChangesEveryDrawing : register(b0)
{
    int gTexIndex;
    float3 gPad;
}

cbuffer CBChangesEveryCube : register(b1)
{
    matrix gWorld;
}

cbuffer CBChangesEveryFrame : register(b2)
{
	matrix gView;
}

cbuffer CBChangesOnResize : register(b3)
{
	matrix gProj;
}


struct VertexPosTex
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexPosHTex
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};







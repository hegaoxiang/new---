
TextureCube g_TexCube : register(t0);
SamplerState g_Sam : register(s0);

cbuffer CBChangesEveryFrame : register(b0)
{
    matrix g_WorldViewProj;
};

cbuffer CB : register(b1)
{
    float4 Corners[4];
    float4 eyePos;
};


struct VertexIn
{
    float3 posL : POSITION;
    float3 norL : NORMAL;
    float4 tan : TANGENT;
    float2 uv : TEXCOORD;
};

struct VertexPosHL
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};



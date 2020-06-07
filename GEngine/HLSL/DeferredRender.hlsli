
Texture2D g_Albedo : register(t0);
sampler2D g_Sam : register(s0);

cbuffer MVP : register(b0)
{
    matrix world;
};

struct VSInput
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
    float2 ux : TEXCOORD;
};

struct VSOutput
{
    float4 posH : SV_Position;
    float3 normalH : NORMAL;
    float2 uv : TEXCOORD;
};

struct PSOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 worldPos : SV_Target2;
};
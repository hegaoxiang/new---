
cbuffer MVP : register(b0)
{
    matrix g_World;
    matrix g_View;
    matrix g_Proj;
};

struct VertexIn
{
    float3 posL : POSITION;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
};

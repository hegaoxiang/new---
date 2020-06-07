#include "Basic.hlsli"



// 像素着色器
float4 PS(VertexOut pIn) : SV_Target
{
    return g_Tex.Sample(g_Sam, pIn.uv);
}

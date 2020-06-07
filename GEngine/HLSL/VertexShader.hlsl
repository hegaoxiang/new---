#include"Light.hlsli"


VertexOut VS(VertexIn vIn) 
{
   
    VertexOut vOut;
    vOut.posH = mul(mul(mul(float4(vIn.posL, 1.0), g_World), g_View), g_Proj);
    return vOut;
}

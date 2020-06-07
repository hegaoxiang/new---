#include "Sky.hlsli"

VertexPosHL VS(VertexIn vIn)
{
    VertexPosHL vOut;
    
   // // 设置z = w使得z/w = 1(天空盒保持在远平面)
   // float4 posH = mul(float4(vIn.PosL, 1.0f), g_WorldViewProj);
   // vOut.PosH = posH.xyww;
   // vOut.PosL = vIn.PosL;
    vOut.PosH = float4(vIn.posL.x, vIn.posL.y, 1.0f, 1.0f);
    vOut.PosL = Corners[vIn.uv.x + vIn.uv.y*2].xyz;
    return vOut;
}
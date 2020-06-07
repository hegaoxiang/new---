
cbuffer ConstantBuffer : register(b0)
{
    matrix g_World; // matrix可以用float4x4替代。不加row_major的情况下，矩阵默认为列主矩阵，
    matrix g_View;  // 可以在前面添加row_major表示行主矩阵
    matrix g_Proj;  // 该教程往后将使用默认的列主矩阵，但需要在C++代码端预先将矩阵进行转置。
}

Texture2D g_Tex : register(t0);
SamplerState g_Sam : register(s0);


struct VertexIn
{
	float3 posL : POSITION;
    float3 norL : NORMAL;
    float4 tan : TANGENT;
    float2 uv : TEXCOORD;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
    float2 uv : TEXCOORD;
};

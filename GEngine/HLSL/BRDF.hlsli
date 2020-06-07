
#define M_PI 3.1415926

cbuffer Material : register(b0)
{
    float3 albedo;
    float roughness;
    
};

float NDF_GGX_TR(float3 normal,float3 halfVec,float roughness)
{
    normal = normalize(normal);
    halfVec = normalize(halfVec);
    
    float roughness2 = roughness * roughness;
    
    float NdotH = dot(normal, halfVec);
    
    float NdotH2 = pow(NdotH, 2);
    
    float numerator = roughness2;
    
    float denominator = M_PI * pow( NdotH2 * (roughness2 - 1) + 1, 2);

    return numerator / denominator;
}

float G_GGX_S(float3 normal,float3 v,float k)
{
    float NdotV = dot(normal, v);
    
    float numerator = NdotV;
    
    float denominator = NdotV * (1 - k) + k;
    
    return numerator / denominator;
}

float G_Smith(float3 normal,float3 vi,float vo,float k)
{
    return G_GGX_S(normal, vi, k) * G_GGX_S(normal, vo, k);
}

float ReMapping(float roughness)
{
    // IBL
    return roughness * roughness / 2;
    
    // Direct
    return pow(roughness + 1, 2) / 8;
}
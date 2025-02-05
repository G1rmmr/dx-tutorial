cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
    
    float3 LightPos;
    float Padd;
    
    float3 AmbColor;
    float AmbPadd;
    
    float3 DiffColor;
    float DiffPadd;
    
    float3 SpecColor;
    float Shininess;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Norm : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION;
    float3 Norm : NORMAL;
};

cbuffer MatrixBuffer : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;

    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(pos, World);
    pos = mul(pos, View);
    pos = mul(pos, Proj);
    
    output.Pos = pos;
    output.TexCoord = normalize(input.Pos);
    
    return output;
}
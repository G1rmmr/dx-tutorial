cbuffer MatrixBuffer : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};


struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 pos = float4(input.Pos, 1.0f);
    
    pos = mul(pos, World);
    pos = mul(pos, View);
    pos = mul(pos, Proj);

    output.Pos = pos;
    output.Color = input.Color;
    
    return output;
}
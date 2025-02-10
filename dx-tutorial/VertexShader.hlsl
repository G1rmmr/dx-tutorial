cbuffer MatrixBuffer : register(b0)
{
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
    
    float4 posWorld = float4(input.Pos, 1.0f);
    
    float4 posView = mul(posWorld, View);
    output.Pos = mul(posView, Proj);
    
    output.Color = input.Color;
    return output;
}

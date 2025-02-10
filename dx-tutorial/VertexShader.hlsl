#include "Common.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.Pos, 1.0), World);
    output.WorldPos = worldPos.xyz;
    
    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Proj);
    
    output.Norm = mul(input.Norm, (float3x3)World);
    output.Norm = normalize(output.Norm);
    
    return output;
}

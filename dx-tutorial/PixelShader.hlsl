#include "Common.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 amb = AmbColor;
    
    float3 lightDir = normalize(LightPos - input.WorldPos);
    
    float _diff = max(dot(input.Norm, lightDir), 0.0);
    float3 diff = _diff * DiffColor;
    
    float3 viewDir = normalize(-input.WorldPos);
    float3 reflectDir = reflect(-lightDir, input.Norm);
    
    float _spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    float3 spec = _spec * SpecColor;
    
    float3 result = amb + diff + spec;
    return float4(result, 1.0);
}

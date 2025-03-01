TextureCube skyboxTexture : register(t0);
SamplerState samplerState : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return skyboxTexture.Sample(samplerState, input.texCoord);
}

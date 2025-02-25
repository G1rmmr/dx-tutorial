cbuffer MatrixBuffer : register(b0)
{
    matrix View;
    matrix Proj;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    matrix view = View;
    view._41 = 0.f;
    view._42 = 0.f;
    view._43 = 0.f;

    float4 pos = mul(float4(input.Pos, 1.0f), view);
    pos = mul(pos, Proj);
    output.Pos = pos;
    output.TexCoord = input.Pos;
    
    return output;
}

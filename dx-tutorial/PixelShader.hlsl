struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return input.Color;
}

struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer GrayscaleSettings : register(b0)
{
    float grayscaleStrength; // 0.0 ~ 1.0
};
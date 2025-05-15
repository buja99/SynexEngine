

cbuffer Material : register(b0)
{
    float4 color;
    float4x4 uvTransform;
};

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float2 uv = mul(float4(input.texcoord, 0, 1), uvTransform).xy;
    float4 texColor = gTexture.Sample(gSampler, uv);
    return color * texColor;
}
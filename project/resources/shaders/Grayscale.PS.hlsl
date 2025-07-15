#include "Grayscale.hlsli"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float3 color = gTexture.Sample(gSampler, input.texcoord).rgb;
    float grayscale = dot(color, float3(0.2125f, 0.7154f, 0.0721f));
    color = lerp(color, float3(grayscale, grayscale, grayscale), grayscaleStrength);
    return float4(color, 1.0f);
}
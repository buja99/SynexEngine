#include "Grayscale.hlsli"

Texture2D gTexture : register(t0); // Scene 텍스처
SamplerState gSampler : register(s0); // 샘플러

float4 main(VSOutput input) : SV_TARGET
{
    float3 color = gTexture.Sample(gSampler, input.texcoord).rgb;

    // Grayscale 변환 (BT.709 방식)
    float grayscale = dot(color, float3(0.2125f, 0.7154f, 0.0721f));
    return float4(grayscale, grayscale, grayscale, 1.0f);
}
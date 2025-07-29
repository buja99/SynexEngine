#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


cbuffer VignetteSettings : register(b0)
{
    float vignetteStrength;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

PixelShaderOutput main(VSOutput input)
{
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);

    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    float vignette = correct.x * correct.y * 16.0f;
    vignette = pow(saturate(vignette), 0.8f);

    baseColor.rgb *= lerp(1.0f, vignette, vignetteStrength);

    PixelShaderOutput output;
    output.color = baseColor;
    return output;
}
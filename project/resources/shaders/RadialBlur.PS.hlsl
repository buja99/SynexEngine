#include "CopyImage.hlsli"

struct CopyPSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

// ------------------------------
// Constant Buffer: RadialBlur 설정값
// ------------------------------
cbuffer RadialBlurSettings : register(b0)
{
    float2 center; // 블러 중심 (0~1)
    float blurWidth; // 샘플 이동 거리
    int numSamples; // 샘플 수
}
Texture2D<float4> gTexture : register(t0);
SamplerState gSamplerLinear : register(s0);
// ------------------------------
// PixelShader Entry
// ------------------------------
float4 main(CopyPSInput input) : SV_TARGET
{
    float2 texcoord = input.texcoord;

    // 중심에서 현재 픽셀로 향하는 방향
    float2 dir = texcoord - center;

    // 누적 색상
    float3 color = float3(0.0, 0.0, 0.0);

    // 반복 샘플링
    for (int i = 0; i < numSamples; ++i)
    {
        float t = (float) i / (float) (numSamples - 1);
        float2 sampleUV = texcoord - dir * blurWidth * t;
        color += gTexture.Sample(gSamplerLinear, sampleUV).rgb;
    }

    // 평균화
    color /= numSamples;

    return float4(color, 1.0f);
}
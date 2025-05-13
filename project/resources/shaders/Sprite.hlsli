#ifndef SPRITE_HLSLI
#define SPRITE_HLSLI

// 정점 셰이더 출력 구조체
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

#endif // SPRITE_HLSLI
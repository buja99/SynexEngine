#ifndef SPRITE_HLSLI
#define SPRITE_HLSLI

/// スプライト用VS出力
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

#endif // SPRITE_HLSLI
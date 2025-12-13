
struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};
/// グレースケール用VS出力
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};
/// グレースケール用定数バッファ
cbuffer GrayscaleSettings : register(b0)
{
    float grayscaleStrength; // 0.0 ~ 1.0
};
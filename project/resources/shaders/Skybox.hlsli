struct VertexInput
{
    float4 position : POSITION;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
};

// RootParameter 0 → b2
cbuffer ViewProjection : register(b2)
{
    float4x4 world;
    float4x4 viewProjectionInverse;
};

// RootParameter 1 → t3
TextureCube gTexture : register(t3);
SamplerState gSampler : register(s0);
#include "Particle.hlsli"



struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};


struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

   
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);

   
    output.color = input.color * textureColor;

  
    if (output.color.a <= 0.1f)
    {
        discard;
    }

    return output;
    
}
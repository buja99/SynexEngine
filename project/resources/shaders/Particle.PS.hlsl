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
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    //output.color = gMaterial.color * textureColor;
    output.color = gMaterial.color * textureColor * input.color;
    //float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    //if (textureColor.a <= 0.5 || output.color.a == 0.0)
    //{
    //    discard;
    //}
    if (output.color.a == 0.0)
    {
        discard;
    }
   //if (gMaterial.enableLighting != 0)
   //{
   //    float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
   //    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
   //    //output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
   //    output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
   //    output.color.a = gMaterial.color.a * textureColor.a;
   //}
   //else
   //{
   //    output.color = gMaterial.color * textureColor;
   //}
    
    return output;
    
}
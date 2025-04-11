#include "Object3d.hlsli"



struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    int reflectModel;
};


struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
struct Camera
{
    float3 worldPosition;
};
struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
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
    
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float3 lightDirection, specular;
    float3 lightColor = gDirectionalLight.color.rgb;
    float lightIntensity = gDirectionalLight.intensity;
  
   
    
    
    if (gMaterial.enableLighting != 0)
    {
    
        if (gMaterial.reflectModel >= 2)
        {
        // Point Light
            lightDirection = normalize(gPointLight.position - input.worldPosition);
            lightColor = gPointLight.color.rgb;
            lightIntensity = gPointLight.intensity;
            
            float distance = length(gPointLight.position - input.worldPosition);
            float factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);
            
            lightIntensity *= factor;
        }
        else
        {
        // Directional Light
            lightDirection = -gDirectionalLight.direction;
        }

        if (gMaterial.reflectModel % 2 == 1)
        {
        // Blinn-Phong
            float3 halfVector = normalize(lightDirection + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            float specularPow = pow(saturate(NdotH), gMaterial.shininess);
            specular = lightColor * lightIntensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        else
        {
        // Phong
            float3 reflectLight = reflect(-lightDirection, normalize(input.normal));
            float RdotE = dot(reflectLight, toEye);
            float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            specular = lightColor * lightIntensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        }

        float NdotL = dot(normalize(input.normal), lightDirection);
        float cosTheta = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor * cosTheta * lightIntensity;

        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
    
        output.color = gMaterial.color * textureColor;
    }
   
   
    
    return output;
    
}
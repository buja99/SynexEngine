#include "Object3d.hlsli"



struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    
    int isBlinnPhong; // true: Blinn, false: Phong
    int useDirectionalLight; // 방향광 사용 여부
    int usePointLight; // 포인트광 사용 여부
    int useSpotLight; // 스팟광 사용 여부
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
struct SpotLight
{
    float4 color;      // 16B
    float3 position;   // 12B
    float intensity;   // 4B

    float3 direction;  // 12B
    float cutoff; // 내각 (코사인 기준)
    float outerCutoff; // 외각 (코사인 기준)

    float decay;
    float radius;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);
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

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);

    if (gMaterial.enableLighting != 0)
    {
        float3 normal = normalize(input.normal);

        // Directional Light
        if (gMaterial.useDirectionalLight != 0)
        {
            float3 lightDir = -normalize(gDirectionalLight.direction);
            float3 lightColor = gDirectionalLight.color.rgb;
            float lightIntensity = gDirectionalLight.intensity;

            float3 specular = 0;
            if (gMaterial.isBlinnPhong != 0)
            {
                float3 halfVector = normalize(lightDir + toEye);
                float NdotH = dot(normal, halfVector);
                specular = lightColor * lightIntensity * pow(saturate(NdotH), gMaterial.shininess);
            }
            else
            {
                float3 reflectVec = reflect(-lightDir, normal);
                float RdotE = dot(reflectVec, toEye);
                specular = lightColor * lightIntensity * pow(saturate(RdotE), gMaterial.shininess);
            }

            float NdotL = dot(normal, lightDir);
            float diffuseFactor = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor * diffuseFactor * lightIntensity;

            totalDiffuse += diffuse;
            totalSpecular += specular;
        }

        // Point Light
        if (gMaterial.usePointLight != 0)
        {
            float3 lightVec = gPointLight.position - input.worldPosition;
            float distance = length(lightVec);
            float3 lightDir = normalize(lightVec);

            float factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);
            float3 lightColor = gPointLight.color.rgb;
            float lightIntensity = gPointLight.intensity * factor;

            float3 specular = 0;
            if (gMaterial.isBlinnPhong != 0)
            {
                float3 halfVector = normalize(lightDir + toEye);
                float NdotH = dot(normal, halfVector);
                specular = lightColor * lightIntensity * pow(saturate(NdotH), gMaterial.shininess);
            }
            else
            {
                float3 reflectVec = reflect(-lightDir, normal);
                float RdotE = dot(reflectVec, toEye);
                specular = lightColor * lightIntensity * pow(saturate(RdotE), gMaterial.shininess);
            }

            float NdotL = dot(normal, lightDir);
            float diffuseFactor = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor * diffuseFactor * lightIntensity;

            totalDiffuse += diffuse;
            totalSpecular += specular;
        }

        // Spot Light
        if (gMaterial.useSpotLight != 0)
        {
            float3 lightVec = gSpotLight.position - input.worldPosition;
            float distance = length(lightVec);
            float3 lightDir = normalize(lightVec);

            float3 spotDir = normalize(gSpotLight.direction);
            float theta = dot(lightDir, -spotDir);

            float epsilon = gSpotLight.cutoff - gSpotLight.outerCutoff;
            float spotIntensity = saturate((theta - gSpotLight.outerCutoff) / epsilon);

            float decayFactor = pow(saturate(-distance / gSpotLight.radius + 1.0f), gSpotLight.decay);
            float3 lightColor = gSpotLight.color.rgb * gSpotLight.intensity * spotIntensity * decayFactor;

            float3 specular = 0;
            if (gMaterial.isBlinnPhong != 0)
            {
                float3 halfVector = normalize(lightDir + toEye);
                float NdotH = dot(normal, halfVector);
                specular = lightColor * pow(saturate(NdotH), gMaterial.shininess);
            }
            else
            {
                float3 reflectVec = reflect(-lightDir, normal);
                float RdotE = dot(reflectVec, toEye);
                specular = lightColor * pow(saturate(RdotE), gMaterial.shininess);
            }

            float NdotL = dot(normal, lightDir);
            float diffuseFactor = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor * diffuseFactor;

            totalDiffuse += diffuse;
            totalSpecular += specular;
        }

        output.color.rgb = totalDiffuse + totalSpecular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    return output;
}
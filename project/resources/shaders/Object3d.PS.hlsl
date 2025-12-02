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
    int useAmbientLight; // 앰비언트광 사용 여부
    int useAreaLight; // 면광원 사용 여부
    int useEnvironmentMap;
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

struct AmbientLight
{
    float4 color; // ambient 색상 (보통 약간 어두운 색)
};

struct AreaLight
{
    float4 color; // 조명 색상
    float3 position; // 중심 위치
    float intensity;

    float3 right; // 면의 가로 방향 벡터
    float halfWidth; // 반 가로 길이

    float3 up; // 면의 세로 방향 벡터
    float halfHeight; // 반 세로 길이
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);
ConstantBuffer<AmbientLight> gAmbientLight : register(b5);
ConstantBuffer<AreaLight> gAreaLight : register(b6);
cbuffer PlayerRange : register(b7)
{
    float3 playerWorldPos;
    float visibleRange;
};
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
TextureCube<float4> gEnvironmentTexture : register(t1);
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    //float dist = distance(input.worldPosition, playerWorldPos);
    //if (dist > visibleRange)
    //{
    //    discard;
    //}
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    //clip(textureColor.a - 0.5f);
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);
    float3 ambient = float3(0, 0, 0);
    
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
        // Ambient Light
        if (gMaterial.useAmbientLight != 0)
        {
            ambient = gAmbientLight.color.rgb * gMaterial.color.rgb * textureColor.rgb;
        }
        if (gMaterial.useAreaLight != 0)
        {
             // 조명 면 중심에서 픽셀까지 벡터
            float3 toPixel = input.worldPosition - gAreaLight.position;

            // 면 정규 방향 계산 (가로 x 세로)
            float3 normal = normalize(cross(gAreaLight.right, gAreaLight.up));

            // 조명이 뒤돌아있으면 조명 없음 처리
            if (dot(normal, toPixel) > 0)
            {
                float3 lightDir = -normalize(normal);
                float3 lightColor = gAreaLight.color.rgb * gAreaLight.intensity;

                float NdotL = dot(normalize(input.normal), lightDir);
                float diffuseFactor = pow(NdotL * 0.5f + 0.5f, 2.0f);

                float3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor * diffuseFactor;
                totalDiffuse += diffuse;
            }
        }
        output.color.rgb = totalDiffuse + totalSpecular + ambient;
        output.color.a = gMaterial.color.a * textureColor.a;
        
        if (gMaterial.useEnvironmentMap != 0)
        {
            float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            float3 reflectedVector = reflect(-toEye, normalize(input.normal));
            float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);

            output.color.rgb += environmentColor.rgb;
        }
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    return output;
}
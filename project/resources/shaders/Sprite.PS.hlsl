struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer Material : register(b0)
{
    float4 color;
    int enableLighting;
    float3 _padding0;
    float4x4 uvTransform;
};

cbuffer DirectionLight : register(b1)
{
    float4 lightColor;
    float3 lightDirection;
    float lightIntensity;
};

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float2 uv = mul(float4(input.texcoord, 0, 1), uvTransform).xy;
    float4 texColor = gTexture.Sample(gSampler, uv);

    if (enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -lightDirection);
        float diffuse = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = color * texColor * lightColor * diffuse * lightIntensity;
    }
    else
    {
        output.color = color * texColor;
    }

    return output;
}
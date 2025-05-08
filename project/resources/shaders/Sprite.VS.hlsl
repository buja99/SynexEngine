
cbuffer WorldMatrix : register(b0)
{
    matrix matWorld;
};
cbuffer MatrixBuffer : register(b2)
{
    matrix view;
    matrix projection;
};
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};



VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    float4 worldPos = mul(input.position, matWorld);
    float4 viewPos = mul(worldPos, view);
    output.position = mul(viewPos, projection);

    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) matWorld));

    return output;
}
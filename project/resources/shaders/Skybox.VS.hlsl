#include "Skybox.hlsli"

VertexShaderOutput main(VertexInput input)
{
    VertexShaderOutput output;

    float4 worldPos = mul(input.position, world);
    float4 viewDir = mul(worldPos, viewProjectionInverse);
    output.position = viewDir.xyww;

    output.texcoord = viewDir.xyz;
    return output;
}
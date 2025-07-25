#include "Skybox.hlsli"

VertexShaderOutput main(VertexInput input)
{
    VertexShaderOutput output;

    float4 worldPos = mul(input.position, world);
    float4 clipPos = mul(worldPos, viewProjectionInverse);
    output.position = clipPos.xyww;

    output.texcoord = worldPos.xyz;
    return output;
}
#include "Skybox.hlsli"

VertexShaderOutput main(VertexInput input)
{
    VertexShaderOutput output;

    float4 pos = input.position;
    output.position = mul(pos, viewProjectionInverse);
    output.position = output.position.xyww;

    output.texcoord = input.position.xyz;
    return output;
}
#include "Grayscale.hlsli"

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0f); // Fullscreen Quad
    output.texcoord = input.texcoord;
    return output;
}
#include "CopyImage.hlsli"
static const uint KNumVertex = 3;
static const float4 kPositions[KNumVertex] =
{
    { -1.0f, 1.0f, 0.0f, 1.0f },
    { 3.0f, 1.0f, 0.0f, 1.0f },
    { -1.0f, -3.0f, 0.0f, 1.0f },
};
static const float2 kTexcoords[KNumVertex] =
{
    {0.0f, 0.0f},
    { 2.0f, 0.0f },
    {0.0f, 2.0f}
};
VSOutput main(uint vertexId : SV_VertexID)
{
    VSOutput output;
    output.position = kPositions[vertexId];
    output.texcoord = kTexcoords[vertexId];
    return output;
}

#include "Particle.hlsli"


struct TransformtionMatrix
{
    
    float4x4 WVP;
};
ConstantBuffer<TransformtionMatrix> gTransformtiomMatrix : register(b0);


struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformtiomMatrix.WVP);
    output.texcoord = input.texcoord;
    return output;
}
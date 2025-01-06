#include "Particle.hlsli"


struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};
struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};


//TransformationMatrix gTransformationMatrices[10];
//ConstantBuffer<TransformationMatrix> gTransformtiomMatrix : register(b0); 

StructuredBuffer<ParticleForGPU> gPaticle : register(t0);



struct VertexShaderInput
{
    float3 position : POSITION0;
    float2 texcoord : TEXCOORD0;
   //float3 normal : NORMAL0;
    //float4 color : COLOR0;
};



VertexShaderOutput main(VertexShaderInput input,uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    // Particle position
    float4 particlePosition = gPaticle[instanceId].WVP[3]; // Extract translation component
    // Apply offset to create a quad
    output.position = mul(float4(input.position, 1.0f), gPaticle[instanceId].WVP) + particlePosition;
    output.texcoord = input.texcoord;
    output.color = gPaticle[instanceId].color;

    return output;
}
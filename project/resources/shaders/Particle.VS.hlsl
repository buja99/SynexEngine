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
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    //float4 color : COLOR0;
};



VertexShaderOutput main(VertexShaderInput input,uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gPaticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.normal, (float3x3) gPaticle[instanceId].World));
    output.color = gPaticle[instanceId].color;
    return output;
}
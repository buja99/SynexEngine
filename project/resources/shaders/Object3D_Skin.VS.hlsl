#include "Object3d.hlsli"

/// ワールド変換用定数バッファ
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformtiomMatrix : register(b0);

// Bone palette (GPU Skinning)
cbuffer BonePalette : register(b8)
{
    float4x4 gBones[256];
};
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint4 boneIndex : BLENDINDICES0;
    float4 boneWeight : BLENDWEIGHT0;
};



VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float wSum = input.boneWeight.x + input.boneWeight.y + input.boneWeight.z + input.boneWeight.w;

    float4 skinnedPos = input.position;
    float3 skinnedNormal = input.normal;

    if (wSum > 0.0f)
    {
        skinnedPos =
            mul(input.position, gBones[input.boneIndex.x]) * input.boneWeight.x +
            mul(input.position, gBones[input.boneIndex.y]) * input.boneWeight.y +
            mul(input.position, gBones[input.boneIndex.z]) * input.boneWeight.z +
            mul(input.position, gBones[input.boneIndex.w]) * input.boneWeight.w;

       
        float4 n = float4(input.normal, 0.0f);
        float3 nSkinned =
            mul(n, gBones[input.boneIndex.x]).xyz * input.boneWeight.x +
            mul(n, gBones[input.boneIndex.y]).xyz * input.boneWeight.y +
            mul(n, gBones[input.boneIndex.z]).xyz * input.boneWeight.z +
            mul(n, gBones[input.boneIndex.w]).xyz * input.boneWeight.w;

        skinnedNormal = nSkinned;
    }

    output.position = mul(skinnedPos, gTransformtiomMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinnedNormal, (float3x3) gTransformtiomMatrix.WorldInverseTranspose));
    output.worldPosition = mul(skinnedPos, gTransformtiomMatrix.World).xyz;

    return output;
}


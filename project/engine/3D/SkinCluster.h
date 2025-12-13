#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "MyMath.h"
#include "DirectXCommon.h"

using Microsoft::WRL::ComPtr;

class Skeleton;

/// <summary>スキニング用最終ボーン行列計算とGPU転送</summary>
class SkinCluster {

public:
    SkinCluster() = default;
    ~SkinCluster() = default;
    SkinCluster(const SkinCluster&) = delete;
    SkinCluster& operator=(const SkinCluster&) = delete;

    /// <summary>スキニング用ボーン行列バッファ初期化</summary>
    void Initialize(DirectXCommon* dxCommon, size_t boneCount);

    /// <summary>Skeletonから最終ボーン行列更新</summary>
    void Update(const Skeleton& skeleton);
	// GPU 주소 / 리소스 반환
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;
    ComPtr<ID3D12Resource> GetResource() const;
	//정보 반환
    uint32_t GetBoneCount() const { return boneCount_; }
    const std::vector<Matrix4x4>& GetFinalMatricesCPU() const { return finalMatrices_; }

private:

    ID3D12Device* device = nullptr;
    DirectXCommon* dxCommon_ = nullptr;

    uint32_t boneCount_ = 0;
    // CPU 캐시: 최종 본 행렬 (BoneWorld * InverseBind)
    std::vector<Matrix4x4> finalMatrices_;

    // GPU 업로드 버퍼(CBV 용)
    Microsoft::WRL::ComPtr<ID3D12Resource> boneMatrixBuffer_;
    Matrix4x4* mappedMatrices_ = nullptr; // 업로드 힙 상시 매핑 포인터

    static ComPtr<ID3D12Resource> CreateUploadBuffer(
        ID3D12Device* device,
        size_t sizeInBytes,
        const wchar_t* name = L"SkinClusterBuffer"
    );

};


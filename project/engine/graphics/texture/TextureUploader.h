#include <wrl.h>
#include <d3d12.h>
#include <DirectXTex.h>
#pragma once

using Microsoft::WRL::ComPtr;


// SRV 생성 정보까지 포함한 업로드
struct UploadResult {
    ComPtr<ID3D12Resource> texture;               // 업로드된 텍스처
    ComPtr<ID3D12Resource> intermediate;          // 중간 버퍼
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;      // SRV 설정 정보
    DXGI_FORMAT format;                           // 텍스처 포맷
    bool isCubemap = false;                       // Cubemap 여부
};

class TextureUploader {
public:
    static ComPtr<ID3D12Resource> UploadTexture(
        ComPtr<ID3D12Device> device,
        ID3D12GraphicsCommandList* commandList,
        const DirectX::ScratchImage& mipImages,
        ComPtr<ID3D12Resource>& intermediateBufferOut // 중간버퍼 반환
    );
    

    // Skybox 등에서 사용할 Upload + SRV 정보 제공
    static UploadResult UploadAndDescribe(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* commandList,
        const DirectX::ScratchImage& mipImages
    );

    static ComPtr<ID3D12Resource> UploadAndWait(
        ComPtr<ID3D12Device> device,
        ComPtr<ID3D12CommandQueue> commandQueue,
        const DirectX::ScratchImage& mipImages
    );
};
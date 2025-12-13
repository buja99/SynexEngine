#include <wrl.h>
#include <d3d12.h>
#include <DirectXTex.h>
#pragma once

using Microsoft::WRL::ComPtr;


/// <summary>テクスチャアップロード結果一式</summary>
struct UploadResult {
    ComPtr<ID3D12Resource> texture;               // 업로드된 텍스처
    ComPtr<ID3D12Resource> intermediate;          // 중간 버퍼
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;      // SRV 설정 정보
    DXGI_FORMAT format;                           // 텍스처 포맷
    bool isCubemap = false;                       // Cubemap 여부
};


/// <summary>DirectX12テクスチャアップロードと関連リソース生成</summary>
class TextureUploader {

public:

    /// <summary>コマンドリストへ転送コマンド登録とGPUテクスチャ生成</summary>
    static ComPtr<ID3D12Resource> UploadTexture(
        ComPtr<ID3D12Device> device,
        ID3D12GraphicsCommandList* commandList,
        const DirectX::ScratchImage& mipImages,
        ComPtr<ID3D12Resource>& intermediateBufferOut // 중간버퍼 반환
    );
    

    /// <summary>アップロード結果とSRV定義生成</summary>
    static UploadResult UploadAndDescribe(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* commandList,
        const DirectX::ScratchImage& mipImages
    );

    /// <summary>アップロード実行とフェンス待機</summary>
    static ComPtr<ID3D12Resource> UploadAndWait(
        ComPtr<ID3D12Device> device,
        ComPtr<ID3D12CommandQueue> commandQueue,
        const DirectX::ScratchImage& mipImages
    );
};
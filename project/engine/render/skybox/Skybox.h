#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <DirectXMath.h>
#include "DirectXCommon.h"
#include "Matrix4x4.h"
#include "VertexData.h"
#include "TextureUploader.h"
#include "SrvManager.h"

using Microsoft::WRL::ComPtr;

struct VertexPos {
    Vector4 position;
};
struct ConstBufferData {
    Matrix4x4 world;
    Matrix4x4 viewProjectionInverse;
};
class Skybox {
public:
    void Initialize(DirectXCommon* dxCommon, const std::wstring& ddsFilePath);
    void Draw(const Matrix4x4& view, const Matrix4x4& projection);
    void Finalize();

private:
    void CreateVertexBuffer();
    void CreatePipeline();
    void CreateRootSignature();

private:
    DirectXCommon* dxCommon_ = nullptr;

    ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbView_{};

    ComPtr<ID3D12RootSignature> rootSignature_;
    ComPtr<ID3D12PipelineState> pipelineState_;

    ComPtr<ID3D12Resource> constantBuffer_;
    ConstBufferData* mappedMatrix_ = nullptr;

    ComPtr<ID3D12Resource> cubemapTexture_;
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};
    uint32_t srvIndex_ = -1;

    std::vector<VertexPos> vertices_;
    VertexPos* vertexMapped_ = nullptr;

   
    
};

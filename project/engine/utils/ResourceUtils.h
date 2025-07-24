#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <DirectXTex.h>
using Microsoft::WRL::ComPtr;
namespace ResourceUtils {


	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device,size_t sizeInBytes);
	ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
};


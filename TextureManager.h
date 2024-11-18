#pragma once
#include <string>
#include <wrl.h>
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include "DirectXCommon.h"
#include <map>

using Microsoft::WRL::ComPtr;

class TextureManager
{
public:

	static TextureManager* GetInstance();

	void Initialize(DirectXCommon* dxCommon);

	void LoadTexture(const std::string& filePath);

	void Finalize();

	const DirectX::ScratchImage& GetTextureImage(uint32_t textureIndex) const;

	//uint32_t LoadTextureByPath(const std::string& textureFilePath);
	//uint32_t GetOrLoadTextureIndex(const std::string& textureFilePath);
	//void ChangeTexture(uint32_t textureIndex, const std::string& newTextureFilePath);

	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	uint32_t GetTextureIndexByFilepath(const std::string& filePath);


private:

	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	static uint32_t kSRVIndexTop;

	std::map<std::string, uint32_t> textureIndexMap;


	DirectXCommon* dxCommon_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	uint32_t descriptorSize_ = 0;
	
	

private:

	struct TextureData {

		//TextureData(const TextureData&) = delete; 
		//TextureData& operator=(const TextureData&) = delete; 

		std::string filePath;
		DirectX::TexMetadata metadata;
		ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
		DirectX::ScratchImage image;
	};

	std::vector<TextureData> textureDatas;

};


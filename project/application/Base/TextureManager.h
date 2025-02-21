#pragma once
#include <string>
#include <wrl.h>
#include "DirectXTex.h"
#include <d3d12.h>
#include "DirectXCommon.h"
#include <map>
#include "SrvManager.h"
#include <unordered_map>

using Microsoft::WRL::ComPtr;

class TextureManager
{
public:

	static uint32_t kSRVIndexTop;

	static TextureManager* GetInstance();

	void Initialize(DirectXCommon* dxCommon,SrvManager* srvManager);

	void LoadTexture(const std::string& filePath);

	void Finalize();

	//const DirectX::ScratchImage& GetTextureImage(uint32_t srvIndex) const;

	

	//uint32_t LoadTextureByPath(const std::string& textureFilePath);
	//uint32_t GetOrLoadTextureIndex(const std::string& textureFilePath);
	//void ChangeTexture(uint32_t textureIndex, const std::string& newTextureFilePath);

	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	uint32_t GetTextureIndexByFilepath(const std::string& filePath);

	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);



private:

	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;


	std::map<std::string, uint32_t> textureIndexMap;


	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	uint32_t descriptorSize_ = 0;
	
	

private:

	struct TextureData {

		//TextureData(const TextureData&) = delete; 
		//TextureData& operator=(const TextureData&) = delete; 

		std::string filePath;
		DirectX::TexMetadata metadata;
		ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
		DirectX::ScratchImage image;
	};

	//std::vector<TextureData> textureDatas;
	std::unordered_map<std::string, TextureData> textureDatas;
};


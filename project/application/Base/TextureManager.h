#pragma once
#include <string>
#include <wrl.h>
#include "DirectXTex.h"
#include <d3d12.h>
#include "DirectXCommon.h"
#include <map>
#include "SrvManager.h"
#include <unordered_map>
#include "TextureUploader.h"
using Microsoft::WRL::ComPtr;


/// <summary>
/// テクスチャの読み込み、SRV 作成、メタデータ管理を一括で行うマネージャークラス。
/// 同じファイルの重複読み込みを防ぎ、GPU リソースと SRV を自動的に管理する。
/// </summary>
class TextureManager
{
public:

	static uint32_t kSRVIndexTop;

	static TextureManager* GetInstance();

	void Initialize(DirectXCommon* dxCommon,SrvManager* srvManager);

	void LoadTexture(const std::string& filePath);

	void LoadTextureDDS(const std::string& filePath, bool isCubeMap = false);

	uint32_t LoadCubemap(const std::wstring& filePath);

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

	static std::unique_ptr<TextureManager> instance;

	friend std::unique_ptr<TextureManager> std::make_unique<TextureManager>();
	friend std::default_delete<TextureManager>;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;


	std::map<std::string, uint32_t> textureIndexMap;


	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	SrvManager* srvManager_ = nullptr;
	ID3D12DescriptorHeap* srvDescriptorHeap_;
	uint32_t descriptorSize_ = 0;
	
	

private:

	struct TextureData {
		DirectX::TexMetadata metadata;
		ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	//std::vector<TextureData> textureDatas;
	std::unordered_map<std::string, TextureData> textureDatas;
};


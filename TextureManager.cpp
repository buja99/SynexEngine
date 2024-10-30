#include "TextureManager.h"
#include "DirectXCommon.h"
#include "StringUtility.h"

using namespace StringUtility;

TextureManager* TextureManager::instance = nullptr;

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
   }
	return instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{

	textureDatas.reserve(DirectXCommon::kMaxSRVCount);

	dxCommon_ = dxCommon;

}

void TextureManager::LoadTexture(const std::string& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) { return textureData.filePath == filePath; }
	);

	if (it != textureDatas.end()) {

		return;
	}

	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);



	textureDatas.resize(textureDatas.size() + 1);
	TextureData& textureData = textureDatas.back();

	textureData.filePath = filePath;
	textureData.metadata = image.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);

	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	textureData.srvHandleCPU = GetCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = GetGPUDescriptorHandle(srvIndex);



	DirectX::ScratchImage mipImages{};
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMap
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);

		HRESULT hr = textureData.resource->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}


	assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);


}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
	// Heap設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// Resource生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	uint32_t descriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	uint32_t descriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{

	assert(textureIndex < textureDatas.size());
	TextureData& textureData = textureDatas[textureIndex];

	return textureData.srvHandleGPU;
}

uint32_t TextureManager::GetTextureIndexByFilepath(const std::string& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) { return textureData.filePath == filePath; }
	);

	if (it != textureDatas.end()) {
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}
	assert(0);
	return 0;
}

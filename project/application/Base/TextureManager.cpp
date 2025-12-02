#include "TextureManager.h"
#include "DirectXCommon.h"
#include "StringUtility.h"
#include <cassert>


using namespace StringUtility;

std::unique_ptr<TextureManager> TextureManager::instance = nullptr;

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = std::make_unique<TextureManager>();
	}
	return instance.get();
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{

	textureDatas.reserve(DirectXCommon::kMaxSRVCount);

	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
	descriptorSize_ = dxCommon_->GetDescriptorSizeSRV();
	srvDescriptorHeap_ = srvManager_->GetSrvDescriptorHeap();

}

void TextureManager::LoadTexture(const std::string& filePath)
{
	//OutputDebugStringA(("Attempting to load texture from file path: " + filePath + "\n").c_str());

	if (textureDatas.contains(filePath)) {
		return; // Textures already loadedS
	}

	assert(srvManager_->CanAllocate());

	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to load texture from WIC file. HRESULT: ");
		OutputDebugStringA(std::to_string(hr).c_str());
		OutputDebugStringA("\n");
		assert(SUCCEEDED(hr));
	}
	//assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	

	TextureData& textureData = textureDatas[filePath];
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);

	
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRV記述子の設定
	srvManager_->CreatSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(),
		textureData.metadata.format, UINT(textureData.metadata.mipLevels));



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

	
	textureDatas[filePath] = std::move(textureData);
	assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);
	
}



void TextureManager::LoadTextureDDS(const std::string& filePath, bool isCubeMap) {

	if (textureDatas.contains(filePath)) {
		return; // 이미 로드된 경우
	}

	assert(srvManager_->CanAllocate());

	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage mipImages{};

	// DDS 로드
	HRESULT hr = DirectX::LoadFromDDSFile(
		ConvertString(filePath).c_str(),
		DirectX::DDS_FLAGS_NONE, &metadata, mipImages);
	if (FAILED(hr)) {
		OutputDebugStringA(("Failed to load DDS: " + filePath + "\n").c_str());
		return;
	}
	assert(SUCCEEDED(hr));
	// GPU 리소스 생성
	TextureData textureData{};
	textureData.metadata = metadata;
	textureData.resource = TextureUploader::UploadAndWait(
		dxCommon_->GetDevice().Get(),
		dxCommon_->GetCommandQueue().Get(),
		mipImages
	);

	// SRV 할당
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRV 생성
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = isCubeMap ? D3D12_SRV_DIMENSION_TEXTURECUBE
		: D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = UINT(metadata.mipLevels);
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	dxCommon_->GetDevice()->CreateShaderResourceView(
		textureData.resource.Get(),
		&srvDesc,
		textureData.srvHandleCPU
	);

	textureDatas[filePath] = std::move(textureData);
}

void TextureManager::Finalize()
{
	
	for (auto& pair : textureDatas) {
		pair.second.resource.Reset();  // ComPtr<ID3D12Resource> 해제
	}
	textureDatas.clear();

	instance.reset();
	
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
	ComPtr<ID3D12Resource> resource = nullptr;
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


uint32_t TextureManager::LoadCubemap(const std::wstring& filePath) {
	// DDS 로드
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage mipImages{};
	HRESULT hr = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, mipImages);
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to load cubemap DDS\n");
		return UINT32_MAX;
	} else {
		std::string msg = "[TextureManager] DDS load success: " + StringUtility::ConvertString(filePath) + "\n";
		OutputDebugStringA(msg.c_str());
	}

	// GPU 업로드
	ComPtr<ID3D12Resource> cubemap = TextureUploader::UploadAndWait(
		dxCommon_->GetDevice().Get(),
		dxCommon_->GetCommandQueue().Get(),
		mipImages
	);

	// SRV 생성
	uint32_t index = srvManager_->Allocate();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = UINT(metadata.mipLevels);
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	dxCommon_->GetDevice()->CreateShaderResourceView(
		cubemap.Get(), &srvDesc, srvManager_->GetCPUDescriptorHandle(index));

	TextureData textureData{};
	textureData.metadata = metadata;
	textureData.resource = cubemap;
	textureData.srvIndex = index;
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(index);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(index);

	std::string key = StringUtility::ConvertString(filePath);
	textureDatas[key] = std::move(textureData);

	// === 디버그 출력 추가 ===
	std::string msg2 = "[TextureManager] Registered cubemap: " + key +
		" (srvIndex=" + std::to_string(index) + ")\n";
	OutputDebugStringA(msg2.c_str());

	return index;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{

	assert(textureDatas.contains(filePath));
	return textureDatas[filePath].srvHandleGPU;
}

uint32_t TextureManager::GetTextureIndexByFilepath(const std::string& filePath)
{
	
	assert(textureDatas.contains(filePath));
	return textureDatas[filePath].srvIndex;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	assert(textureDatas.contains(filePath));
	return textureDatas[filePath].metadata;
}

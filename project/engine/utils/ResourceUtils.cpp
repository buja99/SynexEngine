#include "ResourceUtils.h"
#include "Logger.h"
#include "StringUtility.h"

using namespace Logger;
using namespace StringUtility;
namespace ResourceUtils {
	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
		//頂点Heap
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		//頂点Resource
		D3D12_RESOURCE_DESC vertexResourceDesc{};
		vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertexResourceDesc.Width = sizeInBytes;

		vertexResourceDesc.Height = 1;
		vertexResourceDesc.DepthOrArraySize = 1;
		vertexResourceDesc.MipLevels = 1;
		vertexResourceDesc.SampleDesc.Count = 1;

		vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ComPtr<ID3D12Resource> vertexResource = nullptr;
		HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&vertexResource));
		assert(SUCCEEDED(hr));
		vertexResource->SetName(L"DirectXCommonBufferResource");

		return vertexResource;
	}

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
		ComPtr<ID3D12Resource> buffer;

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = sizeInBytes;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		HRESULT hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&buffer)
		);

		if (FAILED(hr)) {
			OutputDebugStringA("Failed to create upload buffer.");
		}

		return buffer;
	
	}

	ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {
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
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));
		resource->SetName(L"textureResource");

		return resource;
	}

	ComPtr<ID3D12Resource> UploadTextureData(ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages) {
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		//全MipMap
		for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
			const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);

			HRESULT hr = texture.Get()->WriteToSubresource(
				UINT(mipLevel),
				nullptr,
				img->pixels,
				UINT(img->rowPitch),
				UINT(img->slicePitch)
			);
			assert(SUCCEEDED(hr));
		}
		return texture;
	}

	DirectX::ScratchImage LoadTexture(const std::string& filePath) {
		DirectX::ScratchImage image{};
		std::wstring filePathW = ConvertString(filePath);
		HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

		assert(SUCCEEDED(hr));

		DirectX::ScratchImage mipImages{};
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));


		return mipImages;
	}
}
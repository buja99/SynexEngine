#include "ParticleManager.h"
#include "Logger.h"
#include "StringUtility.h"
#include <Model.h>


using namespace Logger;
using namespace StringUtility;

void ParticleManager::Initialize(DirectXCommon* directXCommon)
{
}

void ParticleManager::Update()
{
}

void ParticleManager::Draw()
{
}

void ParticleManager::Emit(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, float size, float lifetime)
{
}

ComPtr<ID3D12Resource> ParticleManager::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes)
{
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
	vertexResource->SetName(L"bufferResource");

	return vertexResource;
}

void ParticleManager::CreateVertexBuffer()
{
	const size_t kNumMaxParticles = 1000;
	const size_t vertexBufferSize = sizeof(VertexData) * kNumMaxParticles;

	vertexBufferResource_ = CreateBufferResource(dxCommon_->GetDevice(), vertexBufferSize);


	vertexBufferView.BufferLocation = vertexBufferResource_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexBufferView_ = vertexBufferView;
}

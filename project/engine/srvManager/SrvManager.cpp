#include "SrvManager.h"

const uint32_t SrvManager::kMaxSRVCount = 512;


void SrvManager::Initialize(DirectXCommon* dxCommon)
{
	this->directXCommon = dxCommon;

	if (!directXCommon) {
		OutputDebugStringA("ERROR: SrvManager::Initialize() - directXCommon is null!\n");
		return;
	}

	srvDescriptorHeap = dxCommon->CreateDescriptorHeap(dxCommon->GetDevice(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);

	if (!srvDescriptorHeap) {
		OutputDebugStringA("ERROR: SrvManager::Initialize() - Failed to create descriptor heap!\n");
		return;
	}
	srvDescriptorHeap->SetName(L"SRVManagerSrvHeap");

	descriptorSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

uint32_t SrvManager::Allocate()
{

	assert(useIndex < kMaxSRVCount);

	int index = useIndex;

	useIndex++;

	return index;


}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
{
	if (!srvDescriptorHeap) {
		OutputDebugStringA("ERROR: SrvManager::GetCPUDescriptorHandle() - srvDescriptorHeap is null!\n");
		return {}; // 빈 핸들 반환
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
{
	if (!srvDescriptorHeap) {
		OutputDebugStringA("ERROR: SrvManager::GetGPUDescriptorHandle() - srvDescriptorHeap is null!\n");
		return {}; // 빈 핸들 반환
	}

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;

}

void SrvManager::CreatSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	directXCommon->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));

}

void SrvManager::CreatSRVforStruturedBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured buffers have no format
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	directXCommon->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::PreDraw()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
	directXCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

}

void SrvManager::SetGraphicsRootDesciptorTable(UINT RootPameterIndex, uint32_t srvIndex)
{
	directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(RootPameterIndex, GetGPUDescriptorHandle(srvIndex));
}

bool SrvManager::CanAllocate() const
{
	return useIndex < kMaxSRVCount;
}

void SrvManager::Finalize()
{
	srvDescriptorHeap.Reset();

}

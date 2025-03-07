#pragma once
#include "DirectXCommon.h"
#include <wrl.h>
#include "MyMath.h"
#include <math.h>
using Microsoft::WRL::ComPtr;
class SrvManager
{

public:
	static SrvManager* GetInstance() {
		static SrvManager instance; 
		return &instance;
	}
	void Initialize(DirectXCommon* dxCommon);

	uint32_t Allocate();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap.Get(); }

	void CreatSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	void CreatSRVforStruturedBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	void PreDraw();
	void SetGraphicsRootDesciptorTable(UINT RootPameterIndex, uint32_t srvIndex);
	bool CanAllocate() const;
	bool IsValid() const { return srvDescriptorHeap != nullptr; }
	void Finalize();


private:

	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(const SrvManager&) = delete;            
	SrvManager& operator=(const SrvManager&) = delete; 

	DirectXCommon* directXCommon = nullptr;

	static const uint32_t kMaxSRVCount;
	uint32_t descriptorSize;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	//ComPtr<ID3D12Device> device;
	uint32_t useIndex = 0;

	MyMath* myMath = new MyMath;

};


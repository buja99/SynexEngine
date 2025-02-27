#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include "WinApp.h"
#include <dxcapi.h>
#include "DirectXTex.h"
#include <string>
#include "FPSLimiter.h"

using Microsoft::WRL::ComPtr;

class DirectXCommon
{

public:

	static DirectXCommon* GetInstance();

	~DirectXCommon();

	void Initialize(WinApp* winApp);

	void Device();

	void Command();

	void SwapChain();

	void CreateDepthBuffer();

	void CreateDescriptorHeaps();

	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ComPtr <ID3D12Device> device, int32_t width, int32_t height);
	ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(
	ComPtr <ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


	void InitializeRTV();
	void InitializeDSV();
	void InitializeFence();
	void InitializeViewport();
	void InitializeScissor();
	void InitializeDXCCompiler();
//#ifdef _DEBUG
//	void InitializeImGui();
//#endif // _DEBUG
	void InitializePSO();

	void PreDraw();
	void PostDraw();

	//D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index) const;
	//D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index) const;

	IDxcBlob* CompileShader(

		const std::wstring& filePath,

		const wchar_t* profile,

		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	
	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
	ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	ComPtr<ID3D12Device> GetDevice() const { return device; }
	//ComPtr <ID3D12DescriptorHeap> GetDescriptorHeap() const { return descriptorHeap; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList; }
	void Cleanup();
	ComPtr<ID3D12PipelineState> GetGraphicsPipelineState() const { return graphicsPipelineState; }

	void UploadTextureDate(ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	
	//ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() const { return srvDescriptorHeap; }
	int GetSwapChainResourcesNum() const { return swapChainDesc.BufferCount; }
	uint32_t GetDescriptorSizeSRV() const { return descriptorSizeSRV; }

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static const uint32_t kMaxSRVCount;

	ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue; }

private:

	DirectXCommon() = default;
	DirectXCommon(DirectXCommon&) = delete;
	const DirectXCommon& operator=(DirectXCommon&) = delete;

	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	ComPtr<ID3D12Resource> depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> swapChainBuffers[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	UINT rtvDescriptorSize;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	std::array<ComPtr<ID3D12Resource>, 2> swapChainResources;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	//ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	ComPtr<ID3D12Fence> fence;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;
	UINT64 fenceVal = 0;

	//IDxcUtils* dxcUtils = nullptr;
	//IDxcCompiler3* dxcCompiler = nullptr;
	//IDxcIncludeHandler* includeHandler = nullptr;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	
	

	WinApp* winApp = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> graphicsPipelineState;
	
	
	FPSLimiter* fpsLimiter = nullptr;

};


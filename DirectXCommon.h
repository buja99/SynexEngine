#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include "WinApp.h"
#include "ResourceObject.h"
#include <dxcapi.h>
#include "externals/DirectXTex/DirectXTex.h"
#include <string>

using Microsoft::WRL::ComPtr;

class DirectXCommon
{

public:
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
	void InitializeImGui();

	void PreDraw();
	void PostDraw();

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index) const;

	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);
	ResourceObject CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);



	void Cleanup();

private:

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
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	ComPtr<ID3D12Fence> fence;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;
	UINT64 fenceVal = 0;

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	WinApp* winApp = nullptr;

};


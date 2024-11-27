#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;


class SpriteCommon
{


public:

	void Initialize(DirectXCommon* dxCommon);

	void CommonDrawSettings();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	IDxcBlob* CompileShader(

		const std::wstring& filePath,

		const wchar_t* profile,

		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	void UploadTextureDate(ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	ComPtr<ID3D12Device> GetDevice() const { return device; }

	ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(
		ComPtr <ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
	ComPtr<ID3D12PipelineState> GetGraphicsPipelineState() const { return graphicsPipelineState; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList; }
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

private:

	void CreateRootSignature();
	void CreateGraphicsPipeline();

	ComPtr<ID3D12Device> device;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> graphicsPipelineState;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	DirectXCommon* dxCommon_ = nullptr;
	
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	

};


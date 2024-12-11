#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include <wrl.h>
#include <random>
#include "VertexData.h"
#include "Material.h"

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

using Microsoft::WRL::ComPtr;
class ParticleManager
{

public:
	void Initialize(DirectXCommon* directXCommon,SrvManager* srvManager, std::mt19937* randomEngine);


	void CommonDrawSettings();
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void InitializeVertexBuffer();
	void UpdateVertexBuffer();

	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
private:
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	std::mt19937* randomEngine_ = nullptr;

	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> graphicsPipelineState;

	ComPtr<ID3D12Resource> vertexBufferResource_; 
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;  
};


#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Camera.h"
using Microsoft::WRL::ComPtr;


class Object3dCommon
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

	ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList; }
	ComPtr<ID3D12PipelineState> GetGraphicsPipelineState() const { return graphicsPipelineState; }

	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	Camera* GetDefaultCamera() const { return defaultCamera; }

	//void SetViewProjectionMatrix(const Matrix4x4& matrix) { viewProjectionMatrix_ = matrix; }
	
	//const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
private:

	void CreateRootSignature();
	void CreateGraphicsPipeline();

	ComPtr<ID3D12Device> device;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> graphicsPipelineState;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	Camera* defaultCamera = nullptr;

	//Matrix4x4 viewProjectionMatrix_;
};


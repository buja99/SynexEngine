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

	static Object3dCommon* GetInstance() {
		static Object3dCommon instance;
		return &instance;
	}

	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

	void CommonDrawSettings();

	void stencilMaskSettings();

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

private:

	Object3dCommon() = default;
	~Object3dCommon() = default;
	Object3dCommon(const Object3dCommon&) = delete;
	Object3dCommon& operator=(const Object3dCommon&) = delete;

	void CreateRootSignature();
	void CreateGraphicsPipeline();

	void CreateStencilWritePipeline(); // 스텐실 값 기록용
	void CreateStencilTestPipeline();  // 값이 1인 경우에만 통과

	ComPtr<ID3D12Device> device;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> graphicsPipelineState;

	ComPtr<ID3D12PipelineState> stencilMaskPipelineState_;
	ComPtr<ID3D12RootSignature> stencilMaskRootSignature_;
	ComPtr<ID3D12PipelineState> stencilTestPipelineState_; // EQUAL 조건용

	ComPtr<ID3D12Resource> stencilMaskVertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW stencilMaskVBView_{};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	Camera* defaultCamera = nullptr;

	

};


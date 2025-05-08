#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Matrix4x4.h"
#include "Lighting.h"


using Microsoft::WRL::ComPtr;

struct ConstBufferData {
	Matrix4x4 mat;
};
struct SpriteMatrix {
	Matrix4x4 view;
	Matrix4x4 projection;
};
class SpriteCommon
{


public:

	static SpriteCommon* GetInstance() {
		static SpriteCommon instance;
		return &instance;
	}

	void Initialize(DirectXCommon* dxCommon);
	void SetBackgroundView();
	void SetForegroundView();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }


private:

	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(const SpriteCommon&) = delete;
	SpriteCommon& operator=(const SpriteCommon&) = delete;

	void CreateRootSignature();
	void CreateGraphicsPipeline(bool background);

	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> psoBackground_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> psoForeground_;
	Matrix4x4 viewMatrix_ = MyMath::MakeIdentity4x4();
	Matrix4x4 projectionMatrix_ = MyMath::MakeOrthographicMatrix(0.0f, 0.0f, WinApp::kClientWidth, WinApp::kClientHeight, -100.0f, 100.0f);
};


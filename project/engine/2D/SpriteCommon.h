#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Matrix4x4.h"
#include "Lighting.h"
#include "MyMath.h"

using Microsoft::WRL::ComPtr;

enum class DrawMode { Background, Foreground };

class SpriteCommon
{


public:

	static SpriteCommon* GetInstance() {
		static SpriteCommon instance;
		return &instance;
	}

	void Initialize(DirectXCommon* dxCommon);
	void SetUIPipeline();         // UI용 (DepthWrite 없음)
	void Set3DOverlayPipeline();  // 3D 위에 Sprite 덮기용 (DepthWrite 있음)


	void Finalize();

	void SetDrawMode(DrawMode mode) { currentDrawMode_ = mode; }
	bool IsBackground() const { return currentDrawMode_ == DrawMode::Background; }
	DrawMode GetDrawMode() const { return currentDrawMode_; }

	DirectXCommon* GetDxCommon() const { return dxCommon_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	ComPtr<ID3D12Device> GetDevice() const { return dxCommon_->GetDevice(); }

private:

	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(const SpriteCommon&) = delete;
	SpriteCommon& operator=(const SpriteCommon&) = delete;

	void CreateRootSignature();
	void CreatePipelineStateForUI();
	void CreatePipelineStateFor3DOverlay();
	//void CreateGraphicsPipeline(bool background);

	DirectXCommon* dxCommon_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> psoUI_;
	ComPtr<ID3D12PipelineState> pso3DOverlay_;
	Matrix4x4 viewMatrix_ = MyMath::MakeIdentity4x4();
	Matrix4x4 projectionMatrix_ = MyMath::MakeOrthographicMatrix(0.0f, 0.0f, WinApp::kClientWidth, WinApp::kClientHeight, -100.0f, 100.0f);

	DrawMode currentDrawMode_ = DrawMode::Foreground;
};


#pragma once
#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#endif // _DEBUG
#include"WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

/// <summary>
/// ImGui の初期化、フレーム管理、描画処理を行うデバッグ用マネージャークラス
/// _DEBUG ビルド時のみ有効になり、DirectX12 と Win32 の ImGui 実装を制御
/// </summary>
class ImGuiManager {
public:
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	void BeginFrame();
	void EndFrame();
	void Draw();
	void Finalize();
private:
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
};


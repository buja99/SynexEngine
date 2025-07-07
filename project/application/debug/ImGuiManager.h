#pragma once
#ifdef _DEBUG
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif // _DEBUG
#include"WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

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


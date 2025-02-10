#pragma once
#ifdef _DEBUG
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif // _DEBUG
#include"WinApp.h"

class ImGuiManager {

	void Initialize(WinApp* winApp);


};


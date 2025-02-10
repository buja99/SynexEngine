#include "ImGuiManager.h"

void ImGuiManager::Initialize(WinApp* winApp) {

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());

}

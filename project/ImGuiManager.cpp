#include "ImGuiManager.h"
#include "SrvManager.h"
#include <iostream>

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {

    dxCommon_ = dxCommon;
    srvManager_ = SrvManager::GetInstance();

  
    uint32_t index = srvManager_->Allocate();
#ifdef _DEBUG
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
#endif // _DEBUG
    //ImGui_ImplDX12_Init(
    //    dxCommon_->GetDevice().Get(),
    //    dxCommon_->GetSwapChainResourcesNum(),
    //    DXGI_FORMAT_R8G8B8A8_UNORM,
    //    srvManager_->GetSrvDescriptorHeap().Get(),
    //    srvManager_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
    //    srvManager_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
    //);
#ifdef _DEBUG
    ImGui_ImplDX12_InitInfo initInfo = {};
    initInfo.Device = dxCommon_->GetDevice().Get();
    initInfo.CommandQueue = dxCommon_->GetCommandQueue().Get();
    initInfo.NumFramesInFlight = dxCommon_->GetSwapChainResourcesNum();
    initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    initInfo.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    initInfo.SrvDescriptorHeap = srvManager_->GetSrvDescriptorHeap();
    //srvManager_->Allocate();
    initInfo.LegacySingleSrvCpuDescriptor = srvManager_->GetCPUDescriptorHandle(index);
    initInfo.LegacySingleSrvGpuDescriptor = srvManager_->GetGPUDescriptorHandle(index);

    ImGui_ImplDX12_Init(&initInfo);
#endif // _DEBUG
}

void ImGuiManager::BeginFrame() {
#ifdef _DEBUG
    ImGui_ImplDX12_NewFrame(); 
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif // _DEBUG
}

void ImGuiManager::EndFrame() {
#ifdef _DEBUG
    ImGui::Render();
#endif // _DEBUG
}

void ImGuiManager::Draw() {
    //ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList().Get();
    //ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetSrvDescriptorHeap() };
    //commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
#ifdef _DEBUG
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList().Get());
#endif // _DEBUG
}

void ImGuiManager::Finalize() {
#ifdef _DEBUG
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif // _DEBUG
    std::cout << "ImGuiManager: Finalized." << std::endl;
}

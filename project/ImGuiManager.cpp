#include "ImGuiManager.h"
#include "SrvManager.h"

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {

    dxCommon_ = dxCommon;
    srvManager_ = SrvManager::GetInstance();

  
    uint32_t index = srvManager_->Allocate();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());

    //ImGui_ImplDX12_Init(
    //    dxCommon_->GetDevice().Get(),
    //    dxCommon_->GetSwapChainResourcesNum(),
    //    DXGI_FORMAT_R8G8B8A8_UNORM,
    //    srvManager_->GetSrvDescriptorHeap().Get(),
    //    srvManager_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
    //    srvManager_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
    //);
    ImGui_ImplDX12_InitInfo initInfo = {};
    initInfo.Device = dxCommon_->GetDevice().Get();
    initInfo.CommandQueue = dxCommon_->GetCommandQueue().Get();
    initInfo.NumFramesInFlight = dxCommon_->GetSwapChainResourcesNum();
    initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    initInfo.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    initInfo.SrvDescriptorHeap = srvManager_->GetSrvDescriptorHeap().Get();
    //srvManager_->Allocate();
    initInfo.LegacySingleSrvCpuDescriptor = srvManager_->GetCPUDescriptorHandle(index);
    initInfo.LegacySingleSrvGpuDescriptor = srvManager_->GetGPUDescriptorHandle(index);

    ImGui_ImplDX12_Init(&initInfo);
}

void ImGuiManager::BeginFrame() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
    ImGui::Render();
    
}

void ImGuiManager::Draw() {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList().Get();
    ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetSrvDescriptorHeap().Get() };
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList().Get());

}

void ImGuiManager::Finalize() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

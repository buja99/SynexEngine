#include "DirectXCommon.h"
#include <cassert>
#include "Logger.h"
#include "StringUtility.h"
#include <format>
#include <dxcapi.h>
#include <dxgidebug.h>
#include <iostream>
#include <d3dx12.h>
#include "SrvManager.h"
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Logger;
using namespace StringUtility;


const uint32_t DirectXCommon::kMaxSRVCount = 512;

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance; //스타틱 멤버변수로 변경
	return &instance;
}

DirectXCommon::~DirectXCommon() {
	//delete winApp;
}

void DirectXCommon::Initialize(WinApp* winApp) {

	if (this == nullptr) {
		OutputDebugStringA("DirectXCommon::Initialize: this pointer is null\n");
		return;
	}
	assert(winApp);

	this->winApp = winApp;
	//this->winApp = std::make_unique<WinApp>(*winApp);

	fpsLimiter = std::make_unique<FPSLimiter>();
	fpsLimiter->InitializeFixFPS();


	Device();
	Command();
	SwapChain();
	CreateDescriptorHeaps();
	//CreateDepthBuffer();
	InitializeRTV();
	InitializeDSV();
	InitializeFence();
	InitializeViewport();
	InitializeScissor();
	InitializeCopyPipeline();
	InitializeGrayscalePipeline();
}

void DirectXCommon::Device() {
	HRESULT hr;

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

		debugController->EnableDebugLayer();

		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to create DXGI Factory.\n");
		return;
	}
	assert(SUCCEEDED(hr));

	ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//Adapterの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//wstringでAdapterの情報Logで出力
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;

		}
		useAdapter = nullptr;
	}

	assert(useAdapter != nullptr);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };


	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//device生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		if (FAILED(hr)) {
			OutputDebugStringA("ERROR: Failed to create D3D12 Device.\n");
			return;
		}
		//device生成できるかを確認
		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");



#ifdef _DEBUG

	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバイエラー
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		//infoQueue->Release();
	}
#endif // _DEBUG

	if (device == nullptr) {
		OutputDebugStringA("Error: device creation failed in Device().\n");
	}

}

void DirectXCommon::Command() {
	HRESULT hr;


	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));


	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create Command Queue!\n");
	}
	assert(SUCCEEDED(hr));
	//hr = device->CreateCommandQueue(&commandQueueDesc,
	//	IID_PPV_ARGS(&commandQueue));
	//assert(SUCCEEDED(hr));
}

void DirectXCommon::SwapChain() {
	HRESULT hr;

	//DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kClientWidth;
	swapChainDesc.Height = WinApp::kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		winApp->GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));



}

void DirectXCommon::CreateDepthBuffer() {
	depthStencilBuffer = CreateDepthStencilTextureResource(device, WinApp::kClientWidth, WinApp::kClientHeight);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::CreateDescriptorHeaps() {
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	rtvDescriptorHeap->SetName(L"RTVHeap");
	//srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
	dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	dsvDescriptorHeap->SetName(L"DSVHeap");
}



ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create resource\n");
		return nullptr;
	}
	resource->SetName(L"depthStencilTextureResource");
	return resource;
}

ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr <ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::InitializeRTV() {
	HRESULT hr;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < 2; ++i) {
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResources[i]));
		assert(SUCCEEDED(hr));
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		device->CreateRenderTargetView(swapChainResources[i].Get(), &rtvDesc, rtvHandle);
		rtvHandles[i] = rtvHandle;
	}
}

void DirectXCommon::InitializeDSV() {
	depthStencilBuffer = CreateDepthStencilTextureResource(device, WinApp::kClientWidth, WinApp::kClientHeight);



	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvDescriptorHeap));
	assert(SUCCEEDED(hr));
	dsvDescriptorHeap->SetName(L"DepthStencilView");


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}


void DirectXCommon::InitializeFence() {
	HRESULT hr;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	fenceEvent.reset(CreateEvent(NULL, FALSE, FALSE, NULL));
	assert(fenceEvent != nullptr);
}

void DirectXCommon::InitializeViewport() {

	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

}

void DirectXCommon::InitializeScissor() {
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;
}


void DirectXCommon::RenderTexturePreDraw() {
	// 리소스 상태: COMMON → RENDER_TARGET
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = offscreenRenderTarget_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// RTV만 설정 (Depth는 사용 안 함)
	commandList->OMSetRenderTargets(1, &offscreenRTVHandle_, FALSE, &dsvHandle);

	// Clear
	float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(offscreenRTVHandle_, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// 뷰포트 / 시저
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::RenderTexturePostDraw() {
	// 리소스 상태: RENDER_TARGET → PIXEL_SHADER_RESOURCE
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = offscreenRenderTarget_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);
}

void DirectXCommon::PreDraw() {
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	if (swapChainResources[backBufferIndex] == nullptr) {
		OutputDebugStringA("Error: swapChainResources[backBufferIndex] is NULL in PreDrawToSwapChain.\n");
		return;
	}

	commandList->SetGraphicsRootSignature(rootSignature.Get());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	if (barrier.Transition.pResource == nullptr) {
		OutputDebugStringA("Error: pResource is NULL in PreDrawToSwapChain.\n");
		return;
	}

	commandList->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHandles[backBufferIndex];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (dsvHandle.ptr == 0) {
		OutputDebugStringA("Error: dsvHandle is invalid in PreDrawToSwapChain.\n");
		return;
	}

	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

}

void DirectXCommon::PostDraw() {


	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	if (swapChainResources[backBufferIndex] == nullptr) {
		OutputDebugStringA("Error: swapChainResources[backBufferIndex] is NULL in PostDraw.\n");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	HRESULT hr = commandList->Close();
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to close command list.\n");
	}
	assert(SUCCEEDED(hr));

	fpsLimiter->UpdateFixFPS();

	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	swapChain->Present(1, 0);
	assert(SUCCEEDED(hr));

	//fenceVal = fenceValue; 
	//hr = commandQueue->Signal(fence.Get(), fenceValue);
	//if (FAILED(hr)) {
	//	OutputDebugStringA("Fence signal failed.\n");
	//}
	fenceValue++;
	hr = commandQueue->Signal(fence.Get(), fenceValue);
	assert(SUCCEEDED(hr));

	if (fence->GetCompletedValue() < fenceValue) {
		hr = fence->SetEventOnCompletion(fenceValue, fenceEvent.get());
		assert(SUCCEEDED(hr));
		WaitForSingleObject(fenceEvent.get(), INFINITE);
	}

	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));


}


IDxcBlob* DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
	//hlsl
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));

	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	assert(SUCCEEDED(hr));


	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	//Compile
	LPCWSTR arguments[] = {
		filePath.c_str(), //Compiler対象のhlalファイル名
		L"-E",L"main",
		L"-T",profile, // ShderProfileの設定
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	//Shader Compile
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,         //読み込んだfile
		arguments,					 //Compile option
		_countof(arguments),		 //Compile optionの数
		includeHandler,				 //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	 //Compile結果
	);
	assert(SUCCEEDED(hr));
	//警告・エラー
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}
	//
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	shaderSource->Release();
	shaderResult->Release();
	return shaderBlob;
}


void DirectXCommon::Cleanup() {

	fpsLimiter.reset();
	if (graphicsPipelineState) { graphicsPipelineState.Reset(); }
	if (rootSignature) { rootSignature.Reset(); }
	if (rtvDescriptorHeap) { rtvDescriptorHeap.Reset(); }
	if (dsvDescriptorHeap) { dsvDescriptorHeap.Reset(); }
	if (offscreenRTVHeap_) { offscreenRTVHeap_.Reset(); }
	if (offscreenRenderTarget_) { offscreenRenderTarget_.Reset(); }
	if (depthStencilBuffer) { depthStencilBuffer.Reset(); }
	for (auto& buffer : swapChainResources) {
		buffer.Reset();
	}
	if (swapChain) { swapChain.Reset(); }
	if (commandList) { commandList.Reset(); }
	if (commandAllocator) { commandAllocator.Reset(); }
	if (commandQueue) { commandQueue.Reset(); }
	if (fence) { fence.Reset(); }

	device.Reset();

	OutputDebugStringA("DirectXCommon: Finalized, all resources released\n");
}

void DirectXCommon::UploadTextureDate(ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages) {
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMap
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);

		HRESULT hr = texture.Get()->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void DirectXCommon::ReportLiveObjects() {
#ifdef _DEBUG
	ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		OutputDebugStringA("=== Reporting Live Objects ===\n");
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	}
#endif
}

ComPtr<ID3D12Resource> DirectXCommon::CreateRenderTextureResource(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,  // 초기 상태를 COMMON으로 설정
		&clearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

void DirectXCommon::InitializeOffscreenRenderTarget() {
	
	const Vector4 clearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	offscreenRenderTarget_ = CreateRenderTextureResource(
		device, WinApp::kClientWidth, WinApp::kClientHeight, format, clearColor);
	offscreenRenderTarget_->SetName(L"OffscreenRenderTarget");

	// RTV Heap 생성 및 RTV
	offscreenRTVHeap_ = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
	offscreenRTVHandle_ = offscreenRTVHeap_->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(offscreenRenderTarget_.Get(), &rtvDesc, offscreenRTVHandle_);

	// 🔽 SRVManager를 통해 SRV 등록

	if (!SrvManager::GetInstance()->CanAllocate()) {
		OutputDebugStringA("ERROR: SRVManager ran out of descriptors!\n");
		return;
	}

	offscreenSRVIndex_ = SrvManager::GetInstance()->Allocate();
	SrvManager::GetInstance()->CreatSRVforTexture2D(
		offscreenSRVIndex_, offscreenRenderTarget_.Get(), format, 1);
}

void DirectXCommon::InitializeCopyPipeline() {
	HRESULT hr;

	// 셰이더 컴파일
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	ComPtr<IDxcIncludeHandler> includeHandler;
	dxcUtils->CreateDefaultIncludeHandler(&includeHandler);

	auto vs = CompileShader(L"Resources/shaders/CopyImage.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	auto ps = CompileShader(L"Resources/shaders/CopyImage.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	// 루트 시그니처 생성 (t0 : SRV, s0 : Sampler)
	CD3DX12_DESCRIPTOR_RANGE range;
	range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER param;
	param.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC sampler(0);

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Init(1, &param, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> sigBlob;
	ComPtr<ID3DBlob> errBlob;
	hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	hr = device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&copyRootSignature_));

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = copyRootSignature_.Get();
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.InputLayout = { nullptr, 0 }; // 입력 레이아웃 없음
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.SampleDesc.Count = 1;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&copyPipelineState_));
}

void DirectXCommon::CopyRenderTextureToSwapChain() {
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// 리소스 상태 전환: SWAPCHAIN의 Present → RenderTarget
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// RTV 설정
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], FALSE, nullptr);
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	if (!copyRootSignature_) {
		OutputDebugStringA("copyRootSignature_ is null\n");
		return;
	}

	// 루트시그니처, 파이프라인 설정
	commandList->SetGraphicsRootSignature(copyRootSignature_.Get());
	commandList->SetPipelineState(copyPipelineState_.Get());

	//// 디스크립터 힙 설정 - SRV 힙만 사용
	//ID3D12DescriptorHeap* ppHeaps[] = { offscreenSRVHeap_.Get() };
	//commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//// SRV 설정
	//commandList->SetGraphicsRootDescriptorTable(0, offscreenSRVHandle_);

	SrvManager::GetInstance()->PreDraw();
	SrvManager::GetInstance()->SetGraphicsRootDesciptorTable(0, offscreenSRVIndex_);

	// 삼각형 1개 (3개 정점)
	commandList->DrawInstanced(3, 1, 0, 0);

	
}

void DirectXCommon::InitializeGrayscalePipeline() {
	HRESULT hr;

	// ───────────── DXC 초기화 ─────────────
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	ComPtr<IDxcIncludeHandler> includeHandler;
	dxcUtils->CreateDefaultIncludeHandler(&includeHandler);

	// ───────────── 셰이더 컴파일 ─────────────
	auto vs = CompileShader(L"Resources/shaders/Grayscale.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	auto ps = CompileShader(L"Resources/shaders/Grayscale.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	// ───────────── 루트 시그니처 생성 ─────────────
	CD3DX12_DESCRIPTOR_RANGE range;
	range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

	CD3DX12_ROOT_PARAMETER params[2];
	params[0].InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_PIXEL); // t0 (텍스처)
	params[1].InitAsConstantBufferView(0); // b0 (강도 조절용 상수 버퍼)

	CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Init(_countof(params), params, 1, &sampler,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> sigBlob;
	ComPtr<ID3DBlob> errBlob;
	hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	assert(SUCCEEDED(hr));
	hr = device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&grayscaleRootSignature_));
	assert(SUCCEEDED(hr));

	// ───────────── 파이프라인 생성 ─────────────
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = grayscaleRootSignature_.Get();
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.InputLayout = { nullptr, 0 }; // SV_VertexID 기반이므로 입력 없음
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.SampleDesc.Count = 1;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&grayscalePipelineState_));
	assert(SUCCEEDED(hr));

	// ───────────── 상수 버퍼 초기화 ─────────────
	grayscaleSettings_.strength = 1.0f;

	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(GrayscaleSettings) + 255) & ~255); // 256바이트 정렬

	hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&grayscaleConstBuffer_)
	);
	assert(SUCCEEDED(hr));
	grayscaleConstBuffer_->SetName(L"GrayscaleConstBuffer");

	// 초기값 업로드
	void* mapped = nullptr;
	grayscaleConstBuffer_->Map(0, nullptr, &mapped);
	memcpy(mapped, &grayscaleSettings_, sizeof(GrayscaleSettings));
	grayscaleConstBuffer_->Unmap(0, nullptr);
}

void DirectXCommon::DrawGrayscaleToSwapChain() {
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// ───────────── 리소스 상태 전환: Present → RenderTarget ─────────────
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// ───────────── 렌더 타겟 설정 ─────────────
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHandles[backBufferIndex];
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // 어두운 회색
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// ───────────── 루트 시그니처 / 파이프라인 설정 ─────────────
	commandList->SetGraphicsRootSignature(grayscaleRootSignature_.Get());
	commandList->SetPipelineState(grayscalePipelineState_.Get());

	// ───────────── 디스크립터 힙 및 리소스 바인딩 ─────────────
	SrvManager::GetInstance()->PreDraw(); // 디스크립터 힙 설정
	SrvManager::GetInstance()->SetGraphicsRootDesciptorTable(0, offscreenSRVIndex_); // t0

	// b0 (GrayscaleSettings)
	commandList->SetGraphicsRootConstantBufferView(1, grayscaleConstBuffer_->GetGPUVirtualAddress());

	// ───────────── Fullscreen Triangle 렌더링 ─────────────
	commandList->DrawInstanced(3, 1, 0, 0);
}

void DirectXCommon::SetGrayscaleStrength(float strength) {
	grayscaleSettings_.strength = strength;

	void* mapped = nullptr;
	grayscaleConstBuffer_->Map(0, nullptr, &mapped);
	memcpy(mapped, &grayscaleSettings_, sizeof(GrayscaleSettings));
	grayscaleConstBuffer_->Unmap(0, nullptr);
}










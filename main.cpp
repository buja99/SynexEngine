#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include <dxcapi.h>
#include "Vector2.h"
#include <cmath>
#include"Matrix3x3.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include<wrl.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <sstream>
#include "ResourceObject.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "StringUtility.h"
#include"Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

using Microsoft::WRL::ComPtr;

using namespace Logger;
using namespace StringUtility;



struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {

		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};





int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;
	
	//COMの初期化
	//CoInitialize(NULL);

	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* winApp = new WinApp();
	DirectXCommon* dxCommon = new DirectXCommon();
	Input* input = new Input();
	//GameScene* gameScene = new GameScene();
	SpriteCommon* spriteCommon = nullptr;
	//Sprite* sprite = new Sprite();
	spriteCommon = new SpriteCommon;
	
	

	winApp->Initialize();
	dxCommon->Initialize(winApp);
	TextureManager::GetInstance()->Initialize(dxCommon);
	input->Initialize(winApp);
	//gameScene->Initialize(dxCommon);
	spriteCommon->Initialize(dxCommon);
	//sprite->Initialize(spriteCommon);

	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon, "resources/texture" + std::to_string(i) + ".png");

		Vector2 position;
		position.x = 120.0f * i; 
		position.y = 0.0f;

		sprite->SetPosition(position); 
		sprites.push_back(sprite);
	}

	OutputDebugStringA("Hello,DirectX!\n");

	
	//dxCommon->InitializePSO();
	
//ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3D.VS.hlsl", L"vs_6_0");
//ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3D.PS.hlsl", L"ps_6_0");


	while (true) {

		
		if (winApp->ProcessMessage()) {

			break;
		} 
		input->Update();

		//gameScene->Update();

		//Vector2 position = sprite->GetPosition();
		//position.x += 1.0f;
		//position.y += 1.0f;
		//sprite->SetPosition(position);   
		//
		//float rotation = sprite->GetRotation();
		//rotation += 0.1f;
		//sprite->SetRotation(rotation);

		//Vector4 color = sprite->GetColor();
		//color.x += 0.01f;
		//if (color.x > 1.0f) {
		//	color.x -= 1.0f;
		//}
		//sprite->SetColor(color);
		//
		//Vector2 size = sprite->GetSize();
		//size.x += 1.0f;
		//size.y += 1.0f;
		//sprite->SetSize(size);

		//sprite->Update();


		for (auto& sprite : sprites) {
			sprite->Update();
		}

		dxCommon->PreDraw();
		dxCommon->GetCommandList()->SetPipelineState(dxCommon->GetGraphicsPipelineState().Get());

		spriteCommon->CommonDrawSettings();
		
		for (auto& sprite : sprites) {
			sprite->Draw();
		}

		//sprite->Draw();



		//gameScene->Draw();
		dxCommon->PostDraw();
			
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CloseHandle(fenceEvent);
	//sprite->Cleanup();
	//delete sprite;
	for (auto& sprite : sprites) {
		sprite->Cleanup();
		delete sprite;
	}
	sprites.clear();
	delete spriteCommon;
	//gameScene->Cleanup();
	//delete gameScene;
	TextureManager::GetInstance()->Finalize();
	dxCommon->Cleanup(); 
	delete dxCommon; 
	winApp->Finalize();
	delete input;
	//delete winApp; 
	//delete myMath;

	CoUninitialize();
	return 0;

}
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
	

	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon, "resources/uvChecker.png");
	
		Vector2 position;
		position.x = 120.0f * i; 
		position.y = 0.0f;
	
		sprite->SetPosition(position); 
		sprites.push_back(sprite);
	
		OutputDebugStringA(("Texture loaded: resources/" + std::to_string(i) + ".png\n").c_str());
	}
	
	OutputDebugStringA("Hello,DirectX!\n");

	



	while (true) {

		
		if (winApp->ProcessMessage()) {

			break;
		} 
		input->Update();

		
		for (auto& sprite : sprites) {
			sprite->Update();
		}

		dxCommon->PreDraw();
		//dxCommon->GetCommandList()->SetPipelineState(dxCommon->GetGraphicsPipelineState().Get());

		spriteCommon->CommonDrawSettings();
		
		for (auto& sprite : sprites) {
			sprite->Draw();
		}

		//sprite->Draw();



		//gameScene->Draw();
		dxCommon->PostDraw();
			
	}



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
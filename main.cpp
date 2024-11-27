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
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ModelCommon.h"
#include "Model.h"

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

	ModelCommon* modelCommon = nullptr;
	modelCommon = new ModelCommon();

	Object3dCommon* object3dCommon = nullptr; 
	object3dCommon = new Object3dCommon();    
	
	Model* model = new Model();

	Object3d* object3d = new Object3d();
	

	//GameScene* gameScene = new GameScene();
	SpriteCommon* spriteCommon = nullptr;
	Sprite* sprite = new Sprite();
	spriteCommon = new SpriteCommon;
	
	

	winApp->Initialize();
	dxCommon->Initialize(winApp);
	TextureManager::GetInstance()->Initialize(dxCommon);
	input->Initialize(winApp);
	//gameScene->Initialize(dxCommon);
	spriteCommon->Initialize(dxCommon);
	modelCommon->Initialize(dxCommon);
	object3dCommon->Initialize(dxCommon);
	model->Initialize(modelCommon, object3dCommon);
	object3d->Initialize(object3dCommon);

	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

	
	sprite->Initialize(spriteCommon, "resources/uvChecker.png");
	
	
	OutputDebugStringA("Hello,DirectX!\n");

	



	while (true) {

		
		if (winApp->ProcessMessage()) {

			break;
		} 
		input->Update();

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
			sprite->Update();
		
			object3d->Updata();

		ImGui::Render();

		dxCommon->PreDraw();
		//dxCommon->GetCommandList()->SetPipelineState(dxCommon->GetGraphicsPipelineState().Get());

		spriteCommon->CommonDrawSettings();
		
		object3dCommon->CommonDrawSettings();
		
			sprite->Draw();

			object3d->Draw();
			model->Draw();
		//sprite->Draw();


		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

		//gameScene->Draw();
		dxCommon->PostDraw();
			
	}



	//CloseHandle(fenceEvent);
	//sprite->Cleanup();
	//delete sprite;
	
	sprite->Cleanup();
	delete sprite;
	
	delete spriteCommon;

	object3d->Cleanup();
	delete object3d;

	delete object3dCommon;
	//gameScene->Cleanup();
	//delete gameScene;
	model->Cleanup(); 
	delete model;

	delete modelCommon;

	TextureManager::GetInstance()->Finalize();
	dxCommon->Cleanup(); 
	delete dxCommon; 
	winApp->Finalize();
	delete input;
	//delete winApp; 

	CoUninitialize();
	return 0;

}
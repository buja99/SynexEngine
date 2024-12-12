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
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#endif // _DEBUG
#include "externals/DirectXTex/DirectXTex.h"
#include<wrl.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <sstream>
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
#include "ModelManager.h"
#include "Camera.h"

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
	
	
	Object3d* object3d = new Object3d();
	Object3d* object3d1 = new Object3d();
	Object3d* object3d2 = new Object3d();

	Camera* camera = new Camera();
	camera->SetRotate({ 1.0f, 0.0f, 0.0f });
	camera->SetTranslate({ 0.0f, 44.0f, 30.0f });
	object3dCommon->SetDefaultCamera(camera);

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
	ModelManager::GetInstance()->Initialize(dxCommon);
	ModelManager::GetInstance()->LoadModel("resources/player/body","body.obj");
	ModelManager::GetInstance()->LoadModel("resources/player/legL","legL.obj");
	ModelManager::GetInstance()->LoadModel("resources/player/legR","legR.obj");
	object3d->Initialize(object3dCommon);
	object3d1->Initialize(object3dCommon);
	object3d2->Initialize(object3dCommon);
	object3d->SetModel("body.obj");
	object3d1->SetModel("legL.obj");
	object3d2->SetModel("legR.obj");
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

	
	sprite->Initialize(spriteCommon, "resources/uvChecker.png");
	
	
	OutputDebugStringA("Hello,DirectX!\n");

	



	while (true) {

		
		if (winApp->ProcessMessage()) {

			break;
		} 
		input->Update();
#ifdef _DEBUG
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

#endif // _DEBUG

		
			sprite->Update();
			camera->Update();
			object3d->Updata();
			object3d1->Updata();
			object3d2->Updata();
#ifdef _DEBUG

		ImGui::Render();
#endif // _DEBUG

		dxCommon->PreDraw();

		spriteCommon->CommonDrawSettings();
		
		object3dCommon->CommonDrawSettings();
		
			sprite->Draw();

			object3d->Draw();
			object3d1->Draw();
			object3d2->Draw();
			
		//sprite->Draw();

#ifdef _DEBUG
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());
#endif // _DEBUG

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
	object3d1->Cleanup();
	object3d2->Cleanup();
	delete object3d;
	delete object3d1;
	delete object3d2;

	delete object3dCommon;
	//gameScene->Cleanup();
	//delete gameScene;

	delete modelCommon;
	TextureManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	dxCommon->Cleanup(); 
	delete dxCommon; 
	winApp->Finalize();
	delete input;
	//delete winApp; 

	CoUninitialize();
	return 0;

}
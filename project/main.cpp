//#include<Windows.h>
//#include<cstdint>
//#include<string>
//#include<format>
//#include<d3d12.h>
//#include<dxgi1_6.h>
//#include<cassert>
//#include<dxgidebug.h>
//#include <dxcapi.h>
//#include "Vector2.h"
//#include <cmath>
//#include"Matrix3x3.h"
//#define _USE_MATH_DEFINES
//#include <math.h>
//#ifdef _DEBUG
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_dx12.h"
//#include "imgui/imgui_impl_win32.h"
//#endif // _DEBUG
//#include "externals/DirectXTex/DirectXTex.h"
//#include<wrl.h>
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#include <fstream>
//#include <sstream>
//#include "Input.h"
//#include "WinApp.h"
//#include "DirectXCommon.h"
//#include "Logger.h"
//#include "StringUtility.h"
//#include"Sprite.h"
//#include "SpriteCommon.h"
//#include "TextureManager.h"
//#include "Object3d.h"
//#include "Object3dCommon.h"
//#include "ModelCommon.h"
////#include "Model.h"
//#include "ModelManager.h"
//#include "Camera.h"
//#include "SrvManager.h"
//#include "ParticleManager.h"
//#include "ImGuiManager.h"
//
//#pragma comment(lib,"d3d12.lib")
//#pragma comment(lib,"dxgi.lib")
//#pragma comment(lib,"dxguid.lib")
//#pragma comment(lib,"dxcompiler.lib")




//using Microsoft::WRL::ComPtr;
//
//using namespace Logger;
//using namespace StringUtility;
//std::random_device seedGenerator;
//std::mt19937 randomEngine(seedGenerator());
//
//
//struct D3DResourceLeakChecker {
//	~D3DResourceLeakChecker() {
//
//		ComPtr<IDXGIDebug1> debug;
//		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
//			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
//			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
//			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
//		}
//	}
//};
//
//
//
//
//
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
//
//	D3DResourceLeakChecker leakCheck;
//	
//	//COMの初期化
//	//CoInitialize(NULL);
//
//	CoInitializeEx(0, COINIT_MULTITHREADED);
//
//	WinApp* winApp = new WinApp();
//	DirectXCommon* dxCommon = new DirectXCommon();
//	Input* input = new Input();
//	SrvManager* srvManager = SrvManager::GetInstance();
//	ImGuiManager* imGuiManager = new ImGuiManager();
//	ModelCommon* modelCommon = nullptr;
//	modelCommon = new ModelCommon();
//
//	
//
//	Object3dCommon* object3dCommon = nullptr; 
//	object3dCommon = new Object3dCommon();    
//	
//	//Model* model = new Model();
//
//	Object3d* object3d = new Object3d();
//
//	Camera* camera = new Camera();
//	camera->SetRotate({ 0.0f, 0.0f, 0.0f });
//	camera->SetTranslate({ 0.0f, 4.0f, -10.0f });
//	object3dCommon->SetDefaultCamera(camera);
//
//	//GameScene* gameScene = new GameScene();
//	SpriteCommon* spriteCommon = nullptr;
//	Sprite* sprite = new Sprite();
//	spriteCommon = new SpriteCommon;
//	
//	
//
//	winApp->Initialize();
//	dxCommon->Initialize(winApp);
//	srvManager->Initialize(dxCommon);
//	imGuiManager->Initialize(winApp, dxCommon);
//
//	TextureManager::GetInstance()->Initialize(dxCommon, srvManager);
//	input->Initialize(winApp);
//	//gameScene->Initialize(dxCommon);
//	spriteCommon->Initialize(dxCommon);
//	modelCommon->Initialize(dxCommon);
//	object3dCommon->Initialize(dxCommon);
//	ModelManager::GetInstance()->Initialize(dxCommon);
//	ModelManager::GetInstance()->LoadModel("resources", "axis.obj");
//	//model->Initialize(modelCommon, object3dCommon, "resources", "axis.obj");
//	object3d->Initialize(object3dCommon);
//	object3d->SetModel("axis.obj");
//
//	
//	sprite->Initialize(spriteCommon, "resources/uvChecker.png");
//	
//	
//	OutputDebugStringA("Hello,DirectX!\n");
//
//	ParticleManager* particleManager = nullptr;
//	particleManager = new ParticleManager();
//	particleManager->Initialize(dxCommon, srvManager);
//	particleManager->CreateParticleGroup("fire","resources/uvChecker.png");
//
//	particleManager->SetCamera(camera);
//
//	Emitter emitter{};
//	emitter.transform.translate = { 0.0f, 0.0f, 0.0f };
//	emitter.count = 10;
//	emitter.frequency = 0.5f;  
//	emitter.frequencyTime = 0.0f;
//
//	//OutputDebugStringA("文字列リテラルを出力\n");
//
//	while (true) {
//
//		
//		if (winApp->ProcessMessage()) {
//
//			break;
//		} 
//		input->Update();
//#ifdef _DEBUG
//		imGuiManager->BeginFrame();
//
//#endif // _DEBUG
//
//		std::list<Particle> newParticles = particleManager->Emit(emitter, randomEngine);
//		
//			sprite->Update();
//			camera->Update();
//			object3d->Updata();
//			particleManager->Update();
//
//			
//
//
//
//#ifdef _DEBUG
//			imGuiManager->EndFrame();
//		
//#endif // _DEBUG
//
//		dxCommon->PreDraw();
//		//dxCommon->GetCommandList()->SetPipelineState(dxCommon->GetGraphicsPipelineState().Get());
//		SrvManager::GetInstance()->PreDraw();
//		spriteCommon->CommonDrawSettings();
//		
//		object3dCommon->CommonDrawSettings();
//		
//			//sprite->Draw();
//
//			object3d->Draw();
//			//model->Draw();
//		//sprite->Draw();
//
//			particleManager->Draw();
//
//			
//
//#ifdef _DEBUG
//			imGuiManager->Draw();
//#endif // _DEBUG
//
//		//gameScene->Draw();
//		dxCommon->PostDraw();
//
//	}
//
//
//	//CloseHandle(fenceEvent);
//	
//	
//	sprite->Cleanup();
//	delete sprite;
//	
//	delete spriteCommon;
//
//	object3d->Cleanup();
//	delete object3d;
//
//	delete object3dCommon;
//	delete particleManager;
//	delete modelCommon;
//	imGuiManager->Finalize();
//	delete imGuiManager;
//	TextureManager::GetInstance()->Finalize();
//	ModelManager::GetInstance()->Finalize();
//	SrvManager::GetInstance()->Finalize();
//	dxCommon->Cleanup(); 
//	delete dxCommon; 
//	winApp->Finalize();
//	delete input;
//	//delete winApp; 
//
//	CoUninitialize();
//	return 0;
//
//}

#include "MyGame.h"
#include "LeakCheck.h"
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	LeakCheck leakCheck;

	std::unique_ptr<MyGame> myGame = std::make_unique<MyGame>();

	myGame->Run();

	return 0;
}

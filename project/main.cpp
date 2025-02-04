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
#include "Player.h"
#include "MyMath.h"
#include "Enemy.h"

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
	MyMath* math = new MyMath();

	ModelCommon* modelCommon = nullptr;
	modelCommon = new ModelCommon();

	

	Object3dCommon* object3dCommon = nullptr; 
	object3dCommon = new Object3dCommon();    
	
	std::vector<std::unique_ptr<Object3d>> object3ds;

	std::vector<std::string> modelPaths = {
	"body", "legL", "legR", "head", "armR", "armL","gun"
	};


	Camera* camera = new Camera();
	camera->SetRotate({ 1.0f, 0.0f, 0.0f });
	camera->SetTranslate({ 0.0f, 90.0f, 10.0f });
	object3dCommon->SetDefaultCamera(camera);

	//GameScene* gameScene = new GameScene();
	SpriteCommon* spriteCommon = nullptr;
	Sprite* sprite = new Sprite();
	spriteCommon = new SpriteCommon;
	
	Player* player = new Player();
	//Enemy* enemy = new Enemy();

	winApp->Initialize();
	dxCommon->Initialize(winApp);
	

	TextureManager::GetInstance()->Initialize(dxCommon);
	input->Initialize(winApp);
	//gameScene->Initialize(dxCommon);
	spriteCommon->Initialize(dxCommon);
	modelCommon->Initialize(dxCommon);
	object3dCommon->Initialize(dxCommon);
	ModelManager::GetInstance()->Initialize(dxCommon);

	for (const auto& modelPath : modelPaths) {
		ModelManager::GetInstance()->LoadModel("resources/player/" + modelPath, modelPath + ".obj");
		auto object = std::make_unique<Object3d>();
		object->Initialize(object3dCommon);
		object->SetModel(modelPath + ".obj");

		object3ds.emplace_back(std::move(object));
	}
	ModelManager::GetInstance()->LoadModel("resources/bullet", "bullet.obj");
	//auto bulletObject = std::make_unique<Object3d>();
	//bulletObject->Initialize(object3dCommon);
	//bulletObject->SetModel("bullet.obj");
	//Object3d* bulletModelPtr = bulletObject.get();

	ModelManager::GetInstance()->LoadModel("resources/enemy", "enemy.obj");
	auto enemyModel = std::make_unique<Object3d>();
	enemyModel->Initialize(object3dCommon);
	enemyModel->SetModel("enemy.obj");

	
	Enemy enemy;
	enemy.Initialize(enemyModel.get());
	

	std::vector<Object3d*> playerParts = {
	object3ds[0].get(),  // body
	object3ds[1].get(),  // legL
	object3ds[2].get(),  // legR
	object3ds[3].get(),  // head
	object3ds[4].get(),  // armL
	object3ds[5].get(),  // armR
	object3ds[6].get()   // gun
	};
	//object3ds.emplace_back(std::move(bulletModel));
	player->Initialize(playerParts, object3dCommon);
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

		
		input->Update();
		player->Updata(input);
		

		Vector3 playerPos = player->GetPosition();
		

		Vector3 cameraOffset = { 0.0f, 90.0f, -40.0f };
		Vector3 cameraPos = math->Add(playerPos, cameraOffset);
		camera->SetTranslate(cameraPos);

		camera->Update();
		enemy.Update();
		//object3dCommon->SetViewProjectionMatrix(camera->GetViewProjectionMatrix());
			for (auto& object : object3ds) {
				object->Updata();
			}
			
			for (auto& bullet : player->GetBullets()) {
				if (math->CheckCollision(bullet->GetPosition(), { 3.0f, 3.0f, 3.0f }, enemy.GetPosition(), enemy.GetScale())) {
					enemy.Destroy();
				}
			}
			//wwddaenemyModel->Updata();
		
			//Vector3 cameraPos =math->Add( pos,cameraOffset);
			//camera->SetTranslate(cameraPos);
			

			
#ifdef _DEBUG

		ImGui::Render();
#endif // _DEBUG

		dxCommon->PreDraw();

		spriteCommon->CommonDrawSettings();
		
		object3dCommon->CommonDrawSettings();
		
			
		player->Draw();

			for (auto& object : object3ds) {
			
				object->Draw();
	        }
			for (const auto& bullet : player->GetBullets()) {
				bullet->GetObject()->Draw();
			}
			 
						if (enemy.IsAlive()) {
				enemy.Draw();
			}



		//sprite->Draw();

#ifdef _DEBUG
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());
#endif // _DEBUG

		//gameScene->Draw();
		dxCommon->PostDraw();
			
	}

#ifdef _DEBUG
	//dxCommon->ReportLiveObjects();
#endif

	//CloseHandle(fenceEvent);
	//sprite->Cleanup();
	//delete sprite;
	
	

	sprite->Cleanup();
	delete sprite;
	
	delete spriteCommon;

	delete player;
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
	delete math;

	CoUninitialize();
	return 0;

}
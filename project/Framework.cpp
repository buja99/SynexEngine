#include "Framework.h"

void Framework::Initialize() {
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(winApp_.get());

	Input::GetInstance()->Initialize(winApp_.get());

	srvManager_ = std::make_unique<SrvManager>();
	srvManager_->Initialize(dxCommon_);

	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_.get());

	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxCommon_);

	object3dCommon_ = std::make_unique<Object3dCommon>();
	object3dCommon_->Initialize(dxCommon_);

	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(dxCommon_);

	//camera_ = std::make_unique<Camera>();
	
#ifdef _DEBUG

	imGuiManager_ = std::make_unique<ImGuiManager>();
	imGuiManager_->Initialize(winApp_.get(), dxCommon_);

#endif // _DEBUG

	sceneManager_ = SceneManager::GetInstance();
}

void Framework::Finalize() {

	dxCommon_->Cleanup();
	winApp_->Finalize();
}

void Framework::Update() {

	if (winApp_->ProcessMessage()) {
		//ゲームループを抜ける
		isEndReqest_ = true;
	}

#ifdef _DEBUG

	imGuiManager_->BeginFrame();

#endif // _DEBUG


	input_->Update();

	sceneManager_->Update();


#ifdef _DEBUG

	imGuiManager_->EndFrame();

#endif // _DEBUG
}

void Framework::Run() {

	Initialize();

	while (true) {
		Update();

		if (GetIsEndReqest()) {
			break;
		}
		Draw();
	}
	Finalize();
}
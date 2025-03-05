#include "Framework.h"

void Framework::CheckHeap() {
	HANDLE heap = GetProcessHeap();
	if (!HeapValidate(heap, 0, nullptr)) {
		OutputDebugStringA("Heap corruption detected!\n");
	}
}

void Framework::Initialize() {
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	dxCommon_ = DirectXCommon::GetInstance();

	if (!dxCommon_) {
		OutputDebugStringA("ERROR: Framework::Initialize() - DirectXCommon is null!\n");
		return;
	}

	dxCommon_->Initialize(winApp_.get());

	input_ = Input::GetInstance();
	input_->Initialize(winApp_.get());

	srvManager_ = SrvManager::GetInstance();

	if (!srvManager_) {
		OutputDebugStringA("ERROR: Framework::Initialize() - Failed to create SrvManager!\n");
		return;
	}

	srvManager_->Initialize(dxCommon_);

#ifdef _DEBUG

	imGuiManager_ = std::make_unique<ImGuiManager>();
	imGuiManager_->Initialize(winApp_.get(), dxCommon_);

#endif // _DEBUG

	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);

	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxCommon_);

	object3dCommon_ = std::make_unique<Object3dCommon>();
	object3dCommon_->Initialize(dxCommon_);

	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(dxCommon_);

	//camera_ = std::make_unique<Camera>();
	

	sceneManager_ = SceneManager::GetInstance();
}

void Framework::Finalize() {

	
#ifdef _DEBUG
	if (imGuiManager_) {
		imGuiManager_->Finalize();
		imGuiManager_.reset();
	}
#endif // _DEBUG

	// DirectX 관련 객체들 먼저 해제
	if (sceneManager_) {
		sceneManager_->Finalize();
		sceneManager_ = nullptr;
	}

	if (srvManager_) {
		srvManager_->Finalize();
		srvManager_ = nullptr;
	}

	spriteCommon_.reset();
	object3dCommon_.reset();
	modelCommon_.reset();

	particleManager_.reset();
	camera_.reset();

	if (input_) {
		input_->Finalize();
		input_ = nullptr;
	}

	if (dxCommon_) {
		dxCommon_->Cleanup();
		dxCommon_ = nullptr;
	}

	if (winApp_) {
		winApp_->Finalize();
		winApp_.reset();
	}
}

void Framework::Update() {
	if (!dxCommon_ || !winApp_) return;
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
		CheckHeap();
		Update();

		if (GetIsEndReqest()) {
			break;
		}
		Draw();
	}
	Finalize();
}
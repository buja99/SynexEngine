#include "GameScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();

	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	ModelManager::GetInstance()->LoadModel("resources/player", "body.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "head.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "arm.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "leg.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "a.obj");

	for (int i = 0; i < kPlayerPartCount; ++i) {
		playerTransforms_[i] = std::make_unique<WorldTransform>();
		playerTransforms_[i]->Initialize();

		playerParts_[i] = std::make_unique<Object3d>();
		playerParts_[i]->Initialize(Object3dCommon::GetInstance(), playerTransforms_[i].get());
	}

	for (int i = 0; i < kPlayerPartCount; ++i) {
		if (i == BODY) continue;
		playerTransforms_[i]->parent_ = playerTransforms_[BODY].get(); // 계층 설정
	}

	playerParts_[ARM_R]->SetModel("arm.obj");
	playerParts_[ARM_L]->SetModel("arm.obj");
	playerParts_[LEG_R]->SetModel("leg.obj");
	playerParts_[LEG_L]->SetModel("leg.obj");
	playerParts_[BODY]->SetModel("body.obj");
	playerParts_[HEAD]->SetModel("head.obj");

	playerParts_[ARM_R]->SetTranslate({ 4.7f, 3.2f, 10.0f });
	playerParts_[ARM_L]->SetTranslate({ -4.7f, 3.2f, 10.0f });
	playerParts_[LEG_R]->SetTranslate({ -2.4f, -2.1f, 10.1f });
	playerParts_[LEG_L]->SetTranslate({ 2.5f, -2.1f, 10.1f });
	playerParts_[BODY]->SetTranslate({ 0.0f, 0.0f, 10.0f });
	playerParts_[HEAD]->SetTranslate({ 0.2f, 3.9f, 10.0f });


	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f, 4.0f, -10.0f });
	camera_->SetTarget({ 0.0f, 0.0f, 0.0f });


	for (int i = 0; i < kPlayerPartCount; ++i) {

		playerParts_[i]->SetCamera(camera_.get());

	}

}

void GameScene::Update() {

	if (input_->TriggerKey(DIK_R)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}

#ifdef _DEBUG
	ImGui::Begin("Player Model Transform");

	for (int i = 0; i < kPlayerPartCount; ++i) {
		std::string label = "Part[" + std::to_string(i) + "]";

		ImGui::SeparatorText(label.c_str());

		Vector3 scale = playerParts_[i]->GetScale();
		Vector3 rotate = playerParts_[i]->GetRotate();
		Vector3 translate = playerParts_[i]->GetTranslate();

		ImGui::DragFloat3((label + " Scale").c_str(), &scale.x, 0.1f);
		ImGui::DragFloat3((label + " Rotate").c_str(), &rotate.x, 0.1f);
		ImGui::DragFloat3((label + " Translate").c_str(), &translate.x, 0.1f);

		playerParts_[i]->SetScale(scale);
		playerParts_[i]->SetRotate(rotate);
		playerParts_[i]->SetTranslate(translate);
	}
	ImGui::End();
#endif

	float speed = 0.2f;
	if (input_->PushKey(DIK_A)) {
		Vector3 pos = playerParts_[BODY]->GetTranslate();
		pos.x -= speed;
		playerParts_[BODY]->SetTranslate(pos);
	}
	if (input_->PushKey(DIK_D)) {
		Vector3 pos = playerParts_[BODY]->GetTranslate();
		pos.x += speed;
		playerParts_[BODY]->SetTranslate(pos);
	}

	playerParts_[BODY]->Update();
	playerTransforms_[BODY]->UpdateMatrix();

	for (int i = 0; i < kPlayerPartCount; ++i) {
		if (i == BODY) continue;
		playerParts_[i]->Update();
		playerTransforms_[i]->UpdateMatrix();
	}

	//camera_->UpdateMatrix();
	camera_->Update();

}

void GameScene::Draw() {
	SpriteCommon::GetInstance()->SetUIPipeline();
	
	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();
	for (auto& part : playerParts_) {
		part->Draw();
	}
	
	
	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	

}

void GameScene::Finalize() {
	/*for (int i = 0; i < kPlayerPartCount; ++i) {

		playerParts_[i]->Cleanup();
		playerParts_[i].reset();
		playerTransforms_[i]->Cleanup();
		playerTransforms_[i].reset();
	}*/


}

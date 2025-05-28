#include "GameScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();

	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	
	player_ = std::make_unique<Player>();
	player_->Initialize();
	

	

	/*for (int i = 0; i < kPlayerPartCount; ++i) {

		playerParts_[i]->SetCamera(camera_.get());

	}*/

}

void GameScene::Update() {

	if (input_->TriggerKey(DIK_R)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}

//#ifdef _DEBUG
//	ImGui::Begin("Player Model Transform");
//
//	for (int i = 0; i < kPlayerPartCount; ++i) {
//		std::string label = "Part[" + std::to_string(i) + "]";
//
//		ImGui::SeparatorText(label.c_str());
//
//		Vector3 scale = playerParts_[i]->GetScale();
//		Vector3 rotate = playerParts_[i]->GetRotate();
//		Vector3 translate = playerParts_[i]->GetTranslate();
//
//		ImGui::DragFloat3((label + " Scale").c_str(), &scale.x, 0.1f);
//		ImGui::DragFloat3((label + " Rotate").c_str(), &rotate.x, 0.1f);
//		ImGui::DragFloat3((label + " Translate").c_str(), &translate.x, 0.1f);
//
//		playerParts_[i]->SetScale(scale);
//		playerParts_[i]->SetRotate(rotate);
//		playerParts_[i]->SetTranslate(translate);
//	}
//	ImGui::End();
//#endif

	
	player_->Updata();
	//camera_->UpdateMatrix();
	//camera_->Update();

}

void GameScene::Draw() {
	SpriteCommon::GetInstance()->SetUIPipeline();
	
	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	
	player_->Draw();

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

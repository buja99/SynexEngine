#include "GameScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();


	TextureManager::GetInstance()->LoadTexture("resources/back.png");


	back_ = std::make_unique<Sprite>();
	back_->Initialize(SpriteCommon::GetInstance(), "resources/back.png");
	back_->SetPosition({ 0.0f, 0.0f });









	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	ModelManager::GetInstance()->LoadModel("resources/obj/tree", "tree.obj");
	ModelManager::GetInstance()->LoadModel("resources/obj/weeds", "weeds.obj");

	treeWorldTransform_ = std::make_unique<WorldTransform>();
	treeWorldTransform_->Initialize();

	tree2WorldTransform_ = std::make_unique<WorldTransform>();
	tree2WorldTransform_->Initialize();

	treeModel_ = std::make_unique<Object3d>();
	treeModel_->Initialize(Object3dCommon::GetInstance(), treeWorldTransform_.get());
	treeModel_->SetModel("tree.obj");

	treeModel2_ = std::make_unique<Object3d>();
	treeModel2_->Initialize(Object3dCommon::GetInstance(), tree2WorldTransform_.get());
	treeModel2_->SetModel("tree.obj");
		
	weedsWorldTransform_ = std::make_unique<WorldTransform>();
	weedsWorldTransform_->Initialize();

	weeds2WorldTransform_ = std::make_unique<WorldTransform>();
	weeds2WorldTransform_->Initialize();

	weedsModel_ = std::make_unique<Object3d>();
	weedsModel_->Initialize(Object3dCommon::GetInstance(), weedsWorldTransform_.get());
	weedsModel_->SetModel("weeds.obj");

	weedsModel2_ = std::make_unique<Object3d>();
	weedsModel2_->Initialize(Object3dCommon::GetInstance(), weeds2WorldTransform_.get());
	weedsModel2_->SetModel("weeds.obj");

	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f, 4.0f, -10.0f });
	camera_->SetTarget({ 0.0f, 0.0f, 0.0f });

	treeModel_->SetCamera(camera_.get());
	treeModel2_->SetCamera(camera_.get());
	weedsModel_->SetCamera(camera_.get());
	weedsModel2_->SetCamera(camera_.get());

	treeModel_->SetTranslate({ -8.0f,0.0f,10.0f });
	treeModel_->SetRotate({ 0.0f, 0.0f, 0.0f });

	treeModel2_->SetTranslate({ 4.0f,0.0f,3.0f });
	treeModel2_->SetRotate({ 0.0f, 0.0f, 0.0f });

	weedsModel_->SetTranslate({ -4.0f,0.0f,3.0f });
	weedsModel_->SetRotate({ 0.0f, 0.0f, 0.0f });

	weedsModel2_->SetTranslate({ 0.0f,0.0f,0.0f });
	weedsModel2_->SetRotate({ 0.0f, 0.0f, 0.0f });

	player_ = std::make_unique<Player>();
	player_->Initialize();
	
	weedsWorldTransform_->translate_ = { 10.0f, 0.0f, 0.0f };

	player_->SetCamera(camera_.get());

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

}

void GameScene::Update() {


	back_->Update();




	camera_->UpdateMatrix();
	camera_->Update();
	treeModel_->Update();
	treeModel2_->Update();
	weedsModel_->Update();
	weedsModel2_->Update();
	


	treeWorldTransform_->UpdateMatrix();
	treeWorldTransform_->TransferMatrix();

	tree2WorldTransform_->UpdateMatrix();
	tree2WorldTransform_->TransferMatrix();

	weedsWorldTransform_->UpdateMatrix();
	weedsWorldTransform_->TransferMatrix();

	weeds2WorldTransform_->UpdateMatrix();
	weeds2WorldTransform_->TransferMatrix();

	player_->Updata();
	if (input_->TriggerKey(DIK_R)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}

	if (input_->TriggerKey(DIK_A)) {
		moveObj();
		treeModel_->SetRotate({ -1.7f, 0.0f, 0.0f });
		treeModel2_->SetRotate({ -1.7f, 0.0f, 0.0f });
		weedsModel_->SetRotate({ -1.7f, 0.0f, 0.0f });
		weedsModel2_->SetRotate({ -1.7f, 0.0f, 0.0f });
	}


#ifdef _DEBUG
	ImGui::Begin("Model Transform");
	if (ImGui::CollapsingHeader("Tree")) {
		ImGui::DragFloat3("Tree Position", &treeWorldTransform_->translate_.x, 0.1f);
		ImGui::DragFloat3("Tree Rotation", &treeWorldTransform_->rotate_.x, 0.1f);
		ImGui::DragFloat3("Tree Scale", &treeWorldTransform_->scale_.x, 0.1f);
	}

	// Weeds Model Transform
	if (ImGui::CollapsingHeader("Weeds")) {
		ImGui::DragFloat3("Weeds Position", &weedsWorldTransform_->translate_.x, 0.1f);
		ImGui::DragFloat3("Weeds Rotation", &weedsWorldTransform_->rotate_.x, 0.1f);
		ImGui::DragFloat3("Weeds Scale", &weedsWorldTransform_->scale_.x, 0.1f);
	}
	
	ImGui::End();
#endif

	


}

void GameScene::Draw() {
	SpriteCommon::GetInstance()->SetUIPipeline();
	
	back_->Draw();

	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	treeModel_->Draw();
	treeModel2_->Draw();

	weedsModel_->Draw();
	weedsModel2_->Draw();

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

	camera_.reset();
}

void GameScene::moveObj() {
	

}

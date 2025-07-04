#include "GameScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "MyMath.h"

GameScene::~GameScene() {
}



void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	TextureManager::GetInstance()->LoadTexture("resources/back.png");


	back_ = std::make_unique<Sprite>();
	back_->Initialize(SpriteCommon::GetInstance(), "resources/back.png");
	back_->SetPosition({ 0.0f, 0.0f });

	ground_ = std::make_unique<Ground>();
	ground_->Initialize();



	


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
ground_->SetCamera(camera_.get());

treeModel_->SetCamera(camera_.get());
treeModel2_->SetCamera(camera_.get());
weedsModel_->SetCamera(camera_.get());
weedsModel2_->SetCamera(camera_.get());

	//weedsWorldTransform_->translate_ = { -10.0f, 0.0f, 0.0f };
	

player_ = std::make_unique<Player>();
player_->Initialize();
player_->SetCamera(camera_.get());

Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());



// Register models and transforms
registry_.Register("treeModel_", treeModel_.get(), treeWorldTransform_.get());
registry_.Register("treeModel2_", treeModel2_.get(), tree2WorldTransform_.get());
registry_.Register("weedsModel_", weedsModel_.get(), weedsWorldTransform_.get());
registry_.Register("weedsModel2_", weedsModel2_.get(), weeds2WorldTransform_.get());

// Load and apply level data
LevelData* levelData = LevelLoader::LoadLevelData("resources/json/test1.json");
LevelApplier::ApplyLevelTransforms(levelData, registry_.GetTransformMap());


DirectXCommon::GetInstance()->SetGrayscaleStrength(0.0f);

TextureManager::GetInstance()->LoadTexture("resources/circle.png");
primitive_ = std::make_unique<ParticleManager>();
primitive_->Initialize(DirectXCommon::GetInstance(), SrvManager::GetInstance());
primitive_->SetCamera(camera_.get());
primitive_->CreateParticleGroup("ash1", "resources/circle.png");


emitter_.count = 4;
emitter_.frequency = 0.05f;
emitter_.frequencyTime = 0.0f;
emitter_.transform.translate = { 0.0f, 10.0f, 0.0f };


camera_->SetRotate({ 1.2f, 0.0f, 0.0f });
cameraOffset_ = { 0.1f, 41.2f, -33.9f };
}

void GameScene::Update() {




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

	ImGui::Begin("cameraOffset ");

	// 카메라 오프셋 조정
	if (ImGui::CollapsingHeader("Camera Offset")) {
		ImGui::DragFloat3("Offset", &cameraOffset_.x, 0.1f);
	}

	
		ImGui::End();

		ImGui::Begin("Stencil Mask");

		ImGui::SliderFloat("Center X", &centerX, -1.0f, 1.0f);
		ImGui::SliderFloat("Center Y", &centerY, -1.0f, 1.0f);
		ImGui::SliderFloat("Half Width", &halfW, 0.0f, 10.0f);
		ImGui::SliderFloat("Half Height", &halfH, 0.0f, 10.0f);

		ImGui::End();
#endif


	back_->Update();

	ground_->Update();

	player_->Updata();
	camera_->Update();
	//Vector3 playerPos = player_->GetPosition();
	//Vector3 eye = MyMath::Add(playerPos, cameraOffset_); // 위에서 바라보는 시점
	//Vector3 target = playerPos;

	//camera_->SetEye(eye);
	//camera_->SetTarget(target);
	
	camera_->UpdateMatrix();
	

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

	
	if (input_->TriggerKey(DIK_R)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}


	emitter_.frequencyTime += 1.0f / 60.0f;

	if (emitter_.frequencyTime >= emitter_.frequency) {
		emitter_.frequencyTime = 0.0f;
		primitive_->Emit("ash1", emitter_, randomEngine_);
	}

	primitive_->Update();

	


}

void GameScene::Draw() {
	SpriteCommon::GetInstance()->SetUIPipeline();
	
	//back_->Draw();

	SrvManager::GetInstance()->PreDraw();

	left = centerX - halfW;
	right = centerX + halfW;
	top = centerY + halfH;
	bottom = centerY - halfH;

	//// 1. 마스크 사각형 지정
	//Object3dCommon::GetInstance()->SetStencilQuadArea(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f);
	//Object3dCommon::GetInstance()->SetStencilWriteMaskValue(0); // ← 필요 시 스텐실 참조값 설정 함수
	//Object3dCommon::GetInstance()->SetStencilWritePipeline();   // ← 전체 1로 설정하는 PSO
	//Object3dCommon::GetInstance()->stencilMaskSettings();       // ← 마스크 렌더링 (DrawInstanced(6))


	Object3dCommon::GetInstance()->SetStencilQuadArea(left, right, top, bottom, 0.0f);
	Object3dCommon::GetInstance()->SetStencilWriteMaskValue(0); // 스텐실 0으로 쓰기
	Object3dCommon::GetInstance()->SetStencilWritePipeline();   // ← 같은 PSO 사용해도 됨
	Object3dCommon::GetInstance()->stencilMaskSettings();

	//auto commandList = DirectXCommon::GetInstance()->GetCommandList();
	//commandList->SetGraphicsRootConstantBufferView(
	//	9, // RootParameter Index
	//	Object3dCommon::GetInstance()->GetPlayerRangeCB()->GetGPUVirtualAddress()
	//);

	//Vector3 playerPos = player_->GetPosition(); // Vector3 → XMFLOAT3
	//float visibleRange = 30.0f;

	//struct PlayerRangeData {
	//	Vector3 playerPos;
	//	float range;
	//};

	//PlayerRangeData* mapped = nullptr;
	//Object3dCommon::GetInstance()->GetPlayerRangeCB()->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	//mapped->playerPos = playerPos;
	//mapped->range = visibleRange;
	//Object3dCommon::GetInstance()->GetPlayerRangeCB()->Unmap(0, nullptr);

	//ground_->Draw();

	

	/*treeModel_->Draw(); 
	treeModel2_->Draw();

	weedsModel_->Draw();
	weedsModel2_->Draw();*/

	
	//player_->Draw();

	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	//primitive_->Draw();

}


void GameScene::Finalize() {
	

	camera_.reset();
}





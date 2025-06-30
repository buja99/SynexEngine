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


	// 🆕 Viewport / Scissor 설정
	static float viewportX = 200.0f;
	static float viewportY = 100.0f;
	static float viewportW = 600.0f;
	static float viewportH = 400.0f;

	ImGui::DragFloat("Viewport X", &viewportX, 1.0f);
	ImGui::DragFloat("Viewport Y", &viewportY, 1.0f);
	ImGui::DragFloat("Viewport W", &viewportW, 1.0f);
	ImGui::DragFloat("Viewport H", &viewportH, 1.0f);

	// 적용
	/*DirectXCommon::GetInstance()->SetViewport(viewportX, viewportY, viewportW, viewportH);
	DirectXCommon::GetInstance()->SetScissorRect(
		(int)viewportX, (int)viewportY,
		(int)(viewportX + viewportW), (int)(viewportY + viewportH));*/







	ImGui::End();
#endif


	back_->Update();

	ground_->Update();

	player_->Updata();

	Vector3 playerPos = player_->GetPosition();
	Vector3 eye = MyMath::Add( playerPos , Vector3{0.0f, 8.0f, -15.0f}); // 위에서 바라보는 시점
	Vector3 target = playerPos;

	camera_->SetEye(eye);
	camera_->SetTarget(target);
	camera_->SetUp({ 0.0f, 1.0f, 0.0f }); // 기본 업벡터
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

	
	if (input_->TriggerKey(DIK_R)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}

	/*if (input_->TriggerKey(DIK_A)) {
		moveObj();
		treeModel_->SetRotate({ -1.7f, 0.0f, 0.0f });
		treeModel2_->SetRotate({ -1.7f, 0.0f, 0.0f });
		weedsModel_->SetRotate({ -1.7f, 0.0f, 0.0f });
		weedsModel2_->SetRotate({ -1.7f, 0.0f, 0.0f });
	}*/


	emitter_.frequencyTime += 1.0f / 60.0f;

	if (emitter_.frequencyTime >= emitter_.frequency) {
		emitter_.frequencyTime = 0.0f;
		primitive_->Emit("ash1", emitter_, randomEngine_);
	}

	primitive_->Update();

	


}

void GameScene::Draw() {
	SpriteCommon::GetInstance()->SetUIPipeline();
	
	back_->Draw();

	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	ground_->Draw();

	

	/*treeModel_->Draw(); 
	treeModel2_->Draw();

	weedsModel_->Draw();
	weedsModel2_->Draw();*/

	player_->Draw();

	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	primitive_->Draw();

}

void GameScene::Finalize() {
	

	camera_.reset();
}





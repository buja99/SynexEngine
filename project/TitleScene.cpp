#include "TitleScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
TitleScene::~TitleScene() {
	Finalize();
}

void TitleScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();
	camera_ = std::make_unique<Camera>();

	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	worldTransform_ = std::make_unique<WorldTransform>();
	worldTransform_->Initialize();

	model_ = std::make_unique<Object3d>();
	model_->Initialize(Object3dCommon::GetInstance(), worldTransform_.get());

	ModelManager::GetInstance()->LoadModel("resources", "terrain.obj");
	model_->SetModel("terrain.obj");

	model_->SetEnableLighting(true);
	model_->SetIsBlinnPhong(true);         // Phong
	model_->SetUsePointLight(false);        // Point Light 
	model_->SetUseDirectionalLight(false);   // Directional Light 
	model_->SetUseSpotLight(true);	  // Spot Light
	// 카메라 생성
	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f, 4.0f, -10.0f });
	camera_->SetTarget({ 0.0f, 0.0f, 0.0f });
	camera_->UpdateMatrix();

	// 모델에 카메라 설정
	model_->SetCamera(camera_.get());

	// 씬 공통 렌더 설정 (이것도 보통 필요함)
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

	/*TextureManager::GetInstance()->LoadTexture("resources/title.png");

	title = std::make_unique<Sprite>();
	title->Initialize(SpriteCommon::GetInstance(), "resources/title.png");*/
	/*title->SetPosition(Vector2(0.0f, 0.0f));
	title->SetSize(Vector2(1280.0f, 720.0f));
	title->SetTextureLeftTop(Vector2(0.0f, 0.0f));
	title->SetTextureSize(Vector2(1280.0f, 720.0f));*/
}

void TitleScene::Update() {

	//title->Update();

	model_->Update();
	worldTransform_->UpdateMatrix();
	camera_->Update();
#ifdef _DEBUG
	ImGui::Begin("Model Transform");

	Vector3 scale = model_->GetScale();
	Vector3 rotate = model_->GetRotate();
	Vector3 translate = model_->GetTranslate();

	// 슬라이더로 값 수정
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
	ImGui::DragFloat3("Translate", &translate.x, 0.1f);

	model_->SetScale(scale);
	model_->SetRotate(rotate);
	model_->SetTranslate(translate);
	auto obj = model_.get();
	if (obj) {
		ImGui::Separator();
		static Vector3 spotPos = { 0.0f, 5.0f, 5.0f };
		static Vector3 spotDir = { 0.0f, -1.0f, -1.0f };
		static float spotIntensity = 1.0f;
		static float cutoffDeg = 15.0f;
		static float outerCutoffDeg = 30.0f;
		static float spotDecay = 1.0f;
		static float spotRadius = 15.0f;

		ImGui::Text("Spot Light");
		ImGui::DragFloat3("Position", &spotPos.x, 0.1f);
		ImGui::DragFloat3("Direction", &spotDir.x, 0.1f);
		ImGui::DragFloat("Intensity", &spotIntensity, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Inner Cutoff (deg)", &cutoffDeg, 1.0f, 1.0f, 89.0f);
		ImGui::DragFloat("Outer Cutoff (deg)", &outerCutoffDeg, 1.0f, 1.0f, 89.0f);
		ImGui::DragFloat("Decay", &spotDecay, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Radius", &spotRadius, 0.1f, 0.0f, 100.0f);

		// 값 적용
		obj->SetSpotLight(
			spotPos,
			MyMath::normalize(spotDir),
			spotIntensity,
			cosf(MyMath::ToRadian(cutoffDeg)),
			cosf(MyMath::ToRadian(outerCutoffDeg)),
			spotDecay,
			spotRadius
		);
		
	}

	ImGui::End();
#endif

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");

		return;
	}

}

void TitleScene::Draw() {

	SpriteCommon::GetInstance()->CommonDrawSettings();
	//title->Draw();	
	//DirectXCommon::GetInstance()->PreDraw();
	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	model_->Draw();
}

void TitleScene::Finalize() {
	if (model_) {
		model_->Cleanup(); 
		model_.reset();
	}
	if (worldTransform_) {
		worldTransform_->Cleanup(); 
		worldTransform_.reset();
	}
    camera_.reset();
    
	title.reset();
}

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

	TextureManager::GetInstance()->LoadTexture("resources/title.png");

	title = std::make_unique<Sprite>();
	title->Initialize(SpriteCommon::GetInstance(), "resources/title.png");
	title->SetAnchorPoint(Vector2(0.5f, 0.5f));       // 중심 기준
	title->SetPosition(Vector2(640.0f, 360.0f));      // 화면 중심 위치
	title->SetSize(Vector2(1280.0f, 720.0f));         // 전체 화면 크기
	title->SetTextureLeftTop(Vector2(0.0f, 0.0f));
	title->SetTextureSize(Vector2(1280.0f, 720.0f));
	//title->SetPosition(Vector2(640.0f, 360.0f));    // 화면 중심에 위치
	////
	//TextureManager::GetInstance()->LoadTexture("resources/circle.png");
	//particleManager_ = std::make_unique<ParticleManager>();
	//particleManager_->Initialize(DirectXCommon::GetInstance(), SrvManager::GetInstance());
	//particleManager_->CreateParticleGroup("star", "resources/circle.png");
	//particleManager_->SetCamera(camera_.get());


	worldTransform_ = std::make_unique<WorldTransform>();
	worldTransform_->Initialize();

	testWorldTransform_ = std::make_unique<WorldTransform>();
	testWorldTransform_->Initialize();

	model_ = std::make_unique<Object3d>();
	model_->Initialize(Object3dCommon::GetInstance(), worldTransform_.get());

	ModelManager::GetInstance()->LoadModel("resources", "terrain.obj");
	model_->SetModel("terrain.obj");

	testModel_ = std::make_unique<Object3d>();
	testModel_->Initialize(Object3dCommon::GetInstance(), testWorldTransform_.get());

	ModelManager::GetInstance()->LoadModel("resources/test", "terrain.gltf");
	testModel_->SetModel("terrain.gltf");

	model_->SetEnableLighting(true);
	model_->SetIsBlinnPhong(true);          // Phong
	model_->SetUsePointLight(false);        // Point Light 
	model_->SetUseDirectionalLight(false);  // Directional Light 
	model_->SetUseSpotLight(false);	        // Spot Light
	model_->SetUseAmbientLight(false);      // Ambient Light
	model_->SetUseAreaLight(true);          // Area Light
	// 카메라 생성
	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f, 4.0f, -10.0f });
	camera_->SetTarget({ 0.0f, 0.0f, 0.0f });
	camera_->UpdateMatrix();

	// 모델에 카메라 설정
	model_->SetCamera(camera_.get());
	testModel_->SetCamera(camera_.get());

	// 씬 공통 렌더 설정 (이것도 보통 필요함)
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

	


	

}

void TitleScene::Update() {

	title->Update();

	model_->Update();
	testModel_->Update();
	worldTransform_->UpdateMatrix();
	testWorldTransform_->UpdateMatrix();
	camera_->Update();
#ifdef _DEBUG
	ImGui::Begin("Model Transform");

	Vector3 scale = testModel_->GetScale();
	Vector3 rotate = testModel_->GetRotate();
	Vector3 translate = testModel_->GetTranslate();

	// 슬라이더로 값 수정
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
	ImGui::DragFloat3("Translate", &translate.x, 0.1f);

	testModel_->SetScale(scale);
	testModel_->SetRotate(rotate);
	testModel_->SetTranslate(translate);
	auto obj = model_.get();
	if (obj) {
		static Vector3 areaPos = { 0.0f, 5.0f, 0.0f };
		static Vector3 areaRight = { 1.0f, 0.0f, 0.0f };
		static float areaHalfWidth = 2.0f;

		static Vector3 areaUp = { 0.0f, 1.0f, 0.0f };
		static float areaHalfHeight = 2.0f;

		static Vector4 areaColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		static float areaIntensity = 1.0f;


		ImGui::Separator();
		ImGui::Text("Area Light");

		ImGui::DragFloat3("Position", &areaPos.x, 0.1f);
		ImGui::DragFloat3("Right Vector", &areaRight.x, 0.1f);
		ImGui::DragFloat("Half Width", &areaHalfWidth, 0.1f, 0.0f, 10.0f);

		ImGui::DragFloat3("Up Vector", &areaUp.x, 0.1f);
		ImGui::DragFloat("Half Height", &areaHalfHeight, 0.1f, 0.0f, 10.0f);

		ImGui::ColorEdit4("Color", &areaColor.x);
		ImGui::DragFloat("Intensity", &areaIntensity, 0.1f, 0.0f, 10.0f);

		obj->SetAreaLight(areaPos, areaRight, areaHalfWidth, areaUp, areaHalfHeight, areaColor, areaIntensity);

	
		
	}

	ImGui::End();
#endif

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");

		return;
	}

	//Emitter emitter{};
	//emitter.transform.translate = { 0.0f, 5.0f, 4.0f }; // 발사 위치
	//emitter.count = 100;
	//emitter.frequency = 0.2f;
	//emitter.frequencyTime += 1.0f / 60.0f;

	//if (emitter.frequencyTime >= emitter.frequency) {
	//	emitter.frequencyTime = 0.0f;
	//	particleManager_->Emit(emitter, randomEngine_);
	//}

	//particleManager_->Update();

}

void TitleScene::Draw() {

	SpriteCommon::GetInstance()->CommonDrawSettings();
	title->Draw();	
	//particleManager_->Draw();
	

	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	model_->Draw();
	//testModel_->Draw();

	

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

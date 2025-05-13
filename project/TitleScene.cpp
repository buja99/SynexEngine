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
	model_->SetUseAreaLight(true);         
	
	testModel_->SetScale({ 14.0f, 1.0f, 9.0f });
	
	
	
	
	// Area Light
	// 카메라 생성
	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f, 4.0f, -10.0f });
	camera_->SetTarget({ 0.0f, 0.0f, 0.0f });
	
	// 모델에 카메라 설정
	model_->SetCamera(camera_.get());
	testModel_->SetCamera(camera_.get());

	// 씬 공통 렌더 설정 (이것도 보통 필요함)
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

	TextureManager::GetInstance()->LoadTexture("resources/title.png");

	title = std::make_unique<Sprite>();
	title->Initialize(SpriteCommon::GetInstance(), "resources/title.png");
	title->SetPosition({ 0.0f, 0.0f });
	

	/*particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(DirectXCommon::GetInstance(), SrvManager::GetInstance());
	particleManager_->SetCamera(camera_.get());
	particleManager_->CreateParticleGroup("star", "resources/circle.png");*/

}

void TitleScene::Update() {

	title->Update();

	model_->Update();
	testModel_->Update();
	worldTransform_->UpdateMatrix();
	testWorldTransform_->UpdateMatrix();
	camera_->UpdateMatrix();
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
	
	ImGui::End();

	ImGui::Begin("Light Settings");

	bool useDir = model_->GetUseDirectionalLight();
	bool usePoint = model_->GetUsePointLight();
	bool useSpot = model_->GetUseSpotLight();
	bool useAmbient = model_->GetUseAmbientLight();
	bool useArea = model_->GetUseAreaLight();

	// Light ON/OFF
	if (ImGui::Checkbox("Use Directional Light", &useDir)) {
		model_->SetUseDirectionalLight(useDir);
	}
	if (ImGui::Checkbox("Use Point Light", &usePoint)) {
		model_->SetUsePointLight(usePoint);
	}
	if (ImGui::Checkbox("Use Spot Light", &useSpot)) {
		model_->SetUseSpotLight(useSpot);
	}
	if (ImGui::Checkbox("Use Ambient Light", &useAmbient)) {
		model_->SetUseAmbientLight(useAmbient);
	}
	if (ImGui::Checkbox("Use Area Light", &useArea)) {
		model_->SetUseAreaLight(useArea);
	}

	// Point Light 상세 조정
	if (usePoint) {
		static Vector3 position = { 0.0f, 5.0f, 0.0f };
		static float intensity = 1.0f;
		static float radius = 10.0f;
		static float decay = 1.0f;

		ImGui::Text("Point Light Settings");
		ImGui::DragFloat3("Position##Point", &position.x, 0.1f);
		ImGui::DragFloat("Intensity##Point", &intensity, 0.1f);
		ImGui::DragFloat("Radius##Point", &radius, 0.1f);
		ImGui::DragFloat("Decay##Point", &decay, 0.1f);
		model_->SetPointLight(position, intensity, radius, decay);
	}

	// Spot Light 상세 조정
	if (useSpot) {
		static Vector3 position = { 0.0f, 5.0f, 5.0f };
		static Vector3 direction = { 0.0f, -1.0f, -1.0f };
		static float intensity = 1.0f;
		static float cutoff = 15.0f;
		static float outerCutoff = 30.0f;
		static float decay = 1.0f;
		static float radius = 15.0f;

		ImGui::Text("Spot Light Settings");
		ImGui::DragFloat3("Position##Spot", &position.x, 0.1f);
		ImGui::DragFloat3("Direction##Spot", &direction.x, 0.1f);
		ImGui::DragFloat("Intensity##Spot", &intensity, 0.1f);
		ImGui::DragFloat("Cutoff##Spot", &cutoff, 1.0f, 0.0f, 90.0f);
		ImGui::DragFloat("OuterCutoff##Spot", &outerCutoff, 1.0f, 0.0f, 90.0f);
		ImGui::DragFloat("Decay##Spot", &decay, 0.1f);
		ImGui::DragFloat("Radius##Spot", &radius, 0.1f);
		model_->SetSpotLight(position, direction, intensity,
			cosf(MyMath::ToRadian(cutoff)),
			cosf(MyMath::ToRadian(outerCutoff)),
			decay, radius);
	}

	// Area Light 상세 조정
	if (useArea) {
		static Vector3 position = { 0.0f, 5.0f, 0.0f };
		static Vector3 right = { 1.0f, 0.0f, 0.0f };
		static float halfWidth = 2.0f;
		static Vector3 up = { 0.0f, 1.0f, 0.0f };
		static float halfHeight = 2.0f;
		static Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		static float intensity = 1.0f;

		ImGui::Text("Area Light Settings");
		ImGui::DragFloat3("Position##Area", &position.x, 0.1f);
		ImGui::DragFloat3("Right##Area", &right.x, 0.1f);
		ImGui::DragFloat("Half Width##Area", &halfWidth, 0.1f);
		ImGui::DragFloat3("Up##Area", &up.x, 0.1f);
		ImGui::DragFloat("Half Height##Area", &halfHeight, 0.1f);
		ImGui::ColorEdit4("Color##Area", &color.x);
		ImGui::DragFloat("Intensity##Area", &intensity, 0.1f);
		model_->SetAreaLight(position, right, halfWidth, up, halfHeight, color, intensity);
	}

	ImGui::End();

	ImGui::Begin("sprite");
	//ImGui::DragFloat3();
	ImGui::End();

#endif

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");

		return;
	}
	//Emitter emitter{};
	//emitter.transform.translate = { 0.0f, 1.0f, 0.0f }; // 발사 위치
	//emitter.count = 10;
	//emitter.frequency = 0.2f;
	//emitter.frequencyTime += 1.0f / 60.0f;

	//if (emitter.frequencyTime >= emitter.frequency) {
	//	emitter.frequencyTime = 0.0f;
	//	particleManager_->Emit(emitter, randomEngine_);
	//}

	//particleManager_->Update();

}

void TitleScene::Draw() {

	SpriteCommon::GetInstance()->SetUIPipeline();
	title->Draw();
	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	//model_->Draw();
	testModel_->Draw();
	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	//particleManager_->Draw();

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

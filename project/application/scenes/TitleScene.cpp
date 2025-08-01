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

	ModelManager::GetInstance()->LoadModel("resources/test1", "head.obj");
	ModelManager::GetInstance()->LoadModel("resources/obj/tree", "tree.obj");
	model_->SetModel("tree.obj");

	testModel_ = std::make_unique<Object3d>();
	testModel_->Initialize(Object3dCommon::GetInstance(), testWorldTransform_.get());

	ModelManager::GetInstance()->LoadModel("resources/test", "terrain.gltf");
	testModel_->SetModel("terrain.gltf");

	testModel_->SetEnableLighting(true);
	testModel_->SetIsBlinnPhong(true);          // Phong
	testModel_->SetUsePointLight(false);        // Point Light 
	testModel_->SetUseDirectionalLight(false);  // Directional Light 
	testModel_->SetUseSpotLight(false);	        // Spot Light
	testModel_->SetUseAmbientLight(false);      // Ambient Light
	testModel_->SetUseAreaLight(true);         
	
	//testModel_->SetScale({ 14.0f, 1.0f, 9.0f });
	
	
	
	
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
	
	TextureManager::GetInstance()->LoadTexture("resources/gradationLine.png");
	TextureManager::GetInstance()->LoadTexture("resources/circle.png");
	
	effectLibrary_ = std::make_unique<ParticleEffectLibrary>();
	effectLibrary_->Initialize(DirectXCommon::GetInstance(), SrvManager::GetInstance(), camera_.get());

	effectLibrary_->EmitPrimitive({ 0.0f, 10.0f, 0.0f }, randomEngine_);
	effectLibrary_->SetUsePrimitiveAutoEmit(true);

	effectLibrary_->EmitRing({ 0.0f, 2.0f, 0.0f }, randomEngine_);
	effectLibrary_->SetUseRingAutoEmit(true);

	effectLibrary_->EmitCylinder({ 8.0f, 2.0f, 0.0f }, randomEngine_);
	effectLibrary_->SetUseCylinderAutoEmit(true);

}

void TitleScene::Update() {

	title->Update();

	model_->Update();
	testModel_->Update();
	worldTransform_->UpdateMatrix();
	testWorldTransform_->UpdateMatrix();
	camera_->Update();
	effectLibrary_->Update();
	
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
	
	ImGui::End();

	ImGui::Begin("Light Settings");

	bool useDir = testModel_->GetUseDirectionalLight();
	bool usePoint = testModel_->GetUsePointLight();
	bool useSpot = testModel_->GetUseSpotLight();
	bool useAmbient = testModel_->GetUseAmbientLight();
	bool useArea = testModel_->GetUseAreaLight();

	// Light ON/OFF
	if (ImGui::Checkbox("Use Directional Light", &useDir)) {
		testModel_->SetUseDirectionalLight(useDir);
	}
	if (ImGui::Checkbox("Use Point Light", &usePoint)) {
		testModel_->SetUsePointLight(usePoint);
	}
	if (ImGui::Checkbox("Use Spot Light", &useSpot)) {
		testModel_->SetUseSpotLight(useSpot);
	}
	if (ImGui::Checkbox("Use Ambient Light", &useAmbient)) {
		testModel_->SetUseAmbientLight(useAmbient);
	}
	if (ImGui::Checkbox("Use Area Light", &useArea)) {
		testModel_->SetUseAreaLight(useArea);
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
		testModel_->SetPointLight(position, intensity, radius, decay);
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
		testModel_->SetSpotLight(position, direction, intensity,
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
		testModel_->SetAreaLight(position, right, halfWidth, up, halfHeight, color, intensity);
	}

	ImGui::End();

	

	ImGui::Begin("sprite");
	//ImGui::DragFloat3();
	ImGui::End();


	ImGui::Begin("PostEffect");

	// === Grayscale ===
	if (ImGui::Checkbox("Use Grayscale", &useGrayscale_)) {
		if (useGrayscale_) {
			useVignette_ = false;
			useRadialBlur_ = false;
			DirectXCommon::GetInstance()->SetVignetteEnabled(false);
			DirectXCommon::GetInstance()->SetRadialBlurEnabled(false);
		}
		DirectXCommon::GetInstance()->SetGrayscaleEnabled(useGrayscale_);
	}

	float grayscaleStrength = DirectXCommon::GetInstance()->GetGrayscaleStrength();
	if (ImGui::SliderFloat("Grayscale Strength", &grayscaleStrength, 0.0f, 1.0f)) {
		DirectXCommon::GetInstance()->SetGrayscaleStrength(grayscaleStrength);
	}

	// === Vignette ===
	if (ImGui::Checkbox("Use Vignette", &useVignette_)) {
		if (useVignette_) {
			useGrayscale_ = false;
			useRadialBlur_ = false;
			DirectXCommon::GetInstance()->SetGrayscaleEnabled(false);
			DirectXCommon::GetInstance()->SetRadialBlurEnabled(false);
		}
		DirectXCommon::GetInstance()->SetVignetteEnabled(useVignette_);
	}

	float vignetteStrength = DirectXCommon::GetInstance()->GetVignetteStrength();
	if (ImGui::SliderFloat("Vignette Strength", &vignetteStrength, 0.0f, 1.0f)) {
		DirectXCommon::GetInstance()->SetVignetteStrength(vignetteStrength);
	}

	// === Radial Blur ===
	if (ImGui::Checkbox("Use RadialBlur", &useRadialBlur_)) {
		if (useRadialBlur_) {
			useGrayscale_ = false;
			useVignette_ = false;
			DirectXCommon::GetInstance()->SetGrayscaleEnabled(false);
			DirectXCommon::GetInstance()->SetVignetteEnabled(false);
		}
		DirectXCommon::GetInstance()->SetRadialBlurEnabled(useRadialBlur_);
	}

	// 샘플 수 조절
	int sampleCount = DirectXCommon::GetInstance()->GetRadialBlurNumSamples();
	if (ImGui::SliderInt("Samples", &sampleCount, 2, 64)) {
		DirectXCommon::GetInstance()->SetRadialBlurNumSamples(sampleCount);
	}

	// 블러 강도
	float strength = DirectXCommon::GetInstance()->GetRadialBlurStrength();
	if (ImGui::SliderFloat("Strength", &strength, 0.0f, 1.0f)) {
		DirectXCommon::GetInstance()->SetRadialBlurStrength(strength);
	}

	// 중심 위치
	float center[2] = {
	DirectXCommon::GetInstance()->GetRadialBlurCenterX(),
	DirectXCommon::GetInstance()->GetRadialBlurCenterY()
	};
	if (ImGui::SliderFloat2("Center", center, 0.0f, 1.0f)) {
		DirectXCommon::GetInstance()->SetRadialBlurCenter(center[0], center[1]);
	}

	ImGui::End();


#endif

	if (input_->TriggerKey(DIK_V)) {
		useVignette_ = !useVignette_;
		DirectXCommon::GetInstance()->SetVignetteEnabled(useVignette_);
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");

		return;
	}
	

	effectLibrary_->GetPrimitiveManager()->Update();
	effectLibrary_->GetRingManager()->Update();
	effectLibrary_->GetCylinderManager()->Update();
}

void TitleScene::Draw() {

	SpriteCommon::GetInstance()->SetUIPipeline();
	//title->Draw();
	Object3dCommon::GetInstance()->CommonDrawSettings();
	SrvManager::GetInstance()->PreDraw();

	//model_->Draw();
	testModel_->Draw();
	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	
	//effectLibrary_->GetPrimitiveManager()->Draw();
	//effectLibrary_->GetRingManager()->Draw();
	//effectLibrary_->GetCylinderManager()->Draw();
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

#include "ParticleScene.h"

ParticleScene::~ParticleScene() {
}

void ParticleScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();
	camera_ = std::make_unique<Camera>();

	TextureManager::GetInstance()->LoadTexture("resources/part.png");
	TextureManager::GetInstance()->LoadTexture("resources/back.png");

	background = std::make_unique<Sprite>();
	background->Initialize(SpriteCommon::GetInstance(), "resources/back.png");
	background->SetPosition({ 0.0f, 0.0f });


	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f, 4.0f, -10.0f });
	camera_->SetTarget({ 0.0f, 0.0f, 0.0f });
	camera_->SetTranslate({ 0.0f, 2.7f, -10.0f });

	TextureManager::GetInstance()->LoadTexture("resources/gradationLine.png");
	TextureManager::GetInstance()->LoadTexture("resources/circle.png");

	primitive_ = std::make_unique<ParticleManager>();
	primitive_->Initialize(DirectXCommon::GetInstance(), SrvManager::GetInstance());
	primitive_->SetCamera(camera_.get());
	primitive_->CreateParticleGroup("milkyway", "resources/circle.png");

	primitive_->SetEmitterCount("milkyway", 1);
	primitive_->SetEmitterFrequency("milkyway", 0.15f);
	primitive_->SetEmitterPosition("milkyway", { 0.0f, 0.0f, 0.0f });
	primitive_->SetUseBillboard(true);

	meteor_ = std::make_unique<ParticleManager>();
	meteor_->Initialize(DirectXCommon::GetInstance(), SrvManager::GetInstance());
	meteor_->SetCamera(camera_.get());
	meteor_->CreateParticleGroup("meteor", "resources/circle.png");

	meteor_->SetEmitterCount("meteor", 7);          
	meteor_->SetEmitterFrequency("meteor", 0.07f);  
	meteor_->SetEmitterPosition("meteor", { 0.0f, 10.0f, 0.0f });
	meteor_->SetUseBillboard(false);
}

void ParticleScene::Update() {

	//camera_->UpdateMatrix();
	camera_->Update();

	background->Update();

	primitive_->Update();

	meteor_->Update();
}

void ParticleScene::Draw() {

	SpriteCommon::GetInstance()->SetUIPipeline();
	
	background->Draw();

	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	
	//primitive_->Draw();
	//meteor_->Draw();
}

void ParticleScene::Finalize() {
}

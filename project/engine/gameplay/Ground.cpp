#include "Ground.h"

void Ground::Initialize() {

	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	worldTransform_ = std::make_unique<WorldTransform>();
	worldTransform_->Initialize();

	//worldTransform_->scale_ = { 100.0f, 1.0f, 100.0f };  
	worldTransform_->translate_ = { 0.0f, 0.0f, 0.0f };

	ground_ = std::make_unique<Object3d>();
	ground_->Initialize(Object3dCommon::GetInstance(), worldTransform_.get());

	ModelManager::GetInstance()->LoadModel("resources/ground", "ground.obj");
	ground_->SetModel("ground.obj");

	if (camera) {
		ground_->SetCamera(camera);
	}

	

}

void Ground::Update() {

	worldTransform_->UpdateMatrix();
	worldTransform_->TransferMatrix();

	ground_->Update();
}

void Ground::Draw() {

	ground_->Draw();

}

void Ground::Finalize() {

	ground_->Cleanup();

}

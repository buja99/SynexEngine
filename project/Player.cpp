#include "Player.h"

Player::~Player() {
}

void Player::Initialize() {

	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	ModelManager::GetInstance()->LoadModel("resources/player", "body.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "head.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "arm.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "leg.obj");
	
	for (int i = 0; i < kPlayerPartCount; ++i) {
		playerTransforms_[i] = std::make_unique<WorldTransform>();
		playerTransforms_[i]->Initialize();

		playerParts_[i] = std::make_unique<Object3d>();
		playerParts_[i]->Initialize(Object3dCommon::GetInstance(), playerTransforms_[i].get());
	}

	for (int i = 0; i < kPlayerPartCount; ++i) {
		if (i == BODY) continue;
		playerTransforms_[i]->parent_ = playerTransforms_[BODY].get(); // 계층 설정
	}

	playerParts_[ARM_R]->SetModel("arm.obj");
	playerParts_[ARM_L]->SetModel("arm.obj");
	playerParts_[LEG_R]->SetModel("leg.obj");
	playerParts_[LEG_L]->SetModel("leg.obj");
	playerParts_[BODY]->SetModel("body.obj");
	playerParts_[HEAD]->SetModel("head.obj");

	playerParts_[ARM_R]->SetTranslate({ 4.7f, 3.2f, 10.0f });
	playerParts_[ARM_L]->SetTranslate({ -4.7f, 3.2f, 10.0f });
	playerParts_[LEG_R]->SetTranslate({ -2.4f, -2.1f, 10.1f });
	playerParts_[LEG_L]->SetTranslate({ 2.5f, -2.1f, 10.1f });
	playerParts_[BODY]->SetTranslate({ 0.0f, 0.0f, 10.0f });
	playerParts_[HEAD]->SetTranslate({ 0.2f, 3.9f, 10.0f });


	





}

void Player::Updata() {

	for (auto& transform : playerTransforms_) {
		transform->UpdateMatrix();
	}

	// 예시: A, D 키 입력으로 BODY 좌우 이동
	Input* input = Input::GetInstance();

	if (input->PushKey(DIK_A)) {
		playerTransforms_[BODY]->translate_.x -= speed;
	}
	if (input->PushKey(DIK_D)) {
		playerTransforms_[BODY]->translate_.x += speed;
	}

	// position 값도 BODY 기준으로 업데이트
	position = playerTransforms_[BODY]->translate_;
}

void Player::Draw() {

	for (auto& part : playerParts_) {
		part->Draw();
	}
}

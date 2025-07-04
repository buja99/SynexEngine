#include "Player.h"
#include "ImGuiManager.h"
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

	
	playerTransforms_[ARM_R]->translate_ = { 4.7f, 3.2f, 0.0f };
	playerTransforms_[ARM_L]->translate_ = { -4.7f, 3.2f, 0.0f };
	playerTransforms_[LEG_R]->translate_ = { -2.4f, -2.1f, 0.1f };
	playerTransforms_[LEG_L]->translate_ = { 2.5f, -2.1f, 0.1f };
	playerTransforms_[BODY]->translate_ = { 0.0f, 5.0f, 0.0f };
	playerTransforms_[HEAD]->translate_ = { 0.2f, 3.9f, 0.0f };

	playerTransforms_[BODY]->scale_ = { 0.5f, 0.5f, 1.0f };
	playerTransforms_[HEAD]->scale_ = { 2.5f, 1.3f, 1.0f };

	if (camera_) {
		
	}




}

void Player::Updata() {

	Input* input = Input::GetInstance();
	Vector3 move = { 0.0f, 0.0f, 0.0f };
	if (input->PushKey(DIK_A)) {
		move.x -= 1.0f;
	}
	if (input->PushKey(DIK_D)) {
		move.x += 1.0f;
	}
	if (input->PushKey(DIK_W)) {
		move.z += 1.0f;
	}
	if (input->PushKey(DIK_S)) {
		move.z -= 1.0f;
	}
	if (move.x != 0.0f || move.z != 0.0f) {
		move = MyMath::normalize(move);  // ← Vector3에 Normalize 함수가 있어야 함
		move = MyMath::Multiply(move,speed);
	}

	playerTransforms_[BODY]->translate_ = MyMath::Add(playerTransforms_[BODY]->translate_, move);

	// 이동 후 범위 제한
	Vector3& pos = playerTransforms_[BODY]->translate_;
	float fieldLimitX = 150.0f;
	float fieldLimitZ = 150.0f;
	pos.x = std::clamp(pos.x, -fieldLimitX, fieldLimitX);
	pos.z = std::clamp(pos.z, -fieldLimitZ, fieldLimitZ);

	position = pos;

	playerTransforms_[BODY]->UpdateMatrix();
	playerTransforms_[BODY]->TransferMatrix();

	// 2. 자식들 업데이트
	for (int i = 0; i < kPlayerPartCount; ++i) {
		if (i == BODY) continue;
		playerTransforms_[i]->UpdateMatrix();
		playerTransforms_[i]->TransferMatrix();
	}


#ifdef _DEBUG
	ImGui::Begin("Player Model Transform");

	static const char* partNames[] = {
		"ARM_R", "ARM_L", "LEG_R", "LEG_L", "BODY", "HEAD"
	};

	for (int i = 0; i < kPlayerPartCount; ++i) {
		if (ImGui::CollapsingHeader(partNames[i])) {
			ImGui::DragFloat3("Position", &playerTransforms_[i]->translate_.x, 0.1f);
			ImGui::DragFloat3("Rotation", &playerTransforms_[i]->rotate_.x, 0.1f);
			ImGui::DragFloat3("Scale", &playerTransforms_[i]->scale_.x, 0.1f);
		}
	}

	ImGui::End();
#endif

}

void Player::Draw() {

	for (auto& part : playerParts_) {
		part->Draw();
	}
}

void Player::SetCamera(Camera* camera) {
	camera_ = camera;
	for (auto& part : playerParts_) {
		if (part) {
			part->SetCamera(camera_);
		}
	}
}

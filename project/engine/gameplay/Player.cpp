#include "Player.h"
#include <algorithm>
#include "ImGuiManager.h"

Player::~Player() {
}

void Player::Initialize() {

	ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

	ModelManager::GetInstance()->LoadModel("resources/player", "body.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "head.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "arm.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "leg.obj");
	ModelManager::GetInstance()->LoadModel("resources/player", "playerWeapon.obj");

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
	playerParts_[WEAPON]->SetModel("playerWeapon.obj");

	
	playerTransforms_[ARM_R]->translate_ = { 4.7f, 3.2f, 0.0f };
	playerTransforms_[ARM_L]->translate_ = { -4.7f, 3.2f, 0.0f };
	playerTransforms_[LEG_R]->translate_ = { -2.4f, -2.1f, 0.1f };
	playerTransforms_[LEG_L]->translate_ = { 2.5f, -2.1f, 0.1f };
	playerTransforms_[BODY]->translate_ = { 0.0f, 5.0f, 0.0f };
	playerTransforms_[HEAD]->translate_ = { 0.2f, 3.9f, 0.0f };
	playerTransforms_[WEAPON]->translate_ = { 4.7f, 3.2f, 0.0f };

	playerTransforms_[BODY]->scale_ = { 0.5f, 0.5f, 1.0f };
	playerTransforms_[HEAD]->scale_ = { 2.5f, 1.3f, 1.0f };
	playerTransforms_[WEAPON]->scale_ = { 1.0f, 4.5f, 1.0f };
	if (camera_) {
		
	}


	originalWeaponAngleX_ = playerTransforms_[WEAPON]->rotate_.x;

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

	// 공격
	if (!isAttacking_ && Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isAttacking_ = true;
		attackTimer_ = 0;
		originalWeaponAngleX_ = playerTransforms_[WEAPON]->rotate_.x;
	}

	if (isAttacking_) {
		const float maxSwing = 3.0f; // 회전량 (radian 기준, 약 90도면 1.57)
		float t = static_cast<float>(attackTimer_) / static_cast<float>(attackDuration_);

		// 공격 전반부는 증가, 후반부는 감소
		float swingAngle = 0.0f;
		if (t <= 0.5f) {
			swingAngle = maxSwing * (t * 2.0f); // 0 → max
		} else {
			swingAngle = maxSwing * (2.0f - t * 2.0f); // max → 0
		}

		playerTransforms_[WEAPON]->rotate_.x = originalWeaponAngleX_ + swingAngle;

		attackTimer_++;

		if (!hasHit_ && attackTimer_ == 15) {
			 
			hasHit_ = HitCheck();
		}

		if (attackTimer_ >= attackDuration_) {
			isAttacking_ = false;
			hasHit_ = false; // 공격이 끝나면 다시 초기화
			attackTimer_ = 0;
			playerTransforms_[WEAPON]->rotate_.x = originalWeaponAngleX_; // 원위치
		}
	}

#ifdef _DEBUG
	ImGui::Begin("Player Model Transform");

	static const char* partNames[] = {
		"ARM_R", "ARM_L", "LEG_R", "LEG_L", "BODY", "HEAD"," WEAPON"
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

void Player::HitEffectDraw() {
	if (hasHit_) {
		effectLibrary_->DrawPrimitive();
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

bool Player::HitCheck() {
	// 1. 무기의 AABB 계산
	Vector3 weaponPos = GetWeaponPosition();
	Vector3 weaponHalfSize = playerTransforms_[WEAPON]->scale_; // 크기는 무기 사이즈에 맞게 조정

	Vector3 weaponMin = MyMath::Subtract(weaponPos,weaponHalfSize);
	Vector3 weaponMax = MyMath::Add(weaponPos,weaponHalfSize);

	bool didHit = false;

	// 2. 각 적의 AABB와 비교
	for (auto& enemy : enemies_) {
		Vector3 enemyPos = enemy->GetWorldTransform().translate_;
		Vector3 enemyHalfSize = enemy->GetWorldTransform().scale_; // 적 사이즈도 설정

		Vector3 enemyMin = MyMath::Subtract(enemyPos,enemyHalfSize);
		Vector3 enemyMax = MyMath::Add(enemyPos,enemyHalfSize);
		
		// 3. AABB 충돌 판정
		if (MyMath::IsAABBCollision(weaponMin, weaponMax, enemyMin, enemyMax)) {

			enemy->OnHit(10.0f);

			// 충돌 지점 계산
			Vector3 hitMin = {
				(std::max)(weaponMin.x, enemyMin.x),
				(std::max)(weaponMin.y, enemyMin.y),
				(std::max)(weaponMin.z, enemyMin.z)
			};
			Vector3 hitMax = {
				(std::min)(weaponMax.x, enemyMax.x),
				(std::min)(weaponMax.y, enemyMax.y),
				(std::min)(weaponMax.z, enemyMax.z)
			};
			Vector3 hitCenter = MyMath::Multiply(MyMath::Add(hitMin, hitMax), 0.5f);
			//hitCenter.x -= 4.0f; // 무기 위치 조정
			//hitCenter.y += 4.0f;
			effectLibrary_->EmitPrimitive(hitCenter, *randomEngine_);

			didHit = true;
		}
	}
	return didHit;
}

const Vector3& Player::GetWeaponPosition() {
	return playerTransforms_[WEAPON]->translate_;
}
void Player::SetEffectLibrary(ParticleEffectLibrary* effectLibrary) {
	effectLibrary_ = effectLibrary;
}

void Player::SetRandomEngine(std::mt19937* engine) {
	randomEngine_ = engine;
}





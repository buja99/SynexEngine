#include "Enemy.h"
#include "ImGuiManager.h"
Enemy::Enemy() {
}
void Enemy::Initialize() {
    ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());
    ModelManager::GetInstance()->LoadModel("resources/enemy", "enemyTest.obj");

    enemyWorldTransform_ = std::make_unique<WorldTransform>();
	enemyWorldTransform_->Initialize();

	enemyModel_ = std::make_unique<Object3d>();
	enemyModel_->Initialize(Object3dCommon::GetInstance(), enemyWorldTransform_.get());
    enemyModel_->SetModel("enemyTest.obj");
    enemyModel_->SetUseEnvironmentMap(true);
    enemyModel_->SetEnvironmentMap("resources/rostock_laage_airport_4k.dds");
	enemyWorldTransform_->translate_ = { 0.0f,5.0f,0.0f };
	//enemyWorldTransform_->scale_ = { 12.0f, 3.0f, 3.0f };
    isHit_ = false;
}

void Enemy::Update() {


	Input* input = Input::GetInstance();
	Vector3 move = { 0.0f, 0.0f, 0.0f };
	float speed = 0.5f;
	if (input->PushKey(DIK_LEFT)) {
		move.x -= 1.0f;
	}
	if (input->PushKey(DIK_RIGHT)) {
		move.x += 1.0f;
	}
	if (input->PushKey(DIK_UP)) {
		move.z += 1.0f;
	}
	if (input->PushKey(DIK_DOWN)) {
		move.z -= 1.0f;
	}
	if (move.x != 0.0f || move.z != 0.0f) {
		move = MyMath::normalize(move);  // ← Vector3에 Normalize 함수가 있어야 함
		move = MyMath::Multiply(move, speed);
	}

	enemyWorldTransform_->translate_ = MyMath::Add(enemyWorldTransform_->translate_, move);

	enemyWorldTransform_->UpdateMatrix();
    enemyWorldTransform_->TransferMatrix();

}

void Enemy::Draw() {
	enemyModel_->Draw();
   
}

int Enemy::GetType() const {
    return 0; // 예: 기본 enemy type
}

void Enemy::OnHit(float damage) {
    
    OutputDebugStringA("Enemy::OnHit hit!\n");

    hp_ -= damage;
    if (hp_ <= 0.0f) {
        isDead_ = true;
    }

    Vector3 pos = enemyWorldTransform_->translate_;
    //pos.y += 2.0f;
   
     // 또는: effectLibrary_->EmitHitEffect(pos, ...);
     isHit_ = true;
}
void Enemy::SetCamera(Camera* camera) {
	camera_ = camera;
	
}
WorldTransform& Enemy::GetWorldTransform() {
	return *enemyWorldTransform_;
}



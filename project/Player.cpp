#include "Player.h"

void Player::Initialize(const std::vector<Object3d*>& playerParts, Object3d* bulletModel, Object3dCommon* Object3dCommon)
{
    this->playerParts_ = playerParts;
    this->bulletModel_ = bulletModel;
    this->Object3dCommon_ = Object3dCommon;
	position = { 0.0f, 0.0f, 50.0f };

    positionB = { 0.0f, 5.0f, 0.0f };
}

void Player::Updata(Input* input)
{
    if (input->PushKey(DIK_W)) {
        position.z -= speed;
        UpdateAnimation();
    }
    if (input->PushKey(DIK_S)) {
        position.z += speed;
        UpdateAnimation();
    }
    if (input->PushKey(DIK_A)) {
        position.x += speed; 
        UpdateAnimation();
    }
    if (input->PushKey(DIK_D)) {
        position.x -= speed; 
        UpdateAnimation();
    }
    if (input->PushKey(DIK_SPACE)) {
        if (playerParts_.size() > 6) {
            Shoot(bulletModel_);  
        }
    }
    for (auto& bullet : bullets_) {
        bullet.Update();
    }

    // 죽은 총알 제거
    bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
        [](const Bullet& bullet) { return !bullet.IsAlive(); }),
        bullets_.end());
}

void Player::Shoot(Object3d* bulletModel)
{
    
    Vector3 gunPosition = playerParts_[6]->GetTranslate();
    Vector3 bulletStartPos = math->Add(gunPosition, positionB);  // 총알 시작 위치
    Vector3 bulletDirection = { 0.0f, 0.0f, 1.0f };  // Z축 방향으로 발사
    float bulletSpeed = 2.0f;
    float bulletLifetime = 2.0f;  // 총알 생존 시간 2초

    //auto bulletObject = std::make_unique<Object3d>();
    //bulletObject->Initialize(Object3dCommon_);
    //bulletObject->SetModel("bullet.obj");
    //bulletObject->SetTranslate(bulletStartPos);

    Bullet bullet;
    bullet.Initialize(bulletModel, bulletStartPos, bulletDirection, bulletSpeed, bulletLifetime);
    bullets_.emplace_back(bullet);
}

void Player::UpdateAnimation()
{
    float animationSpeed = 3.0f;   
    float time = static_cast<float>(GetTickCount64()) / 1000.0f;

    float swingAngle = std::sin(time * animationSpeed) * 10.0f;

    playerParts_[1]->SetRotate({ -swingAngle, 0.0f, 0.0f });  // legL
    playerParts_[2]->SetRotate({ swingAngle, 0.0f, 0.0f });   // legR

}



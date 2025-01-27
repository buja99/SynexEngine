#include "Player.h"


Player::~Player() {
    //delete bulletObject3d_;  
}

void Player::Initialize(const std::vector<Object3d*>& playerParts, Object3dCommon* Object3dCommon)
{
    this->playerParts_ = playerParts;
    //this->bulletObject3d_ = std::move(bulletObject3d);
    this->Object3dCommon_ = Object3dCommon;
	position = { 0.0f, 0.0f, 50.0f };
    positionB = { 0.0f, 5.0f, 0.0f };
    playerParts_[1]->SetTranslate({ 1.0f,4.0f,50.0f });
    playerParts_[2]->SetTranslate({ -1.0f,4.0f,50.0f });
}

void Player::Updata(Input* input)
{
    if (input->PushKey(DIK_W)) {
        position.z -= speed;
        UpDownAnimation();

    }
    if (input->PushKey(DIK_S)) {
        position.z += speed;
        UpDownAnimation();
    }
    if (input->PushKey(DIK_A)) {
        position.x += speed; 
        LightLeftAnimation();
    }
    if (input->PushKey(DIK_D)) {
        position.x -= speed; 
        LightLeftAnimation();
    }
    if (input->PushKey(DIK_SPACE)) {
        if (playerParts_.size() > 6) {
            Shoot(bulletModel_);  
        }
    }
    for (auto& bullet : bullets_) {
        bullet->Update();
    }

    // 죽은 총알 제거
    bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
        [](const std::unique_ptr<Bullet>& bullet) {
            return !bullet->IsAlive(); 
        }),
        bullets_.end());
}

void Player::Shoot(Model* bulletModel)
{
    
    auto bulletObject = std::make_unique<Object3d>();
    bulletObject->Initialize(Object3dCommon_);
    bulletObject->SetModel("bullet.obj");
   // bulletObject->SetRotate({ 90.0f,90.0f,90.0f });
    bulletObject->SetScale({ 0.01f,0.01f,0.01f });
    Vector3 gunPosition = playerParts_[6]->GetTranslate();
    Vector3 bulletStartPos = math->Add(gunPosition, positionB);
    Vector3 bulletDirection = { 0.0f, 0.0f, 1.0f };  // Z축 방향

    float bulletSpeed = 2.0f;
    float bulletLifetime = 20.0f;

    auto bullet = std::make_unique<Bullet>();
    bullet->Initialize(std::move(bulletObject), bulletStartPos, bulletDirection, bulletSpeed, bulletLifetime);
    //std move
    // 총알 벡터에 추가
    //bullets_.emplace_back(std::move(bullet));
    bullets_.push_back(std::move(bullet));
}

void Player::Darw()
{



}

void Player::UpDownAnimation()
{
    float animationSpeed = 5.0f;   
    float time = static_cast<float>(GetTickCount64()) / 1000.0f;

    float swingAngle = std::sinf(time * animationSpeed) * 1.0f;

    playerParts_[1]->SetRotate({ -swingAngle, 0.0f, 0.0f });  // legL
    playerParts_[2]->SetRotate({ swingAngle, 0.0f, 0.0f });   // legR

}

void Player::LightLeftAnimation()
{
    float animationSpeed = 8.0f;
    float time = static_cast<float>(GetTickCount64()) / 1000.0f;

    float swingAngle = std::cosf(time * animationSpeed) * 0.5f;

    playerParts_[1]->SetRotate({ 0.0f,0.0f ,-swingAngle });  // legL
    playerParts_[2]->SetRotate({ 0.0f, 0.0f, swingAngle });   // legR



}



#include "Player.h"

void Player::Initialize(const std::vector<Object3d*>& playerParts)
{
    this->playerParts_ = playerParts;
	position = { 0.0f, 0.0f, 50.0f };
}

void Player::Updata(Input* input)
{
    if (input->PushKey(DIK_W)) {
        position.z -= speed;
    }
    if (input->PushKey(DIK_S)) {
        position.z += speed;
    }
    if (input->PushKey(DIK_A)) {
        position.x += speed; 
    }
    if (input->PushKey(DIK_D)) {
        position.x -= speed; 
    }
    UpdateAnimation();
}

void Player::UpdateAnimation()
{
    float animationSpeed = 5.0f;   
    float time = static_cast<float>(GetTickCount64()) / 1000.0f;

    float swingAngle = std::sin(time * animationSpeed) * 30.0f;

    playerParts_[4]->SetRotate({ swingAngle, 0.0f, 0.0f });   // armL
    playerParts_[5]->SetRotate({ -swingAngle, 0.0f, 0.0f });  // armR
    playerParts_[1]->SetRotate({ -swingAngle, 0.0f, 0.0f });  // legL
    playerParts_[2]->SetRotate({ swingAngle, 0.0f, 0.0f });   // legR

}



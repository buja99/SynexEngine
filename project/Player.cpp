#include "Player.h"
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

Player::~Player() {
    //delete bulletObject3d_;  
}

void Player::Initialize(const std::vector<Object3d*>& playerParts, Object3dCommon* Object3dCommon)
{
    this->playerParts_ = playerParts;
    //this->bulletObject3d_ = std::move(bulletObject3d);
    this->Object3dCommon_ = Object3dCommon;
	position = { 0.0f, 0.0f, 50.0f };
    positionB = { 0.0f, 5.0f, 50.0f };
    playerParts_[1]->SetTranslate({ 1.0f,4.0f,50.0f });
    playerParts_[2]->SetTranslate({ -1.0f,4.0f,50.0f });
    playerParts_[6]->SetTranslate({ 0.0f,0.0f,50.0f });
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
       
            Shoot(bulletModel_);  
       
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

#ifdef _DEBUG
    ImGui::Begin("payer");
    ImGui::DragFloat3("position", &position.x, 0.1f);
    ImGui::Text("Gun Position: %.2f, %.2f, %.2f", playerParts_[6]->GetTranslate().x, playerParts_[6]->GetTranslate().y, playerParts_[6]->GetTranslate().z);
    ImGui::Text("Bullets (%d)", static_cast<int>(bullets_.size()));
   // for (size_t i = 0; i < playerParts_.size(); i++) {
   //     std::string label = "Part " + std::to_string(i);
   //     Vector3 partPos = playerParts_[i]->GetTranslate();
   //     Vector3 partRot = playerParts_[i]->GetRotate();
   //     Vector3 partScale = playerParts_[i]->GetScale();
   //
   //     ImGui::Separator();
   //     ImGui::Text("%s", label.c_str());
   //     ImGui::DragFloat3((label + " Pos").c_str(), &partPos.x, 0.1f);
   //     ImGui::DragFloat3((label + " Rot").c_str(), &partRot.x, 0.1f);
   //     ImGui::DragFloat3((label + " Scale").c_str(), &partScale.x, 0.1f);
   // }
    for (size_t i = 0; i < bullets_.size(); i++) {
        Vector3 bulletPos = bullets_[i]->GetPosition();
        std::string label = "Bullet " + std::to_string(i);
        ImGui::DragFloat3(label.c_str(), &bulletPos.x, 0.1f);
    }
    ImGui::End();
#endif // _DEBUG


}

void Player::Shoot(Model* bulletModel)
{
    
    auto bulletObject = std::make_unique<Object3d>();
    bulletObject->Initialize(Object3dCommon_);
    bulletObject->SetModel("resources/bullet/bullet.obj");
   // bulletObject->SetRotate({ 90.0f,90.0f,90.0f });
    bulletObject->SetScale({ 0.01f,0.01f,0.01f });
    Vector3 gunPosition = playerParts_[6]->GetTranslate();
    //Vector3 bulletStartPos = math->Add(gunPosition, positionB);
    Vector3 bulletDirection = { 0.0f, 0.0f, 1.0f };  // Z축 방향

    float bulletSpeed = 0.1f;
    float bulletLifetime = 10.0f;

    auto bullet = std::make_unique<Bullet>();
    bullet->Initialize(std::move(bulletObject), position, bulletDirection, bulletSpeed, bulletLifetime);
    //std move
    // 총알 벡터에 추가
    //bullets_.emplace_back(std::move(bullet));
    bullets_.push_back(std::move(bullet));
}

void Player::Draw()
{
    for (auto& part : playerParts_) {
        if (part) {
            part->Draw();
        }
    }

    // 총알 그리기
    for (auto& bullet : bullets_) {
        bullet->Draw();
    }



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



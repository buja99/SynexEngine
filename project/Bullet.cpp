#include "Bullet.h"

Bullet::~Bullet()
{
    model_.reset();
}

void Bullet::Initialize(std::unique_ptr<Object3d> model, const Vector3& startPos, const Vector3& direction, float speed, float lifetime)
{

    model_ = std::move(model);
    position_ = startPos;
    direction_ = direction;
    speed_ = speed;
    lifetime_ = lifetime;

}

void Bullet::Update()
{

    if (lifetime_ <= 0.0f) {
        isAlive_ = false;
        return;
    }
  
    lifetime_ -= 1.0f / 60.0f;  // Assuming 60 FPS
    position_ =math->Add(position_ ,math->Multiply(speed_, direction_)) ;
    model_->Updata();
    model_->SetTranslate(position_);
}

void Bullet::Draw()
{

    if (model_) {
        model_->Draw();
    }


}

#include "Enemy.h"

void Enemy::Initialize(Object3d* model)
{
	model_ = model;
}

void Enemy::SetPosition(const Vector3& position)
{
	position_ = position;
	model_->SetTranslate(position_);
}

void Enemy::SetScale(const Vector3& scale)
{
	scale_ = scale;
	model_->SetScale(scale_);
}

void Enemy::Update()
{
	model_->Updata();
	model_->SetTranslate(position_);

}

void Enemy::Draw()
{
	model_->Draw();
}

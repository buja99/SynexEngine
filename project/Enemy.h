#pragma once
#include "Object3d.h"
#include "Vector3.h"
class Enemy
{
public:
    void Initialize(Object3d* model);
    void SetPosition(const Vector3& position);
    void SetScale(const Vector3& scale);
    void Update();
    void Draw();
    bool IsAlive() const { return isAlive_; }
    void Destroy() { isAlive_ = false; }

    const Vector3& GetPosition() const { return position_; }
    const Vector3& GetScale() const { return scale_; }

private:
    Object3d* model_ = nullptr;
    Vector3 position_ = { 0.0f, 3.0f, 50.0f };
    Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
    bool isAlive_ = true;

};


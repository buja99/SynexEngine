#pragma once
#include "Vector3.h"
#include "Object3d.h"
#include "MyMath.h"
class Bullet
{

public:
    void Initialize(Object3d* model, const Vector3& startPos, const Vector3& direction, float speed, float lifetime);
    void Update();
    bool IsAlive() const { return isAlive_; }
    Object3d* GetObject() const { return model_; }
    const Vector3& GetPosition() const { return position_; }
private:
    Object3d* model_ = nullptr;
    Vector3 position_;
    Vector3 direction_;
    float speed_ = 1.0f;
    float lifetime_ = 0.0f;
    bool isAlive_ = true;

    MyMath* math = nullptr;

};


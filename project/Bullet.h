#pragma once
#include "Vector3.h"
#include "Object3d.h"
#include "MyMath.h"
class Bullet
{
    
public:
    ~Bullet();
    void Initialize(std::unique_ptr<Object3d> model, const Vector3& startPos, const Vector3& direction, float speed, float lifetime);
    void Update();
    void Draw();
    bool IsAlive() const { return isAlive_; }
    Object3d* GetObject() const { return model_.get(); }
    const Vector3& GetPosition() const { return position_; }
private:
    std::unique_ptr<Object3d> model_;
    Vector3 position_;
    Vector3 direction_;
    float speed_ = 1.0f;
    float lifetime_ = 0.0f;
    bool isAlive_ = true;

    MyMath* math = nullptr;

};


#pragma once
#include "Matrix4x4.h"
class SpriteCamera {
public:
    void Initialize(float width, float height, float nearZ = 0.0f, float farZ = 1.0f);

    const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
    const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }

private:
    Matrix4x4 viewMatrix_;
    Matrix4x4 projectionMatrix_;
};


#include "SpriteCamera.h"
#include "MyMath.h"

using namespace MyMath;

void SpriteCamera::Initialize(float width, float height, float nearZ, float farZ) {
    viewMatrix_ = MyMath::Identity(); // 2D 스프라이트는 고정된 View

    projectionMatrix_ = MyMath::MakeOrthographicMatrix(
        0.0f, width,
        height, 0.0f, // Y축 반전 (Top-Left 기준)
        nearZ, farZ
    );
}

#include "Camera.h"
#include "WinApp.h" 
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

Camera::Camera()
    : transform_({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 7.0f, -10.0f} }) {
    aspectRatio_ = static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight);
}

void Camera::Update()
{
    // 카메라 행렬 계산
    if (mode_ == CameraMode::Transform) {
        worldMatrix_ = MyMath::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
        viewMatrix_ = MyMath::Inverse(worldMatrix_);
    } else {
        viewMatrix_ = MyMath::CreateLookAtMatrix(eye_, target_, up_);
    }

    projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(fov_, aspectRatio_, nearZ_, farZ_);
    viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);

#ifdef _DEBUG
    ImGui::Begin("Camera Debug");

    // 모드 선택 UI
    static int selectedMode = static_cast<int>(mode_);
    const char* modeNames[] = { "Transform", "LookAt" };

    if (ImGui::Combo("Camera Mode", &selectedMode, modeNames, IM_ARRAYSIZE(modeNames))) {
        mode_ = static_cast<CameraMode>(selectedMode);
    }

    // 모드에 따른 위치/회전 조절
    if (mode_ == CameraMode::Transform) {
        ImGui::DragFloat3("Translate", &transform_.translate.x, 0.1f);
        ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f);
    } else {
        ImGui::DragFloat3("Eye", &eye_.x, 0.1f);
        ImGui::DragFloat3("Target", &target_.x, 0.1f);
        ImGui::DragFloat3("Up", &up_.x, 0.1f);
    }

    // 공통 설정
    ImGui::DragFloat("FOV", &fov_, 0.01f, 0.1f, 3.14f);
    ImGui::DragFloat("Aspect Ratio", &aspectRatio_, 0.01f, 0.1f, 10.0f);
    ImGui::DragFloat("Near Clip", &nearZ_, 0.01f, 0.01f, 10.0f);
    ImGui::DragFloat("Far Clip", &farZ_, 1.0f, 10.0f, 10000.0f);

    ImGui::End();
#endif
}




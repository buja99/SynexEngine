#include "Camera.h"
#include "WinApp.h" 
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

Camera::Camera()
    : transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 7.0f, -10.0f} }) {
    aspectRatio_ = static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight);
}

void Camera::Update()
{
    worldMatrix_ = MyMath::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    viewMatrix_ = MyMath::Inverse(worldMatrix_);
    projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(fov_, aspectRatio_, nearZ_, farZ_);
    viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);

#ifdef _DEBUG
    ImGui::Begin("Camera Transform Mode");
    ImGui::DragFloat3("Position", &transform.translate.x, 0.1f);
    ImGui::DragFloat3("Rotation", &transform.rotate.x, 0.1f);
    ImGui::DragFloat("FOV", &fov_, 0.01f, 0.1f, 3.14f);
    ImGui::DragFloat("Aspect Ratio", &aspectRatio_, 0.01f, 0.1f, 10.0f);
    ImGui::DragFloat("Near Clip", &nearZ_, 0.01f, 0.01f, 10.0f);
    ImGui::DragFloat("Far Clip", &farZ_, 1.0f, 10.0f, 10000.0f);
    ImGui::End();
#endif

}

void Camera::UpdateMatrix() {
    viewMatrix_ = MyMath::CreateLookAtMatrix(eye_, target_, up_);
    projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(fov_, aspectRatio_, nearZ_, farZ_);
    viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);

#ifdef _DEBUG
    ImGui::Begin("Camera LookAt Mode");
    ImGui::DragFloat3("Eye", &eye_.x, 0.1f);
    ImGui::DragFloat3("Target", &target_.x, 0.1f);
    ImGui::DragFloat3("Up", &up_.x, 0.1f);
    ImGui::DragFloat("FOV", &fov_, 0.01f, 0.1f, 3.14f);
    ImGui::DragFloat("Aspect Ratio", &aspectRatio_, 0.01f, 0.1f, 10.0f);
    ImGui::DragFloat("Near Clip", &nearZ_, 0.01f, 0.01f, 10.0f);
    ImGui::DragFloat("Far Clip", &farZ_, 1.0f, 10.0f, 10000.0f);
    ImGui::End();
#endif
}

//Camera::Camera()
//
//    : transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} })
//    ,horizontalFOV(0.45f)
//    ,aspectRatio(static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight))
//    ,nearClipDistance(0.1f)
//    ,farClipDistance(100.0f)
//    ,worldMatrix(MyMath::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate))
//    ,viewMatrix(MyMath::Inverse(worldMatrix))
//    ,projectionMatrix(MyMath::MakePerspectiveFovMatrix(horizontalFOV, aspectRatio, nearClipDistance, farClipDistance))
//    ,viewProjectionMatrix(MyMath::Multiply(viewMatrix, projectionMatrix))
//{}



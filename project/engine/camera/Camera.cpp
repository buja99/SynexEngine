#include "Camera.h"
#include "WinApp.h" 
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

void Camera::Update()
{
	worldMatrix = myMath->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix = myMath->Inverse(worldMatrix);
	projectionMatrix = myMath->MakePerspectiveFovMatrix(horizontalFOV, aspectRatio, nearClipDistance, farClipDistance);
	viewProjectionMatrix = myMath->Multiply(viewMatrix, projectionMatrix);

#ifdef _DEBUG
   
    ImGui::Begin("Camera Debug");
    ImGui::DragFloat3("Position", &transform.translate.x, 0.1f);
    ImGui::DragFloat3("Rotation", &transform.rotate.x, 0.1f);
    ImGui::DragFloat("FOV", &horizontalFOV, 0.01f, 0.1f, 3.14f);
    ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.01f, 0.1f, 10.0f);
    ImGui::DragFloat("Near Clip", &nearClipDistance, 0.01f, 0.01f, 1.0f);
    ImGui::DragFloat("Far Clip", &farClipDistance, 1.0f, 10.0f, 1000.0f);
    ImGui::End();
#endif

}

Camera::Camera()

    : transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} })
    ,horizontalFOV(0.45f)
    ,aspectRatio(static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight))
    ,nearClipDistance(0.1f)
    ,farClipDistance(100.0f)
    ,worldMatrix(myMath->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate))
    ,viewMatrix(myMath->Inverse(worldMatrix))
    ,projectionMatrix(myMath->MakePerspectiveFovMatrix(horizontalFOV, aspectRatio, nearClipDistance, farClipDistance))
    ,viewProjectionMatrix(myMath->Multiply(viewMatrix, projectionMatrix))
{}



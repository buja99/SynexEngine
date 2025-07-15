#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include "MyMath.h"
#include "Vector3.h"
#include <memory>

struct CameraForGPU {
	Vector3 worldPosition;
};

enum class CameraMode {
	Transform, // 위치+회전 기반
	LookAt     // Eye, Target, Up 기반
};

class Camera
{

public:

	void Update();

	//void UpdateMatrix();

	Camera();

	// --- Setter ---
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	void SetFOV(float fovY) { fov_ = fovY; }
	void SetAspectRatio(float aspect) { aspectRatio_ = aspect; }
	void SetNearClip(float nearClip) { nearZ_ = nearClip; }
	void SetFarClip(float farClip) { farZ_ = farClip; }

	void SetEye(const Vector3& eye) { eye_ = eye; }
	void SetTarget(const Vector3& target) { target_ = target; }
	void SetUp(const Vector3& up) { up_ = up; }

	// --- Getter ---
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }

	const Vector3& GetEye() const { return eye_; }
	const Vector3& GetTarget() const { return target_; }
	const Vector3& GetUp() const { return up_; }
	
private:

	CameraMode mode_ = CameraMode::Transform;

	// transform 기반
	Transform transform_;
	Matrix4x4 worldMatrix_;

	// 공통
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjectionMatrix_;

	float fov_ = 0.45f * float(M_PI);
	float aspectRatio_ = 16.0f / 9.0f;
	float nearZ_ = 0.1f;
	float farZ_ = 1000.0f;

	// LookAt 기반
	Vector3 eye_ = { 0.0f, 0.0f, -5.0f };
	Vector3 target_ = { 0.0f, 0.0f, 0.0f };
	Vector3 up_ = { 0.0f, 1.0f, 0.0f };

	

private:
	


};


#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include "MyMath.h"
#include "Vector3.h"
class Camera
{

public:

	void Update();

	Camera();

	//Setter
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetFOVY(float fovY) { horizontalFOV = fovY; }
	void SetAspectRatio(float aspect) { aspectRatio = aspect; }
	void SetNearClip(float nearClip) { nearClipDistance = nearClip; }
	void SetFarClip(float farClip) { farClipDistance = farClip; }
	

	//Getter
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	

private:
	Transform transform;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 viewProjectionMatrix;
	float horizontalFOV;   
	float aspectRatio;     
	float nearClipDistance; 
	float farClipDistance;  

	

	MyMath* myMath = new MyMath;

private:
	


};


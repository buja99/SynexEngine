#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Geo.h"
#include <assimp/scene.h>

namespace  MyMath
{


	Vector3 Add(const Vector3& v1, const Vector3& v2);
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	Vector3 divide(const Vector3& v, float s);
	Vector3 AddVector2AndVector3(const Vector2& vec2, const Vector3& vec3);
	Vector3 Multiply(const Vector3& vec, float scalar);
	float length(Vector3 distance);
	Vector3 normalize(Vector3 distance);
	float Dot(Vector3 c, Vector3 d);
	Vector3 cross(const Vector3& u, const Vector3& v);
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	Matrix4x4 MakeRotateXMatrix(float radian);
	Matrix4x4 MakeRotateYMatrix(float radian);
	Matrix4x4 MakeRotateZMatrix(float radian);
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
	Vector3 Transforms(const Vector3& vector, const Matrix4x4& matrix);
	Matrix4x4 Inverse(const Matrix4x4& m);
	Matrix4x4 MakeIdentity4x4();
	Matrix4x4 CreateLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
	Matrix4x4 Transpose(const Matrix4x4& m);
	float ToRadian(float degree);
	Vector3 SphericalToCartesian(float radius, float theta, float phi);
	Vector3 Project(const Vector3& v1, const Vector3& v2);
	Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
	Vector3 Perpendicular(const Vector3& vector);
	void EnsureAABB(AABB& aabb);
	Vector3 ClosestPointOnAABB(const Vector3& point, const AABB& aabb);
	Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
	Matrix4x4 ConvertMatrix( aiMatrix4x4& aiMat);
};


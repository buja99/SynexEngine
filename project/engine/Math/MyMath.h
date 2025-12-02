#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Geo.h"
#include <assimp/scene.h>
#include "WorldTransform.h"

namespace  MyMath
{


	Vector3 Add(const Vector3& v1, const Vector3& v2);
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	Vector3 divide(const Vector3& v, float s);
	Vector3 AddVector2AndVector3(const Vector2& vec2, const Vector3& vec3);
	Vector3 Multiply(const Vector3& vec, float scalar);
	inline Vector3 Multiply(float scalar, const Vector3& vec) {
		return Multiply(vec, scalar);
	}
	float length(Vector3 distance);
	Vector3 normalize(Vector3 distance);
	float Dot(Vector3 c, Vector3 d);
	Vector3 cross(const Vector3& u, const Vector3& v);
	Matrix4x4 Identity();
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	Matrix4x4 MakeRotateMatrix(const Vector3& rotate);
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
	bool IsAABBCollision(const Vector3& aMin, const Vector3& aMax, const Vector3& bMin, const Vector3& bMax);
	Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
	Matrix4x4 ConvertMatrix( aiMatrix4x4& aiMat);
	float Distance(const Vector3& a, const Vector3& b);
	Vector3 GetTranslate(const Matrix4x4& m);
	OBB MakeOBB(const WorldTransform& wt, const Vector3& modelSize);
	bool IsOBBCollision(const OBB& a, const OBB& b);
	Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
	Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);
	Quaternion Add(const Quaternion& lns, const Quaternion& rhs);
	Quaternion Subtract(const Quaternion& q1, const Quaternion& q2);
	Quaternion Multiply(float a, const Quaternion& q);
	Quaternion Multiply(const Quaternion& lns, const Quaternion& rhs);
	Quaternion MakeIdentityQuaternion();
	Quaternion Conjugate(const Quaternion& quaternion);
	float Norm(const Quaternion& quaternion);
	Quaternion Normalize(const Quaternion& quaternion);
	Quaternion Inverse(const Quaternion& quaternion);
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
	Vector3 QuaternionToEuler(const Quaternion& q);
	Quaternion EulerToQuaternion(const Vector3& euler);
	bool DecomposeMatrix(const Matrix4x4& m, Vector3& outScale, Vector3& outRotationEuler, Vector3& outTranslation);
};


#include "MyMath.h"
#include <algorithm>
#include <assert.h>
namespace MyMath {

	Vector3 Add(const Vector3& v1, const Vector3& v2) {
		Vector3 result;

		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
		result.z = v1.z + v2.z;

		return result;
	}
	Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
		Vector3 result;

		result.x = v1.x - v2.x;
		result.y = v1.y - v2.y;
		result.z = v1.z - v2.z;

		return result;
	}
	Vector3 divide(const Vector3& v, float s) {
		Vector3 result;

		result.x = v.x / s;
		result.y = v.y / s;
		result.z = v.z / s;

		return result;
	}
	Vector3 AddVector2AndVector3(const Vector2& vec2, const Vector3& vec3) {
		return Vector3{ vec2.x + vec3.x, vec2.y + vec3.y, vec3.z };
	}

	Vector3 Multiply(const Vector3& vec, float scalar) {
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	float length(Vector3 distance) {
		return sqrtf(powf(distance.x, 2.0f) + powf(distance.y, 2.0f) + powf(distance.z, 2.0f));
	}

	Vector3 normalize(Vector3 distance) {
		return { distance.x / length(distance),distance.y / length(distance),distance.z / length(distance) };
	}

	float Dot(Vector3 c, Vector3 d) {
		return c.x * d.x + c.y * d.y + c.z * d.z;
	}

	Vector3 cross(const Vector3& u, const Vector3& v) {
		return Vector3(
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x
		);
	}

	Matrix4x4 Identity() {
		return {
	   1.0f, 0.0f, 0.0f, 0.0f,
	   0.0f, 1.0f, 0.0f, 0.0f,
	   0.0f, 0.0f, 1.0f, 0.0f,
	   0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
		Matrix4x4 result;

		result = {
			scale.x,0.0f,0.0f,0.0f,
			0.0f,scale.y,0.0f,0.0f,
			0.0f,0.0f,scale.z,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};

		return result;
	}

	Matrix4x4 MakeRotateMatrix(const Vector3& rotate) {
		Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
		Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
		Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);

		return Multiply(rotateYMatrix, Multiply(rotateXMatrix, rotateZMatrix));
	}

	Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
		Matrix4x4 result;

		result = {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			translate.x,translate.y,translate.z,1.0f

		};
		return result;
	}

	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 result;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; ++k) {
					result.m[i][j] += m1.m[i][k] * m2.m[k][j];
				}
			}
		}
		return result;
	}

	Matrix4x4 MakeRotateXMatrix(float radian) {
		Matrix4x4 result;

		result = {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,cosf(radian),sinf(radian),0.0f,
			0.0f,-sinf(radian),cosf(radian),0.0f,
			0.0f,0.0f,0.0f,1.0f };
		return result;
	}

	Matrix4x4 MakeRotateYMatrix(float radian) {
		Matrix4x4 result;

		result = {
			cosf(radian),0.0f,-sinf(radian),0.0f,
			0.0f,1.0f,0.0f,0.0f,
			sinf(radian),0.0f,cosf(radian),0.0f,
			0.0f,0.0f,0.0f,1.0f };
		return result;
	}

	Matrix4x4 MakeRotateZMatrix(float radian) {
		Matrix4x4 result;

		result = {
			cosf(radian),sinf(radian),0.0f,0.0f,
			-sinf(radian),cosf(radian),0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f };
		return result;

	}

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
		Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
		Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
		Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		Matrix4x4 result;
		result = {
			scale.x * rotateXYZMatrix.m[0][0],scale.x * rotateXYZMatrix.m[0][1],scale.x * rotateXYZMatrix.m[0][2],0.0f,
			scale.y * rotateXYZMatrix.m[1][0],scale.y * rotateXYZMatrix.m[1][1],scale.y * rotateXYZMatrix.m[1][2],0.0f,
			scale.z * rotateXYZMatrix.m[2][0],scale.z * rotateXYZMatrix.m[2][1],scale.z * rotateXYZMatrix.m[2][2],0.0f,
			translate.x,translate.y,translate.z,1.0f
		};
		return result;
	}

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		Matrix4x4 result;

		result = {
			1 / aspectRatio * (1 / tanf(fovY / 2)),0.0f,0.0f,0.0f,
			0.0f,(1 / tanf(fovY / 2)),0.0f,0.0f,
			0.0f,0.0f,farClip / (farClip - nearClip),1.0f,
			0.0f,0.0f,-farClip * nearClip / (farClip - nearClip),0.0f
		};

		return result;
	}

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
		Matrix4x4 result;
		result = {
		2 / (right - left),0.0f,0.0f,0.0f,
		0.0f,2 / (top - bottom),0.0f,0.0f,
		0.0f,0.0f,1 / (farClip - nearClip),0.0f,
		(left + right) / (left - right),(top + bottom) / (bottom - top),nearClip / (nearClip - farClip),1.0f
		};
		return result;
	}

	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
		Matrix4x4 result;

		result = {
		width / 2,0.0f,0.0f,0.0f,
		0.0f,-(height / 2),0.0f,0.0f,
		0.0f,0.0f,maxDepth - minDepth,0.0f,
		left + (width / 2),top + (height / 2),minDepth,1.0f
		};


		return result;
	}

	Vector3 Transforms(const Vector3& vector, const Matrix4x4& matrix) {
		Vector3 result;
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;


		return result;
	}

	Matrix4x4 Inverse(const Matrix4x4& m) {
		Matrix4x4 result;
		float A;
		A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
			- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
			- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
			+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
			+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
			- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
			- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
			+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

		result.m[0][0] = 1 / A * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
			- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
		result.m[0][1] = 1 / A * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
			+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
		result.m[0][2] = 1 / A * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
			- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
		result.m[0][3] = 1 / A * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
			+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

		result.m[1][0] = 1 / A * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
			+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
		result.m[1][1] = 1 / A * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
			- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
		result.m[1][2] = 1 / A * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
			+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
		result.m[1][3] = 1 / A * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
			- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

		result.m[2][0] = 1 / A * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
			- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
		result.m[2][1] = 1 / A * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
			+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
		result.m[2][2] = 1 / A * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
			- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
		result.m[2][3] = 1 / A * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

		result.m[3][0] = 1 / A * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
			+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
		result.m[3][1] = 1 / A * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
		result.m[3][2] = 1 / A * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
			+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
		result.m[3][3] = 1 / A * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);
		return result;
	}

	Matrix4x4 MakeIdentity4x4() {
		Matrix4x4 result;

		result = {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f

		};
		return result;
	}

	Matrix4x4 CreateLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up) {
		Vector3 zaxis = normalize(Subtract(target, eye));    // Forward
		Vector3 xaxis = normalize(cross(up, zaxis));         // Right
		Vector3 yaxis = cross(zaxis, xaxis);                 // Up

		Matrix4x4 viewMatrix = {
			xaxis.x, yaxis.x, zaxis.x, 0.0f,
			xaxis.y, yaxis.y, zaxis.y, 0.0f,
			xaxis.z, yaxis.z, zaxis.z, 0.0f,
			-Dot(xaxis, eye), -Dot(yaxis, eye), -Dot(zaxis, eye), 1.0f
		};

		return viewMatrix;
	}

	Matrix4x4 Transpose(const Matrix4x4& m) {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m.m[j][i];
			}
		}
		return result;
	}

	float ToRadian(float degree) {
		return degree * (3.141592f / 180.0f);
	}

	Vector3 SphericalToCartesian(float radius, float theta, float phi) {
		Vector3 cartesian;
		cartesian.x = radius * sinf(theta) * cosf(phi);
		cartesian.y = radius * sinf(theta) * sinf(phi);
		cartesian.z = radius * cosf(theta);
		return cartesian;
	}

	Vector3 Project(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		float a = ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
		float b = ((v2.x * v2.x) + (v2.y * v2.y) + (v2.z * v2.z));
		result.x = a / b * v2.x;
		result.y = a / b * v2.y;
		result.z = a / b * v2.z;

		return result;
	}

	Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
		Vector3 project = Project(Subtract(point, segment.origin), segment.diff);

		project = Add(project, segment.origin);

		return project;
	}

	Vector3 Perpendicular(const Vector3& vector) {
		if (vector.x != 0.0f || vector.y != 0.0f) {
			return { -vector.y,vector.x,0.0f };
		}
		return { 0.0f,-vector.z,vector.y };
	}

	void EnsureAABB(AABB& aabb) {
		aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
		aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);
		aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
		aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);
		aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
		aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);
	}

	Vector3 ClosestPointOnAABB(const Vector3& point, const AABB& aabb) {
		Vector3 closestPoint;
		closestPoint.x = std::clamp(point.x, aabb.min.x, aabb.max.x);
		closestPoint.y = std::clamp(point.y, aabb.min.y, aabb.max.y);
		closestPoint.z = std::clamp(point.z, aabb.min.z, aabb.max.z);
		return closestPoint;
	}

	bool IsAABBCollision(const Vector3& aMin, const Vector3& aMax, const Vector3& bMin, const Vector3& bMax) {
		return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
			   (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
			   (aMin.z <= bMax.z && aMax.z >= bMin.z);
	}

	Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
		Vector3 result;
		result.x = t * a.x + (1.0f - t) * b.x;
		result.y = t * a.y + (1.0f - t) * b.y;
		result.z = t * a.z + (1.0f - t) * b.z;
		return result;
	}

	Matrix4x4 ConvertMatrix( aiMatrix4x4& aiMat) {
		Matrix4x4 result;

		result.m[0][0] = aiMat.a1; result.m[0][1] = aiMat.a2; result.m[0][2] = aiMat.a3; result.m[0][3] = aiMat.a4;
		result.m[1][0] = aiMat.b1; result.m[1][1] = aiMat.b2; result.m[1][2] = aiMat.b3; result.m[1][3] = aiMat.b4;
		result.m[2][0] = aiMat.c1; result.m[2][1] = aiMat.c2; result.m[2][2] = aiMat.c3; result.m[2][3] = aiMat.c4;
		result.m[3][0] = aiMat.d1; result.m[3][1] = aiMat.d2; result.m[3][2] = aiMat.d3; result.m[3][3] = aiMat.d4;

		return result;
	}

	float Distance(const Vector3& a, const Vector3& b) {
		return length(Subtract(a, b));
	}

	Vector3 GetTranslate(const Matrix4x4& m) {
		return { m.m[3][0], m.m[3][1], m.m[3][2] };
	}

	OBB MakeOBB(const WorldTransform& wt, const Vector3& modelSize) {
		OBB obb;
		obb.center = GetTranslate(wt.matWorld_);
		obb.halfSize = Multiply(modelSize, 0.5f);

		// 로컬 축을 world 회전으로 변환
		Matrix4x4 rotMat = MakeRotateZMatrix(wt.rotate_.z);
		rotMat = Multiply(rotMat, MakeRotateYMatrix(wt.rotate_.y));
		rotMat = Multiply(rotMat, MakeRotateXMatrix(wt.rotate_.x));

		obb.axis[0] = { rotMat.m[0][0], rotMat.m[0][1], rotMat.m[0][2] }; // X축
		obb.axis[1] = { rotMat.m[1][0], rotMat.m[1][1], rotMat.m[1][2] }; // Y축
		obb.axis[2] = { rotMat.m[2][0], rotMat.m[2][1], rotMat.m[2][2] }; // Z축

		return obb;
	}

	bool IsOBBCollision(const OBB& a, const OBB& b) {
		const float EPSILON = 1e-6f;
		float ra, rb;
		float R[3][3], AbsR[3][3];

		// 1. 두 박스의 축 내적 (회전행렬 R)
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				R[i][j] = Dot(a.axis[i], b.axis[j]);
				AbsR[i][j] = std::fabs(R[i][j]) + EPSILON;
			}
		}

		// 2. 두 중심의 벡터를 A 좌표계로 변환
		Vector3 t = Subtract(b.center, a.center);
		t = { Dot(t, a.axis[0]), Dot(t, a.axis[1]), Dot(t, a.axis[2]) };

		// 3. A의 세 축에 대해 검사
		for (int i = 0; i < 3; i++) {
			ra = a.halfSize[i];
			rb = b.halfSize[0] * AbsR[i][0] + b.halfSize[1] * AbsR[i][1] + b.halfSize[2] * AbsR[i][2];
			if (std::fabs(t[i]) > ra + rb) return false;
		}

		// 4. B의 세 축에 대해 검사
		for (int i = 0; i < 3; i++) {
			ra = a.halfSize[0] * AbsR[0][i] + a.halfSize[1] * AbsR[1][i] + a.halfSize[2] * AbsR[2][i];
			rb = b.halfSize[i];
			float proj = std::fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]);
			if (proj > ra + rb) return false;
		}

		// 5. 외적 축 9개 검사
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				ra = a.halfSize[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + a.halfSize[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
				rb = b.halfSize[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + b.halfSize[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
				float proj = std::fabs(t[(i + 2) % 3] * R[(i + 1) % 3][j] - t[(i + 1) % 3] * R[(i + 2) % 3][j]);
				if (proj > ra + rb) return false;
			}
		}

		// 모든 축에서 분리가 없으면 충돌
		return true;
	}

	Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
		Vector3 n = normalize(axis);

		float cosTheta = cosf(angle);
		float sinTheta = sinf(angle);
		float oneMinusCosTheta = 1.0f - cosTheta;

		float nx = n.x;
		float ny = n.y;
		float nz = n.z;

		Matrix4x4 result = {
			nx * nx * oneMinusCosTheta + cosTheta,          nx * ny * oneMinusCosTheta + nz * sinTheta, nx * nz * oneMinusCosTheta - ny * sinTheta, 0.0f,
			nx * ny * oneMinusCosTheta - nz * sinTheta,     ny * ny * oneMinusCosTheta + cosTheta,      ny * nz * oneMinusCosTheta + nx * sinTheta, 0.0f,
			nx * nz * oneMinusCosTheta + ny * sinTheta,     ny * nz * oneMinusCosTheta - nx * sinTheta, nz * nz * oneMinusCosTheta + cosTheta,      0.0f,
			0.0f,                                          0.0f,                                      0.0f,                                      1.0f
		};
		return result;
	}
	Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
		Vector3 axis = cross(from, to);
		float dot = Dot(from, to);

		Vector3 n = axis;
		if (length(axis) == 0.0f && dot < 0.0f) {
			if (from.x != 0 || from.y != 0) {

				n.x = from.y;
				n.y = -from.x;
				n.z = 0.0f;
			} else if (from.x != 0 || from.z != 0) {

				n.x = from.z;
				n.y = 0.0f;
				n.z = -from.x;
			}
		}

		n = normalize(n);

		float cosTheta = dot;
		float sinTheta = length(axis);
		float oneMinusCosTheta = 1.0f - cosTheta;

		float nx = n.x;
		float ny = n.y;
		float nz = n.z;

		Matrix4x4 result = {
			nx * nx * oneMinusCosTheta + cosTheta,          nx * ny * oneMinusCosTheta + nz * sinTheta, nx * nz * oneMinusCosTheta - ny * sinTheta, 0.0f,
			nx * ny * oneMinusCosTheta - nz * sinTheta,     ny * ny * oneMinusCosTheta + cosTheta,      ny * nz * oneMinusCosTheta + nx * sinTheta, 0.0f,
			nx * nz * oneMinusCosTheta + ny * sinTheta,     ny * nz * oneMinusCosTheta - nx * sinTheta, nz * nz * oneMinusCosTheta + cosTheta,      0.0f,
			0.0f,                                          0.0f,                                      0.0f,                                      1.0f
		};
		return result;
	}

	Quaternion Add(const Quaternion& lns, const Quaternion& rhs) {
		Quaternion result;

		result.x = lns.x + rhs.x;
		result.y = lns.y + rhs.y;
		result.z = lns.z + rhs.z;
		result.w = lns.w + rhs.w;

		return result;
	}

	Quaternion Subtract(const Quaternion& q1, const Quaternion& q2) {
		Quaternion result;

		result.x = q1.x - q2.x;
		result.y = q1.y - q2.y;
		result.z = q1.z - q2.z;
		result.z = q1.w - q2.w;

		return result;
	}

	Quaternion Multiply(float a, const Quaternion& q) {
		Quaternion result;

		result.x = q.x * a;
		result.y = q.y * a;
		result.z = q.z * a;
		result.w = q.w * a;

		return result;
	}

	Quaternion Multiply(const Quaternion& lns, const Quaternion& rhs) {
		Vector3 qv = { lns.x,lns.y,lns.z };
		Vector3 rv = { rhs.x,rhs.y,rhs.z };
		float qrw = lns.w * rhs.w - Dot(qv, rv);
		Vector3 qrv = Add(cross(qv, rv), Add(Multiply(rhs.w, qv), Multiply(lns.w, rv)));

		Quaternion result{ qrv.x,qrv.y,qrv.z,qrw };

		return result;

	}

	Quaternion MakeIdentityQuaternion() {
		return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	Quaternion Conjugate(const Quaternion& quaternion) {
		return Quaternion{ -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
	}

	float Norm(const Quaternion& quaternion) {
		return sqrtf(quaternion.x * quaternion.x +
			quaternion.y * quaternion.y +
			quaternion.z * quaternion.z +
			quaternion.w * quaternion.w);
	}

	Quaternion Normalize(const Quaternion& quaternion) {
		float norm = Norm(quaternion);
		return { quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
	}

	Quaternion Inverse(const Quaternion& quaternion) {
		float normSquared = (powf(quaternion.x, 2.0f) + powf(quaternion.y, 2.0f) + powf(quaternion.z, 2.0f) + powf(quaternion.w, 2.0f));

		Quaternion conjugate = Conjugate(quaternion);

		return Quaternion{ conjugate.x / normSquared, conjugate.y / normSquared, conjugate.z / normSquared, conjugate.w / normSquared };
	}

	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
		Vector3 normalizedAxis = normalize(axis);

		float halfAngle = angle / 2.0f;
		float cosHalfAngle = cosf(halfAngle);
		float sinHalfAngle = sinf(halfAngle);

		Quaternion result;
		result.w = cosHalfAngle;
		result.x = normalizedAxis.x * sinHalfAngle;
		result.y = normalizedAxis.y * sinHalfAngle;
		result.z = normalizedAxis.z * sinHalfAngle;

		return result;
	}

	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
		Quaternion vQuaternion{ vector.x, vector.y, vector.z, 0.0f };

		Quaternion conjugate = Conjugate(quaternion);

		Quaternion temp = Multiply(quaternion, vQuaternion); // q * v
		Quaternion result = Multiply(temp, conjugate);       // (q * v) * q^*
		return Vector3{ result.x, result.y, result.z };
	}

	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
		float x = quaternion.x;
		float y = quaternion.y;
		float z = quaternion.z;
		float w = quaternion.w;

		Matrix4x4 result = {
		   (w * w) + (x * x) - (y * y) - (z * z),  2.0f * (x * y + w * z),        2.0f * (x * z - w * y),        0.0f,
		   2.0f * (x * y - w * z),        w * w - x * x + y * y - z * z,  2.0f * (y * z + w * x),        0.0f,
		   2.0f * (x * z + w * y),        2.0f * (y * z - w * x),        w * w - x * x - y * y + z * z,  0.0f,
		   0.0f,                          0.0f,                          0.0f,                          1.0f
		};

		return result;
	}

	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
		float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

		Quaternion q1Adjusted = q1;
		if (dot < 0) {
			q1Adjusted = Quaternion{ -q1.x, -q1.y, -q1.z, -q1.w };
			dot = -dot;
		}

		const float EPSILON = 1e-6f;
		if (dot > 1.0f - EPSILON) {

			return Normalize(Add(Multiply(1.0f - t, q0), Multiply(t, q1Adjusted)));
		}


		float theta = std::acos(dot);
		float sinTheta = std::sin(theta);

		float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
		float scale1 = std::sin(t * theta) / sinTheta;


		return Add(Multiply(scale0, q0), Multiply(scale1, q1Adjusted));

	}

	Vector3 QuaternionToEuler(const Quaternion& q) {
		Vector3 euler;
		euler.y = atan2(2.0f * (q.w * q.y + q.x * q.z), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
		euler.x = asin(2.0f * (q.w * q.x - q.z * q.y));
		euler.z = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.x * q.x + q.z * q.z));
		return euler;
	}

	Quaternion EulerToQuaternion(const Vector3& euler) {
		float cx = cosf(euler.x * 0.5f);
		float sx = sinf(euler.x * 0.5f);
		float cy = cosf(euler.y * 0.5f);
		float sy = sinf(euler.y * 0.5f);
		float cz = cosf(euler.z * 0.5f);
		float sz = sinf(euler.z * 0.5f);

		Quaternion q;
		q.w = cx * cy * cz + sx * sy * sz;
		q.x = sx * cy * cz - cx * sy * sz;
		q.y = cx * sy * cz + sx * cy * sz;
		q.z = cx * cy * sz - sx * sy * cz;
		return q;
	}

	bool DecomposeMatrix(const Matrix4x4& m, Vector3& outScale, Vector3& outRotationEuler, Vector3& outTranslation) {
		// 1. translation
		outTranslation.x = m.m[3][0];
		outTranslation.y = m.m[3][1];
		outTranslation.z = m.m[3][2];

		// 2. scale (각 basis vector의 길이)
		outScale.x = sqrtf(m.m[0][0] * m.m[0][0] + m.m[0][1] * m.m[0][1] + m.m[0][2] * m.m[0][2]);
		outScale.y = sqrtf(m.m[1][0] * m.m[1][0] + m.m[1][1] * m.m[1][1] + m.m[1][2] * m.m[1][2]);
		outScale.z = sqrtf(m.m[2][0] * m.m[2][0] + m.m[2][1] * m.m[2][1] + m.m[2][2] * m.m[2][2]);

		if (outScale.x == 0 || outScale.y == 0 || outScale.z == 0) {
			return false;
		}

		// 3. rotation (행렬에서 스케일을 제거하고 Euler 추출)
		Matrix4x4 rotM = m;

		rotM.m[0][0] /= outScale.x;  rotM.m[0][1] /= outScale.x;  rotM.m[0][2] /= outScale.x;
		rotM.m[1][0] /= outScale.y;  rotM.m[1][1] /= outScale.y;  rotM.m[1][2] /= outScale.y;
		rotM.m[2][0] /= outScale.z;  rotM.m[2][1] /= outScale.z;  rotM.m[2][2] /= outScale.z;

		// rotation from rotation matrix (YXZ or XYZ 등 엔진 기준에 따라 수정)
		outRotationEuler.x = atan2f(rotM.m[2][1], rotM.m[2][2]);
		outRotationEuler.y = atan2f(-rotM.m[2][0],
			sqrtf(rotM.m[2][1] * rotM.m[2][1] + rotM.m[2][2] * rotM.m[2][2]));
		outRotationEuler.z = atan2f(rotM.m[1][0], rotM.m[0][0]);

		return true;
	}
	

}
#pragma once
#include "Vector3.h"
#include "Vector4.h"

/// <summary>ディレクショナルライト</summary>
struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};

/// <summary>ポイントライト</summary>
struct PointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	float padding[2];
};

/// <summary>スポットライト</summary>
struct SpotLight {
	Vector4 color;      // 16B

	Vector3 position;   // 12B
	float intensity;    // 4B

	Vector3 direction;  // 12B
	float cutoff;       // 4B

	float outerCutoff;  // 4B
	float decay;        // 4B
	float radius;       // 4B
	float padding;      // 4B → 정렬용
};

/// <summary>環境光</summary>
struct AmbientLight {
	Vector4 color;
};

/// <summary>エリアライト</summary>
struct AreaLight {
	Vector4 color;           // 16B

	Vector3 position;        // 12B
	float intensity;         // 4B

	Vector3 right;           // 12B
	float halfWidth;         // 4B

	Vector3 up;              // 12B
	float halfHeight;        // 4B
};
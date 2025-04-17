#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct PointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	float padding[2];
};
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
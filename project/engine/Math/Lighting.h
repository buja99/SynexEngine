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
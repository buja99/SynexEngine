#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};
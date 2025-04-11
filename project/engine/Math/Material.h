#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"
#include <string>

struct  Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
	int reflectModel;
};

struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};
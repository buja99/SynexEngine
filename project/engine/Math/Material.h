#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"
#include <string>

/// <summary>3D描画用マテリアル定数</summary>
struct  Material
{
	Vector4 color;               // 16B
	int32_t enableLighting;		 // 4B
	float padding[3];            // 12B
	Matrix4x4 uvTransform;       // 64B
	float shininess;             // 4B

	int32_t isBlinnPhong;        // 4B
	int32_t useDirectionalLight; // 4B
	int32_t usePointLight;       // 4B
	int32_t useSpotLight;        // 4B
	int32_t useAmbientLight;     // 4B
	int32_t useAreaLight;        // 4B
	int32_t useEnvironmentMap;
};

/// <summary>マテリアルリソース情報</summary>
struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};

/// <summary>スプライト用マテリアル定数</summary>
struct SpriteMaterial {
	Vector4 color;
	Matrix4x4 uvTransform;
};
#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/// <summary>3D描画用頂点データ</summary>
struct  VertexData
{
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
};

/// <summary>スプライト描画用頂点データ</summary>
struct SpriteVertexData {
	Vector4 position;
	Vector2 texcoord;
};
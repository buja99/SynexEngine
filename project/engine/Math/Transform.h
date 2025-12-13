#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

/// <summary>スケール・回転・平行移動定義</summary>
struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

};

/// <summary>ワールド・ビュー投影行列セット</summary>
struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

/// <summary>スプライト用行列セット</summary>
struct SpriteMatrix {
	Matrix4x4 WVP;
};
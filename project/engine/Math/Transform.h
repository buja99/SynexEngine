#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};
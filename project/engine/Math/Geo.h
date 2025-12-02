#pragma once
#include "Vector3.h"

struct Sphere {
	Vector3 center;
	float radius;
};
struct Segment {
	Vector3 origin;
	Vector3 diff;

};
struct Plane {
	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 vertices[3];
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;       // 중심 좌표 (world)
	Vector3 halfSize;     // 반쪽 크기
	Vector3 axis[3];      // 로컬 축 (X,Y,Z) → world 변환된 것
};

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};


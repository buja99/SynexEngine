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
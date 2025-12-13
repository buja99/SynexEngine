#pragma once
#include "Vector3.h"

/// <summary>球コリジョン形状</summary>
struct Sphere {
	Vector3 center;
	float radius;
};

/// <summary>線分定義</summary>
struct Segment {
	Vector3 origin;
	Vector3 diff;

};

/// <summary>平面定義</summary>
struct Plane {
	Vector3 normal;
	float distance;
};

/// <summary>三角形定義</summary>
struct Triangle {
	Vector3 vertices[3];
};

/// <summary>軸平行境界ボックス</summary>
struct AABB {
	Vector3 min;
	Vector3 max;
};

/// <summary>有向境界ボックス</summary>
struct OBB {
	Vector3 center;       // 중심 좌표 (world)
	Vector3 halfSize;     // 반쪽 크기
	Vector3 axis[3];      // 로컬 축 (X,Y,Z) → world 변환된 것
};

/// <summary>クォータニオン</summary>
struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};


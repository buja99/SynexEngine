#pragma once
#include <string>
#include "Matrix4x4.h"
#include "MyMath.h"
#include <vector>

struct QuaternionTransform {
    Vector3 scale{ 1,1,1 };
    Quaternion rotate{ 0,0,0,1 };
    Vector3 translate{ 0,0,0 };
};

struct Bone {
    std::string name;
    int32_t parentIndex = -1;
    Matrix4x4 localMatrix;
    Matrix4x4 worldMatrix;
    Matrix4x4 offsetMatrix;
    std::vector<int32_t> children;
    QuaternionTransform transform;
};

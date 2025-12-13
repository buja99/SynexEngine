#pragma once
#include <string>
#include "Matrix4x4.h"
#include "MyMath.h"
#include <vector>

/// <summary>ボーンの階層構造と変換情報を保持するデータ構造 </summary>
struct QuaternionTransform {
    Vector3 scale{ 1,1,1 };
    Quaternion rotate{ 0,0,0,1 };
    Vector3 translate{ 0,0,0 };
};

/// <summary>スケルトン内の1つのボーンを表す構造体 </summary>
struct Bone {
    std::string name;
    int32_t parentIndex = -1;
    Matrix4x4 localMatrix;
    Matrix4x4 worldMatrix;
    Matrix4x4 offsetMatrix;
    std::vector<int32_t> children;
    QuaternionTransform transform;
};

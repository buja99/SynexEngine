#pragma once
#include "Bone.h"
#include <vector>
#include <unordered_map>
#include <assimp/scene.h>

/// <summary>ボーン階層構築とボーン行列更新</summary>
class Skeleton {


public:

    /// <summary>Assimpシーンからボーン階層構築</summary>
    void Initialize(const aiScene* scene); // GLTF에서 본 정보 추출

    /// <summary>ローカル行列からワールド行列更新</summary>
    void UpdateWorldMatrix();              // 모든 본의 월드 행렬 갱신

    // FindBone は名前だけで意味が明確なのでコメント省略でもOK
    Bone* FindBone(const std::string& name);
    const Bone* FindBone(const std::string& name) const;

public:

    std::vector<Bone> bones;
    std::unordered_map<std::string, int> boneIndexMap; // 이름→인덱스
    int rootIndex = -1;
};


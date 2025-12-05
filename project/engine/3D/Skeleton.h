#pragma once
#include "Bone.h"
#include <vector>
#include <unordered_map>
#include <assimp/scene.h>

/// <summary>ボーン階層の構築と各ボーンの行列更新を行うスケルトン管理クラス。</summary>
class Skeleton {


public:

    void Initialize(const aiScene* scene); // GLTF에서 본 정보 추출
    void UpdateWorldMatrix();              // 모든 본의 월드 행렬 갱신


    Bone* FindBone(const std::string& name);
    const Bone* FindBone(const std::string& name) const;

public:

    std::vector<Bone> bones;
    std::unordered_map<std::string, int> boneIndexMap; // 이름→인덱스
    int rootIndex = -1;
};


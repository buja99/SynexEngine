#include "Skeleton.h"
#include "MyMath.h"
#include <functional>
#include <cassert>

void Skeleton::Initialize(const aiScene* scene) {
    bones.clear();
    boneIndexMap.clear();

    assert(scene);
    assert(scene->mRootNode);

    std::function<void(aiNode*, int)> traverse = [&](aiNode* node, int parentIndex) {
        Bone bone;
        bone.name = node->mName.C_Str();
        bone.parentIndex = parentIndex;

        // Assimp 행렬(row-major) → DirectX용(row-major)로 변환 (전치)
        aiMatrix4x4 aiLocal = node->mTransformation;
        aiLocal.Transpose();
        bone.localMatrix = MyMath::ConvertMatrix(aiLocal);

        // 아직 worldMatrix는 갱신 전
        bone.worldMatrix = MyMath::MakeIdentity4x4();
        bone.offsetMatrix = MyMath::MakeIdentity4x4();

        int thisIndex = static_cast<int>(bones.size());
        bones.push_back(bone);
        boneIndexMap[bone.name] = thisIndex;

        if (parentIndex >= 0) {
            bones[parentIndex].children.push_back(thisIndex);
        }

        // 자식 노드 순회
        for (uint32_t i = 0; i < node->mNumChildren; ++i) {
            traverse(node->mChildren[i], thisIndex);
        }
        };

    traverse(scene->mRootNode, -1);

    // 루트 본 인덱스 설정
    rootIndex = bones.empty() ? -1 : 0;

    // 첫 번째 업데이트 호출로 worldMatrix 초기화
    UpdateWorldMatrix();

}

void Skeleton::UpdateWorldMatrix() {
    if (bones.empty()) return;

    for (size_t i = 0; i < bones.size(); ++i) {
        int parent = bones[i].parentIndex;
        if (parent >= 0) {
            bones[i].worldMatrix = MyMath::Multiply(bones[i].localMatrix, bones[parent].worldMatrix);
        } else {
            bones[i].worldMatrix = bones[i].localMatrix;
        }
    }
}

Bone* Skeleton::FindBone(const std::string& name) {
    auto it = boneIndexMap.find(name);
    if (it != boneIndexMap.end()) {
        return &bones[it->second];
    }
    return nullptr;
}

const Bone* Skeleton::FindBone(const std::string& name) const {
    auto it = boneIndexMap.find(name);
    if (it != boneIndexMap.end()) {
        return &bones[it->second];
    }
    return nullptr;
}

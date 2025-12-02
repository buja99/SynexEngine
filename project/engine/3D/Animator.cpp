#include "Animator.h"
#include <algorithm>
#include <cassert>
#include "Skeleton.h"
#include "MyMath.h"
#include "Model.h"

void Animator::Play(const AnimationData& anim, bool loop, float startTime) {
    currentAnim_ = &anim;
    time_ = startTime;
    loop_ = loop;
    finished_ = false;
    paused_ = false;
}

void Animator::Update(float deltaTime, Skeleton& skeleton) {
    // 애니메이션 데이터가 없거나 정지 상태면 스킵
    if (!currentAnim_ || paused_) {
        return;
    }

    const AnimationData& anim = *currentAnim_;

    if (anim.duration <= 0.0f) {
        return;
    }

    // 시간 진행
    time_ += deltaTime * speed_;

    // 루프 or 종료 처리
    if (loop_) {
        time_ = fmodf(time_, anim.duration);
    } else if (time_ >= anim.duration) {
        time_ = anim.duration;
        finished_ = true;
        return;
    }

    // 각 채널(본 단위) 순회
    for (const auto& channel : anim.channels) {
        auto it = skeleton.boneIndexMap.find(channel.nodeName);
        if (it == skeleton.boneIndexMap.end()) continue;

        int boneIndex = it->second;
        if (boneIndex < 0 || boneIndex >= skeleton.bones.size()) continue;

        Bone& bone = skeleton.bones[boneIndex]; // ✅ 매번 참조를 만들어두면 깔끔함

        Vector3 pos = bone.transform.translate;
        Vector3 scl = bone.transform.scale;
        Quaternion rot = bone.transform.rotate;

        // --- 위치 보간 ---
        if (channel.translate.keyframes.size() >= 2) {
            const auto& keys = channel.translate.keyframes;
            size_t idx = 0;
            while (idx + 1 < keys.size() && keys[idx + 1].time < time_) ++idx;
            const auto& k0 = keys[idx];
            const auto& k1 = keys[std::min(idx + 1, keys.size() - 1)];
            float t = (time_ - k0.time) / (k1.time - k0.time + 0.0001f);
            pos = MyMath::Lerp(k0.value, k1.value, t);
        }

        // --- 스케일 보간 ---
        if (channel.scale.keyframes.size() >= 2) {
            const auto& keys = channel.scale.keyframes;
            size_t idx = 0;
            while (idx + 1 < keys.size() && keys[idx + 1].time < time_) ++idx;
            const auto& k0 = keys[idx];
            const auto& k1 = keys[std::min(idx + 1, keys.size() - 1)];
            float t = (time_ - k0.time) / (k1.time - k0.time + 0.0001f);
            scl = MyMath::Lerp(k0.value, k1.value, t);
        }

        // --- 회전 보간 (Slerp) ---
        if (channel.rotate.keyframes.size() >= 2) {
            const auto& keys = channel.rotate.keyframes;
            size_t idx = 0;
            while (idx + 1 < keys.size() && keys[idx + 1].time < time_) ++idx;
            const auto& k0 = keys[idx];
            const auto& k1 = keys[std::min(idx + 1, keys.size() - 1)];
            float t = (time_ - k0.time) / (k1.time - k0.time + 0.0001f);
            rot = MyMath::Slerp(k0.value, k1.value, t);
        }

        // ✅ ① 변환값을 Bone의 transform에 기록
        bone.transform.scale = scl;
        bone.transform.rotate = rot;
        bone.transform.translate = pos;

        // ✅ ② 실제 localMatrix 생성
        bone.localMatrix = MyMath::MakeAffineMatrix(
            bone.transform.scale,
            MyMath::QuaternionToEuler(bone.transform.rotate),
            bone.transform.translate
        );
    }

    // ✅ 모든 본의 월드 행렬 갱신
    skeleton.UpdateWorldMatrix();
}

float Animator::GetDuration() const {
    return currentAnim_ ? currentAnim_->duration : 0.0f;
}

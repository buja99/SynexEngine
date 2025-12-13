#pragma once
#include "Model.h"
#include "Skeleton.h"


/// <summary> アニメーション再生と骨格(Skeleton)の各ボーン変換を更新クラス </summary>
class Animator {

public:
    /// <summary>アニメーション再生開始</summary>
    void Play(const AnimationData& anim, bool loop = true, float startTime = 0.0f);

    /// <summary>再生中アニメーションをSkeletonへ反映</summary>
    void Update(float deltaTime, Skeleton& skeleton);

    // 상태/제어
    void Stop() { currentAnim_ = nullptr; time_ = 0.0f; }
    void Pause(bool p) { paused_ = p; }
    void SetSpeed(float s) { speed_ = (s < 0.0f ? 0.0f : s); }
    void SetLoop(bool l) { loop_ = l; }

    // 조회/유틸
    bool IsPlaying()           const { return currentAnim_ != nullptr && !paused_; }
    bool IsFinished()          const { return !loop_ && finished_; }
    void SetTime(float t) { time_ = t; finished_ = false; }
    float GetTime()            const { return time_; }
    float GetSpeed()           const { return speed_; }
    bool  GetLoop()            const { return loop_; }
    float GetDuration()        const; // 구현부에서 currentAnim_ 검사 후 반환

private:
    // 외부 수명 객체를 참조만 함 (소유 X)
    const AnimationData* currentAnim_ = nullptr;

    float time_ = 0.0f;   // 현재 재생 시간(초)
    float speed_ = 1.0f;   // 재생 속도 배율
    bool  loop_ = true;   // 루프 여부
    bool  paused_ = false;  // 일시정지
    bool  finished_ = false;  // 비루프 재생 종료 플래그
};


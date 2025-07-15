#pragma once
#include "ParticleManager.h"
#include "Camera.h"
#include <memory>
#include <string>
#include <random>

class ParticleEffectLibrary {
public:
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera);
    void Update();
    void Draw();

    void DrawPrimitive();
    void DrawRing();
    void DrawCylinder();
    void DrawLongLine();
    void DrawGradationRing();

    void EmitPrimitive(const Vector3& pos, std::mt19937& randomEngine);
    void EmitRing(const Vector3& pos, std::mt19937& randomEngine);
    void EmitCylinder(const Vector3& pos, std::mt19937& randomEngine);
    void EmitLongLine(const Vector3& pos, std::mt19937& randomEngine);
    void EmitGradationRing(const Vector3& pos, std::mt19937& randomEngine);

    ParticleManager* GetPrimitiveManager() { return primitive_.get(); }
    ParticleManager* GetRingManager() { return ring_.get(); }
    ParticleManager* GetCylinderManager() { return cylinder_.get(); }
    ParticleManager* GetLongLineManager() { return longLine_.get(); }
    ParticleManager* GetGradationRingManager() { return gradationRing_.get(); }

    void SetPrimitiveInterval(float interval);
    void SetRingInterval(float interval);
    void SetCylinderInterval(float interval);

    void SetUsePrimitiveAutoEmit(bool flag);
    void SetUseRingAutoEmit(bool flag);
    void SetUseCylinderAutoEmit(bool flag);

   

private:

    std::mt19937 randomEngine_;

    std::unique_ptr<ParticleManager> primitive_;
    std::unique_ptr<ParticleManager> ring_;
	std::unique_ptr<ParticleManager> cylinder_;
    std::unique_ptr<ParticleManager> longLine_;
    std::unique_ptr<ParticleManager> gradationRing_;

    Emitter primitiveEmitter_{};
    Emitter ringEmitter_{};
    Emitter cylinderEmitter_{};
    Emitter longLineEmitter_;
    Emitter gradationRingEmitter_;

    float primitiveInterval_ = 0.2f;
    float primitiveElapsedTime_ = 0.0f;

    float ringInterval_ = 0.3f;      
    float ringElapsedTime_ = 0.0f;   

    float cylinderInterval_ = 0.3f;
    float cylinderElapsedTime_ = 0.0f;

    bool usePrimitiveAutoEmit_ = true;
    bool useRingAutoEmit_ = true;
    bool useCylinderAutoEmit_ = true;

    
};
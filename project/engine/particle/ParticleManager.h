#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include <wrl.h>
#include <random>
#include "VertexData.h"
#include "Material.h"
#include "Transform.h"
#include "MyMath.h"
#include "TextureManager.h"
#include <numbers>
#include <list>
#include "Camera.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "MyMath.h"

//struct ModelData
//{
//	std::vector<VertexData> vertices;
//	MaterialData material;
//};

struct Particle {
    Sprite* sprite;       // 각 파티클은 Sprite로 관리
    Vector3 velocity;     // 속도
    float lifetime;       // 전체 수명
    float currentTime;    // 현재 경과 시간
};

struct ParticleGroup {
    std::vector<Particle> particles; // 파티클 리스트
    size_t instanceCount;            // 현재 활성화된 파티클 수
};

class ParticleManager {
public:
    void Initialize(DirectXCommon* directXCommon, SpriteCommon* spriteCommon);
    void CreateParticleGroup(const std::string& name, const std::string& textureFilePath);
    void Emit(const std::string& name, const Vector3& position, uint32_t count);
    void Update();
    void Draw();

private:
    DirectXCommon* dxCommon_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr;
    std::unordered_map<std::string, ParticleGroup> particleGroups_; // 그룹 컨테이너
    const float kDeltaTime = 1.0f / 60.0f; // 고정 시간 간격
    MyMath* math = nullptr;
};


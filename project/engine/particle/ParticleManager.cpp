#include "ParticleManager.h"
#include "Logger.h"
#include "StringUtility.h"
#include <Model.h>


using namespace Logger;
using namespace StringUtility;

void ParticleManager::Initialize(DirectXCommon* directXCommon, SpriteCommon* spriteCommon)
{
	dxCommon_ = directXCommon;
	spriteCommon_ = spriteCommon;
}

void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilePath)
{
    assert(particleGroups_.find(name) == particleGroups_.end() && "ParticleGroup already exists!");

    ParticleGroup group;
    for (size_t i = 0; i < 100; ++i) { // 최대 100개의 파티클을 미리 생성
        Particle particle;
        particle.sprite = new Sprite();
        particle.sprite->Initialize(spriteCommon_, textureFilePath);
        particle.lifetime = 0.0f;
        particle.currentTime = 0.0f;
        group.particles.push_back(particle);
    }
    group.instanceCount = 0;
    particleGroups_[name] = group;
}

void ParticleManager::Emit(const std::string& name, const Vector3& position, uint32_t count)
{
    auto& group = particleGroups_[name];

    size_t emitted = 0;
    for (auto& particle : group.particles) {
        if (emitted >= count) break;
        if (particle.currentTime >= particle.lifetime) { // 수명이 다한 파티클만 재사용
            particle.sprite->SetPosition({ position.x, position.y });
            particle.velocity = { 0.0f, 1.0f, 0.0f }; // 위로 올라가는 기본 속도
            particle.lifetime = 1.0f; // 1초 동안 지속
            particle.currentTime = 0.0f;
            ++emitted;
        }
    }
    group.instanceCount += emitted;
}

void ParticleManager::Update()
{
    for (auto& [name, group] : particleGroups_) {
        size_t activeCount = 0;
        for (auto& particle : group.particles) {
            if (particle.currentTime < particle.lifetime) {
                particle.currentTime += kDeltaTime;
                Vector3 newPos = particle.sprite->GetPosition() + math->Add(particle.velocity,kDeltaTime);
                particle.sprite->SetPosition(newPos);
                ++activeCount;
            }
        }
        group.instanceCount = activeCount;
    }
}

void ParticleManager::Draw()
{
    for (auto& [name, group] : particleGroups_) {
        for (auto& particle : group.particles) {
            if (particle.currentTime < particle.lifetime) {
                particle.sprite->Draw();
            }
        }
    }
}

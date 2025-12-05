#pragma once
#include "BaseEnemy.h"
#include "ParticleEffectLibrary.h"

/// <summary>基本的な移動・描画・被弾処理を行う標準的な敵キャラクター。</summary>
class Enemy : public BaseEnemy {
public:
    Enemy();
    ~Enemy() override = default;

    void Initialize() override;
    void Update() override;
    void Draw() override;

    int GetType() const override;
    void OnHit(float damage) override;
    void SetCamera(Camera* camera);
    WorldTransform& GetWorldTransform() override;

private:
	std::unique_ptr<Object3d> enemyModel_;
	std::unique_ptr<WorldTransform> enemyWorldTransform_;
    Camera* camera_ = nullptr;
   
    float hp_ = 100.0f;
	bool isDead_ = false;
	bool isHit_ = false;
};


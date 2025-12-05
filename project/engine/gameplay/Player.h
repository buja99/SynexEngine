#pragma once
#include "BaseScene.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "Object3d.h"
#include "Camera.h"
#include "Sound.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include "BaseEnemy.h"
#include "Enemy.h"
#include "ParticleEffectLibrary.h"

/// <summary>移動・攻撃・パーツ描画と当たり判定を行うプレイヤーキャラクター。</summary>
class Player {


public:

	~Player();

	void Initialize();

	void Updata();

	void Draw();

	void HitEffectDraw();

	const Vector3& GetPosition() const { return position; }

	void SetCamera(Camera* camera);

	bool HitCheck();

	void SetEnemies(const std::vector<BaseEnemy*>& enemies) {
		enemies_ = enemies;
	}
	const Vector3& GetWeaponPosition();
	void SetEffectLibrary(ParticleEffectLibrary* effectLibrary);
	void SetRandomEngine(std::mt19937* engine);
	Vector3 GetWeaponWorldPosition();
private:

	Vector3 position;
	
	float speed = 0.5f;

	Camera* camera_ = nullptr;

	//player model
	static constexpr int kPlayerPartCount = 7; // head, body, armR, armL, legR, legL, weapon
	std::array<std::unique_ptr<Object3d>, kPlayerPartCount> playerParts_;
	std::array<std::unique_ptr<WorldTransform>, kPlayerPartCount> playerTransforms_;

	enum PlayerPartIndex {
		ARM_R, ARM_L, LEG_R, LEG_L, BODY, HEAD, WEAPON

	};

	bool isAttacking_ = false;
	int attackTimer_ = 0;
	const int attackDuration_ = 60; // 60프레임
	float originalWeaponAngleX_ = 0.0f;
	bool hasHit_ = false;
	bool isHitProcessed_ = false;

	std::vector<BaseEnemy*> enemies_;
	ParticleEffectLibrary* effectLibrary_ = nullptr;
	std::mt19937* randomEngine_ = nullptr;
};


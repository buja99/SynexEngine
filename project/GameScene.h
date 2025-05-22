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

class GameScene : public BaseScene {
public:

	~GameScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;

private:
	DirectXCommon* dxCommon_;
	Sound* audio_;
	Input* input_;


	std::unique_ptr<Camera> camera_ = nullptr;

	//model
	std::unique_ptr<Object3d> ground_ = nullptr;
	std::unique_ptr<WorldTransform> groundWorldTransform_ = nullptr;

	//player model
	static constexpr int kPlayerPartCount = 6; // head, body, armR, armL, legR, legL
	std::array<std::unique_ptr<Object3d>, kPlayerPartCount> playerParts_;
	std::array<std::unique_ptr<WorldTransform>, kPlayerPartCount> playerTransforms_;

	enum PlayerPartIndex {
		ARM_R, ARM_L, LEG_R, LEG_L, BODY, HEAD

	};
};


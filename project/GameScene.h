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
#include "Player.h"

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


	

	//model
	std::unique_ptr<Object3d> ground_ = nullptr;
	std::unique_ptr<WorldTransform> groundWorldTransform_ = nullptr;

	std::unique_ptr<Player> player_ = nullptr;
};


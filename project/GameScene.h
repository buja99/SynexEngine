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

	void moveObj();

private:
	DirectXCommon* dxCommon_;
	Sound* audio_;
	Input* input_;


	std::unique_ptr<Camera> camera_ = nullptr;

	//sprite
	std::unique_ptr<Sprite> back_ = nullptr;





	//model
	std::unique_ptr<Object3d> ground_ = nullptr;
	std::unique_ptr<WorldTransform> groundWorldTransform_ = nullptr;

	std::unique_ptr<Object3d> treeModel_ = nullptr;
	std::unique_ptr<Object3d> treeModel2_ = nullptr;
	std::unique_ptr<WorldTransform> treeWorldTransform_ = nullptr;
	std::unique_ptr<WorldTransform> tree2WorldTransform_ = nullptr;

	std::unique_ptr<Object3d> weedsModel_ = nullptr;
	std::unique_ptr<Object3d> weedsModel2_ = nullptr;

	std::unique_ptr<WorldTransform> weedsWorldTransform_ = nullptr;
	std::unique_ptr<WorldTransform> weeds2WorldTransform_ = nullptr;





	std::unique_ptr<Player> player_ = nullptr;
};


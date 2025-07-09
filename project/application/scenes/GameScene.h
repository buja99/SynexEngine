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
#include <fstream>
#include "Ground.h"
#include "json.hpp"
#include "LevelLoader.h"
#include "LevelApplier.h"
#include "ModelRegistry.h"


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


	//json
	

	

	std::unique_ptr<Camera> camera_ = nullptr;

	//sprite
	std::unique_ptr<Sprite> back_ = nullptr;





	//model
	std::unique_ptr<Ground> ground_ = nullptr;



	std::vector<std::unique_ptr<Object3d>> levelObjects_;
	std::vector<std::unique_ptr<WorldTransform>> levelTransforms_;


	std::unique_ptr<Player> player_ = nullptr;

	//efects
	std::unique_ptr<ParticleManager> primitive_;
	std::mt19937 randomEngine_;

	Emitter emitter_{};
	
	
	
	
	
	//json
	//ModelRegistry registry;


	Vector3 cameraOffset_;

	float centerX = 0.0f;
	float centerY = 0.0f;
	float halfW = 0.1f;
	float halfH = 0.1f;

	float left = centerX - halfW;
	float right = centerX + halfW;
	float top = centerY + halfH;
	float bottom = centerY - halfH;
};


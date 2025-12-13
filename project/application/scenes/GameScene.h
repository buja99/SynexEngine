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
#include "Enemy.h"
#include "ParticleEffectLibrary.h"
#include "Skybox.h"

/// <summary>ゲーム内のプレイシーンを構成し、プレイヤー・敵・カメラ・レベル等を管理するシーン </summary>
class GameScene : public BaseScene {
public:

	~GameScene() override;

	/// <summary> ゲームプレイに必要なリソースを読み込み,プレイヤー・敵・レベル・カメラ・エフェクトを初期化 </summary>
	void Initialize() override;

	void Update() override;

	/// <summary> ゲームシーンの描画処理,スプライト、3Dオブジェクト、エフェクトを順序通りに描画 </summary>
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
	std::unique_ptr<Object3d> test_plyer = nullptr;
	std::unique_ptr<WorldTransform> test_plyerTransforms_;

	std::unique_ptr<Object3d> block_ = nullptr;
	std::unique_ptr<WorldTransform> blockTransforms_;

	std::unique_ptr<Ground> ground_ = nullptr;


	std::vector<std::unique_ptr<Object3d>> levelObjects_;
	std::vector<std::unique_ptr<WorldTransform>> levelTransforms_;


	std::unique_ptr<Player> player_ = nullptr;

	std::unique_ptr<Enemy> enemy_ = nullptr;
	std::unique_ptr<ParticleEffectLibrary> effectLibrary_;

	//efects
	std::mt19937 randomEngine_;

	
	//skybox
	std::unique_ptr<Skybox> skybox_ = nullptr;
	
	//json
	ModelRegistry registry;


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


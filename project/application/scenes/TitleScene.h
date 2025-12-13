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
#include "ParticleEffectLibrary.h"

/// <summary>タイトル画面の表示とシーン遷移を管理するシーン </summary>
class TitleScene : public BaseScene {

public:

	~TitleScene() override;

	/// <summary> タイトル画面で使用するモデル・スプライト・カメラ・エフェクトを初期化 </summary>
	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;


private:

	DirectXCommon* dxCommon_;
	Sound* audio_;
	Input* input_;

	std::unique_ptr<Sprite> title = nullptr;


	std::unique_ptr<Object3d> model_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

	std::unique_ptr<Camera> camera_ = nullptr;

	std::unique_ptr<Object3d> testModel_ = nullptr;
	std::unique_ptr<WorldTransform> testWorldTransform_ = nullptr;

	
	std::mt19937 randomEngine_;

	Emitter primitiveEmit_{};
	Emitter ringEmit_{};

	//postEffect
	bool useGrayscale_ = true;
	bool useVignette_ = false;
	bool useRadialBlur_ = false;

	std::unique_ptr<ParticleEffectLibrary> effectLibrary_;

};


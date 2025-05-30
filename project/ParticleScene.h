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
#include "ParticleManager.h"

class ParticleScene : public BaseScene  {


public:

	~ParticleScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;


private:

	DirectXCommon* dxCommon_;
	Sound* audio_;
	Input* input_;

	std::unique_ptr<Camera> camera_ = nullptr;

	std::unique_ptr<ParticleManager> primitive_;
	std::mt19937 randomEngine_;

	std::unique_ptr<Sprite> background= nullptr;

	std::unique_ptr<ParticleManager> meteor_;

	bool useGrayscale_ = true;

};


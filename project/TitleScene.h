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

class TitleScene : public BaseScene {

public:

	~TitleScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;


private:

	DirectXCommon* dxCommon_;
	Sound* audio_;
	Input* input_;

};


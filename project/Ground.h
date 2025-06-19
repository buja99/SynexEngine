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


class Ground{


public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();

	void SetCamera(Camera* camera) {
		this->camera = camera;
		if (ground_) {
			ground_->SetCamera(camera);
		}
	}
private:

	std::unique_ptr<Object3d> ground_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

	Camera* camera = nullptr;
};


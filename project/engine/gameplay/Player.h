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

class Player {


public:

	~Player();

	void Initialize();

	void Updata();

	void Draw();

	const Vector3& GetPosition() const { return position; }

	void SetCamera(Camera* camera);

private:

	Vector3 position;
	
	float speed = 0.5f;

	Camera* camera_ = nullptr;

	//player model
	static constexpr int kPlayerPartCount = 6; // head, body, armR, armL, legR, legL
	std::array<std::unique_ptr<Object3d>, kPlayerPartCount> playerParts_;
	std::array<std::unique_ptr<WorldTransform>, kPlayerPartCount> playerTransforms_;

	enum PlayerPartIndex {
		ARM_R, ARM_L, LEG_R, LEG_L, BODY, HEAD

	};

	


};


#pragma once
#include "Input.h"
#include "Vector3.h" 
#include "Object3d.h"
#include "Bullet.h"
#include "Object3dCommon.h"
#include "MyMath.h"
#include "Model.h"

class Player
{


public:

	~Player();

	void Initialize(const std::vector<Object3d*>& playerParts, Object3dCommon* Object3dCommon);

	void Updata(Input* input);

	void Shoot(Model* bulletModel);

	void Draw();

	const Vector3& GetPosition() const { return position; }

	std::vector<std::unique_ptr<Bullet>>& GetBullets() { return bullets_; }
private:

	Vector3 position;
	Vector3 positionB;
	float speed = 1.0f; 

	std::vector<Object3d*> playerParts_; 

	std::vector<std::unique_ptr<Bullet>> bullets_;

	Model* bulletModel_ = nullptr;

	std::unique_ptr<Object3d> bulletObject3d_ = nullptr;

	Object3dCommon* Object3dCommon_ = nullptr;

	MyMath* math = nullptr;

	void UpDownAnimation();

	void LightLeftAnimation();
};


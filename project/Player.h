#pragma once
#include "Input.h"
#include "Vector3.h" 
#include "Object3d.h"

class Player
{


public:

	void Initialize(const std::vector<Object3d*>& playerParts);

	void Updata(Input* input);

	const Vector3& GetPosition() const { return position; }

private:

	Vector3 position;
	float speed = 1.0f; 

	std::vector<Object3d*> playerParts_;

	void UpdateAnimation();
};


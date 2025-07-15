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

class BaseEnemy {

public:

		BaseEnemy() = default;
	virtual ~BaseEnemy() = default;
	// Initialize the enemy
	virtual void Initialize() = 0;
	// Update the enemy state
	virtual void Update() = 0;
	// Draw the enemy
	virtual void Draw() = 0;
	// Check if the enemy is dead
	virtual bool IsDead() const;
	// Get the type of the enemy
	virtual int GetType() const = 0;
	// Get the position of the enemy
	virtual void OnHit(float damage)=0;
	virtual WorldTransform& GetWorldTransform() = 0;
private:
	// Disable copy constructor and assignment operator
	BaseEnemy(const BaseEnemy&) = delete;
	BaseEnemy& operator=(const BaseEnemy&) = delete;

	bool isDead_ = false;

	


};


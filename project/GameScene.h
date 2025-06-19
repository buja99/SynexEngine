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
#include "json.hpp"
#include "Ground.h"

struct  LevelData {
	struct ObjectData {
		std::string fileName; // Model file name
		Vector3 translation; // Position
		Vector3 rotation;
		Vector3 scaling;
	};

	std::vector<ObjectData> objects; // List of objects in the level

};
extern std::unordered_map<std::string, Model*> models;

class GameScene : public BaseScene {
public:

	~GameScene() override;

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;

	void moveObj();

	void DeserializeObjectRecursive(nlohmann::json& object, LevelData* levelData);

	
private:
	DirectXCommon* dxCommon_;
	Sound* audio_;
	Input* input_;


	//json
	std::vector<std::unique_ptr<Object3d>> levelObjects_;

	

	std::unique_ptr<Camera> camera_ = nullptr;

	//sprite
	std::unique_ptr<Sprite> back_ = nullptr;





	//model
	std::unique_ptr<Ground> ground_ = nullptr;

	std::unique_ptr<Object3d> treeModel_ = nullptr;
	std::unique_ptr<Object3d> treeModel2_ = nullptr;
	std::unique_ptr<WorldTransform> treeWorldTransform_ = nullptr;
	std::unique_ptr<WorldTransform> tree2WorldTransform_ = nullptr;

	std::unique_ptr<Object3d> weedsModel_ = nullptr;
	std::unique_ptr<Object3d> weedsModel2_ = nullptr;

	std::unique_ptr<WorldTransform> weedsWorldTransform_ = nullptr;
	std::unique_ptr<WorldTransform> weeds2WorldTransform_ = nullptr;


	std::unordered_map<std::string, Object3d*> modelTable;
	std::unordered_map<std::string, WorldTransform*> transformTable;


	std::unique_ptr<Player> player_ = nullptr;
};


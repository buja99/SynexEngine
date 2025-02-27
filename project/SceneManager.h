#pragma once
#include <memory>
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

class SceneManager {

public:

	static SceneManager* GetInstance();

	void Finalize();

	void Update();

	void Draw();

	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	~SceneManager() = default;
	SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

	std::unique_ptr<BaseScene> scene_ = nullptr;

	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	AbstractSceneFactory* sceneFactory_ = nullptr;


};


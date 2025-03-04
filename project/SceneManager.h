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

    void SetSceneFactory(std::unique_ptr<AbstractSceneFactory> sceneFactory) { sceneFactory_ = std::move(sceneFactory); }

private:
	~SceneManager() = default;
	SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

	std::unique_ptr<BaseScene> scene_ = nullptr;

	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;


};


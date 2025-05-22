#include "SceneManager.h"
#include <assert.h>
#include <iostream>

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Finalize() {
	if (scene_) {
		scene_->Finalize();
		scene_.reset();
	}
	if (sceneFactory_) {
		sceneFactory_.reset();
	}
	std::cout << "SceneManager: Finalized.\n";
}

void SceneManager::Update() {


	if (!nextSceneName_.empty()) {
		if (scene_) {
			scene_->Finalize();
			scene_.reset();
		}

		assert(sceneFactory_);
		scene_ = sceneFactory_->CreateScene(nextSceneName_);
		scene_->SetSceneManager(this);
		scene_->Initialize();

		nextSceneName_.clear(); // 다음 씬 이름 초기화
	}

	if (scene_) {
		scene_->Update();
	}
}

void SceneManager::Draw() {

	if (scene_) {
		scene_->Draw();
	}
}

void SceneManager::ChangeScene(const std::string& sceneName) {
	nextSceneName_ = sceneName;

}

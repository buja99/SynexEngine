#include "SceneManager.h"
#include <assert.h>

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Finalize() {
}

void SceneManager::Update() {


	if (nextScene_) {
		
		scene_ = std::move(nextScene_);

		
		scene_->SetSceneManager(this);

	
		scene_->Initialize();
	}

	scene_->Update();

}

void SceneManager::Draw() {

	scene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	// 次のシーン生成
	nextScene_ = std::move(sceneFactory_->CreateScene(sceneName));

}

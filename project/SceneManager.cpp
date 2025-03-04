#include "SceneManager.h"
#include <assert.h>

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Finalize() {
	scene_.reset();
	nextScene_.reset();
	sceneFactory_.reset();
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
	if (nextScene_) {  // 기존 씬이 있으면 해제
		nextScene_.reset();
	}

	// 次のシーン生成
	nextScene_ = std::move(sceneFactory_->CreateScene(sceneName));

}

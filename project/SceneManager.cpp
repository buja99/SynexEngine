#include "SceneManager.h"
#include <assert.h>
#include <iostream>

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Finalize() {
	if (scene_) {
		scene_.reset();
	}
	if (nextScene_) {
		nextScene_.reset();
	}
	if (sceneFactory_) {
		sceneFactory_.reset();
	}

	// 디버그 로그 추가
	std::cout << "SceneManager: Finalized, all scenes deleted." << std::endl;
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

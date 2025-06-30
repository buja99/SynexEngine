#include "SceneManager.h"
#include <assert.h>
#include <iostream>
#include "DirectXCommon.h"

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
		

		DirectXCommon* dx = DirectXCommon::GetInstance();
		dx->SetViewport(0.0f, 0.0f, WinApp::kClientWidth, WinApp::kClientHeight);
		dx->SetScissorRect(0, 0, WinApp::kClientWidth, WinApp::kClientHeight);



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

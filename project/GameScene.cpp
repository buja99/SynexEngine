#include "GameScene.h"
#include "SceneManager.h"

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();

}

void GameScene::Update() {

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}

}

void GameScene::Draw() {
}

void GameScene::Finalize() {
}
